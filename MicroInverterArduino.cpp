#include <Arduino.h>

#include "MicroInverterArduino.h"

MicroInverterArduino::MicroInverterArduino(Stream& stream) : mStream(stream) { }

MicroInverterArduino::~MicroInverterArduino() { }

MicroInverterArduino::Status MicroInverterArduino::getStatus(const uint32_t deviceID)
{
    sendCommand(Command::STATUS, 0x00, deviceID);
    Status status;
    if (waitForAnswer(26)) // command == Command::STATUS ? 26 : 14
    {
        status.valid = true;

        status.deviceID = mBuffer[6] << 24 | mBuffer[7] << 16 | mBuffer[8] << 8 | (mBuffer[9] & 0xFF);

        status.state = 0; // not reversed
        status.powerGrade = 0; // not reversed

        status.dcVoltage = (mBuffer[15] << 8 | mBuffer[16]) / 100;
        status.dcCurrent = (mBuffer[17] << 8 | mBuffer[18]) / 100;
        status.dcPower = status.dcVoltage * status.dcCurrent;

        status.acVoltage = (mBuffer[19] << 8 | mBuffer[20]) / 100;
        status.acCurrent = (mBuffer[21] << 8 | mBuffer[22]) / 100;
        status.acPower = status.acVoltage * status.acCurrent;

        status.totalGeneratedPower = 0; // not reversed
        status.temperature = mBuffer[26]; // not reversed
    }
    else
    {
        status.valid = false;
    }
    return status;
}

void MicroInverterArduino::sendCommand(const Command command, const uint8_t value, const uint32_t deviceID)
{
    uint8_t* bufferPointer = &mBuffer[0];

    *bufferPointer++ = 0x43;
    *bufferPointer++ = command;
    *bufferPointer++ = 0x00;
    *bufferPointer++ = 0x00;
    *bufferPointer++ = 0x00;
    *bufferPointer++ = 0x00;
    *bufferPointer++ = (deviceID >> 24) & 0xFF;
    *bufferPointer++ = (deviceID >> 16) & 0xFF;
    *bufferPointer++ = (deviceID >> 8) & 0xFF;
    *bufferPointer++ = deviceID & 0xFF;
    *bufferPointer++ = 0x00;
    *bufferPointer++ = 0x00;
    *bufferPointer++ = 0x00;
    *bufferPointer++ = value;
    *bufferPointer++ = calcCRC();

    mStream.write(&mBuffer[0], 15);
}

bool MicroInverterArduino::waitForAnswer(const size_t expectedSize)
{
    const uint32_t startTime = millis();
    while (millis() - startTime < 100 && !mStream.available())
    {
        delay(1);
    }

    const int available = mStream.available();
    if (available)
    {
        const size_t read = mStream.readBytes(&mBuffer[0], expectedSize);

        while (mStream.available())
        {
            mStream.read();
        }

        return read == expectedSize;
    }

    return false;
}

uint8_t MicroInverterArduino::calcCRC() const
{
    uint8_t crc = 0;
    for (int i = 0; i < 14; ++i)
    {
        crc += mBuffer[i];
    }
    return crc;
}