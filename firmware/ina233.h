#ifndef INA233_H
#define	INA233_H

#include <stdbool.h>
#include <stdint.h>

typedef struct ina233_res_s {
    int16_t V; // * 1000 mV  // VVMMM
    int16_t I; // * 1000 mA  // AAMMM
    bool valid;
} ina233_res_t;

/**
 * Initialize the sensor submodule
 */
void ina233_init(void);


ina233_res_t ina233_read(void);

void ina233_configure_mfr(uint16_t mfr_time_config);

bool ina233_test(void);

void ina233_oc_set_limit(uint16_t current_limit_mA);
void ina233_oc_clear(void);
void ina233_oc_disable(void);

#endif