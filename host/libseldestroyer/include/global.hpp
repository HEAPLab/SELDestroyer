#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include "libseldestroyer.h"

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <mutex>

class LSDException: public std::runtime_error
{
public:
    explicit LSDException(lsd_return_val_t ec, const char *message)
        : std::runtime_error(message), error_code(ec) {}

    explicit LSDException(lsd_return_val_t ec, const std::string &message)
        : std::runtime_error("ERROR: " + message), error_code(ec) {}

    virtual ~LSDException() noexcept = default;

    lsd_return_val_t get_error_code() const noexcept {
        return this->error_code;
    }

protected:
    const lsd_return_val_t error_code;
};

inline void debug(const std::string & sec, const std::string &msg) {
    extern bool overall_debug_enabled;

    if(overall_debug_enabled) {
        static std::mutex mx;
        std::lock_guard lk(mx);
        std::cerr << '[' << std::setw(10) << sec  << "] " << msg << std::endl;
    }
}

#endif