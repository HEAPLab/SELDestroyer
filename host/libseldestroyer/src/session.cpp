#include "session.hpp"

#include "global.hpp"

bool overall_debug_enabled=false;

namespace libdestroyer {

    Session::Session() {
        // Set default parameters
        this->lls_params.device_name = "/dev/ttyUSB0";
        this->lls_params.baudrate = 57600;
        this->lls_params.number_bits = 8;
        this->lls_params.parity_on = false;
        this->lls_params.parity_is_odd = false;
        this->lls_params.two_stop_bits = false;
    }

    void Session::device_connect(std::string device_name) {

        this->lls_params.device_name = device_name;

        protocol = std::make_shared<Protocol>(this->lls_params);
        protocol->init();
    }


} // namespace libdestroyer