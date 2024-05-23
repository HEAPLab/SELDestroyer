#include "protocol.hpp"

#include "global.hpp"

#include <chrono>

namespace libdestroyer {

    Protocol::~Protocol() {

        lls.close_fd();
        if(this->thread.joinable()) {
            debug("Protocol", "Waiting for receiver thread to close...");
            this->thread.join();
        }
        debug("Protocol", "Done, bye bye.");
    }

    void Protocol::init() {

        debug("Protocol", "Initialization...");
        lls.connect();

        // Just reset everything on start
        lls.reset_trasmission();

        this->thread = std::thread(&Protocol::run, this);

        std::this_thread::yield();  // Give chance the receiver thread to start.
                                    // (not a big deal in any case)

        debug("Protocol", "Sending ping...");

        lls.send("P");

        debug("Protocol", "Waiting for pong...");

        std::unique_lock lk(this->data_mx);

        while(event_pong_bool == false) {
            if(event_pong_cv.wait_for(lk,std::chrono::seconds(2))==std::cv_status::timeout) {
                throw LSDException(LSD_CONN_PROBLEM, std::string("Ping unanswered."));
            }
        }

        lk.unlock();

        debug("Protocol", "Pong received.");

    }

    readings_t Protocol::get_VI() {

        std::unique_lock lk(this->data_mx);

        last_V.reset();
        last_I.reset();

        lk.unlock();

        debug("Protocol", "Requesting V/I...");
        lls.send("A");

        lk.lock();

        while(!last_V || !last_I) {
            if(event_VI.wait_for(lk,std::chrono::seconds(2))==std::cv_status::timeout) {
                throw LSDException(LSD_CONN_PROBLEM, std::string("Unable to read data."));
            }
        }

        lk.unlock();

        debug("Protocol", "Got V/I pairs.");

        readings_t r;
        r.V = *last_V;
        r.I = *last_I;

        return r;
    }

    unsigned int Protocol::get_N_SEL() {

        std::unique_lock lk(this->data_mx);

        N_SEL_detected.reset();

        lk.unlock();

        debug("Protocol", "Requesting SEL count...");
        lls.send("N");

        lk.lock();

        while(!N_SEL_detected) {
            if(event_N_SEL.wait_for(lk,std::chrono::seconds(2))==std::cv_status::timeout) {
                throw LSDException(LSD_CONN_PROBLEM, std::string("Unable to read data."));
            }
        }

        lk.unlock();

        debug("Protocol", "Got SEL count.");

        return *N_SEL_detected;
    }

    void Protocol::reset_N_SEL() {
        debug("Protocol", "Sending SEL RESET...");

        lls.send("R");

        std::unique_lock lk(this->data_mx);
        if(event_ack.wait_for(lk,std::chrono::seconds(2))==std::cv_status::timeout) {
            throw LSDException(LSD_CONN_PROBLEM, std::string("Missing ACK."));
        }

        lk.unlock();

    }

    void Protocol::get_config(float *sel_curr_max_mA, uint16_t *sel_hold_time_100us, uint16_t *config, char* output_status) {

        std::unique_lock lk(this->data_mx);

        config_current_limit.reset();
        config_hold_time.reset();
        config_average_mode.reset();
        config_output_status.reset();

        lk.unlock();

        debug("Protocol", "Requesting current configuration...");
        lls.send("?");

        lk.lock();

        while(!config_current_limit || !config_hold_time || !config_average_mode || !config_output_status) {
            if(event_config.wait_for(lk,std::chrono::seconds(2))==std::cv_status::timeout) {
                throw LSDException(LSD_CONN_PROBLEM, std::string("Unable to read data."));
            }
        }

        lk.unlock();

        debug("Protocol", "Got configuration.");

        *sel_curr_max_mA = *config_current_limit / 1000.;
        *sel_hold_time_100us = *config_hold_time;
        *config = *config_average_mode;
        *output_status = *config_output_status;
    }

