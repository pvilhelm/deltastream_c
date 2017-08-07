// dstream.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>

#include "tinycthread\source\tinycthread.h"

#include "config.h"
#include "udp.h"
#include "dgram_sink.h"
#include "dgram_wrapper.h"
#include "clock.h"
#include "cbuff.h"
#include "part.h"
#include "broadcast.h"

struct cbuff *in_dgram_cbuff[MAX_SIMULT_BROADCASTS];
struct cbuff *in_part_cbuff[MAX_SIMULT_BROADCASTS];
struct broadcast *bcast[MAX_SIMULT_BROADCASTS];

int main(char argc, char argv[])
{
    /*if (argc != 1)
        exit(-1);*/

    int result = init_udp_socket(11111,0);
    if (result != 0) {
        printf("Udp init error: %i\n", result);
        exit(EXIT_FAILURE);
    }
    
    /*thrd_t sink_thread;
    thrd_create(&sink_thread, )*/

    in_dgram_cbuff[0] = new_cbuff(SOURCE_DGRAM_BUFFER_SIZE);

    uint64_t start_time = get_time_deci_ms();
    uint64_t time_last_part = start_time; 
    int errn;
    do{
        struct dgram_wrapper *dgram = calloc(1, sizeof(struct dgram_wrapper));

        if (errn = rx_dgram(dgram,0))
            goto free_dgram;
        uint64_t now = get_time_deci_ms();
        printf("Datagram recevied:\n");
        struct in_addr tmp = {0};
        tmp.S_un.S_addr = htonl(dgram->remote_ip);
        printf("    from ip    : %s\n", inet_ntoa(tmp));
        printf("        port: %u\n", dgram->port);
        printf(" data length: %llu\n", dgram->data_length);
        if (dgram->data_length && dgram->data[dgram->data_length - 1] == 0)
            printf("        data:\n%s\n", dgram->data);

        process_dgram(dgram,0);

        /* Time to make a new part? */
        if (now - time_last_part > DEFAULT_PART_PERIOD_DECI_MS) {
            make_part(0);
            time_last_part = now; 
        }

        continue; 

    free_dgram:
        free(dgram->data);
        free(dgram);
    } while (!errn);
    
    

    return 0;
}

