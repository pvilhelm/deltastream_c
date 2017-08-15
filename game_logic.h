#pragma once 

#include <stddef.h>

#include "dgram_wrapper.h"

int process_ext_dgram(struct dgram_wrapper *dgram, size_t index);

int game_logic(int index);

int game_logic_thrd(void* arg);

int ping_node(uint64_t broadcast_id, uint8_t *el, size_t el_length, struct node *n);
int ping_node_init(uint64_t broadcast_id, struct node *n);
struct dgram_wrapper *callocate_ping_dgw(uint64_t broadcast_id, uint8_t i, uint8_t final_i);