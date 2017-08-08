#pragma once

#include <stddef.h>

#include "tinycthread\source\tinycthread.h"

/* Circular buffer of void pointers. If a element is added and the buffer 
 * is full then the oldest element is overwritten. 
 */
struct cbuff {
    void **mem_start; /* Point to start of buffer */
    void **mem_end; /* Points to one past end of buffer */ 
    size_t mem_size; /* mem_end - mem_start */
    void **first; /* "First" element */
    size_t n_elements; /* Number of added elements currently in buffer */
    mtx_t *lock;
};

/* Create a new buffer of size size */
struct cbuff* new_cbuff(size_t size);
/* Get a ptr to i:t element from start (wrappin index) */
void* get_element(struct cbuff *b, size_t i);
/* Put a element at the end of the buffer */
void* put_element(struct cbuff *b, void *el);
/* Retrieve first element */
void* pull_element(struct cbuff *b);
/* Query if the buffer is full */
int is_full(struct cbuff *b);
/* Free a cbuff */
void free_cbuff(struct cbuff *b);
