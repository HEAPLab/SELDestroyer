#include "protocol.hpp"

#include "global.hpp"

namespace libdestroyer {

    void Protocol::init() {

        debug("Protocol", "Initialization...");
        lls.connect();

    }

} // namespace libdestroyer
