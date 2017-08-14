#include "cbuff.h"

#include <stdio.h>
#include <stdlib.h>

struct cbuff *new_cbuff(size_t size) {
    struct cbuff* b = calloc(1, sizeof (struct cbuff));
    b->mem_start = calloc(size, sizeof(void*));
    b->mem_end = b->mem_start+size;
    b->mem_size = size;  
    b->lock = calloc(1, sizeof(mtx_t));
    int err = mtx_init(b->lock, mtx_plain);
    if (err == thrd_error) {
        free(b->lock);
        free(b->mem_start);
        free_cbuff(b); 
        b = 0;
        printf("Couldn't init mutex in %s:%u ... aborting ", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    return b;
}

void *get_element(struct cbuff *b, size_t i) {
    mtx_lock(b->lock);
    if(!b->first || b->n_elements < i + 1)
        return mtx_unlock(b->lock), 0;
    size_t mem_i = (b->first - b->mem_start + i) % b->mem_size;
    void *ret = b->mem_start[mem_i];
    return mtx_unlock(b->lock), ret;
}

void *put_element_front(struct cbuff *b, void *el) {

    mtx_lock(b->lock);

    if (!b->first) { /* If buffer is empty */
        b->mem_start[0] = el;
        b->first = b->mem_start;
        b->n_elements++;
        return mtx_unlock(b->lock), 0;
    }

    size_t i_end = (b->first - b->mem_start + b->n_elements) % b->mem_size;
    size_t rel_i = b->first - b->mem_start - 1;
    size_t i_pre_start = rel_i < 0 ? b->mem_end - b->mem_start : b->first - 1 - b->mem_start;

    /* Check if buffer is full */
    if (i_end == i_pre_start) {
        void* ret = b->mem_start[i_pre_start];
        b->mem_start[i_pre_start] = el;
        b->first = b->mem_start+i_pre_start;
        return mtx_unlock(b->lock), ret;
    }
    else {
        b->mem_start[i_pre_start] = el;
        b->n_elements++;
        return mtx_unlock(b->lock), 0;
    }

}

void *put_element(struct cbuff *b, void *el) {
    
    mtx_lock(b->lock); 

    if (!b->first) { /* If buffer is empty */
        b->mem_start[0] = el;
        b->first = b->mem_start;
        b->n_elements++;
        return mtx_unlock(b->lock), 0;
    }

    size_t i_end = (b->first-b->mem_start+b->n_elements) % b->mem_size; 
    size_t i_start = (b->first - b->mem_start); 
     
    /* Check if buffer is full */
    if (i_end == i_start) {
        void* ret = b->mem_start[i_end];
        b->mem_start[i_end] = el;
        b->first = b->mem_start + (i_start + 1) % b->mem_size;
        return mtx_unlock(b->lock), ret;
    } else {
        b->mem_start[i_end] = el;
        b->n_elements++;
        return mtx_unlock(b->lock), 0;
    }
     
}

void *pull_element(struct cbuff *b) {
    mtx_lock(b->lock); 

    if (!b->first)
        return mtx_unlock(b->lock), 0;
    
    void *ret = *b->first; /* Return first element */

    --b->n_elements; 

    if (b->n_elements)
        /* Increase first by one and wrapp */
        b->first = b->mem_start+(b->first - b->mem_start + 1) % b->mem_size;
    else 
        b->first = 0;

    return mtx_unlock(b->lock), ret;
}

int is_full(struct cbuff *b) {
    mtx_lock(b->lock);
    int ret = b->n_elements == b->mem_size; 

    return mtx_unlock(b->lock), ret;
}

void free_cbuff(struct cbuff *b) {
    mtx_destroy(b->lock);
    free(b->lock);
    free(b->mem_start);
    free(b);
}
