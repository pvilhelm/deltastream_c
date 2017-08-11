

#include "tinycthread\source\tinycthread.h"
#include "dgram_wrapper.h"
#include "game_logic.h"

int game_logic_thrd(void* arg) {

    for (;;) {
        thrd_sleep(&(struct timespec) { .tv_nsec = 1000000LL }, NULL); // sleep 1 ms
    }

    return 0; 
}

int game_logic(int index) {
    return 0;
}

int process_ext_dgram(struct dgram_wrapper *dgram, size_t index) {
    printf("Recevied external datagram\n");
    return 0;
}