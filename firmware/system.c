#include "system.h"

void system_primary_init(void) {
    //  Let's wait for the oscillator
    
    while(OSCSTAT1bits.EXTOR == 0); // Wait for external oscillator
    
}

void system_io_init(void) {

    ANSELA = ANSELC = 0;    // All inputs are digital
    
    
    
    // If debug is active, we can't use the buzzer
#if DEBUG_IS_ACTIVE
    TRISAbits.TRISA0 = 1;
#else
    TRISAbits.TRISA0 = 0;
#endif
    
    TRISAbits.TRISA1 = 1;   // Unused - PGC
    TRISAbits.TRISA2 = 0;   // Output - OUT_off
    TRISAbits.TRISA4 = 1;   // OSC1
    TRISAbits.TRISA5 = 1;   // OSC1

    TRISCbits.TRISC0 = 1;   // ### - I2C managed
    TRISCbits.TRISC1 = 1;   // ### - I2C managed
    TRISCbits.TRISC2 = 1;   // Input - INA233_ALARMIN   // TODO: we need pull up?
    TRISCbits.TRISC3 = 0;   // Output - LED status
    TRISCbits.TRISC4 = 1;   // ### - Serial managed
    TRISCbits.TRISC5 = 1;   // ### - Serial managed
    
    
    RXPPS = 0b10101;    // RX is RC5
    RC4PPS = 0b10100;   // RC4 is TX
    
    // I2C CLK RC0
    SSP1CLKPPS = 0b10000;
    RC0PPS = 0b11000;

    // I2C DAT RC1
    SSP1DATPPS = 0b10001;
    RC1PPS = 0b11001;
    
    // TODO: lock PPS registers
    
}

