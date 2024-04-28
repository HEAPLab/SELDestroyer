#ifndef LOW_LEVEL_SERIAL_HPP_
#define LOW_LEVEL_SERIAL_HPP_

#include <string>

namespace libdestroyer {

typedef struct low_level_serial_params_s {
    std::string device_name;
} low_level_serial_params_t;

class LowLevelSerial {

public:
    LowLevelSerial(const low_level_serial_params_t & params);

    void connect();

private:
    const low_level_serial_params_t & serial_params;

    int fd;

};

} // namespace libdestroyer

#endif