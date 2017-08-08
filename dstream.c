// dstream.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "tinycthread\source\tinycthread.h"

#include "config.h"
#include "udp.h"
#include "dgram_sink.h"
#include "dgram_wrapper.h"
#include "clock.h"
#include "cbuff.h"
#include "part.h"
#include "broadcast.h"

extern struct cbuff *in_dgram_cbuff[MAX_SIMULT_BROADCASTS];
extern struct cbuff *in_part_cbuff[MAX_SIMULT_BROADCASTS];
extern struct broadcast *bcast[MAX_SIMULT_BROADCASTS];
extern mtx_t *tx_buffer_sig_mtx[];
extern cnd_t *tx_buffer_sig[];

 
int instream_thrd(size_t *ptr_index) {
    size_t index = (size_t)ptr_index; 

    int errn;
    do {
        struct dgram_wrapper *dgram = calloc(1, sizeof(struct dgram_wrapper));

        if (errn = rx_dgram(dgram, index))
            goto free_dgram;

        uint64_t now = get_time_deci_ms();
        printf("Datagram recevied:\n");
        struct in_addr tmp = { 0 };
        tmp.S_un.S_addr = htonl(dgram->remote_ip);
        printf("    from ip    : %s\n", inet_ntoa(tmp));
        printf("        port: %u\n", dgram->port);
        printf(" data length: %llu\n", dgram->data_length);
        if (dgram->data_length && dgram->data[dgram->data_length - 1] == 0)
            printf("        data:\n%s\n", dgram->data);

        process_dgram(dgram, index);

        continue;

    free_dgram:
        free(dgram->data);
        free(dgram);

    } while (!errn);

    fprintf(stderr, "Input stream thread udp error nr: %d ... aborting \n", errn);
    exit(EXIT_FAILURE);
    return 1;
}

int main(char argc, char argv[])
{
    /*if (argc != 1)
        exit(-1);*/
    size_t index_rx_thrd = 0;
    init_udp_lock();
    int result = init_udp_socket(11111, index_rx_thrd);
    if (result < 0) {
        fprintf(stderr, "Udp init error ... aborting\n");
        exit(EXIT_FAILURE);
    }

    /* Init tx buffer signal mutexes */
    for (int i = 0; i < MAX_SIMULT_BROADCASTS * 2; i++) {
        tx_buffer_sig_mtx[i] = calloc(1, sizeof(mtx_t));
        int ans = mtx_init(tx_buffer_sig_mtx[i], mtx_plain);
        if (ans != thrd_success)
            fprintf(stderr, "Could not init mutex ... aborting \n"), exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_SIMULT_BROADCASTS * 2; i++) {
        tx_buffer_sig[i] = calloc(1, sizeof(cnd_t));
        int ans = cnd_init(tx_buffer_sig[i]);
        if (ans != thrd_success)
            fprintf(stderr, "Could not init cnd ... aborting \n"), exit(EXIT_FAILURE);
    }
    

    /* Setup broadcast struct */
    bcast[0] = calloc(1, sizeof(struct broadcast));
    srand(get_time_deci_ms());
    bcast[0]->broadcast_id = rand();
    bcast[0]->broadcast_id += (uint64_t)rand() << 16;
    bcast[0]->broadcast_id += (uint64_t)rand() << 32;
    bcast[0]->broadcast_id += (uint64_t)rand() << 48;
    bcast[0]->broadcast_type = UDPSOCKET_RELAY;

    thrd_t source_udp_input_thrd;
    thrd_create(&source_udp_input_thrd, instream_thrd, index_rx_thrd);

    in_dgram_cbuff[0] = new_cbuff(SOURCE_DGRAM_BUFFER_SIZE);
    in_part_cbuff[0] = new_cbuff(PART_BUFFER_SIZE);

    
    
    uint64_t start_time = get_time_deci_ms();
    uint64_t time_last_part = start_time;
    /* Time to make a new part? */
    for (;;) {
        thrd_sleep(&(struct timespec) { .tv_nsec = 1000000LL }, NULL); // sleep 1 ms
        uint64_t now = get_time_deci_ms();
        if (now - time_last_part > DEFAULT_PART_PERIOD_DECI_MS) {
            make_part(0);
#ifdef _DEBUG
            printf("Making part ... \n");
#endif
            time_last_part = now;
        }
    }

    return 0;
}

