#ifndef SESSION_HPP_
#define SESSION_HPP_

#include "protocol.hpp"

#include <string>
#include <memory>

namespace libdestroyer {



class Session {

public:

    // We keep this variable public for an easy access from the C
    // code. It contains only the last error that occurred and needs
    // to be passed to C interface.
    std::string last_error;

    Session();

    void device_connect(std::string device_name);

    readings_t device_get_readings() {
        return this->protocol->get_VI();
    }

    unsigned int device_get_SEL_count() {
        return this->protocol->get_N_SEL();
    }

    void device_reset_SEL_count() {
        this->protocol->reset_N_SEL();
    }

    void get_config(float *sel_curr_max_mA, uint16_t *sel_hold_time_100us, uint16_t *config, char *output_status) {
        this->protocol->get_config(sel_curr_max_mA, sel_hold_time_100us, config, output_status);
    }

    void set_config(float sel_curr_max_mA, uint16_t sel_hold_time_100us, uint16_t config, char output_status) {
        this->protocol->set_config(sel_curr_max_mA, sel_hold_time_100us, config);
        this->protocol->set_output(output_status);
    }

    void set_output(char output_status) {
        this->protocol->set_output(output_status);
    }

private:
    std::shared_ptr<Protocol> protocol;
    low_level_serial_params_t lls_params;

};

} // namespace libdestroyer

#endif