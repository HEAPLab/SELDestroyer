
#ifndef SYSTEM_H
#define SYSTEM_H

#define _XTAL_FREQ 32000000 // Thsi is actually the system one

#include <xc.h>

#define DEBUG_IS_ACTIVE 1

#define IO_LED_STATUS_SET(x) LATCbits.LATC3 = (x);

#if DEBUG_IS_ACTIVE
    #define IO_LED_BUZZER_SET(x)
#else
    #define IO_LED_BUZZER_SET(x) LATAbits.LATA0 = (x);
#endif


void system_primary_init(void);

void system_io_init(void);


static inline void serial_send_char(char c) {
    while(TRMT==0);
    TX1REG = c;
}


#endif /* SYSTEM_H */