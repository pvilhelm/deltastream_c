#include "cbuff.h"
#include "config.h"
#include "tinycthread\source\tinycthread.h"

struct cbuff *send_buffer;
mtx_t *tx_buffer_sig_mtx;
cnd_t *tx_buffer_sig;
struct cbuff *in_dgram_cbuff[MAX_SIMULT_BROADCASTS];
struct cbuff *in_part_cbuff[MAX_SIMULT_BROADCASTS];
struct broadcast *bcast[MAX_SIMULT_BROADCASTS];

int terminate_program = 0; 