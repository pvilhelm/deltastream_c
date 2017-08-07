#include "cbuff.h"

#include <stdlib.h>

struct cbuff * new_cbuff(size_t size) {
    struct cbuff* b = calloc(1, sizeof (struct cbuff));
    b->mem_start = calloc(size, sizeof(void*));
    b->mem_end = b->mem_start+size;
    b->mem_size = size;  
}

void* get_element(struct cbuff * b, size_t i) {
    if(!b->first)
        return 0;
    size_t mem_i = (b->first - b->mem_start + i) % b->mem_size;
    return b->mem_start[mem_i];
}

void* put_element(struct cbuff * b, void * el) {
    

    if (!b->first) { /* If buffer is empty */
        b->mem_start[0] = el;
        b->first = b->mem_start;
        b->n_elements++;
        return 0; 
    }

    size_t i_end = (b->first-b->mem_start+b->n_elements) % b->mem_size; 
    size_t i_start = (b->first - b->mem_start); 
    /* Check if buffer is full */
    if (i_end == i_start) {
        void* ret = b->mem_start[i_end];
        b->mem_start[i_end] = el;
        b->first = b->mem_start+ (b->first - b->mem_start + 1) % b->mem_size;
        return ret; 
    } else {
        b->mem_start[i_end] = el;
        b->n_elements++;
        return 0;
    }
}

void* pull_element(struct cbuff * b) {
    if (!b->first)
        return 0;
    
    void* ret = *b->first; /* Return first element */

    --b->n_elements; 

    if (b->n_elements)
        /* Increase first by one and wrapp */
        b->first = b->mem_start+(b->first - b->mem_start + 1) % b->mem_size;
    else 
        b->first = 0;
            
    return ret;
}

int is_full(struct cbuff * b) {
    return b->n_elements == b->mem_size;
}

void free_cbuff(struct cbuff* b) {
    free(b->mem_start);
    free(b);
}
