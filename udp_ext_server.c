#include <stddef.h>
#include <stdio.h>

#include "tinycthread\source\tinycthread.h"
#include "udp_ext_server.h"
#include "udp.h"
#include "cbuff.h"
#include "config.h"
#include "clock.h"
#include "dgram_wrapper.h"
#include "game_logic.h"

extern struct cbuff *send_buffer;
extern mtx_t *tx_buffer_sig_mtx;
extern cnd_t *tx_buffer_sig;
extern int terminate_program; 

int udp_ext_tx_server_thrd(void* arg) {
    
    size_t index = (size_t)arg; 

    int ret = init_udp_socket(0, index);
    if (ret < 0) {
        fprintf(stderr, "UDP external server init error ... aborting \n");
        exit(EXIT_FAILURE);
    }

    while (!terminate_program) {
        mtx_lock(tx_buffer_sig_mtx);
        cnd_wait(tx_buffer_sig, tx_buffer_sig_mtx);
        mtx_unlock(tx_buffer_sig_mtx);

        for (struct dgram_wrapper *dw = 0; dw = pull_element(send_buffer);) {
            send_dgram(dw, index);
            free(dw->data);
            free(dw);
        }
    }
#ifdef _DEBUG
    printf("Exiting udp_ext_tx_server_thrd\n");
#endif    

    kill_udp_socket(index);

    return 0; 
}

int udp_ext_rx_server_thrd(void* arg) {
    size_t index = (size_t)arg; /* Using ptr value as int argument to avoid races ... */

    int ans = init_udp_socket(0, index);
    if (ans < 0)
        return ans;
    else
        index = ans; 

    
    while(!terminate_program) {
        struct dgram_wrapper *dgram = calloc(1, sizeof(struct dgram_wrapper));

        int errn;
        if (errn = rx_dgram(dgram, index))
            goto err;

        uint64_t now = get_time_deci_ms();
        //printf("Datagram recevied:\n");
        struct in_addr tmp = { 0 };
        tmp.S_un.S_addr = htonl(dgram->remote_ip);
       /* printf("    from ip    : %s\n", inet_ntoa(tmp));
        printf("        port: %u\n", dgram->port);
        printf(" data length: %llu\n", dgram->data_length);
        if (dgram->data_length && dgram->data[dgram->data_length - 1] == 0)
            printf("        data:\n%s\n", dgram->data);*/

        process_ext_dgram(dgram, index);

        continue;

    err:
        free(dgram->data);
        free(dgram);
        fprintf(stderr, "Input stream thread udp error nr: %d ... aborting \n", errn);
        exit(EXIT_FAILURE);
    }
#ifdef _DEBUG
    printf("Exiting udp_ext_rx_server_thrd\n");
#endif

    kill_udp_socket(index);

    return 0;
}