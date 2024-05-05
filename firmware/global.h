#ifndef GLOBAL_H    /* Guard against multiple inclusion */
#define GLOBAL_H

#include <stdbool.h>
#include <xc.h>


/* ****************************************************************************/
/* Constants and Macros                                                       */
/* ****************************************************************************/

#define USE_EXT_OSCILLATOR 0

#define GET_BIT(n, pos) ((n >> pos) & 0x01)

#define SET_BIT(n, pos, val) n = (n & ~((unsigned)1 << pos)) | ((unsigned)val << pos);

#define INTERRUPTS_DISABLE() asm volatile ("di")
#define INTERRUPTS_ENABLE()  asm volatile ("ei")

typedef enum fatal_exception_e {
    FE_NONE=0b000,
    FE_UNABLE_INIT_INA233=0b001,
} fatal_exception_t;

extern void __attribute__((noreturn)) throw_fatal_exception(fatal_exception_t error_code);

#endif