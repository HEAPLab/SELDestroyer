#include "protocol.h"


#include "destroyer.h"
#include "system.h"
#include "ina233.h"

#include <stdint.h>

#define SPECIAL_CHAR '$'

typedef enum protocol_state_e {
    WAIT_BEGIN,
    WAIT_DATA,
    PROCESS_DATA,
    ERROR_STATE
} protocol_state_t;

static protocol_state_t prot_state=WAIT_BEGIN;

static bool escaping_char = false;
static char buffer_msg[10];
static uint8_t buffer_msg_idx;

bool protocol_alert_SEL=false;

void protocol_digest_char(char c) {
    
    if(c == '\\') {
        if(escaping_char) {
            escaping_char = false;
        } else {
            escaping_char = true;
            return;
        }
    }
    
    switch(prot_state) {
        case WAIT_BEGIN:
            if(c != SPECIAL_CHAR || escaping_char) {
                return; // Discard value
            } else {
                prot_state = WAIT_DATA;
                buffer_msg_idx = 0;
            }
        break;
        case WAIT_DATA:
            if(c == '\n' && (!escaping_char)) {
                buffer_msg[buffer_msg_idx] = '\n';
                prot_state = PROCESS_DATA;
                return;
            } else if(buffer_msg_idx < 10) {
                buffer_msg[buffer_msg_idx++] = c;
            } else {
                // Buffer is full!
                prot_state = ERROR_STATE;    // Error
                return;
            }
        break;
        case PROCESS_DATA:
        case ERROR_STATE:
            // Just wait previosu answer/errro to send
        break;
    }
}

static char * i16_to_str(int16_t val, char *buf){

    uint8_t i = 7;
    buf[i--] = '\0';
    bool is_positive = true;

    if(val < 0) {
        val = - val;
        is_positive = false;
    }
    
    do {
        buf[i] = '0' + (val % 10);
        i--;
        val = val / 10;
    } while (val > 0);

    buf[i] = is_positive ? '+' : '-';
    
    return buf + i;
}

#define BUFF_SIZE 16

static void protocol_send_VIN(void) {
    extern ina233_res_t main_current_readings;

    char buffer[BUFF_SIZE];
    char *my_point;
    
    my_point = i16_to_str(main_current_readings.V, buffer);
    *(--my_point) = ',';
    *(--my_point) = 'V';
    serial_send_cmd(my_point);
    
    my_point = i16_to_str(main_current_readings.I, buffer);
    *(--my_point) = ',';
    *(--my_point) = 'I';
    serial_send_cmd(my_point);
    
}

static void protocol_send_N_SELs(void) {
    char buffer[BUFF_SIZE];
    char *my_point;
    
    my_point = i16_to_str(destroyer_data.count, buffer);
    *(--my_point) = ',';
    *(--my_point) = 'N';
    serial_send_cmd(my_point);

}

static void protocol_send_SEL(void) {

    serial_send_cmd("S");

}

static char * u16_to_str(uint16_t val, char *buf){

    uint8_t i = 10;
    buf[i--] = '\0';
    
    do {
        buf[i] = '0' + (val % 10);
        i--;
        val = val / 10;
    } while (val > 0);
    
    return buf + i + 1;
}


static uint16_t str_to_u16(const char * s){
    uint16_t res = 0;
    while(*s != '\n') {
        res = (res * 10) + (*s - '0');
        s++;
    }
    return res;
}



static void destroyer_send_config(void) {
    char buffer[BUFF_SIZE];

    char *my_point = u16_to_str((uint16_t)destroyer_data.I_limit, buffer);
    *(--my_point) = ',';
    *(--my_point) = 'L';
    *(--my_point) = 'C';
    serial_send_cmd(my_point);
    
    my_point = u16_to_str((uint16_t)destroyer_data.T_hold_us, buffer);
    *(--my_point) = ',';
    *(--my_point) = 'H';
    *(--my_point) = 'C';
    serial_send_cmd(my_point);

    my_point = u16_to_str((uint16_t)destroyer_data.avg_mode, buffer);
    *(--my_point) = ',';
    *(--my_point) = 'M';
    *(--my_point) = 'C';
    serial_send_cmd(my_point);
    
    buffer[0] = 'C';
    buffer[1] = 'O';
    buffer[2] = ',';
    buffer[3] = destroyer_data.out_status == 0x00 ? '0' : (destroyer_data.out_status == 0xEE ? '1' : 'A');
    buffer[4] = '\0';
    serial_send_cmd(buffer);
    
}

static void protocol_send_done(void) {
    serial_send_cmd("*");
}

static void protocol_update_config(void) {
    if(buffer_msg_idx <= 3) {
        prot_state = ERROR_STATE;
        return; // Empty command?
    }
    
    const char *start_point = buffer_msg + 3;
    
    switch(buffer_msg[1]) {
        case 'L':
        {
            uint16_t value = str_to_u16(start_point);
            if(value > 10000) {
                prot_state = ERROR_STATE;
                return;
            }
            destroyer_data.I_limit = str_to_u16(start_point);
            destroyer_apply_config();
            destroyer_save_I_lim();
            protocol_send_done();
        }
        break;
        case 'H':
            destroyer_data.T_hold_us = str_to_u16(start_point);
            destroyer_save_T_lim();
            protocol_send_done();
        break;
        case 'M':
        {
            uint16_t value = str_to_u16(start_point);
            if(value > 0b111111111) {
                prot_state = ERROR_STATE;
                return;
            }
            destroyer_data.avg_mode = value;
            destroyer_apply_config();
            destroyer_save_AVG_mode();
            protocol_send_done();
        }
        break;
        case 'O':
            if(*start_point == '0') {
                destroyer_data.out_status = 0x00;
            } else if(*start_point == '1') {
                destroyer_data.out_status = 0xEE;                
            } else if(*start_point == 'A') {
                destroyer_data.out_status = 0xAA;
            } else {
                prot_state = ERROR_STATE;
                return;
            }
            destroyer_apply_config();
            destroyer_save_OUT_stat();
            protocol_send_done();
        break;
        default:
            prot_state = ERROR_STATE;
        break;
    }
}

void protocol_update(void) {

    if(protocol_alert_SEL) {
        // SEL alert needs to be sent, this has the precedence
        // to any other message
        // TODO: Optimize: use a non-blocking function here to send the char
        // (if we lost it who cares, but we need to be fast if the flag is true)
        protocol_send_SEL();
        protocol_alert_SEL = false;
        return;
    }
    
    if(prot_state == PROCESS_DATA) {
        if(buffer_msg_idx == 0) {
            prot_state = ERROR_STATE;
            return; // Empty command?
        }
        
        switch(buffer_msg[0]) {
            case 'P':
                serial_send_cmd("P");
            break;
            case 'A':
                protocol_send_VIN();
            break;
            case 'N':
                protocol_send_N_SELs();
            break;
            case 'R':
                destroyer_clear_N_SELs();
                protocol_send_done();
            break;
            case '?':
                destroyer_send_config();
            break;
            case 'C':
                protocol_update_config();
            break;
            default:
                prot_state = ERROR_STATE;
                return; // Empty command?
            break;
        }

        prot_state = WAIT_BEGIN;
    }
    else if (prot_state == ERROR_STATE) {
        serial_send_cmd("E");
        prot_state = WAIT_BEGIN;
    }
    
}


