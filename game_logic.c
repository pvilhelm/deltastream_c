#include <stdint.h>
#include <stddef.h>

#include "tinycthread\source\tinycthread.h"
#include "dgram_wrapper.h"
#include "game_logic.h"
#include "config.h"
#include "broadcast.h"
#include "node.h"
#include "node_tree.h"
#include "part.h"
#include "clock.h"

extern struct tree_node *arr_node_tree[];
extern struct broadcast bcast[MAX_SIMULT_BROADCASTS];
extern struct cbuff *send_buffer;
extern cnd_t *tx_buffer_sig;

/**
 * @brief Que a ping message for sending 
 */

static int ping_node(uint64_t broadcast_id, uint8_t *el, size_t el_length, struct node *n) {

    struct ping_state {
        uint64_t t;
        uint8_t i;
        uint8_t final_i;
    };

    /* Parse the received ping element */
    struct chunk_el_ping *ping_el = el;
    /* Check that the values are sane */
    if (ping_el->ping_final > CTRL_MSG_PING_MAX_I || ping_el->ping_final < 3)
        return -1; 
    if (ping_el->ping_final < ping_el->ping_i)
        return -1;

    struct dgram_wrapper *dw = calloc(1, sizeof(struct dgram_wrapper));
    dw->data = calloc(1,sizeof(struct chunk_h) + sizeof(struct chunk_sub_h) + sizeof(struct chunk_el_ping));

    struct chunk_h *ch = dw->data;
    ch->broadcast_id = broadcast_id; 
    ch->version_nr = DSTREAM_V;
    ch->config = CHUNK_CONFIG_DEFAULT;

    struct chunk_sub_h *sch = dw->data + sizeof(struct chunk_h);
    sch->type = CHUNK_TYPE_PING;
    sch->size = (uint16_t)sizeof(struct chunk_el_ping);

    struct chunk_el_ping *cep = (uint8_t*)sch + sizeof(struct chunk_h);

    
    struct cbuff *cb = n->cb_ping_exchange; /* The buffer keeping track of ping messages I sent */
    /* Check if there is a ping exchange ongoing */
    if (!cb) {
        size_t l = ping_el->ping_final / 2 + ping_el->ping_final % 2; /**/
        cb = new_cbuff(l);
        if (ping_el->ping_i != 0) /* i should be 0 in first ping */
            goto err; 
    } else if (is_full(cb)) { /* Check so that the buffer isnt full */
        goto err; /* Oopps */
    }

    struct ping_state {
        uint64_t t;
        uint8_t i;
        uint8_t final_i;
    };

    

    struct ping_state *ps = calloc(1, sizeof(struct ping_state));
    ps->final_i = ping_el->ping_final;
    ps->i = ping_el->ping_i;
    ps->t = get_time_deci_ms();

    put_element(cb, ps);

    /* If the buffer is full the exchange is done */
    if (!is_full(cb)) {


        struct dgram_wrapper *dw_old = put_element_front(send_buffer, dw);
        if (dw_old) { /* If a element is displaced in the buffer free it */
            free(dw_old->data);
            free(dw_old);
        }
        cnd_signal(tx_buffer_sig); /* Signal tx thread to wake up if sleepy */
    }

    if (is_full(cb)) {
        struct ping_state *ps0 = pull_element(cb);
        uint64_t t_prev = ps0->t;
        uint64_t dt = 0;
        uint8_t final_i_0 = ps0->final_i;
        free(ps0);

        /* Iterate over all ping_state and sum delta t */
        size_t l = cb->n_elements;
        for (int i = 0; i < l; i++) {
            struct ping_state *ps_i = pull_element(cb);
            dt += ps_i->t - t_prev;
            t_prev = ps_i->t;
            free(ps_i);
        }
        n->ping = (double)dt / l;
    }

    return 0;

err:/* Exchange error, clean up */
    free(dw->data);
    free(dw);
    return -1;

}

int game_logic_thrd(void* arg) {
    size_t index = (size_t)arg; 


    for (;;) {
        thrd_sleep(&(struct timespec) { .tv_nsec = 10000000LL }, NULL); // sleep 10 ms
        game_logic(index);
    }

    return 0; 
}

int game_logic(int index) {
    return 0;
}

int process_ext_dgram(struct dgram_wrapper *dgram, size_t index) {
    printf("Recevied external datagram\n");

    struct tree_node *nt = arr_node_tree[index];
    uint32_t ip = dgram->remote_ip;
    uint16_t port = dgram->port;
    
    /* Check if remote node is allready in node tree */
    struct tree_node *tn = find_child(nt, ip, port);
    /* If not, add it to tree */
    if (!tn) {
        struct node *n = calloc(1, sizeof(struct node));
        n->ip = ip;
        n->port = port;
        tn = add_child(nt, n);
    }  
    /* Update last seen */
    tn->n->last_seen = get_time_deci_ms();
     
    /* Process the datagram */
    
    /* Ensure datagram are as big as chunk min size */
    size_t chunk_min_size = sizeof(struct chunk_h) + sizeof(struct chunk_sub_h);
    if (chunk_min_size > dgram->data_length)
        goto err;

    /* Parse chunk header */
    struct chunk_h *ch = dgram->data;
    uint64_t bcast_id = ch->broadcast_id;
    uint8_t v;
    uint8_t config = ch->config; 
    v = ch->version_nr;
    /* Assure correct broadcast id for the socket */
    if (bcast_id != bcast[index].broadcast_id)
        goto err;
    
    /* For each element parse chunk */
    for (int i = 0, j = sizeof(struct chunk_h); j + sizeof(struct chunk_sub_h) < dgram->data_length; i++) {
        struct chunk_sub_h *csh0 = j + dgram->data;
        j += sizeof(struct chunk_sub_h);
        uint8_t chunk_el_type = csh0->type;
        uint16_t chunk_el_data_size = csh0->size;
        if (j + chunk_el_data_size > dgram->data_length) /* Check that element doesn't overflow datagram data length */
            goto err;

        switch (chunk_el_type) {
            case CHUNK_TYPE_INVALID:
                goto err; 
            case CHUNK_TYPE_PING:
                ping_node(bcast_id, dgram->data + j, chunk_el_data_size, tn->n);
                break;
        }
    }
    return 0;
err:
    return -1;
}

