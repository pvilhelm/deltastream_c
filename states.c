#include "cbuff.h"
#include "config.h"
#include "tinycthread\source\tinycthread.h"

struct cbuff *send_buffers[MAX_SIMULT_BROADCASTS * 2];
mtx_t *tx_buffer_sig_mtx[MAX_SIMULT_BROADCASTS * 2];
cnd_t *tx_buffer_sig[MAX_SIMULT_BROADCASTS * 2];

struct cbuff *in_dgram_cbuff[MAX_SIMULT_BROADCASTS];
struct cbuff *in_part_cbuff[MAX_SIMULT_BROADCASTS];
struct broadcast *bcast[MAX_SIMULT_BROADCASTS];