#include "dgram_sink.h"
#include "cbuff.h"
#include "part_defs.h"
#include "node.h"

extern struct cbuff *in_dgram_cbuff[];

void process_dgram(struct dgram_wrapper *dgram, size_t broadcast_index)
{
    struct cbuff *b = in_dgram_cbuff[broadcast_index];
    struct dgram_wrapper *old = put_element_back(b, dgram);
    #if _DEBUG
    if (old)
        printf("In datagram circular buffer wrapps\n");
    #endif
    if (old) /* If circular buffer is wrapping free oldest datagram */
        free(old->data), free(old);
}
