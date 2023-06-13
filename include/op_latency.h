#ifndef __OP_LATENCY_H__
#define __OP_LATENCY_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline uint64_t get_clock_cycle(void)
{
    uint64_t time_cycle=0;
    uint32_t low=0;
    uint32_t high=0;

    /*lint -e40 -e522 -e10*/
    __asm__ __volatile__ ("rdtsc" : "=a" (low), "=d" (high));
    /*lint +e40 +e522 +e10*/

    time_cycle = ((((uint64_t)high) << 32) | ((uint64_t)low));

    // 低32位  高32位
    // cout << "low = " << low << endl;
    // cout << "high = " << high << endl;

    // cout << "tc = " << time_cycle << endl;

    return time_cycle;
}

#ifdef __cplusplus
}
#endif

#endif