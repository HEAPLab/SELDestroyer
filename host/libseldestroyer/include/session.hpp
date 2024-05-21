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

private:
    std::shared_ptr<Protocol> protocol;
    low_level_serial_params_t lls_params;

};

} // namespace libdestroyer

#endif