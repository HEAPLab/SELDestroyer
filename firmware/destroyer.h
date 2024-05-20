#ifndef DESTROYER_H
#define	DESTROYER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct destroyer_data_s {
    int16_t I_limit; // in mA  // AAMMM
    int16_t count;
    bool dut_is_active; // Actual current value
    bool sel_to_manage;
    uint8_t avg_mode;   // See INA233 datasheet, only 3 bit significative
    uint8_t out_status; // 0x00 - Always OFF, 0xEE - always ON, 0xAA - auto
    uint16_t T_hold_us; // in 100 us // SMMMU 6.5535

} destroyer_data_t;


#define DESTROYER_OUT_STATUS_OFF  0x00
#define DESTROYER_OUT_STATUS_ON   0xEE
#define DESTROYER_OUT_STATUS_AUTO 0xAA

extern destroyer_data_t destroyer_data;

void destroyer_init(void);

void destroyer_clear_N_SELs(void);

void destroyer_sel_occurred(void);

void destroyer_update(void); 

void destroyer_save_I_lim(void);
void destroyer_save_T_lim(void);
void destroyer_save_AVG_mode(void);
void destroyer_save_OUT_stat(void);
void destroyer_apply_config(void);

#endif