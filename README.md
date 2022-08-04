# Twiboot

Twiboot is a Particle library that allows for interfacing with the Twiboot bootloader.
It is a simple library that allows for the flashing of a new firmware to a Twiboot-enabled device.

## Installation:

Make sure that your MCU of choice has [twiboot](https://github.com/orempel/twiboot) installed. A makefile
is included here, however, this is mainly for my own purposes (for custom-building twiboot) and is not
recommended for use.

## Notes:

This library unfortunately requires more than the default TWI buffer size. Therefore,
to increase the buffer size, you must place the following code snippet at the bottom of your main .ino file:

```
#define TWI_BUFFER_SIZE 140

hal_i2c_config_t acquireWireBuffer()
{
    hal_i2c_config_t config = {
        .size = sizeof(hal_i2c_config_t),
        .version = HAL_I2C_CONFIG_VERSION_1,
        .rx_buffer = new (std::nothrow) uint8_t[TWI_BUFFER_SIZE],
        .rx_buffer_size = TWI_BUFFER_SIZE,
        .tx_buffer = new (std::nothrow) uint8_t[TWI_BUFFER_SIZE],
        .tx_buffer_size = TWI_BUFFER_SIZE};
    return config;
}
```
