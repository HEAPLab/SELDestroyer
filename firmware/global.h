#ifndef GLOBAL_H    /* Guard against multiple inclusion */
#define GLOBAL_H

#include <stdbool.h>
#include <xc.h>


/* ****************************************************************************/
/* Constants and Macros                                                       */
/* ****************************************************************************/


#define GET_BIT(n, pos) ((n >> pos) & 0x01)

#define SET_BIT(n, pos, val) n = (n & ~((unsigned)1 << pos)) | ((unsigned)val << pos);

#define INTERRUPTS_DISABLE() asm volatile ("di")
#define INTERRUPTS_ENABLE()  asm volatile ("ei")


#endif