#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "cmsis_os.h"

#ifdef  __cplusplus
extern "C"
{
#endif

#define  MAX(a,b)             ( ((a) > (b)) ? (a) : (b) )

// ARM Cortex BitBand access macros
// Convert SRAM address
#define BITBAND_SRAM(addr, bit) ((bool *)((SRAM_BB_BASE + ((uint32_t)(addr)-SRAM_BASE)*32 + (bit*4))))
#define BITM( addr, bit ) (BITBAND_SRAM(addr,bit))

// Convert PERIPHERAL address
#define BITBAND_PERI(addr, bit) ((bool *)((PERIPH_BB_BASE + ((uint32_t)(addr)-PERIPH_BASE)*32 + (bit*4))))
#define BITP( addr, bit ) (BITBAND_PERI(addr,bit))


#define ARRAY_SIZE(arr)     (sizeof(arr)/sizeof(arr[0]))
#define UNUSED_PARAM(x)     (void)(x)


#define ASSERT(condition)                                               \
    do {                                                                \
        if (!(condition))                                               \
        {                                                               \
        /* should be replaced with project specific assert function */  \
            abort();                                                    \
        }                                                               \
    } while(0)


#ifdef  __cplusplus
}
#endif

#endif /* __COMMON_H__ */
