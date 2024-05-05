#ifndef INA233_H
#define	INA233_H

#include <stdbool.h>

typedef struct ina233_res_s {
    float V;
    float I;
    bool valid;
} ina233_res_t;

/**
 * Initialize the sensor submodule
 */
void ina233_init(void);


ina233_res_t ina233_read(void);

#endif