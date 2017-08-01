#include "clock.h"

#include <Windows.h>

uint64_t get_time_deci_ms() {
    FILETIME ft = { 0 };

    GetSystemTimePreciseAsFileTime(&ft);
    
    uint64_t h = ft.dwHighDateTime;
    uint64_t l = ft.dwLowDateTime;
    
    uint64_t t = l;
    t += h << 32;
    /* Substract time between 1601 and 1970 */
    t -= 11644473600LL*1000LL*1000LL*10LL; /* FILETIME is in 100 ns */

    return t*1000; /* Convert to 0,1 ms unit */
}
