/*
 * File:   main.c
 * Author: federico
 *
 * Created on April 7, 2024, 1:05 AM
 */


#include "config.h"
#include "system.h"
#include "destroyer.h"
#include "display.h"
#include "i2c.h"
#include "ssd1306.h"
#include "ina233.h"
#include "protocol.h"

ina233_res_t main_current_readings;


void main(void) {

    system_io_init();
    
    IO_OUTPUT_DIS_SET(1);   // Disable output for safety
    IO_BUZZER_SET(0);       // Disable buzzer (we have an hardware problem with it)

    IO_LED_STATUS_SET(1);
    system_primary_init();
    IO_LED_STATUS_SET(0);

    for (int i=0; i<3; i++) {        
        IO_LED_STATUS_SET(1);
        __delay_ms(50);
        
        IO_LED_STATUS_SET(0);
        __delay_ms(50);
    }
    
    i2c_init();

    ina233_init();
    
    if(!ina233_test()) {
        throw_fatal_exception();
    }
        
    destroyer_init();
    SSD1306_init();
    
    display_init_sequence();
        
    while(1) {

        if(destroyer_data.out_status != 0xAA || destroyer_data.T_hold_us >= 20) {
            main_current_readings = ina233_read();
        }
        
        destroyer_update();
        display_update();
        protocol_update();
        
        CLRWDT();
    }
    
    return;
}

void __attribute__((noreturn)) throw_fatal_exception (void) {
    while(1) {
        IO_LED_STATUS_SET(1);
    }
}