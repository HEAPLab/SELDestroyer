/*
 * Copyright 2024 Politecnico di Milano
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef LIBSELDESTROYER_H_
#define LIBSELDESTROYER_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum lsd_return_val_e {
    LSD_OK = 0,
    LSD_GENERIC_ERROR = 1,
    LSD_UNABLE_TO_CONNECT = 2,
    LSD_INVALID_ARGUMENT = 3,
    LSD_CONN_PROBLEM = 4,
    LSD_CANT_UNDERSTAND = 5
} lsd_return_val_t;

typedef enum lsd_avg_e {
    LSD_AVG_1    = 0,
    LSD_AVG_4    = 1,
    LSD_AVG_16   = 2,
    LSD_AVG_64   = 3,
    LSD_AVG_128  = 4,
    LSD_AVG_256  = 5,
    LSD_AVG_512  = 6,
    LSD_AVG_1024 = 7
} lsd_avg_t;

typedef enum lsd_conv_e {
    LSD_CT_140_US  = 0,
    LSD_CT_204_US  = 1,
    LSD_CT_332_US  = 2,
    LSD_CT_588_US  = 3,
    LSD_CT_1100_US = 4,
    LSD_CT_2116_US = 5,
    LSD_CT_4156_US = 6,
    LSD_CT_8244_US = 7
} lsd_conv_t;

typedef enum lsd_output_status_e {
    LSD_OUTPUT_STATUS_OFF   = 0,
    LSD_OUTPUT_STATUS_ON    = 1,
    LSD_OUTPUT_STATUS_AUTO  = 2,
    LSD_OUTPUT_STATUS_INVALID = 3
} lsd_output_status_t;

typedef struct lsd_config_s {
    float sel_curr_max_A;
    uint32_t sel_hold_time_us;  // Precision 128 us

    lsd_avg_t avg_num;
    lsd_conv_t voltage_conv_time;
    lsd_conv_t current_conv_time;
    lsd_output_status_t output_status;

} lsd_config_t;

typedef struct lsd_readings_s {
    float V;
    float I;
} lsd_readings_t;

typedef void * lsd_obj_t;

lsd_obj_t lsd_init(void);

void lsd_debug_mode(bool is_enabled);

void lsd_close(lsd_obj_t session);

const char *lsd_get_error(lsd_obj_t session);

lsd_return_val_t lsd_connect(lsd_obj_t session, const char* device_name);

lsd_return_val_t lsd_set_config(lsd_obj_t session, const lsd_config_t *config);

lsd_return_val_t lsd_get_config(lsd_obj_t session, lsd_config_t *config);

lsd_return_val_t lsd_get_readings(lsd_obj_t session, lsd_readings_t *out);
lsd_return_val_t lsd_get_SEL_count(lsd_obj_t session, unsigned int *out);
lsd_return_val_t lsd_reset_SEL_count(lsd_obj_t session);
lsd_return_val_t lsd_set_output(lsd_obj_t session, lsd_output_status_t output_state);

const char* lsd_get_version(void);

void lsd_register_sel_callback(lsd_obj_t session, void (*)(void));

void lsd_wait(lsd_obj_t session);

#ifdef __cplusplus
}
#endif

#endif