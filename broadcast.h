#pragma once

#include <stdint.h>
#include <stddef.h>

#define BROADCAST_TYPE_INVALID 0
#define BROADCAST_TYPE_UDP_DGRM_TIMED 1
#define BROADCAST_TYPE_UDP_DGRM_UNTIMED 2

#define BROADCAST_SUBTYPE_UDP_DGRM_TIMED_DEFAULT 0


/**
 * @brief Broadcast object for state of broadcast 
 *
 * The broadcast struct keeps the state of the broadcast and is 
 * used by functions to make decitions. 
 */

struct broadcast {
    size_t broadcast_index; ///< Index of the broadcast used for indexing global arrays etc
    uint8_t broadcast_type; ///< Type of broadcast
    uint8_t broadcast_subtype; ///< Subtype of broadcast 
    uint64_t broadcast_id; ///< Id of broadcast 
    uint8_t i_am_source; ///< Non zero when this computer is the source of the broadcast 
};