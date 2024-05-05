/*
 * File:   main.c
 * Author: federico
 *
 * Created on April 7, 2024, 1:05 AM
 */


#include "config.h"
#include "system.h"
#include "i2c.h"
#include "ssd1306.h"
#include "ina233.h"


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
    
//    ina233_res_t x = ina233_read();
    
    SSD1306_init();
    bool cx = SSD1306_clear();
    //SSD1306_fill_screen();
//    SSD1306_gotoXY(1,1);
//    SSD1306_putc_custom("Ciao");
    
    while(cx) {
        
    }
    
    return;
}

void __attribute__((noreturn)) throw_fatal_exception (fatal_exception_t error_code) {

    uint8_t code = (uint8_t) error_code;
    
    bool state = false; // Pause or blink?
    int8_t n = 0;
    uint8_t bit_pos = 0;
    const int8_t long_period = 10;
    const int8_t short_period = 1;
    const int8_t pause_period = 5;

    IO_LED_STATUS_SET(0);

    while(1) {
        IO_LED_STATUS_SET(1);
        __delay_ms(100);
        IO_LED_STATUS_SET(0);
        __delay_ms(100);            

        n++;
        if(state) {
            if(n > (GET_BIT(code, bit_pos) == 0 ? short_period : long_period)) {
                bit_pos = (bit_pos + 1) % 8;
                if(bit_pos == 0) {
                    n=-5*pause_period;
                } else {
                    n=0;
                }
                state = false;
            }
        } else {
            if(n > pause_period) {
                n=0;
                state = true;
            }
        }
        IO_LED_STATUS_SET(state);
    }
}