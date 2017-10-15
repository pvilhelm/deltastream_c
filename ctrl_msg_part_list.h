#pragma once

#include <stdint.h>

#include "part.h"


struct bit_part_list {
    uint32_t newest_part_nr; 
    size_t size_list;///< In bytes
    uint8_t *array;
};

int has_part_nr(struct bit_part_list *bpl, uint32_t part_nr);
int set_part_nr(struct bit_part_list *bpl, uint32_t part_nr);
int copy_bit_part_list(struct bit_part_list *src, struct bit_part_list *dst);

int receive_part_list(uint64_t broadcast_id, uint8_t *el, size_t el_length, struct node *n);
int send_part_list(uint64_t broadcast_id, struct node *n); 