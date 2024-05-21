#ifndef PROTOCOL_HPP_
#define PROTOCOL_HPP_

#include "low_level_serial.hpp"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <optional>

namespace libdestroyer {

typedef struct readings_s {
    float V;
    float I;
} readings_t;

class Protocol {

public:
    Protocol(const low_level_serial_params_t &lls_params) :
    lls(lls_params), event_pong_bool(false) {
    }

    void init();

    readings_t get_VI();

private:
    LowLevelSerial lls;
    std::thread thread;

    std::mutex data_mx;

    bool event_pong_bool;
    std::condition_variable event_pong_cv;

    bool event_sel_bool;
    std::condition_variable event_sel_cv;

    std::optional<float> last_V, last_I;
    std::condition_variable event_VI;


    void run();

};

} // namespace libdestroyer

#endif