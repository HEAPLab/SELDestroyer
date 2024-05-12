#include "destroyer.h"

#include "system.h"

destroyer_data_t destroyer_data;


void destroyer_save_I_lim(void) {
    EEPROM_write(EEPROM_ADDR_ULIM_HI, ((uint16_t)destroyer_data.I_limit) >> 8);
    EEPROM_write(EEPROM_ADDR_ULIM_LO, ((uint16_t)destroyer_data.I_limit) & 0xFF);
}

void destroyer_save_T_lim(void) {
    EEPROM_write(EEPROM_ADDR_ULIM_HI, (uint8_t)(destroyer_data.T_hold_us >> 8));
    EEPROM_write(EEPROM_ADDR_ULIM_LO, (uint8_t)(destroyer_data.T_hold_us & 0xFF));
}

void destroyer_save_AVG_mode(void) {
    EEPROM_write(EEPROM_ADDR_AVG_MODE, destroyer_data.avg_mode);
}

void destroyer_save_OUT_stat(void) {
    EEPROM_write(EEPROM_ADDR_OUT_STAT, destroyer_data.out_status);
}


void destroyer_init(void) {
    //
    destroyer_data.I_limit = EEPROM_read(EEPROM_ADDR_ULIM_HI) << 8 | EEPROM_read(EEPROM_ADDR_ULIM_LO);
    destroyer_data.T_hold_us = ((uint32_t)(EEPROM_read(EEPROM_ADDR_TLIM_HI)) << 16U) | (uint32_t)(EEPROM_read(EEPROM_ADDR_TLIM_MI) << 8U) || (uint32_t)(EEPROM_read(EEPROM_ADDR_TLIM_LO));
    destroyer_data.avg_mode = EEPROM_read(EEPROM_ADDR_AVG_MODE);
    destroyer_data.out_status = EEPROM_read(EEPROM_ADDR_OUT_STAT);
    destroyer_data.count = 0;
    destroyer_data.dut_is_active = destroyer_data.out_status == 0xEE ? 1 : 0;
    
    // Safe checks
    if (destroyer_data.I_limit == 0 || destroyer_data.I_limit == 0xFFFF) {
        destroyer_data.I_limit = 10000;
        destroyer_save_I_lim();
    }
    
    if (destroyer_data.T_hold_us == 0 || destroyer_data.T_hold_us == 0xFFFFFF) {
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
}

void destroyer_clear_N_SELs(void) {
    destroyer_data.count = 0;
}