#pragma once 

#include <stddef.h>

#include "dgram_wrapper.h"

int process_ext_dgram(struct dgram_wrapper dgram, size_t index);

int game_logic(int index);

int game_logic_thrd(void* arg);