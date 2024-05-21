#include "protocol.hpp"

#include "global.hpp"

#include <chrono>

namespace libdestroyer {

    void Protocol::init() {

        debug("Protocol", "Initialization...");
        lls.connect();

        // Just reset everything on start
        lls.reset_trasmission();

        this->thread = std::thread(&Protocol::run, this);

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

        debug("Protocol", "Polling data request...");
        lls.send("A");

        lk.lock();

        while(!last_V || !last_I) {
            if(event_VI.wait_for(lk,std::chrono::seconds(2))==std::cv_status::timeout) {
                throw LSDException(LSD_CONN_PROBLEM, std::string("Unable to read data."));
            }
        }

        readings_t r;
        r.V = *last_V;
        r.I = *last_I;

        return r;
    }

    void Protocol::run() {
        while(true) {
            std::string line = lls.recv_line();

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
            }
            else {
                throw LSDException(LSD_CANT_UNDERSTAND, std::string("Unkonwn value: ") + line);
            }

        }
    }

} // namespace libdestroyer
