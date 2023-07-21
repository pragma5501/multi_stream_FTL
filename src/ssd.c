#include <stdio.h>
#include "ssd.h"

double get_WAF (ssd_t* my_ssd) 
{
        printf("traffic_ftl    : %ld\n", my_ssd->traff_ftl);
        printf("traffic_client : %ld\n", my_ssd->traff_client);
        double WAF =  my_ssd->traff_ftl / my_ssd->traff_client;
        return WAF;
}

void show_stream_group_log(ssd_t* my_ssd) 
{
        int i;
        for (i = 0; i < STREAM_NUM; i++) {
                printf("===========================================\n");
                printf("stream %d valid copy ratio : %ld\n", i, my_ssd->log_group[i]->valid_copy_ratio);
                printf("stream %d segment num      : %ld\n", i, my_ssd->log_group[i]->segment_num     );
                printf("stream %d traffic_erase    : %ld\n", i, my_ssd->log_group[i]->traff_erase     );
        }
}