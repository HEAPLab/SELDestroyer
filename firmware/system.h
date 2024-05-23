
#ifndef SYSTEM_H
#define SYSTEM_H

#define _XTAL_FREQ 32000000 // Thsi is actually the system one

#include <xc.h>

#include <stdint.h>

#define DEBUG_IS_ACTIVE 1

#define IO_OUTPUT_DIS_SET(x) LATAbits.LATA2 = (x);
#define IO_LED_STATUS_SET(x) LATCbits.LATC3 = (x);

#define IO_BUZZER_SET(x) LATAbits.LATA0 = (x);

#define EEPROM_ADDR_ULIM_LO 0x10
#define EEPROM_ADDR_ULIM_HI 0x11

#define EEPROM_ADDR_TLIM_LO 0x15
#define EEPROM_ADDR_TLIM_HI 0x16

#define EEPROM_ADDR_AVG_MODE_HI 0x1A
#define EEPROM_ADDR_AVG_MODE_LO 0x1B

#define EEPROM_ADDR_OUT_STAT 0x1F

void system_primary_init(void);

void system_io_init(void);


#define serial_send_char(c) {\
    while(TRMT==0);\
    TX1REG = (c);\
}

void serial_send_cmd(const char *s);

uint8_t EEPROM_read(uint8_t addr);

void EEPROM_write(uint8_t addr, uint8_t data);

#define system_get_time_point() ((((uint16_t)TMR0H)<<8U) | TMR0L) 
#define system_start_timer2() { TMR2 = 0; T2CONbits.TMR2ON = 1; }
#define system_stop_timer2() { T2CONbits.TMR2ON = 0; }


#endif /* SYSTEM_H */