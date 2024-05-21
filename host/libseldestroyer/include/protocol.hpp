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
    unsigned int get_N_SEL();
    void get_config(float *sel_curr_max_mA, uint16_t *sel_hold_time_100us, uint16_t *config, char* output_status);

    void set_config(float sel_curr_max_mA, uint16_t sel_hold_time_100us, uint16_t config);
    void set_output(char output_status);


    void reset_N_SEL();

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

    std::optional<unsigned int> N_SEL_detected;
    std::condition_variable event_N_SEL;

    std::optional<unsigned short> config_current_limit;
    std::optional<unsigned short> config_hold_time;
    std::optional<unsigned short> config_average_mode;
    std::optional<char> config_output_status;
    std::condition_variable event_config;

    std::condition_variable event_ack;

    void run();

};

} // namespace libdestroyer

#endif