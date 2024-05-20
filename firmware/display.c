#include "display.h"

#include <stdio.h>

#include "global.h"
#include "system.h"
#include "ina233.h"
#include "destroyer.h"

#define DISABLE_SPLASH 1

char str_buffer[10];

static void fixed2buffer(int16_t value) {

    uint8_t pos=0;
    if(value < 0) {
        str_buffer[pos++] = '-';
        value = - value;
    } else {
        str_buffer[pos++] = ' ';
    }
    
    pos += 6;
    str_buffer[pos--] = '\0';

    str_buffer[pos--] = value % 10 + '0';
    value /= 10;
    str_buffer[pos--] = value % 10 + '0';
    value /= 10;
    str_buffer[pos--] = value % 10 + '0';
    value /= 10;
    str_buffer[pos--] = '.';
    str_buffer[pos--] = value % 10 + '0';
    value /= 10;
    if(value > 0) {
        str_buffer[pos] = value % 10 + '0';
    } else {
        str_buffer[pos] = ' ';
    }
   
}

static void u162buffer_pad(uint16_t value) {
    str_buffer[5] = '\0';
    str_buffer[4] = '0' + value % 10;
    value = value / 10;
    str_buffer[3] = (value % 10) > 0 ? '0' + (value % 10) : ' ';
    value = value / 10;
    str_buffer[2] = (value % 10) > 0 ? '0' + (value % 10) : ' ';
    value = value / 10;
    str_buffer[1] = (value % 10) > 0 ? '0' + (value % 10) : ' ';
    value = value / 10;
    str_buffer[0] = (value % 10) > 0 ? '0' + (value % 10) : ' ';
}

static void u82buffer_pad(uint8_t value) {
    str_buffer[3] = '\0';
    str_buffer[2] = '0' + value % 10;
    value = value / 10;
    str_buffer[1] = (value % 10) > 0 ? '0' + (value % 10) : ' ';
    value = value / 10;
    str_buffer[0] = (value % 10) > 0 ? '0' + (value % 10) : ' ';
}

#if 0
static char * _float_to_char(float x, char *p, uint8_t nr_decimals) {
    char *s = p + CHAR_BUFF_SIZE-1; // go to end of buffer
    uint16_t decimals;  // variable to store the decimals
    int16_t units;  // variable to store the units (part to left of decimal place)
    const uint16_t n_opt[5] = {0, 10, 100, 1000, 10000};
    uint16_t n = n_opt[nr_decimals];

    if(nr_decimals > 0) {
        if (x < 0) { // take care of negative numbers
            decimals = (int16_t)(x * -n) % n; // make 1000 for 3 decimals etc.
            units = (int16_t)(-1 * x);
        } else { // positive numbers
            decimals = (int16_t)(x * n) % n;
            units = (int)x;
        }
    } else {
        units = x<0 ? (int)-x : (int)x;
    }

    for(uint8_t i=1;i<nr_decimals; i++) {
        *--s = (decimals % 10) + '0';
        decimals /= 10; // repeat for as many decimal places as you need
    }
    if(nr_decimals>0) {
        *--s = (decimals % 10) + '0';
        *--s = '.';
    }
    
    if(units ==0) {
       *--s = '0';
    }

    while (units > 0) {
        *--s = (units % 10) + '0';
        units /= 10;
    }
    if (x < 0) *--s = '-'; // unary minus sign for negative numbers
    return s;
}

#endif


static void print_str(const char* str) {
    uint8_t i=0;
    while(str[i]!='\0') {
        SSD1306_putc(str[i]);
        i++;
    }
}

void display_init_sequence(void) {

    SSD1306_set_text_wrap(false);

    SSD1306_clear();

#if DISABLE_SPLASH
    return;
#endif
    
    
    //SSD1306_fill_screen();
    SSD1306_gotoXY(9,3);
    SSD1306_putc_stretch('H', 2);
    __delay_ms(200);
    SSD1306_putc_stretch('E', 2);
    __delay_ms(200);
    SSD1306_gotoXY(9,5);
    SSD1306_putc_stretch('A', 2);
    __delay_ms(200);
    SSD1306_putc_stretch('P', 2);
    __delay_ms(200);
    SSD1306_gotoXY(13,6);
    __delay_ms(200);
    print_str("Lab");

    __delay_ms(2000);
    

    SSD1306_clear();
    

    SSD1306_gotoXY(5,1);
    print_str("SEL Destroyer");

    SSD1306_gotoXY(1,3);
    print_str("Developer: HEAP Lab");
    
    SSD1306_gotoXY(1,4);
    print_str("Politecnico di Milano");

    SSD1306_gotoXY(1,6);
    print_str("Version: 1.0");

    SSD1306_gotoXY(1,7);
    print_str("Date:    May 2024");
    
    SSD1306_gotoXY(1,8);

    for(uint8_t i=0; i<21; i++) {
        print_str("#");
        __delay_ms(200);
    }
    
    SSD1306_clear();

}

static void set_title(uint8_t title) {
    SSD1306_gotoXY(1,1);

    if (title == 0) {
        print_str("     NOT LIMITING    ");    
        SSD1306_gotoXY(1,2);
        print_str("                     ");                    
    } else if (title == 1) {
        print_str("        READY        ");            
        SSD1306_gotoXY(1,2);
        print_str("                     ");                    
    } else if (title == 2) {
        print_str("#########SEL#########");                    
        SSD1306_gotoXY(1,2);
        print_str("#########   #########");                    
    }

}

void display_update(void) {

    extern ina233_res_t main_current_readings;
    
    SSD1306_gotoXY(1,3);
    fixed2buffer(main_current_readings.V);
    
    print_str("V:   ");
    print_str(str_buffer);
    print_str("  V");
    SSD1306_gotoXY(1,4);
    fixed2buffer(main_current_readings.I);
    print_str("I:   ");
    print_str(str_buffer);
    print_str("  A");
    
    if (destroyer_data.I_limit > 0) {
       int32_t perc = main_current_readings.I;
       perc = perc * 100 / destroyer_data.I_limit;
       u82buffer_pad((uint8_t)perc);
       print_str("  ");
       print_str(str_buffer);
    print_str("%");
    }

    SSD1306_gotoXY(1,5);
    
    
    fixed2buffer(destroyer_data.I_limit);

    print_str("Ilim:");
    print_str(str_buffer);
    print_str("  A");

    SSD1306_gotoXY(1,6);

    fixed2buffer((int16_t)(((int32_t)main_current_readings.V) * main_current_readings.I / 1000));
    print_str("P:   ");
    print_str(str_buffer);
    print_str("  W");


    SSD1306_gotoXY(1,8);
    u162buffer_pad((uint16_t)destroyer_data.count);
    print_str("Count:    ");
    print_str(str_buffer);
    
    SSD1306_gotoXY(19,7);
    print_str("DUT");
    SSD1306_gotoXY(19,8);
    
    print_str(destroyer_data.dut_is_active ? " ON" : "OFF");
            
    if(destroyer_data.sel_to_manage) {
        set_title(2);
    } else {
        set_title(destroyer_data.out_status == DESTROYER_OUT_STATUS_AUTO ? 1 : 0);
    }

}