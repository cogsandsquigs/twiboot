#ifndef twiboot_h
#define twiboot_h

#include <inttypes.h>
#include "Particle.h"

class Twiboot
{
public:
    /**
     * Construct a new Twiboot object
     */
    Twiboot();

    /**
     * Construct a new Twiboot object
     *
     * @param address The address of the Twiboot device.
     */
    Twiboot(uint8_t address);

    /**
     * Stops the bootloader from automatically timing out and starting the application.
     */
    void AbortBootTimeout();

    /**
     * Flashes a buffer of data to the device
     *
     * @param buf The data to write.
     * @param len The length of the buffer
     */
    void Flash(uint8_t *buf, int len);

    /**
     * Verifies that the device contains the same data as the buffer.
     * Uses the CRC16 standard to verify the data.
     *
     * @param buf
     * @param len
     */
    bool Verify(uint8_t *buf, int len);

    /**
     * Starts the main (non-bootloader) application on the device. Automatically
     * lets go of the Wire buffer.
     */
    void JumpToApp();

private:
    uint8_t addr;
};

#endif // twiboot_h