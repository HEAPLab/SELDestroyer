#ifndef LOW_LEVEL_SERIAL_HPP_
#define LOW_LEVEL_SERIAL_HPP_

#include <string>
#include <vector>

namespace libdestroyer {

typedef struct low_level_serial_params_s {
    std::string device_name;
    unsigned int baudrate;
    unsigned int number_bits;
    bool parity_on;
    bool parity_is_odd;
    bool two_stop_bits;
} low_level_serial_params_t;

class LowLevelSerial {

public:
    LowLevelSerial(const low_level_serial_params_t & params);

    virtual ~LowLevelSerial();

    void connect();

    void reset_trasmission();

    void send(const std::string &data);

    unsigned char recv();

    std::string recv_line();


private:
    const low_level_serial_params_t & serial_params;

    int fd;

};

} // namespace libdestroyer

#endif