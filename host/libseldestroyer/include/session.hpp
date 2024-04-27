#ifndef SESSION_H_
#define SESSION_H_

#include <string>

namespace libdestroyer {



class Session {

public:

    // We keep this variable public for an easy access from the C
    // code. It contains only the last error that occurred and needs
    // to be passed to C interface.
    std::string last_error;

private:

};

} // namespace libdestroyer

#endif