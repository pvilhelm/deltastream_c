#include <stdint.h>
#include <stddef.h>
#include <stdio.h> 

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
    uint16_t port = dgram->remote_port;
    
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
    
    /* Update bytes rx */
    tn->n->bytes_he_sent_to_me += (uint64_t)dgram->data_length;

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
                break; /* For symmetry =) */
            case CHUNK_TYPE_PING:
                ping_node(bcast_id, dgram->data + j, chunk_el_data_size, tn->n);
                break;
            case CHUNK_TYPE_PART_LIST:
                break;
            default:
                goto err;
                break;
        }
    }
    tn->n->chunk_sucess++;
    return 0;
err:
    tn->n->chunk_errors++;
    return -1;
}

