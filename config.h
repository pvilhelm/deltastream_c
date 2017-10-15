#pragma once

#define DSTREAM_V_MAJOR 0
#define DSTREAM_V_MINOR 1
#define DSTREAM_V (( DSTREAM_V_MAJOR << 4)+(DSTREAM_V_MINOR))

#define MAX_SIMULT_BROADCASTS 1
#define SOURCE_DGRAM_BUFFER_SIZE (1 << 11)
#define PART_BUFFER_SIZE (1<<12)
#define SEND_BUFFER_SIZE 256
#define CTRL_MSG_BUFFER_SIZE 256

#define CTRL_MSG_PART_LIST_MAX_PARTS (1024*8)

#define DEFAULT_PARTS_PER_S 10
#define DEFAULT_PART_PERIOD_DECI_MS (10*1000/DEFAULT_PARTS_PER_S)

#define CHUNK_CONFIG_DEFAULT 0

#define CTRL_MSG_PING_MAX_I 8
#define CTRL_MSG_PING_FINAL_DEFAULT 8