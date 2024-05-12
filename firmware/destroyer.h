#ifndef DESTROYER_H
#define	DESTROYER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct destroyer_data_s {
    int16_t I_limit; // in mA  // AAMMM
    int16_t count;
    bool dut_is_active; // Actual current value
    uint8_t avg_mode;   // See INA233 datasheet, only 3 bit significative
    uint8_t out_status; // 0x00 - Always OFF, 0xEE - always ON, 0xAA - auto
    uint32_t T_hold_us; // in 100 us // SSMMMU

} destroyer_data_t;

extern destroyer_data_t destroyer_data;

void destroyer_init(void);

void destroyer_clear_N_SELs(void);

#endif