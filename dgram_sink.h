#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "dgram_wrapper.h"

void process_dgram(struct dgram_wrapper *dgram, size_t broadcast_index);