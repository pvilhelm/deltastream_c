 
#include "ctrl_msg_ping.h"

#include "dgram_wrapper.h"
#include "config.h"
#include "part.h"
#include "node.h"



extern struct cbuff *send_buffer;
extern cnd_t *tx_buffer_sig;

/**
* @brief callocate a ping message #dgram_wrapper object.
*/
struct dgram_wrapper *callocate_ping_dgw(uint64_t broadcast_id, uint8_t i, uint8_t final_i) {

    /* Check input */
    if (i > final_i)
        return 0;

    struct dgram_wrapper *dw = calloc(1, sizeof(struct dgram_wrapper));
    dw->data = calloc(1, sizeof(struct chunk_h) + sizeof(struct chunk_sub_h) + sizeof(struct ctrl_msg_ping));

    struct chunk_h *ch = dw->data;
    ch->broadcast_id = broadcast_id;
    ch->version_nr = DSTREAM_V;
    ch->config = CHUNK_CONFIG_DEFAULT;

    struct chunk_sub_h *sch = dw->data + sizeof(struct chunk_h);
    sch->type = CHUNK_TYPE_PING;
    sch->size = (uint16_t)sizeof(struct ctrl_msg_ping);

    struct ctrl_msg_ping *cep = (uint8_t*)sch + sizeof(struct chunk_sub_h);
    cep->ping_i = i;
    cep->ping_final = final_i;

    return dw;
}

/**
* @brief Que a initial ping message.
*
* Starts a ping exchange with target computer.
*/
int ping_node_init(uint64_t broadcast_id, struct node *n) {

    if (!n)
        return -1;

    /* Initialize the #cbuff for the ping exchange */
    struct cbuff *cb = n->cb_ping_exchange; /* The buffer keeping track of ping messages I sent */
                                            /* Check if there is a ping exchange ongoing */
                                            /* Empty any existing ping exchange with the target */
    if (cb) {
        struct ping_state *ps;
        while (ps = pull_element_front(cb))
            free(ps);
        free_cbuff(cb);
        n->cb_ping_exchange = cb = 0;
    }

    /* Allocate a cbuff of the correct length */
    size_t l = CTRL_MSG_PING_FINAL_DEFAULT / 2 + CTRL_MSG_PING_FINAL_DEFAULT % 2; /* 3 -> 2, 4 -> 2, 5 -> 3, 6 -> 3 etc */
    n->cb_ping_exchange = cb = new_cbuff(l + 1);

    /* Create a ping datagram */
    struct dgram_wrapper *dw = callocate_ping_dgw(broadcast_id, 0, CTRL_MSG_PING_FINAL_DEFAULT);

    /* Create a ping_state object and store in buffer */
    struct ping_state *ps = calloc(1, sizeof(struct ping_state));
    ps->final_i = CTRL_MSG_PING_FINAL_DEFAULT;
    ps->i = -1; /* To always have 2 between ping_states i when checking*/
    ps->t = get_time_deci_ms();
    put_element_back(cb, ps);

    /* Put ping datagram infront of datagram tx que */
    struct dgram_wrapper *dw_old = put_element_front(send_buffer, dw);
    if (dw_old) { /* If an element is displaced in the buffer free it */
        free(dw_old->data);
        free(dw_old);
    }
    cnd_signal(tx_buffer_sig); /* Signal tx thread to wake up if sleepy */


    return 0;
}

/**
* @brief Que a ping message for sending
*/
int ping_node(uint64_t broadcast_id, uint8_t *el, size_t el_length, struct node *n) {

    struct cbuff *cb = n->cb_ping_exchange; /* The buffer keeping track of ping messages I sent */
    struct dgram_wrapper *dw = 0;

    /* Check if missing target node */
    if (!n) goto err;

    /* Check that ping element is not null */
    if (!el) goto err;


    /* Parse the received ping element */
    struct ctrl_msg_ping *ping_el = el;
    /* Check that the values are sane */
    if (ping_el->ping_final > CTRL_MSG_PING_MAX_I || ping_el->ping_final < 3)
        goto err;
    if (ping_el->ping_final < ping_el->ping_i)
        goto err;

    uint8_t i_rx = ping_el->ping_i;
    uint8_t i_rx_final = ping_el->ping_final;

    /* Check if there is a ping exchange ongoing */
    if (!cb) {
        size_t l = ping_el->ping_final / 2 + ping_el->ping_final % 2; /**/
        n->cb_ping_exchange = cb = new_cbuff(l + 1);
        if (ping_el->ping_i != 0) /* i should be 0 in first ping */
            goto err;
    } else if (is_full(cb)) { /* Check so that the buffer isnt full */
        goto err; /* Oopps */
    }

    struct ping_state *ps = calloc(1, sizeof(struct ping_state));
    ps->final_i = ping_el->ping_final;
    ps->i = ping_el->ping_i;
    ps->t = get_time_deci_ms();

    put_element_back(cb, ps);

    if (i_rx != i_rx_final) { /* Not done sending */
        dw = callocate_ping_dgw(broadcast_id, i_rx + 1, i_rx_final);
        if (!dw)
            goto err;
        struct dgram_wrapper *dw_old = put_element_front(send_buffer, dw);
        if (dw_old) { /* If a element is displaced in the buffer free it */
            free(dw_old->data);
            free(dw_old);
        }
        cnd_signal(tx_buffer_sig); /* Signal tx thread to wake up if sleepy */
    }

    if (is_full(cb)) { /* Is full, so all ping datagrams are received */

        struct ping_state *ps0 = pull_element_front(cb);
        uint64_t t_prev = ps0->t;
        uint64_t dt = 0;
        uint8_t final_i_0 = ps0->final_i;
        uint8_t old_i = ps0->i;
        free(ps0);

        /* Iterate over all ping_state and sum delta t */
        size_t l = cb->n_elements;

        for (int i = 0; i < l; i++) {
            struct ping_state *ps_i = pull_element_front(cb);
            dt += ps_i->t - t_prev;
            t_prev = ps_i->t;
            uint8_t new_i = ps_i->i;
            free(ps_i);
            if ((uint8_t)(new_i - old_i) != (uint8_t)2) /* ping_states has to be in order/no missing states */
                goto err;
            old_i = new_i;
        }
        n->ping = (double)dt / l;

        free_cbuff(cb);
        n->cb_ping_exchange = cb = 0;
    }

    return 0;

err:/* Exchange error, clean up */
    if (cb) {
        struct ping_state *ps;
        while (ps = pull_element_front(cb))
            free(ps);
        free_cbuff(cb);
        n->cb_ping_exchange = cb = 0;
    }

    if (dw)
        free(dw->data), free(dw);

    return -1;
}