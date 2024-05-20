#include "i2c.h"
#include "system.h"

#include <pic16f18325.h>
#include <stdint.h>


void i2c_init(void) {
#if _XTAL_FREQ == 32000000
    SSP1ADDbits.SSP1ADD=19;      // 400 KHz speed = 32M / (4(SSP1AA +1))
#else
    #error "Umanaged frequency."
#endif
    
    SSP1STATbits.SMP = 0;   // Enable slew rate control for 400 kHz operations
    SSP1STATbits.CKE = 1;   // Enable SMBus voltages

    SSP1CON1bits.WCOL  = 0; // Clear status bits just to be sure
    SSP1CON1bits.SSPOV = 0; // Clear status bits just to be sure
    
    SSP1CON1bits.SSPM = 0b1000; // Configue modules as I2C Master mode
    
    SSP1CON3bits.SDAHT = 0; // Increase SDA hold time for robustness
    
    SSP1CON1bits.SSPEN = 1; // Enable module
}

void i2c_send_start(void) {
    SSP1CON2bits.SEN = 1;
    while(SSP1CON2bits.SEN == 1);
}

static void i2c_send_restart(void) {
    SSP1CON2bits.RSEN = 1;
    while(SSP1CON2bits.RSEN == 1);
}

void i2c_send_stop(void) {
    SSP1CON2bits.PEN = 1;
    while(SSP1CON2bits.PEN == 1);
}

static bool i2c_send_raw(uint8_t address, const uint8_t* data, uint8_t nr_bytes) {
    
    PIR1bits.SSP1IF = 0;
    SSP1BUF = (uint8_t)(address << 1);
    
    uint8_t count = 0;
    do {
        while(SSP1STATbits.BF==1 || PIR1bits.SSP1IF == 0);

        PIR1bits.SSP1IF = 0;

        if(SSP1CON2bits.ACKSTAT == 1) {
            // NACK received
            return false;
        }
        
        if (count < nr_bytes) {
            SSP1BUF = data[count];
        }
        
    } while(count++ < nr_bytes);
    
    return true;
}

bool i2c_send_raw_byte(uint8_t data) {
    
    PIR1bits.SSP1IF = 0;
    SSP1BUF = data;
    
    while(SSP1STATbits.BF==1 || PIR1bits.SSP1IF == 0);

    PIR1bits.SSP1IF = 0;

    if(SSP1CON2bits.ACKSTAT == 1) {
        // NACK received
        return false;
    }
            
    return true;
}


static bool i2c_recv_raw(uint8_t address, uint8_t* data, uint8_t nr_bytes) {
    
    PIR1bits.SSP1IF = 0;
    
    SSP1BUF = (uint8_t)(address << 1);

    // Wait full transmit of the address
    while(SSP1STATbits.BF==1 || PIR1bits.SSP1IF == 0);
    
    PIR1bits.SSP1IF = 0;
    
    if(SSP1CON2bits.ACKSTAT == 1) {
        // NACK received
        return false;
    }
    
    uint8_t count = 0;
    
    for (; count < nr_bytes; count++) {
        SSP1CON2bits.RCEN = 1;
        while(SSP1STATbits.BF==0 || PIR1bits.SSP1IF == 0);
        PIR1bits.SSP1IF = 0;
        
        data[count] = SSP1BUF;  // This clears BG
        
        SSP1CON2bits.ACKDT = (count+1 == nr_bytes ? 1 : 0);
        SSP1CON2bits.ACKEN = 1;
        while(PIR1bits.SSP1IF == 0);
        PIR1bits.SSP1IF = 0;

    }
    
    return true;
}

bool i2c_send(uint8_t address, const uint8_t* data, uint8_t nr_bytes) {
    i2c_send_start();
    bool status = i2c_send_raw(address, data, nr_bytes);
    i2c_send_stop();
    
    return status;
}

#if 0
bool i2c_recv(uint8_t address, uint8_t* data, uint8_t nr_bytes) {
    i2c_send_start();
    bool status = i2c_recv_raw(address, data, nr_bytes);
    i2c_send_stop();
    
    return status;
}
#endif

uint8_t i2c_send_recv(uint8_t address, const uint8_t* send_data, uint8_t send_nr_bytes, uint8_t* recv_data, uint8_t recv_nr_bytes) {
    bool status;
    i2c_send_start();
    status = i2c_send_raw(address, send_data, send_nr_bytes);

    if(!status) {
        i2c_send_stop();
        return 1;
    }
    
    i2c_send_restart();
    
    status = i2c_recv_raw(address, recv_data, recv_nr_bytes);

    i2c_send_stop();
    
    return status ? 0 : 2;
}


bool i2c_send_many_bytes(uint8_t address, uint8_t command, uint8_t byte, uint16_t nr_bytes) {
    i2c_send_start();
    
    PIR1bits.SSP1IF = 0;
    
    SSP1BUF = (uint8_t)(address << 1);
    
    while(SSP1STATbits.BF==1 || PIR1bits.SSP1IF == 0);

    PIR1bits.SSP1IF = 0;
    
    if(SSP1CON2bits.ACKSTAT == 1) {
        // NACK received
        i2c_send_stop();
        return false;
    }

    SSP1BUF = command;

    while(SSP1STATbits.BF==1 || PIR1bits.SSP1IF == 0);

    PIR1bits.SSP1IF = 0;
    
    if(SSP1CON2bits.ACKSTAT == 1) {
        // NACK received
        i2c_send_stop();
        return false;
    }

    uint16_t count;
    for (count = 0; count < nr_bytes; count++) {
        SSP1BUF = byte;

        while(SSP1STATbits.BF==1 || PIR1bits.SSP1IF == 0);

        PIR1bits.SSP1IF = 0;
        
        if(SSP1CON2bits.ACKSTAT == 1) {
            // NACK received
            i2c_send_stop();

            return false;
        }
    }

    i2c_send_stop();
    return true;
}