    void Protocol::set_config(float sel_curr_max_mA, uint16_t sel_hold_time_100us, uint16_t config) {

        debug("Protocol", "Sending new configuration...");

        lls.send("CL," + std::to_string(static_cast<unsigned int>(sel_curr_max_mA*1000)));

        std::unique_lock lk(this->data_mx);
        if(event_ack.wait_for(lk,std::chrono::seconds(2))==std::cv_status::timeout) {
            throw LSDException(LSD_CONN_PROBLEM, std::string("Missing ACK."));
        }
        lk.unlock();

        lls.send("CH," + std::to_string(sel_hold_time_100us));
        lk.lock();
        if(event_ack.wait_for(lk,std::chrono::seconds(2))==std::cv_status::timeout) {
            throw LSDException(LSD_CONN_PROBLEM, std::string("Missing ACK."));
        }
        lk.unlock();

        lls.send("CM," + std::to_string(config));
        lk.lock();
        if(event_ack.wait_for(lk,std::chrono::seconds(2))==std::cv_status::timeout) {
            throw LSDException(LSD_CONN_PROBLEM, std::string("Missing ACK."));
        }
        lk.unlock();

    }
    void Protocol::set_output(char output_status) {
        debug("Protocol", "Sending new output state...");

        lls.send(std::string("CO,") + output_status);

        std::unique_lock lk(this->data_mx);
        if(event_ack.wait_for(lk,std::chrono::seconds(2))==std::cv_status::timeout) {
            throw LSDException(LSD_CONN_PROBLEM, std::string("Missing ACK."));
        }
        lk.unlock();
    }

    void Protocol::run() {
        debug("Protocol", "Starting receiver thread...");

        try {

            while(true) {
                std::string line = lls.recv_line();

                if(line.size() == 0) return;

                if(line == "$P") {
                    std::lock_guard lk(this->data_mx);
                    event_pong_bool = true;
                    event_pong_cv.notify_all();
                } else if(line == "$S") {
                    std::lock_guard lk(this->data_mx);
                    event_sel_bool = true;
                    event_sel_cv.notify_all();
                } else if(line.rfind("$V,", 0) == 0) {
                    std::lock_guard lk(this->data_mx);
                    this->last_V = std::stoi(line.substr(3))/1000.;
                    event_VI.notify_all();
                } else if(line.rfind("$I,", 0) == 0) {
                    std::lock_guard lk(this->data_mx);
                    this->last_I = std::stoi(line.substr(3))/1000.;
                    event_VI.notify_all();
                } else if(line.rfind("$N,", 0) == 0) {
                    std::lock_guard lk(this->data_mx);
                    this->N_SEL_detected = std::stoi(line.substr(3));
                    event_N_SEL.notify_all();
                } else if(line.rfind("$*", 0) == 0) {
                    std::lock_guard lk(this->data_mx);
                    event_ack.notify_all();
                } else if(line.rfind("$CL,", 0) == 0) {
                    std::lock_guard lk(this->data_mx);
                    this->config_current_limit = std::stoi(line.substr(4));
                    event_config.notify_all();
                } else if(line.rfind("$CH,", 0) == 0) {
                    std::lock_guard lk(this->data_mx);
                    this->config_hold_time = std::stoi(line.substr(4));
                    event_config.notify_all();
                } else if(line.rfind("$CM,", 0) == 0) {
                    std::lock_guard lk(this->data_mx);
                    this->config_average_mode = std::stoi(line.substr(4));
                    event_config.notify_all();
                } else if(line.rfind("$CO,", 0) == 0 && line.length() > 4) {
                    std::lock_guard lk(this->data_mx);
                    this->config_output_status = line[4];
                    event_config.notify_all();
                } else if(line == "$E") {
                    // Protocol error occurred on the device side, just log and ignore it.
                    debug("Protocol", "Received error message, ignoring...");
                }
                else {
                    throw LSDException(LSD_CANT_UNDERSTAND, std::string("Unkonwn value: ") + line);
                }
            }
        }
        catch(const LSDException &lsde) {
            debug("LSDException: ", lsde.what());
        }
    }

} // namespace libdestroyer
