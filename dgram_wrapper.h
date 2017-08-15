#pragma once

#include <stdlib.h>
#include <stdint.h>

/** @brief A wrapper for a datagram
 *
 * This wrapper wraps a datagram and stores remote and local port, remote ip and pointer to data with length and 
 * time of creation.
 *
 */
struct dgram_wrapper{
    char* data; ///< Pointer to data of the datagram
    size_t data_length; ///< Length of the data. 
    uint32_t remote_ip; ///< TX: The IP this datagram is to be sent to. RX: The source ip of this datagram.
    uint16_t remote_port; ///< TX: The port to be sent to. RX: Port this datagram was sent from.
    uint16_t local_port; ///< TX: The port this datagram to be sent from. RX: Port this datagram was received on.
    uint64_t time; ///< Time of creation of this datagram object in ms/10 POSIX time. 
};
