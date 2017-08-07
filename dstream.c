// dstream.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>

#include "tinycthread\source\tinycthread.h"

#include "udp.h"
#include "dgram_sink.h"
#include "dgram_wrapper.h"
#include "clock.h"
#include "cbuff.h"

int main(char argc, char argv[])
{
    /*if (argc != 1)
        exit(-1);*/

    int result = init_udp_socket(11111);
    if (result != 0) {
        printf("Udp init error: %i\n", result);
        exit(EXIT_FAILURE);
    }
    
    printf("time: %llu \n", get_time_deci_ms());

    /*thrd_t sink_thread;
    thrd_create(&sink_thread, )*/

    struct cbuff* part_buffer = new_cbuff(100);

    int errn;
    do{
        dgram_wrapper_t dgram = {0}; 

        errn = rx_dgram(&dgram);
         
        printf("Datagram recevied:\n");
        struct in_addr tmp = {0};
        tmp.S_un.S_addr = htonl(dgram.remote_ip);
        printf("    from ip    : %s\n", inet_ntoa(tmp));
        printf("        port: %u\n", dgram.port);
        printf(" data length: %llu\n", dgram.data_length);
        if(dgram.data_length && dgram.data[dgram.data_length-1] == 0)
            printf("        data:\n%s\n", dgram.data);

        process_dgram(dgram);

        free(dgram.data);

    } while (!errn);
    
    free_cbuff(part_buffer), part_buffer = 0;

    return 0;
}

