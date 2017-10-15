#pragma once

#include <stdint.h>

struct ping_state {
    uint64_t t;
    uint8_t i;
    uint8_t final_i;
};

struct dgram_wrapper *callocate_ping_dgw(uint64_t broadcast_id, uint8_t i, uint8_t final_i);
int ping_node_init(uint64_t broadcast_id, struct node *n);
int ping_node(uint64_t broadcast_id, uint8_t *el, size_t el_length, struct node *n);