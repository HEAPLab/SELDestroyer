#include "options.hpp"

#include <boost/program_options.hpp>
#include <iostream>
namespace po = boost::program_options;


namespace seldestroyer {

Options::Options(int argc, const char* argv[]) {

    po::options_description desc("Allowed options");

    desc.add_options()
        ("compression", po::value<int>(), "set compression level")
        ("debug", "Enable library debug mode")
        ("device", po::value<std::string>()->default_value("/dev/ttyUSB0"),"Specify a device file name")
        ("readVI", "Read instanteneous V/I values")
        ("getSEL", "Read counter of SEL")
        ("resetSEL", "Reset counter of SEL")
        ("output", po::value<char>(),"Set a new value for the output state (0: OFF, 1: ON, A: Auto)")
        ("getCONFIG", "Read and print the current configuration")
        ("config_max_I", po::value<float>(), "Set max current for SEL detection (in A)")
        ("config_hold_us", po::value<unsigned int>(), "Set hold time after SEL detection (in us, resolution 128us)")
        ("config_avg_nr", po::value<unsigned int>(), "Set the configuratoin for number of averages. See documentation for possible values.")
        ("config_ct_V", po::value<unsigned int>(), "Set the configuratoin for conversion time Voltage. See documentation for possible values.")
        ("config_ct_I", po::value<unsigned int>(), "Set the configuratoin for conversion time Current. See documentation for possible values.")
        ("version", "Print version information")
        ("help", "Print this help message")
    ;

    po::variables_map vm;
    const po::positional_options_description p;

    try {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);    
    } catch ( const boost::program_options::error& e ) {
        std::cerr << "Command line arguments problem: " << e.what() << "\n";
        throw e;
    }

    if (vm.count("help")) {
        std::cerr << desc << std::endl;
        throw std::exception();
    }

    if (vm.count("v")) {
        std::cerr << "SEL Destroyer" << std::endl;
        std::cerr << "Library version: " << lsd_get_version() << std::endl;
        throw std::exception();
    }

    this->arg_debug = vm.count("debug");
    this->arg_device = vm["device"].as<std::string>();
    this->arg_readVI = vm.count("readVI");
    this->arg_getSEL = vm.count("getSEL");
    this->arg_resetSEL = vm.count("resetSEL");
    this->arg_getCONFIG = vm.count("getCONFIG");

    if(vm.count("output")) {
        switch(vm["output"].as<char>()) {
            case '0': arg_output = LSD_OUTPUT_STATUS_OFF; break;
            case '1': arg_output = LSD_OUTPUT_STATUS_ON; break;
            case 'A': arg_output = LSD_OUTPUT_STATUS_AUTO; break;
            default:
                std::cerr << "Invalid output provided." << std::endl;
                throw std::exception();
        }
    }


    if(vm.count("config_max_I")) {
        this->arg_max_I = vm["config_max_I"].as<float>();
    }

    if(vm.count("config_hold_us")) {
        this->arg_hold_time = vm["config_hold_us"].as<unsigned int>();
    }

    if(vm.count("config_avg_nr")) {
        unsigned int temp = vm["config_avg_nr"].as<unsigned int>();
        if (temp < 0 || temp > LSD_AVG_1024) {
            std::cerr << "Invalid config_avg_nr provided." << std::endl;
            throw std::exception();
        }
        this->arg_avg_num = static_cast<lsd_avg_t>(temp);
    }

    if(vm.count("config_ct_V")) {
        unsigned int temp = vm["config_ct_V"].as<unsigned int>();
        if (temp < 0 || temp > LSD_AVG_1024) {
            std::cerr << "Invalid config_ct_V provided." << std::endl;
            throw std::exception();
        }
        this->arg_voltage_conv_time = static_cast<lsd_conv_t>(temp);
    }

    if(vm.count("config_ct_I")) {
        unsigned int temp = vm["config_ct_I"].as<unsigned int>();
        if (temp < 0 || temp > LSD_CT_8244_US) {
            std::cerr << "Invalid config_ct_I provided." << std::endl;
            throw std::exception();
        }
        this->arg_current_conv_time = static_cast<lsd_conv_t>(temp);
    }



}

}