#ifndef DESTROYER_H
#define	DESTROYER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct destroyer_data_s {
    int16_t I_limit; // * 1000 mA  // AAMMM
    uint16_t count;
    bool dut_is_active;
} destroyer_data_t;

extern destroyer_data_t destroyer_data;

void destroyer_init(void);

#endif