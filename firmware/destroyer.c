#include "destroyer.h"

#include "system.h"
#include "ina233.h"
#include "protocol.h"
#include "global.h"

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
    // Interrupt routine
    
    if(destroyer_data.out_status != 0xAA) {
        return;
    }
    
    sel_time_point = system_get_time_point();
    destroyer_data.count++;
    destroyer_data.sel_to_manage = true;
    IO_LED_STATUS_SET(1);
    protocol_alert_SEL = true;
    system_start_timer2();
}

void destroyer_check_sel_end(void) {
    // Called when timer2 expires
    
    if(system_get_time_point() - sel_time_point > (destroyer_data.T_hold_tick-1)) {
        system_stop_timer2();
        destroyer_data.hold_timer_expired = true;
        IO_LED_STATUS_SET(0);
    }
}

typedef enum state_machine_e {
    INIT,
    MAN_ALWAYS_ON,
    MAN_ALWAYS_OFF,
    AUTO_READY,
    AUTO_HOLD,
    AUTO_HOLD_CLEARED
} state_machine_t;

static state_machine_t destroyer_fsm_state = INIT;

static void destroyer_update_fsm(void) {
   
    state_machine_t next_state = destroyer_fsm_state;

    // Cases valid for any state:
    if(destroyer_data.out_status == 0xEE) {
        next_state = MAN_ALWAYS_ON;
    }
    
    if(destroyer_data.out_status == 0x00) {
        next_state = MAN_ALWAYS_OFF;
    }
    
    // State-specific cases:
    if(destroyer_fsm_state == AUTO_READY) {
        if(destroyer_data.sel_to_manage) {
            destroyer_data.sel_to_manage = false;
            next_state = AUTO_HOLD;
        }
    }
    else if(destroyer_fsm_state == AUTO_HOLD) {
        next_state = AUTO_HOLD_CLEARED;
    }
    else if(destroyer_fsm_state == AUTO_HOLD_CLEARED) {
        if(destroyer_data.hold_timer_expired) {
            destroyer_data.hold_timer_expired = false;
            next_state = AUTO_READY;
        }
    }

    else {
        if(destroyer_data.out_status == 0xAA) {
            next_state = AUTO_READY;
        }
    }
    
    destroyer_fsm_state = next_state;
}

static void destroyer_update_outputs(void) {

    destroyer_data.dut_is_active = destroyer_fsm_state == MAN_ALWAYS_OFF
                                || destroyer_fsm_state == AUTO_READY;
    
    IO_OUTPUT_DIS_SET(destroyer_fsm_state == MAN_ALWAYS_OFF);
    
    if(destroyer_fsm_state == AUTO_HOLD) {
        ina233_oc_clear();
    } 
    
}

void destroyer_update(void) {
    
    destroyer_update_fsm();
    destroyer_update_outputs();

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