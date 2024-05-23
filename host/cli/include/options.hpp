#ifndef OPTIONS_HPP_
#define OPTIONS_HPP_

#include "libseldestroyer.h"

#include <optional>
#include <string>

namespace seldestroyer {
    class Options {
    public:
        Options(int argc, const char* argv[]);

        bool arg_debug;
        bool arg_readVI;
        bool arg_getSEL;
        bool arg_resetSEL;
        bool arg_getCONFIG;
        std::optional<lsd_output_status_t> arg_output;
        std::optional<float> arg_max_I;
        std::optional<unsigned int> arg_hold_time;
        std::optional<lsd_avg_t>  arg_avg_num;
        std::optional<lsd_conv_t> arg_voltage_conv_time;
        std::optional<lsd_conv_t> arg_current_conv_time;

        std::string arg_device;
    };
}

#endif