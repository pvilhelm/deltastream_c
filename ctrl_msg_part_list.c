
#include "ctrl_msg_part_list.h"

#include "config.h"

int has_part_nr(struct bit_part_list *bpl, uint32_t part_nr) {
    if(!bpl)
        return 0;
    if (bpl->newest_part_nr < part_nr)
        return 0;

    uint32_t diff = bpl->newest_part_nr - part_nr;  
    size_t arr_offset = diff / 8;
    size_t bit_offset = diff % 8;
    
    if (arr_offset > bpl->size_list - 1)
        return 0;

    return bpl->array[arr_offset] & (1 << bit_offset) ? 1 : 0;
}

int set_part_nr(struct bit_part_list *bpl, uint32_t part_nr) {
    if(!bpl)
        return -1;
    if (bpl->newest_part_nr < part_nr)
        return -1;

    uint32_t diff = bpl->newest_part_nr - part_nr;
    size_t arr_offset = diff / 8;
    size_t bit_offset = diff % 8;

    if (arr_offset > bpl->size_list - 1)
        return -1;

    bpl->array[arr_offset] |= (1 << bit_offset);
    
    return 0;
}

int copy_bit_part_list(struct bit_part_list *src, struct bit_part_list *dst) {
    if(src == 0 || dst == 0){
        return -1;
    }

    uint64_t diff_lists = dst->newest_part_nr - src->newest_part_nr;

    if (diff_lists > 0) { /* dst has higher newest nr */
        for (uint64_t i = dst->newest_part_nr - diff_lists; i > dst->newest_part_nr - dst->size_list*8; i--) {
            if(has_part_nr(src, i))
                set_part_nr(dst,i); 
        }
    } else {
        return -1;
    }

    return 0;
}


int receive_part_list(uint64_t broadcast_id, uint8_t *el, size_t el_length, struct node *n){
    return 0;
}

int send_part_list(uint64_t broadcast_id, struct node *n) {
    return 0;
}