#ifndef GLOBAL_H    /* Guard against multiple inclusion */
#define GLOBAL_H

#include <stdbool.h>
#include <xc.h>


/* ****************************************************************************/
/* Constants and Macros                                                       */
/* ****************************************************************************/

#define USE_EXT_OSCILLATOR 1

#define GET_BIT(n, pos) ((n >> pos) & 0x01)

#define SET_BIT(n, pos, val) n = (n & ~((unsigned)1 << pos)) | ((unsigned)val << pos);

#define INTERRUPTS_DISABLE() INTCONbits.GIE = 0;
#define INTERRUPTS_ENABLE()  INTCONbits.GIE = 1;

extern void __attribute__((noreturn)) throw_fatal_exception(void);

#endif