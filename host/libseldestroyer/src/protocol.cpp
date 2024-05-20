#include "protocol.hpp"

#include "global.hpp"

namespace libdestroyer {

    void Protocol::init() {

        debug("Protocol", "Initialization...");
        lls.connect();

        // Just reset everything on start
        lls.reset_trasmission();

        debug("Protocol", "Sending ping...");

        lls.send("P");

        debug("Protocol", "Waiting for pong...");

        std::string line = lls.recv_line();
        if(line != "$P") {
            throw LSDException(LSD_CANT_UNDERSTAND, "Received invalid command: "+line);
        }

        debug("Protocol", "Pong received.");

    }

} // namespace libdestroyer
