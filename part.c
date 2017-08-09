#include "part.h"
#include "cbuff.h"
#include "dgram_wrapper.h"
#include "config.h"
#include "broadcast.h"

#ifdef _WIN32
    #include <Winsock2.h>
#else
    /* Linux ntohl etc */
#endif

extern struct cbuff *in_dgram_cbuff[];
extern struct cbuff *in_part_cbuff[];
extern struct broadcast *bcast[];

void make_part(size_t broadcast_index) {
    struct cbuff *b_in_dgram = in_dgram_cbuff[broadcast_index];
    struct cbuff *b_in_part = in_part_cbuff[broadcast_index];
    struct broadcast *b = bcast[broadcast_index];

    size_t n_dgrams = b_in_dgram->n_elements;

    /* Calculate size of all datagram data fields */
    size_t tot_data_size = 0;
    for (size_t i = 0; i < n_dgrams; i++) {
        struct dgram_wrapper *dw = get_element(b_in_dgram, i);
        tot_data_size += dw->data_length;
    }

    /* Allocate part object */
    size_t part_size = sizeof(struct part_h) + sizeof(struct part_element_h)*n_dgrams +
        sizeof(struct part_element_UDP_timed_h)*n_dgrams+tot_data_size;
    uint8_t *part = calloc(1, part_size);

    /* Make the part */
    struct part_h *p_h = (void*)part;
    p_h->protocol_version = DSTREAM_V;
    p_h->broadcast_type = b->broadcast_type;
    p_h->broadcast_subtype = b->broadcast_subtype;
    p_h->broadcast_id = htonll(b->broadcast_id);
    p_h->part_data_size = (uint32_t) (part_size -  sizeof(struct part_h));

    size_t el_offset = sizeof(struct part_h);
    uint8_t *el = part + el_offset;
    for (int i = 0; i < n_dgrams; i++) {
        size_t n;
        
        struct dgram_wrapper *dgw = pull_element(b_in_dgram);

        struct part_element_h peh = {
            .element_type = UDP_DGRM_TIMED,
            .element_subtype = 0,
            .element_size = (uint32_t)(dgw->data_length + sizeof(struct part_element_UDP_timed_h)),
        };

        struct part_element_UDP_timed_h peuth = {
            .src_port = htons(dgw->port),
            .trgt_port = htons(dgw->local_port),
            .birth_time = htonll(dgw->time),
            .data_length = htons((uint16_t)dgw->data_length),
        };

        n = sizeof(struct part_element_h);
        memcpy(el, &peh, n);
        el += n;
        n = sizeof(struct part_element_UDP_timed_h);
        memcpy(el, &peuth, n);
        el += n;

        /* Copy the data payload of the datagram */
        memcpy(el, dgw->data, dgw->data_length);
        el += dgw->data_length;
        free(dgw->data);
        free(dgw);
    }

    struct part *old_part = put_element(b_in_part, part);
    if (old_part)
        free(old_part);
}