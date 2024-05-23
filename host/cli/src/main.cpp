#include "libseldestroyer.h"

#include "options.hpp"

#include <memory>
#include <iostream>

static lsd_obj_t lobj;

static void ret_checker(lsd_return_val_t ret) {
    if(ret != LSD_OK) {
        std::cerr << "Error! " << std::endl
                  << "Code:    " << (int) ret << std::endl
                  << "Details: " << lsd_get_error(lobj) << std::endl;

        throw std::exception();
    }
}

static void perform_connect(const std::string &device_name) {
    lsd_return_val_t ret = lsd_connect(lobj, device_name.c_str());
    ret_checker(ret);
}

static void perform_readVI() {
    lsd_readings_t out;
    lsd_return_val_t ret = lsd_get_readings(lobj, &out);
    ret_checker(ret);
    std::cout << "Voltage (V): " << out.V << std::endl;
    std::cout << "Current (A): " << out.I << std::endl;
    std::cout << "Power   (W): " << out.V * out.I << std::endl;
}

static void perform_getSEL() {
    unsigned int out;
    lsd_return_val_t ret = lsd_get_SEL_count(lobj, &out);
    ret_checker(ret);
    std::cout << "SEL counter: " << out << std::endl;
}

static void perform_resetSEL() {
    unsigned int out;
    lsd_return_val_t ret = lsd_reset_SEL_count(lobj);
    ret_checker(ret);
    std::cout << "RESET SEL OK." << std::endl;
}

static void perform_getCONFIG() {
    lsd_config_t out;
    lsd_return_val_t ret = lsd_get_config(lobj, &out);
    ret_checker(ret);
    std::cout << "Max current              (A): " << out.sel_curr_max_A << std::endl;
    std::cout << "Hold time               (us): " << out.sel_hold_time_us << std::endl;
    std::cout << "Number of averages:           " << out.avg_num << std::endl;
    std::cout << "Voltage conversion time (us): " << out.voltage_conv_time << std::endl;
    std::cout << "Current conversion time (us): " << out.current_conv_time << std::endl;
    std::cout << "Output state:                 ";
    switch (out.output_status) {
        case LSD_OUTPUT_STATUS_OFF: std::cout  << "OFF"; break;
        case LSD_OUTPUT_STATUS_ON: std::cout   << "ON"; break;
        case LSD_OUTPUT_STATUS_AUTO: std::cout << "AUTO"; break;
        default: std::cout << "UNKONWN"; break;
    }
    std::cout << std::endl;

}

void perform_set_output(lsd_output_status_t output_state) {

    lsd_return_val_t ret = lsd_set_output(lobj, output_state);
    ret_checker(ret);

    std::cout << "Output setting updated." << std::endl;
}

void perform_set_config(std::optional<float> arg_max_I, std::optional<unsigned int> arg_hold_time, std::optional<lsd_avg_t> arg_avg_num, std::optional<lsd_conv_t> arg_voltage_conv_time, std::optional<lsd_conv_t> arg_current_conv_time) {

    lsd_config_t out;
    lsd_return_val_t ret = lsd_get_config(lobj, &out);
    ret_checker(ret);

    if(arg_max_I) out.sel_curr_max_A = *arg_max_I;
    if(arg_hold_time) out.sel_hold_time_us = *arg_hold_time;
    if(arg_avg_num) out.avg_num = *arg_avg_num;
    if(arg_voltage_conv_time) out.voltage_conv_time = *arg_voltage_conv_time;
    if(arg_current_conv_time) out.current_conv_time = *arg_current_conv_time;

    ret = lsd_set_config(lobj, &out);
    ret_checker(ret);

    std::cout << "Config setting updated." << std::endl;
}

int main(int argc, const char* argv[]) {

    try {
        seldestroyer::Options opt(argc, argv);

        if(opt.arg_debug) {
            lsd_debug_mode(true);
        }

        lobj = lsd_init();

        perform_connect(opt.arg_device);

        if(opt.arg_readVI)    perform_readVI();
        if(opt.arg_getSEL)    perform_getSEL();
        if(opt.arg_resetSEL)  perform_resetSEL();
        if(opt.arg_getCONFIG) perform_getCONFIG();
        if(opt.arg_output)    perform_set_output(*opt.arg_output);

        if (opt.arg_max_I || opt.arg_hold_time || opt.arg_avg_num 
        || opt.arg_voltage_conv_time || opt.arg_current_conv_time
        ) {
            perform_set_config(opt.arg_max_I, opt.arg_hold_time, opt.arg_avg_num, opt.arg_voltage_conv_time, opt.arg_current_conv_time);
        }

        lsd_close(lobj);
        
    } catch(...) {
        if(lobj) {
            lsd_close(lobj);
        }
        return 1;
    }
    return 0;
}
