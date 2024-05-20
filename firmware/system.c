#include "system.h"

#include "destroyer.h"
#include "protocol.h"

void system_primary_init(void) {
    //  Let's wait for the oscillator
        
    while(OSCSTAT1bits.EXTOR == 0); // Wait for external oscillator

    
    // Start Timer 0 to keep track of timing
    TMR0 = 0;   // Rollover every 0.008192 sec
    T0CON1bits.T0CS = 0b010;    // FOsc/4 = 8 MHz
    T0CON1bits.T0CKPS = 0b1010; // / 1024 = 7812.5 Hz - Rollover in ~ 8 secondi
    // Resolution: 0000000..001 = 128 us
    T0CON0bits.T016BIT = 1; // Enable 16 bit mode
    T0CON0bits.T0EN = 1; // Enable timer

    
}

static void serial_init(void) {
    // Baud rate configuration (57600)
    TX1STAbits.SYNC  = 0;
    TX1STAbits.BRGH  = 1;
    BAUD1CONbits.BRG16 = 1;
    SP1BRG = 138;
    
    RC1STAbits.SPEN = 1;    // Enable serial module
    TX1STAbits.TXEN = 1;    // Enable transmission mode
    PIR1bits.TXIF = 0;      // Clear the interrupt flag
 
    PIE1bits.RCIE = 1;      // Enable interrupts
    RC1STAbits.CREN = 1;    // Enable receiver mode
    PIR1bits.RCIF = 0;      // Clear the interrupt flag

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

    // "In Master mode, the SDA and SCK pins must be configured as inputs. "
    TRISCbits.TRISC0 = 1;   // ### - I2C managed
    TRISCbits.TRISC1 = 1;   // ### - I2C managed

    TRISCbits.TRISC2 = 1;   // Input - INA233_ALARMIN
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
    
    // INT is RC2
    INTPPS = 0b10010;
    
    // TODO: lock PPS registers

    // Enable interrupts
    INTCONbits.PEIE = 1;
    INTCONbits.GIE  = 1;
    INTCONbits.INTEDG = 0;  // Falling edge of INT pin
    PIE0bits.INTE   = 1;    // Enable interrupt on INT pin
    
    // Init serial port
    serial_init();
    
}

void __interrupt() ISR(void) {
    
    if(PIR0bits.INTF) {
        destroyer_sel_occurred();
        PIR0bits.INTF = 0;
    }
    
    if(PIR1bits.RCIF) {
        if(RC1STAbits.OERR) {
            // The only way to clear overflow error:
            RC1STAbits.CREN = 0;
            RC1STAbits.CREN = 1;
            // Should we call a protocol discharge?
        } else {
            protocol_digest_char(RC1REG);
        }
        PIR1bits.RCIF = 0;
    }
}

void serial_send_cmd(const char *s) {
    serial_send_char('$');
    while (*s != '\0') {
        serial_send_char(*s);
        s++;
    }
    serial_send_char('\n');
}

uint8_t EEPROM_read(uint8_t addr) {
    // FSR address space between 0x7000 and 0x70FF.
    NVMCON1bits.NVMREGS = 1;    // Reading from EEPROM
    NVMADRH = 0xF0;
    NVMADRL = addr;
    NVMCON1bits.RD = 1; // Initiate the read
    return NVMDATL;
}

void EEPROM_write(uint8_t addr, uint8_t data) {

    NVMCON1bits.NVMREGS = 1;    // Writing to EEPROM

    NVMCON1bits.WREN = 1;   // Enable WRITE mode

    NVMADRH = 0xF0;
    NVMADRL = addr;
    NVMDAT = data;

    // Unlock sequence
    INTCONbits.GIE = 0; // Disable interrupts
    NVMCON2 = 0x55;
    NVMCON2 = 0xAA;
    NVMCON1bits.WR = 1;
    INTCONbits.GIE = 1; // Re-enable interrupts

    NVMCON1bits.WREN = 0;   // Disable WRITE mode (it's safe)

    while(NVMCON1bits.WR);  // Wait completion

}

