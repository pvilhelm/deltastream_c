#pragma once

#include <stdint.h>
#include <stddef.h>

struct broadcast {
    uint8_t broadcast_type;
    uint8_t broadcast_subtype;
    uint64_t broadcast_id;
};