#pragma once

#include <stdint.h>

/* Define node */

#define NODE_STATE_ACTIVE 1U
#define NODE_STATE_INACTIVE 0U
#define NODE_STATE_BANNED 255U

typedef struct node {
    uint64_t ip;
    uint64_t reserved;
    uint16_t port;
    uint8_t node_state;
    uint64_t bytes_sent_to_him;
    uint64_t bytes_he_sent_to_me;
    uint64_t dled_parts_from_him;
    uint64_t uled_parts_to_him;
    int64_t balance; /*his debt - my debt to him*/
    int8_t rating; 
    uint64_t last_seen;
} node_t;

