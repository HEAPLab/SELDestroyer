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

#include "libseldestroyer.h"
#include "session.hpp"
#include "global.hpp"

#include "version.h"

#include <string>

#define SPTR_CONV(x) ((libdestroyer::Session*)x)

#define EXCEPTION_PROTECT_START try {


#define EXCEPTION_PROTECT_END } catch(const LSDException &lsde) { \
                        SPTR_CONV(session)->last_error = lsde.what();\
                        return lsde.get_error_code();\
                    } catch(const std::exception &e) {\
                        SPTR_CONV(session)->last_error = std::string("Generic error: ") + e.what();\
                        return LSD_GENERIC_ERROR;\
                    } catch(...) {\
                        SPTR_CONV(session)->last_error = "Unknown error.";\
                        return LSD_GENERIC_ERROR;\
                    }

const char* lsd_get_version(void) {
    return LIBSELDESTROYER_VERSION;
}

lsd_obj_t lsd_init(void) {
    return new (std::nothrow) libdestroyer::Session;
}

void lsd_close(lsd_obj_t session) {
    if(session != nullptr) {
        delete SPTR_CONV(session);
    }
}

void lsd_debug_mode(bool is_enabled) {
    extern bool overall_debug_enabled;
    overall_debug_enabled = is_enabled;
}


lsd_return_val_t lsd_connect(lsd_obj_t session, const char* device_name) {
    EXCEPTION_PROTECT_START
    SPTR_CONV(session)->device_connect(device_name);
    EXCEPTION_PROTECT_END

    return LSD_OK;
}

const char *lsd_get_error(lsd_obj_t session) {
    return SPTR_CONV(session)->last_error.c_str();
}

lsd_return_val_t lsd_get_readings(lsd_obj_t session, lsd_readings_t *out) {

    libdestroyer::readings_t r;

    EXCEPTION_PROTECT_START
    r = SPTR_CONV(session)->device_get_readings();
    EXCEPTION_PROTECT_END

    out->V = r.V;
    out->I = r.I;

    return LSD_OK;
}


lsd_return_val_t lsd_get_SEL_count(lsd_obj_t session, unsigned int *out) {

    unsigned int r;

    EXCEPTION_PROTECT_START
    r = SPTR_CONV(session)->device_get_SEL_count();
    EXCEPTION_PROTECT_END

    *out = r;

    return LSD_OK;
}

lsd_return_val_t lsd_reset_SEL_count(lsd_obj_t session) {

    EXCEPTION_PROTECT_START
    SPTR_CONV(session)->device_reset_SEL_count();
    EXCEPTION_PROTECT_END

    return LSD_OK;
}


lsd_return_val_t lsd_get_config(lsd_obj_t session, lsd_config_t *config) {

    uint16_t sel_hold_time_100us;
    uint16_t adc_config;
    char output_status;

    EXCEPTION_PROTECT_START

    SPTR_CONV(session)->get_config(&(config->sel_curr_max_A), &sel_hold_time_100us, &adc_config, &output_status);

    EXCEPTION_PROTECT_END

    config->sel_hold_time_us = sel_hold_time_100us * 100;

    config->current_conv_time = static_cast<lsd_conv_t>(adc_config & 0b111);
    config->voltage_conv_time = static_cast<lsd_conv_t>((adc_config & 0b111000) >> 3);
    config->avg_num = static_cast<lsd_avg_t>((adc_config & 0b111000000) >> 6);

    switch(output_status) {
    case '0':
        config->output_status = LSD_OUTPUT_STATUS_OFF;
    break;
    case '1':
        config->output_status = LSD_OUTPUT_STATUS_ON;
    break;
    case 'A':
        config->output_status = LSD_OUTPUT_STATUS_AUTO;
    break;
    default:
        config->output_status = LSD_OUTPUT_STATUS_INVALID;
    break;
    }


    return LSD_OK;
}

static char outputstatus2char(lsd_output_status_t output_state) {
    switch(output_state) {
    case LSD_OUTPUT_STATUS_OFF:
    default:
        return '0';
    case LSD_OUTPUT_STATUS_ON:
        return '1';
    case LSD_OUTPUT_STATUS_AUTO:
        return 'A';
    }
}

lsd_return_val_t lsd_set_config(lsd_obj_t session, const lsd_config_t *config) {

    uint16_t sel_hold_time_100us;
    uint16_t adc_config;
    char output_status;

    sel_hold_time_100us = config->sel_hold_time_us / 100;
    adc_config = (config->current_conv_time & 0b111) | ((config->voltage_conv_time & 0b111) << 3) | ((config->avg_num & 0b111) << 6);

    output_status = outputstatus2char(config->output_status);

    EXCEPTION_PROTECT_START

    SPTR_CONV(session)->set_config(config->sel_curr_max_A, sel_hold_time_100us, adc_config, output_status);

    EXCEPTION_PROTECT_END

    return LSD_OK;
}

lsd_return_val_t lsd_set_output(lsd_obj_t session, lsd_output_status_t output_state) {
    char char_os = outputstatus2char(output_state);

    EXCEPTION_PROTECT_START

    SPTR_CONV(session)->set_output(char_os);

    EXCEPTION_PROTECT_END

    return LSD_OK;

}