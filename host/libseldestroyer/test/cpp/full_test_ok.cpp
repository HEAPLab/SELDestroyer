#include "libseldestroyer.h"

#include "test.hpp"

#include <thread>

int main() {

    lsd_debug_mode(true);

    lsd_obj_t lobj = lsd_init();

    lsd_return_val_t val = lsd_connect(lobj, "/dev/ttyUSB0");

//    std::cout << lsd_get_error(lobj) << std::endl;

    ASSERT_EQ(val,LSD_OK);

    // V/I

    lsd_readings_t r;

    val = lsd_get_readings(lobj, &r);

    ASSERT_EQ(val,LSD_OK);

    std::cout << "V=" << r.V << "\tI=" << r.I << std::endl;

    // SEL COUNT

    unsigned int N;
    val = lsd_get_SEL_count(lobj, &N);

    ASSERT_EQ(val,LSD_OK);

    std::cout << "N SEL=" << N << std::endl;

    val = lsd_reset_SEL_count(lobj);

    ASSERT_EQ(val,LSD_OK);

    val = lsd_get_readings(lobj, &r);

    ASSERT_EQ(val,LSD_OK);

    std::cout << "V=" << r.V << "\tI=" << r.I << std::endl;


    lsd_config_t config;
    val = lsd_get_config(lobj, &config);

    ASSERT_EQ(val,LSD_OK);

    std::cout << "config.sel_curr_max_mA=" << config.sel_curr_max_A << std::endl;
    std::cout << "config.sel_hold_time_us=" << config.sel_hold_time_us << std::endl;
    std::cout << "config.avg_num=" << config.avg_num << std::endl;
    std::cout << "config.voltage_conv_time=" << config.voltage_conv_time << std::endl;
    std::cout << "config.current_conv_time=" << config.current_conv_time << std::endl;
    std::cout << "config.output_status=" << config.output_status << std::endl;

    config.sel_curr_max_A = 2;
    config.output_status = LSD_OUTPUT_STATUS_OFF;

    val = lsd_set_config(lobj, &config);

    ASSERT_EQ(val,LSD_OK);



    return 0;
}