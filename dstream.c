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
#include "udp_ext_server.h"
#include "node_tree.h"

extern struct cbuff *in_dgram_cbuff[];
extern struct cbuff *in_part_cbuff[];
extern struct broadcast *bcast[];
extern mtx_t *tx_buffer_sig_mtx;
extern cnd_t *tx_buffer_sig;
extern struct cbuff *send_buffer;
extern int terminate_program;
extern struct node_tree *arr_node_tree[];

 
int instream_thrd(size_t *ptr_index) {
    size_t index = (size_t)ptr_index * 3; 

    int result = init_udp_socket(11111, index);
    if (result < 0) {
        fprintf(stderr, "Udp init error ... aborting\n");
        exit(EXIT_FAILURE);
    }

    int errn;
    while (!terminate_program) {
        struct dgram_wrapper *dgram = calloc(1, sizeof(struct dgram_wrapper));

        if (errn = rx_dgram(dgram, index))
            goto err;

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

    err:
        free(dgram->data);
        free(dgram);
        fprintf(stderr, "Input stream thread udp error nr: %d ... aborting \n", errn);
        exit(EXIT_FAILURE);
    }  
#ifdef _DEBUG
    printf("Exiting instream_thrd\n");
#endif
    kill_udp_socket(index);

    return 0;
}

int part_thread(void* arg) {
    size_t index = (size_t)arg * 3; 

    uint64_t start_time = get_time_deci_ms();
    uint64_t time_last_part = start_time;
    /* Time to make a new part? */
    while (!terminate_program) {
        thrd_sleep(&(struct timespec) { .tv_nsec = 1000000LL }, NULL); // sleep 1 ms
        uint64_t now = get_time_deci_ms();
        if (now - time_last_part > DEFAULT_PART_PERIOD_DECI_MS) {
            make_part(index);
            time_last_part = now;
        }
    }
#ifdef _DEBUG
    printf("Exiting part_thread\n");
#endif
    kill_udp_socket(index);

    return 0; 
}

int main(char argc, char argv[])
{
    /*if (argc != 1)
        exit(-1);*/
    size_t broadcast_index = 0;

    /* Init udp config mutex */
    init_udp_lock();
    
    /* Init tx buffer signal mutexes */
    {
        tx_buffer_sig_mtx = calloc(1, sizeof(mtx_t));
        int ans = mtx_init(tx_buffer_sig_mtx, mtx_plain);
        if (ans != thrd_success)
            fprintf(stderr, "Could not init mutex ... aborting \n"), exit(EXIT_FAILURE);
    }
    {
        tx_buffer_sig = calloc(1, sizeof(cnd_t));
        int ans = cnd_init(tx_buffer_sig);
        if (ans != thrd_success)
            fprintf(stderr, "Could not init cnd ... aborting \n"), exit(EXIT_FAILURE);
    }
    
    /* Init circular buffers */
    in_dgram_cbuff[0] = new_cbuff(SOURCE_DGRAM_BUFFER_SIZE);
    in_part_cbuff[0] = new_cbuff(PART_BUFFER_SIZE);
    send_buffer = new_cbuff(SEND_BUFFER_SIZE);

    /* Setup broadcast struct */
    bcast[0] = calloc(1, sizeof(struct broadcast));
    srand((unsigned int)get_time_deci_ms());
    bcast[0]->broadcast_id = rand();
    bcast[0]->broadcast_id += (uint64_t)rand() << 16;
    bcast[0]->broadcast_id += (uint64_t)rand() << 32;
    bcast[0]->broadcast_id += (uint64_t)rand() << 48;
    bcast[0]->broadcast_type = UDPSOCKET_RELAY;

    /* Setup node_tree */
    for (int i = 0; i < 1; i++) {
        arr_node_tree[i] = calloc(1, sizeof(struct tree_node));
    }
        
    /* Start threads */    
    thrd_t source_udp_input_thrd;
    thrd_create(&source_udp_input_thrd, instream_thrd, (void*)broadcast_index);

    thrd_t ext_udp_tx_thrd;
    thrd_create(&ext_udp_tx_thrd, udp_ext_tx_server_thrd, (void*)broadcast_index);
    
    thrd_t ext_udp_rx_thrd;
    thrd_create(&ext_udp_rx_thrd, udp_ext_rx_server_thrd, (void*)broadcast_index);

    /* Make parts in main thread */
    thrd_t source_part_thrd;
    thrd_create(&source_part_thrd, part_thread, (void*)broadcast_index);
    

    thrd_join(source_udp_input_thrd, 0);
    thrd_join(ext_udp_tx_thrd, 0);
    thrd_join(ext_udp_rx_thrd, 0);
    thrd_join(source_part_thrd, 0);

    return 0;
}

