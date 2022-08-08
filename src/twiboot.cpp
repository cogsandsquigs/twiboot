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

bool Twiboot::AbortBootTimeout()
{
    startWire();

    Wire.beginTransmission(addr);
    Wire.write(0x00);
    return Wire.endTransmission() == 0; // if there are any errors, return false. Otherwise, return true.
}

void Twiboot::GetBootloaderVersion(char *buf)
{
    startWire();
    Wire.beginTransmission(addr);
    Wire.write(0x01);
    Wire.endTransmission();
    Wire.requestFrom(addr, 16);
    int i = 0;
    while (Wire.available())
    {
        buf[i] = Wire.read();
        i++;
    }
}

void Twiboot::GetChipInfo(uint64_t *signature, uint8_t *pageSize, uint16_t *flashSize, uint16_t *eepromSize)
{
    startWire();
    Wire.beginTransmission(addr);
    Wire.write(0x02);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(addr, 8);
    while (!Wire.available())
        ;
    *signature = Wire.read() << 16;
    *signature |= Wire.read() << 8;
    *signature |= Wire.read();
    *pageSize = Wire.read();
    *flashSize = Wire.read() << 8;
    *flashSize |= Wire.read();
    *eepromSize = Wire.read() << 8;
    *eepromSize |= Wire.read();
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

        delay(21);
    }
}

bool Twiboot::Verify(uint8_t *buf, int len)
{
    crcInit(); // has to be called before crcFast() to update the CRC tables

    for (int i = 0; i < len / 128 + 1; i++)
    {
        uint8_t read[128];
        uint8_t tmpbuf[128];
        startWire();
        Wire.beginTransmission(addr);
        Wire.write(0x02);
        Wire.write(0x01);
        Wire.write((i * 128) >> 8 & 0xFF);
        Wire.write((i * 128) & 0xFF);
        Wire.endTransmission();
        Wire.requestFrom(addr, 128);
        int j = 0;
        while (Wire.available())
        {
            if ((i * 128 + j) > len)
            {
                read[j] = 0xFF;
                tbuf[j] = 0xFF;
            }

            read[j] = Wire.read();
            tbuf[j] = buf[i * 128 + j];
            j++;
        }
        if (crcFast(read, len) != crcFast(buf, len))
        {
            return false;
        }
    }

    return true;
}

void Twiboot::JumpToApp()
{
    startWire();

    Wire.beginTransmission(addr);
    Wire.write(0x01);
    Wire.write(0x80);
    Wire.endTransmission();
}
