#pragma once

#include <stdlib.h>
#include <stdint.h>


struct dgram_wrapper{
    char* data; /* Payload */
    size_t data_length; /* Bytes of data */
    uint32_t remote_ip; /* IP this datagram is to be sent to or the source ip of this datagram */
    uint16_t port; /* Port of sender */
    uint16_t local_port; /* Port received on */
    uint64_t time; /* Time of creation */
};
