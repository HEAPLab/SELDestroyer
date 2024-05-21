#include "destroyer.h"

#include "system.h"
#include "ina233.h"
#include "protocol.h"

destroyer_data_t destroyer_data;

static uint16_t sel_time_point;

void destroyer_save_I_lim(void) {
    EEPROM_write(EEPROM_ADDR_ULIM_HI, ((uint16_t)destroyer_data.I_limit) >> 8);
    EEPROM_write(EEPROM_ADDR_ULIM_LO, ((uint16_t)destroyer_data.I_limit) & 0xFF);
}

void destroyer_save_T_lim(void) {
    EEPROM_write(EEPROM_ADDR_ULIM_HI, (uint8_t)(destroyer_data.T_hold_us >> 8));
    EEPROM_write(EEPROM_ADDR_ULIM_LO, (uint8_t)(destroyer_data.T_hold_us & 0xFF));    
}

void destroyer_save_AVG_mode(void) {
    EEPROM_write(EEPROM_ADDR_AVG_MODE_HI, (uint8_t)(destroyer_data.avg_mode >> 8));
    EEPROM_write(EEPROM_ADDR_AVG_MODE_LO, (uint8_t)(destroyer_data.avg_mode & 0xFF));
}

void destroyer_save_OUT_stat(void) {
    EEPROM_write(EEPROM_ADDR_OUT_STAT, destroyer_data.out_status);
}


void destroyer_init(void) {
    //
    destroyer_data.sel_to_manage = false;
    destroyer_data.I_limit = (uint16_t)(EEPROM_read(EEPROM_ADDR_ULIM_HI) << 8U | EEPROM_read(EEPROM_ADDR_ULIM_LO));
    destroyer_data.T_hold_us = (uint16_t)(EEPROM_read(EEPROM_ADDR_TLIM_HI) << 8 | EEPROM_read(EEPROM_ADDR_TLIM_LO));
    destroyer_data.avg_mode = (uint16_t)(EEPROM_read(EEPROM_ADDR_AVG_MODE_HI) << 8 | EEPROM_read(EEPROM_ADDR_AVG_MODE_LO));
    destroyer_data.out_status = EEPROM_read(EEPROM_ADDR_OUT_STAT);
    destroyer_data.count = 0;
    destroyer_data.dut_is_active = destroyer_data.out_status == 0xEE ? 1 : 0;
    
    // Safe checks
    if (destroyer_data.I_limit == 0 || destroyer_data.I_limit == 0xFFFF) {
        destroyer_data.I_limit = 10000;
        destroyer_save_I_lim();
    }
    
    if (destroyer_data.T_hold_us == 0 || destroyer_data.T_hold_us == 0xFFFF) {
        destroyer_data.T_hold_us = 1000;    // 100 ms
        destroyer_save_T_lim();
    }

    if (destroyer_data.avg_mode == 0xFF) {
        destroyer_data.avg_mode = 0;    // Fastest one
        destroyer_save_AVG_mode();
    }
    
    if (destroyer_data.out_status == 0xFF) {
        destroyer_data.out_status = 0;    // Always OFF
        destroyer_save_OUT_stat();
    }
    
    destroyer_apply_config();
}

void destroyer_clear_N_SELs(void) {
    destroyer_data.count = 0;
}

void destroyer_sel_occurred(void) {
    
    if(destroyer_data.out_status != 0xAA) {
        return;
    }
 
    //ina233_res_t x = ina233_read();
    sel_time_point = system_get_time_point();
    destroyer_data.count++;
    destroyer_data.sel_to_manage = true;
    IO_LED_STATUS_SET(1);
    protocol_alert_SEL = true;
}

void destroyer_update(void) {
    static bool waiting_hold_time = false;

    if(destroyer_data.sel_to_manage) {
        if(waiting_hold_time) {
            if(system_get_time_point() - sel_time_point > destroyer_data.T_hold_tick) {
                waiting_hold_time = false;
                destroyer_data.sel_to_manage = false;
                ina233_oc_clear();
                IO_LED_STATUS_SET(0);
            }
        } else {
            waiting_hold_time = true;
        }
    }
    
    // Updated for dut_is_active
    if(destroyer_data.out_status == 0xEE) {
        destroyer_data.dut_is_active = true;
    } else if (destroyer_data.out_status == 0xAA) {
        destroyer_data.dut_is_active = !destroyer_data.sel_to_manage;
    } else {
        destroyer_data.dut_is_active = false;
    }

    // Update the actual PIN
    IO_OUTPUT_DIS_SET(!destroyer_data.dut_is_active);
}

void destroyer_apply_config(void) {

    if(destroyer_data.out_status == 0xAA) {
        ina233_oc_clear();
        ina233_oc_set_limit(destroyer_data.I_limit);
    } else {
        ina233_oc_disable();
    }

    destroyer_data.T_hold_tick = (destroyer_data.T_hold_us*100UL/128UL);

    
}