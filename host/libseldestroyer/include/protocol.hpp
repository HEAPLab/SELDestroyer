#ifndef PROTOCOL_HPP_
#define PROTOCOL_HPP_

#include "low_level_serial.hpp"

namespace libdestroyer {

class Protocol {

public:
    Protocol(const low_level_serial_params_t &lls_params) :
    lls(lls_params) {
    }

    void init();

private:
    LowLevelSerial lls;
};

} // namespace libdestroyer

#endif