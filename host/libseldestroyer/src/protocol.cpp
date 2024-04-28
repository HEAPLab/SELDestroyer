#include "protocol.hpp"

#include "global.hpp"

namespace libdestroyer {

    void Protocol::init() {

        debug("Protocol", "Initialization...");
        lls.connect();

        // Just reset everything on start
        lls.reset_trasmission();

    }

} // namespace libdestroyer
