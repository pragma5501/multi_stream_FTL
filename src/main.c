#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/kernel.h>
#include <unistd.h>
#include <stdint.h>
#include <regex.h>

#include "FTL.h"


#define BUFF_SIZE 1024

//#define DEBUG 1
#define DEBUG_PRINT 1

enum TYPE {
        READ  = 0,
        WRITE = 1,
        TRIM  = 3,                
};

long long progress; 
int GB;

int stream_log[STREAM_NUM];

int parse (char *text, ssd_t* my_ssd, _queue* free_q) {
        double time;
        int type;
        int LBA;
        int size;
        int stream_id;

        if (sscanf(text, "%lf %d %d %d %d", &time, &type, &LBA, &size, &stream_id) == 5) {
                #ifdef DEBUG
                
                printf("==========\n");
                printf("time : %.9lf\n", time);
                printf("type : %d\n",  type);
                printf("LBA  : %d\n",  LBA);
                printf("size : %d\n",  size);
                printf("stream num : %d\n", stream_id);
                printf("==========\n");
                #endif
        }

        stream_log[stream_id]++;
        
        switch (type) {
        // read
        case READ:
                break;        

        // write
        case WRITE:

                my_ssd = trans_IO_to_ssd(my_ssd, free_q, LBA, stream_id);
                if ((++progress) % (262144 * 100) == 0) {
                        GB += 1;
                        progress = 0;

                        printf("[Progress %d GB] : WAF : %.10f\n", GB, get_WAF(my_ssd));
                        //printf("[Progress %d GB]\n", GB);
                        for(int i = 0; i < STREAM_NUM; i++) {
                                printf("stream %d request : %d\n", i,stream_log[i]);
                        }
                        show_stream_group_log(my_ssd);
                }
                break;
                
        // trim
        case TRIM:
                break;
        
        default:
                printf("Error : request type is undefined!!\n");
                break;
        }
        

}

int read_request (FILE* fp, ssd_t* my_ssd, _queue* free_q) 
{
        char buf[BUFF_SIZE];

        while (fgets(buf, sizeof(buf), fp)) {
                parse(buf, my_ssd, free_q);
        }

}

int main (int argc, char** argv) 
{
        // initialize ssd
        ssd_t* my_ssd = ssd_t_init();
        _queue* free_q = free_q_init(my_ssd, q_init());

        // initialze mapping table by set value of mapping table -1
        init_mapping_table();
        
        FILE* fp = fopen("./src/intern-trace", "r");
        if( fp == NULL ) {
                printf("Error : File not opened\n");
                return 0;
        }
        progress = 0;
        GB = 0;
        read_request(fp, my_ssd, free_q);
        fclose(fp);

        printf("User Write : %ld\n", my_ssd->traff_client);
        printf("GC Write   : %ld\n", my_ssd->traff_GC);
        printf("free q size : %d\n", free_q->size);

        destroy_ssd(my_ssd);
        q_destroy(free_q);

        return 0;
}


