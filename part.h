#pragma once 

#include <stdint.h>
#include <stddef.h>

enum ELEMENT_TYPE {
    UDP_DGRM_TIMED = 1,
    UDP_DGRM_UNTIMED,
    STREAM_SHORT_DESCR,
    STREAM_HTML_DESCR,
    STREAM_INFO,
};

enum BROADCAST_TYPE {
    UDPSOCKET_RELAY = 1,
};

enum BROADCAST_SUBTYPE {
    UNDEFINED = 0,
};

#ifdef DOXYGEN
/** @brief A chunk object is container of split or whole control messages and/or smaller part of a part
  *
  * The chunk struct is only for documentation porpuses and is not acctually used. The subheader and 
  * data part might be repeated 0 or more times. 
  */
struct chunk {
    chunk_header_h chunk_header; ///< The header of the chunk
    chunk_sub_h sub_header_0; ///< The first subheader
    char data_0; ///< The data of the corrensponing element
    /* ... */
    chunk_sub_h sub_header_n; ///< The n:th subheader
    char data_n; ///< The data of the corrensponing element
};
#endif

/**
 * @brief Byte alignment set to one to make casting/copying easy. 
 *
 * The compiler needs to support \#pragma pack()
 */
#pragma pack(push,1) /* Set alignment to one byte */

struct part_h {
    uint8_t protocol_version;
    uint8_t reserved;
    uint8_t broadcast_type;
    uint8_t broadcast_subtype; 
    uint64_t broadcast_id; 
    uint32_t part_data_size;
};

struct part_element_h {
    uint8_t element_type;
    uint8_t element_subtype;
    uint32_t element_size;  
};

struct part_element_UDP_timed_h {
    uint16_t src_port;
    uint16_t trgt_port;
    uint64_t birth_time;
    uint16_t data_length; 
};



/**
 * @brief The header of a chunk.
 *
 * A chunk is a generic message sent between nodes that cointans data, control data etc. 
 */
struct chunk_h {
    uint64_t broadcast_id; ///< The id of the broadcast
    uint8_t version_nr; ///< Version number of the senders Deltastream implimentation 
    uint8_t config; ///< Configuration bits for the cunk
};

/**
 * @brief chunk types
 */

enum cunk_types {
    CHUNK_TYPE_INVALID = 0,
    CHUNK_TYPE_CHUNKS, ///< The chunk that contains chunks split over multiple chunks
    CHUNK_TYPE_PING, ///< A ping 
    CHUNK_TYPE_PART_BID, ///< A bid on part(s) 
    CHUNK_TYPE_PART_BID_CONFIRM, ///< A confirmation of a previospeus bid
    CHUNK_TYPE_PART_LIST, ///< A list of parts in the senders possetion
    CHUNK_TYPE_NODE_LIST, ///< A list of nodes in the broadcast 
};

/**
 * @brief The subheader of each element of a chunk 
 */
struct chunk_sub_h {
    uint8_t type; ///< The main type of the chunk 
    uint16_t size; ///< The size of the element
};

/** 
 * @brief A subheader of a chunk that contains part data. 
 *
 * The subheader is placed under the chunk header if the part is of type data. 
 */
struct chunk_part_h { 
    uint16_t chunks_for_part; ///< The number of chunks that the part is made up of 
    uint16_t chunk_nr; ///< The number of this chunk 
};

struct generic_chunk {
    struct chunk_h chunk_header;
    uint16_t size;
    uint8_t* data;
};

struct chunk_el_ping {
    uint8_t ping_i; ///< The i:th ping in the series of pings. Increased by one each transmission. 
    uint8_t ping_final; ///< The final value of #ping_i
};

#pragma pack(pop) /* Restore default value of alignement */

void make_part(size_t broadcast_id);