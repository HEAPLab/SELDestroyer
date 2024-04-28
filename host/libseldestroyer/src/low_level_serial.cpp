#include "low_level_serial.hpp"

#include "global.hpp"

#include <unistd.h>
#include <fcntl.h>      // File control definitions
#include <cerrno>
#include <cstring>
#include <termios.h>    // POSIX terminal control definitions

#define SPECIAL_CHAR '$'

namespace libdestroyer {

static int rate_to_constant(unsigned int baudrate) 
{
#define B(x) case x: return B##x
switch(baudrate) {
    B(50);     B(75);     B(110);    B(134);    B(150);
    B(200);    B(300);    B(600);    B(1200);   B(1800);
    B(2400);   B(4800);   B(9600);   B(19200);  B(38400);
    B(57600);  B(115200); B(230400); B(460800); B(500000); 
    B(576000); B(921600); B(1000000);B(1152000);B(1500000);

default: return 0;
}
#undef B
}    


LowLevelSerial::LowLevelSerial(const low_level_serial_params_t &params) 
: serial_params(params) {
    this->fd = -1;
}

LowLevelSerial::~LowLevelSerial() {
    if(this->fd != -1)
        close(this->fd);
}

void LowLevelSerial::connect() {

    // 1 - Open the device
    debug("LLS", "Opening device " + std::string(serial_params.device_name) + "...");

    this->fd = open(serial_params.device_name.c_str(), O_RDWR | O_NOCTTY );
    if(this->fd == -1) {
        throw LSDException(LSD_UNABLE_TO_CONNECT, std::string("open: ") + std::strerror(errno));
    }

    // 2 - Setting serial port information
    struct termios tty;
    memset (&tty, 0, sizeof(tty));

    // 2.1 get current parameters
    if ( tcgetattr (this->fd , &tty ) != 0 ) {
        throw LSDException(LSD_UNABLE_TO_CONNECT, std::string("tcgetattr: ") + std::strerror(errno));
    }

    cfsetospeed(&tty, rate_to_constant(serial_params.baudrate));
    cfsetispeed(&tty, rate_to_constant(serial_params.baudrate));

    // 2.2 set various parameters
    if(serial_params.parity_on)
        tty.c_cflag     |=  PARENB;
    else
        tty.c_cflag     &=  ~PARENB;
    if(serial_params.parity_is_odd)
        tty.c_cflag     |=  PARODD;
    else
        tty.c_cflag     &=  ~PARODD;

    if(serial_params.two_stop_bits)
        tty.c_cflag     |=  CSTOPB;
    else
        tty.c_cflag     &=  ~CSTOPB;


    tty.c_cflag     &=  ~CSIZE; // Reset size

    switch(serial_params.number_bits) {
        case 8: tty.c_cflag     |=  CS8;
        break;
        case 7: tty.c_cflag     |=  CS7;
        break;
        case 6: tty.c_cflag     |=  CS6;
        break;
        case 5: tty.c_cflag     |=  CS5;
        break;
        default:
            throw LSDException(LSD_INVALID_ARGUMENT, std::string("Invalid number of bits"));
    }

    tty.c_cflag     &=  ~CRTSCTS;           // no flow control
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

    tty.c_cc[VMIN]   =  0;                  // read block

    // RAW serial: input is available character by character, echoing is disabled,
    // and all special processing of terminal input and output characters is disabled.
    cfmakeraw(&tty);

    // Discard any previous data (if any)
    tcflush(this->fd, TCIOFLUSH);

    debug("LLS", "Setting new parameters to the serial port...");

    if ( tcsetattr (this->fd, TCSANOW, &tty) != 0) {
        throw LSDException(LSD_UNABLE_TO_CONNECT, std::string("tcsetattr: ") + std::strerror(errno));
    }

    // All set!
}

void LowLevelSerial::reset_trasmission() {
    constexpr int nr_chars = 2;
    const char reset_signal[nr_chars] = { SPECIAL_CHAR, 'r' };
    int count = write(this->fd, reset_signal, nr_chars);
    if (count != nr_chars) {
        throw LSDException(LSD_CONN_PROBLEM, std::string("write(1): ") + std::strerror(errno));
    }
}

void LowLevelSerial::send(const std::vector<unsigned char> &data) {
    for (unsigned char uc : data) {
        if (uc == SPECIAL_CHAR) {
            // Escaping special char
            char escape = SPECIAL_CHAR;
            int count = write(this->fd, &escape, 1);
            if (count != 1) {
                throw LSDException(LSD_CONN_PROBLEM, std::string("write(2): ") + std::strerror(errno));
            }
        }
        int count = write(this->fd, &uc, 1);
        if (count != 1) {
            throw LSDException(LSD_CONN_PROBLEM, std::string("write(3): ") + std::strerror(errno));
        }
    }
}

unsigned char LowLevelSerial::recv() {
    int count;
    char to_ret;

    count = read(this->fd, &to_ret, 1);

    if(count <= 0) {
        throw LSDException(LSD_CONN_PROBLEM, std::string("read(1): ") + std::strerror(errno));
    }

    return to_ret;
}


std::string LowLevelSerial::recv_line() {

    std::string to_ret;

    char c;

    do {
        c = this->recv();
        to_ret += c;
    } while(c!='\n');

    to_ret.pop_back();  // Remove \n

    return to_ret;
}


} // namespace libdestroyer
