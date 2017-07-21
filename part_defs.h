#pragma once

#include <stdint.h>

#include "node.h"

/* Define fundamental part */

typedef struct chunk_list {
    void* first_chunk;
    void* last_chunk;
} chunk_list_t;

typedef struct chunk_header {
    uint8_t version_nr[2];
    uint64_t broadcast_id;
    uint8_t type;
    uint8_t subtype;
    uint16_t chunks_for_part;
    uint16_t chunk_nr; 
} chunk_header_t;

typedef struct generic_chunk {
    chunk_header_t chunk_header; 
    uint16_t size;
    uint8_t* data;
} generic_chunk_t;

typedef struct chunk_list_wrapper {
    node_t node;
} chunk_list_wrapper_t;


typedef struct part_header {
    uint8_t version_nr[2]; /* Protocol version and magic-nr:ish*/
    uint64_t broadcast_id; /* Broadcast identifier */
    uint8_t type;
    uint8_t subtype;
    //uint64_t time_stamp; /* ms POSIX time of creation or target playback time */
} part_header_t;

typedef struct generic_part {
    part_header_t part_header; 
    chunk_list_t chunk_list;
} generic_part_t;

/* Defines some specific parts*/

typedef struct part_request {
    part_header_t part_header; 
     
} part_request_t;


