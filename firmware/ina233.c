#include "ina233.h"
#include "i2c.h"

#include "global.h"

#define INA233_RESTORE_DEFAULT 0x12
#define INA233_ADC_CONFIG 0xD0
#define INA233_MFR_CAL 0xD4
#define INA233_MFR_ID 0x99
#define INA233_READ_VIN 0x88    // Retrieves the measurement for the VBUS voltage
#define INA233_READ_IIN 0x89     // Retrieves the input current measurement

// Computation for the INA233
// Max current: 10A
// Current_LSB = (2^4)/(2^15) = 2^(-11)
// MFR_CALIBRATION = 0.00512 / (2^(-11) * 0.002) = 5243 = 0x147B
#define INA233_CURRENT_LSB_M1 0x1p11F
#define INA233_MFR_CALIBRATION 0x147B

#define INA233_ADDRESS 0b1000101


void ina233_init(void) {
    const uint8_t command = INA233_RESTORE_DEFAULT;

    bool val = i2c_send(INA233_ADDRESS, &command, 1);
    
    if(!val) {
        throw_fatal_exception();
    }
    
    // Config the scale coefficient
    const uint8_t register_and_data[3] = { 
        INA233_MFR_CAL,
        (INA233_MFR_CALIBRATION & 0x00FF),
        ((INA233_MFR_CALIBRATION & 0xFF00) >> 8)
    };

    val = i2c_send(INA233_ADDRESS, register_and_data, 3);

    if(!val) {
        throw_fatal_exception();
    }

    
    // And config the ADC
    const uint8_t register_and_data_2[3] = { 
        INA233_ADC_CONFIG,
        0b00100111,
        0b00001001
    };

    val = i2c_send(INA233_ADDRESS, register_and_data_2, 3);

    if(!val) {
        throw_fatal_exception();
    }

}

#define REG2VAL(x) ((int16_t)((((uint16_t)x[1]) << 8) | x[0]))


ina233_res_t ina233_read(void) {
    ina233_res_t to_ret;
    uint8_t readed_vin[2];
    uint8_t readed_iin[2];
    uint8_t register_id;
    uint8_t status;
    
    
    to_ret.valid = true;
    
    register_id = INA233_READ_VIN;
    status = i2c_send_recv(INA233_ADDRESS, &register_id, 1, readed_vin, 2);

    if(status != 0) to_ret.valid = false;
        
    register_id = INA233_READ_IIN;
    status = i2c_send_recv(INA233_ADDRESS, &register_id, 1, readed_iin, 2);

    if(status != 0) to_ret.valid = false;
    
    to_ret.V = REG2VAL(readed_vin) * 10 / 8;
    to_ret.I = REG2VAL(readed_iin) * 1000L / 2048L;

    return to_ret;

}

