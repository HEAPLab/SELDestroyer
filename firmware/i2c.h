#ifndef I2C_H /* Guard against multiple inclusion */
#define I2C_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Initiaize the I2C bus
 */
void i2c_init(void);

/**
 * Send a sequence of bytes on the I2C bus in BLOCKING mode
 */
bool i2c_send(uint8_t address, const uint8_t* data, uint8_t nr_bytes);

/**
 * Send a sequence of identical bytes on the I2C bus in BLOCKING mode
 */
bool i2c_send_zeros(uint8_t address, uint8_t command, uint8_t byte, uint16_t nr_bytes);


/**
 * Receive a sequence of bytes on the I2C bus in BLOCKING mode
 * @note You usually DO NOT want to use this function but you usually want to
 *       use the send/receive function
 */
bool i2c_recv(uint8_t address, uint8_t* data, uint8_t nr_bytes);

/**
 * Send/Receive a sequence of bytes on the I2C bus in BLOCKING mode
 */
uint8_t i2c_send_recv(uint8_t address, const uint8_t* send_data, uint8_t send_nr_bytes, uint8_t* recv_data, uint8_t recv_nr_bytes);

#endif /* I2C_H */
