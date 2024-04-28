/*
 * File:   main.c
 * Author: federico
 *
 * Created on April 7, 2024, 1:05 AM
 */


#include "config.h"
#include "system.h"

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
    
   
    while(1) {
        
    }
    
    return;
}
