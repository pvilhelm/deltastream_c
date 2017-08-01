#pragma once 

#include <stdint.h>

enum ELEMENT_TYPE {
    UDP_DGRM_TIMED = 1,
    UDP_DGRM_UNTIMED,
    STREAM_SHORT_DESCR,
    STREAM_HTML_DESCR,
    STREAM_INFO,
};

struct part_header {
    uint8_t protocol_version;
    uint8_t reserved;
    uint8_t broadcast_type;
    uint8_t broadcast_subtype_sub_type; 
    uint64_t broadcast_id; 
    uint32_t part_data_size;
};

struct part_element_header {
    uint8_t element_type;
    uint8_t element_subtype;
    uint32_t element_size;  
};

