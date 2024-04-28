#include "session.hpp"

#include "global.hpp"

bool overall_debug_enabled=false;

namespace libdestroyer {

    Session::Session() {
        // TODO set lls_params
    }

    void Session::device_connect(std::string device_name) {

        this->lls_params.device_name = device_name;

        protocol = std::make_shared<Protocol>(this->lls_params);
        protocol->init();
    }


} // namespace libdestroyer