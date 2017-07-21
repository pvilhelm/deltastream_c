#pragma once

#include <stdint.h>

/* Define node */

#define NODE_STATE_ACTIVE 1U
#define NODE_STATE_INACTIVE 0U
#define NODE_STATE_BANNED 255U

typedef struct node {
    uint32_t ip;                    /* Stored in host endianess */
    uint16_t port;                  /* Remote port nr */
    uint16_t reserved;              /* Need be zero for tree search */
    uint8_t node_state;                
    uint64_t bytes_sent_to_him;
    uint64_t bytes_he_sent_to_me;
    uint64_t dled_parts_from_him;
    uint64_t uled_parts_to_him;
    int64_t balance;                /* His debt minus my debt to him*/
    int8_t rating;                    /* Normalized rating of node */
    uint64_t last_seen;                
} node_t;

