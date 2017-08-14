#include "cbuff.h"
#include "config.h"
#include "tinycthread\source\tinycthread.h"
#include "node_tree.h"
#include "broadcast.h"

/**
 * @brief Pointer to circular buffer of datagrams to send.
 *
 * The buffer is common for all broadcasts and the datagrams are 
 * stored in a dgram_wrapper struct to which the buffer element points.
 */
struct cbuff *send_buffer;

/**
 * @brief Mutex for send_buffer.
 */
mtx_t *tx_buffer_sig_mtx;
cnd_t *tx_buffer_sig; ///< Condition signal for send_buffer.
/**
 * @brief Array of pointers to circular buffers of incoming datagrams per broadcast
 *
 * Each broadcast has it's own buffer and the datagrams are stored in dgram_wrapper objects.
 * The elements are pointer to dgram_wrapper. 
 */
struct cbuff *in_dgram_cbuff[MAX_SIMULT_BROADCASTS];
struct cbuff *in_part_cbuff[MAX_SIMULT_BROADCASTS];
/**
 * @brief Array of broadcast objects.
 *
 * The broadcast objects tracks the state of a broadcast. 
 */
struct broadcast bcast[MAX_SIMULT_BROADCASTS];

/**
 * @brief Setting this to non-zero terminates the program gracefully.
 */
int terminate_program;
/**
 * @brief Setting i:th element to non-zero terminates i:th broadcast. 
 */
int terminate_bcast[MAX_SIMULT_BROADCASTS];
/**
 * @brief Binary tree containing remote nodes per broadcast.
 */
struct node_tree *arr_node_tree[MAX_SIMULT_BROADCASTS];
