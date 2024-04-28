#include "low_level_serial.hpp"

#include "global.hpp"

#include <fcntl.h>      // File control definitions
#include <cerrno>
#include <cstring>
#include <termios.h>    // POSIX terminal control definitions


namespace libdestroyer {

    LowLevelSerial::LowLevelSerial(const low_level_serial_params_t &params) 
    : serial_params(params) {

    }

    void LowLevelSerial::connect() {

        // 1 - Open the device
        debug("LLS", "Opening device " + std::string(serial_params.device_name) + "...");

        this->fd = open(serial_params.device_name.c_str(), O_RDWR | O_NOCTTY );
        if(this->fd == -1) {
            throw LSDException(LSD_UNABLE_TO_CONNECT, std::strerror(errno));
        }

    }

} // namespace libdestroyer
