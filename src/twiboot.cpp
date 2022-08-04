#include "Particle.h"
#include "twiboot.h"
#include "crc.h"

inline void startWire()
{
    if (!Wire.isEnabled())
    {
        Wire.begin();
    }
}

Twiboot::Twiboot()
{
    this->addr = 0x29;
    startWire();
}

Twiboot::Twiboot(uint8_t address)
{
    this->addr = address;
    startWire();
}

void Twiboot::AbortBootTimeout()
{
    startWire();

    Wire.beginTransmission(addr);
    Wire.write(0x00);
    Wire.endTransmission();
}

void Twiboot::Flash(uint8_t *buf, int len)
{
    startWire();

    for (int i = 0; i < (len / 128) + 1; i++)
    {
        Wire.beginTransmission(addr);
        Wire.write(0x02);
        Wire.write(0x01);
        Wire.write((i * 128) >> 8 & 0xFF);
        Wire.write((i * 128) & 0xFF);

        for (int j = i * 128; j < (i + 1) * 128; j++)
        {
            if ((i == (len / 128)) && ((128 - len % 128) != 0) && j >= len)
            {
                Wire.write(0xFF);
            }
            else if ((i == (len / 128)) && ((128 - len % 128) != 0))
            {
                break;
            }
            else
            {
                Wire.write(buf[j]);
            }
        }

        Wire.endTransmission();

        delay(50);
    }
}

bool Twiboot::Verify(uint8_t *buf, int len)
{
    delay(5000);         // temp
    Serial.println("a"); // temp
    crcInit();           // has to be called before crcFast() to update the CRC tables
    int checksum = crcFast(buf, len);
    Serial.println(checksum, HEX); // temp
    startWire();

    uint8_t read[len];

    // for (int i = 0; i < (len / 128) + 1; i++)
    // {
    //     Wire.beginTransmission(addr);
    //     Wire.write(0x02);
    //     Wire.write(0x01);
    //     Wire.write((i * 128) >> 8 & 0xFF);
    //     Wire.write((i * 128) & 0xFF);
    //     Wire.endTransmission();
    //     Wire.requestFrom(addr, 128);
    //     int j = 0;
    //     while (Wire.available())
    //     {
    //         read[i * 128 + j] = Wire.read();
    //         j++;
    //     }
    // }

    return checksum == crcFast(read, len);
}

void Twiboot::JumpToApp()
{
    startWire();

    Wire.beginTransmission(addr);
    Wire.write(0x01);
    Wire.write(0x80);
    Wire.endTransmission();
}
