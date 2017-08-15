/** \file
 *
 */


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
/** @brief A chunk object is container of split or whole control messages and/or "smaller parts" of a part.
  *
  * The chunk struct is only for documentation purposes and is not actually used in the code. There are atleast one subheader and 
  * data pair. The length of the data array might be 0. 
  *
  */
struct chunk {
    chunk_header_h chunk_header; ///< The header of the chunk
    chunk_sub_h sub_header_0; ///< The first sub-header
    char data_0[]; ///< The data of the corresponding element
    /* ... */
    chunk_sub_h sub_header_n; ///< The n:th sub-header
    char data_n[]; ///< The data of the corresponding element
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
 * A chunk is a generic message sent between nodes that contains data, control data etc. 
 */
struct chunk_h {
    uint64_t broadcast_id; ///< The id of the broadcast
    uint8_t version_nr; ///< Version number of the senders Deltastream implementation 
    uint8_t config; ///< Configuration bits for the chunk
};

/**
 * @brief chunk types
 */

enum cunk_types {
    CHUNK_TYPE_INVALID = 0,
    CHUNK_TYPE_CHUNKS, ///< The chunk that contains chunks split over multiple chunks
    CHUNK_TYPE_PING, ///< A ping 
    CHUNK_TYPE_PART_BID, ///< A bid on part(s) 
    CHUNK_TYPE_PART_BID_CONFIRM, ///< A confirmation of a previous bid
    CHUNK_TYPE_PART_LIST, ///< A list of parts in the senders possession
    CHUNK_TYPE_NODE_LIST, ///< A list of nodes in the broadcast 
};

/**
 * @brief The sub-header of each element of a chunk 
 */
struct chunk_sub_h {
    uint8_t type; ///< The main type of the chunk 
    uint16_t size; ///< The size of the element
};

/** 
 * @brief A sub-header of a chunk that contains part data. 
 *
 * The sub-header is placed under the chunk header if the part is of type data. 
 */
struct chunk_part_h { 
    uint16_t chunks_for_part; ///< The number of chunks that the part is made up of 
    uint16_t chunk_nr; ///< The number of this chunk 
};

/** 
 * @brief A ping control message.
 *
 * For pinging other nodes. Contains its sequence number and the final number. 
 */
struct ctrl_msg_ping {
    uint8_t ping_i; ///< The i:th ping in the series of pings. Increased by one each transmission. 
    uint8_t ping_final; ///< The final value of #ping_i
};

/**
 * @brief Header for part-list control message.
 *
 * Contains option opt, which decides the type of the part-list message.
 */
struct ctrl_msg_part_list_h {
    uint8_t opt; ///< Decides what type of part list it is. 
};

/** 
 * @brief A type of part-list message with bit-encoding
 *
 * The newest part number corresponds the initial part number
 * and each following n:th bit marks that the node has the 
 * (part-number - n):th part, where n is zero-indexed. 
 * 
 * I.e. 0x00 00 00 01 0x00 01 b0000 1001
 * indicates that the node has part 1 and 4. 
 */
struct ctrl_msg_part_list_bits_posetive {
    uint32_t newest_part_nr;
    uint16_t size_bitfield;
};

/** @brief Byte alignment reset. */
#pragma pack(pop) /* Restore default value of alignment */

void make_part(size_t broadcast_id);