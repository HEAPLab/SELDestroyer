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


ina233_res_t main_current_readings;


void main(void) {

    system_io_init();

    IO_LED_STATUS_SET(1);
    system_primary_init();
    IO_LED_STATUS_SET(0);
        
    for (int i=0; i<3; i++) {        
        IO_LED_STATUS_SET(1);
        IO_LED_BUZZER_SET(1);
        __delay_ms(50);
        
        IO_LED_STATUS_SET(0);
        IO_LED_BUZZER_SET(0);
        __delay_ms(50);
    }
    
    i2c_init();

    ina233_init();
    
    destroyer_init();
    SSD1306_init();
    
    display_init_sequence();
        
    while(1) {
        display_update();
        main_current_readings = ina233_read();
        IO_LED_STATUS_SET(1);
        __delay_ms(50);
        IO_LED_STATUS_SET(0);
        __delay_ms(50);
        serial_send_char('X');
    }
    
    return;
}

void __attribute__((noreturn)) throw_fatal_exception (void) {
    while(1) {
        IO_LED_STATUS_SET(1);
    }
}