#include <Arduino.h>

#include "NETSGPClient.h"

NETSGPClient::NETSGPClient(Stream& stream, const uint8_t progPin) : mStream(stream), mProgPin(progPin)
{
    pinMode(mProgPin, OUTPUT);
    disableProgramming();
}

NETSGPClient::~NETSGPClient() { }

NETSGPClient::InverterStatus NETSGPClient::getStatus(const uint32_t deviceID)
{
    flushRX(); // Need to flush RX now to make sure it is empty for waitForAnswer()
    sendCommand(Command::STATUS, 0x00, deviceID);
    InverterStatus status;
    if (waitForAnswer(27)) // command == Command::STATUS ? 27 : 15
    {
        status.valid = true;

        status.deviceID = mBuffer[6] << 24 | mBuffer[7] << 16 | mBuffer[8] << 8 | (mBuffer[9] & 0xFF);

        const uint32_t tempTotal = mBuffer[10] << 24 | mBuffer[11] << 16 | mBuffer[12] << 8 | (mBuffer[13] & 0xFF);
        status.totalGeneratedPower = *((float*)&tempTotal);

        // CRC = mBuffer[14]

        status.dcVoltage = (mBuffer[15] << 8 | mBuffer[16]) / 100;
        status.dcCurrent = (mBuffer[17] << 8 | mBuffer[18]) / 100;
        status.dcPower = status.dcVoltage * status.dcCurrent;

        status.acVoltage = (mBuffer[19] << 8 | mBuffer[20]) / 100;
        status.acCurrent = (mBuffer[21] << 8 | mBuffer[22]) / 100;
        status.acPower = status.acVoltage * status.acCurrent;

        status.state = mBuffer[25]; // not fully reversed
        status.temperature = mBuffer[26]; // not fully reversed
    }
    else
    {
        status.valid = false;
    }
    return status;
}

LC12S::Settings NETSGPClient::readRFModuleSettings()
{
    uint8_t* bufferPointer = &mBuffer[0];

    *bufferPointer++ = 0xAA; // command byte
    *bufferPointer++ = 0x5C; // command byte
    *bufferPointer++ = 0x00; // module identifier
    *bufferPointer++ = 0x00; // module identifier
    *bufferPointer++ = 0x00; // networking identifier
    *bufferPointer++ = 0x00; // networking identifier
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = 0x00; // RF power
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = 0x00; // Baudrate
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = 0x00; // RF channel (0 - 127)
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = 0x12; // Length
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = 0x18; // Checksum

    enableProgramming();

    mStream.write(&mBuffer[0], 18);
    const size_t read = mStream.readBytes(&mBuffer[0], 18);

    disableProgramming();

    LC12S::Settings settings;
    if (read == 18 && mBuffer[0] == 0xAA && mBuffer[1] == 0x5D && mBuffer[17] == calcCRC(17))
    {
        settings.valid = true;

        settings.moduleID = mBuffer[2] << 8 | (mBuffer[3] & 0xFF);
        settings.networkID = mBuffer[4] << 8 | (mBuffer[5] & 0xFF);
        settings.rfPower = static_cast<LC12S::RFPower>(mBuffer[7]);
        settings.baudrate = static_cast<LC12S::Baudrate>(mBuffer[9]);
        settings.rfChannel = mBuffer[11];
    }
    else
    {
        settings.valid = false;
    }
    return settings;
}

bool NETSGPClient::writeRFModuleSettings(const LC12S::Settings& settings)
{
    uint8_t* bufferPointer = &mBuffer[0];

    *bufferPointer++ = 0xAA; // command byte
    *bufferPointer++ = 0x5A; // command byte
    *bufferPointer++ = (settings.moduleID >> 8) & 0xFF; // module identifier
    *bufferPointer++ = settings.moduleID & 0xFF; // module identifier
    *bufferPointer++ = (settings.networkID >> 8) & 0xFF; // networking identifier
    *bufferPointer++ = settings.networkID & 0xFF; // networking identifier
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = settings.rfPower; // RF power
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = settings.baudrate; // Baudrate
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = settings.rfChannel; // RF channel (0 - 127)
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = 0x12; // Length
    *bufferPointer++ = 0x00; // NC must be 0
    *bufferPointer++ = calcCRC(16); // Checksum, we can calc for 16 bytes since 17th one is 0

    enableProgramming();

    mStream.write(&mBuffer[0], 18);
    const size_t read = mStream.readBytes(&mBuffer[0], 18);

    disableProgramming();

    return read == 18 && mBuffer[0] == 0xAA && mBuffer[1] == 0x5B && mBuffer[17] == calcCRC(17);
}

bool NETSGPClient::setDefaultRFSettings()
{
    LC12S::Settings settings = readRFModuleSettings();
    if (settings != LC12S::DEFAULT_SETTINGS)
    {
        // Copy over default settings without moduleID since that is uinque for each module
        settings.networkID = LC12S::DEFAULT_SETTINGS.networkID;
        settings.rfPower = LC12S::DEFAULT_SETTINGS.rfPower;
        settings.baudrate = LC12S::DEFAULT_SETTINGS.baudrate;
        settings.rfChannel = LC12S::DEFAULT_SETTINGS.rfChannel;
        return writeRFModuleSettings(settings);
    }
    return true;
}

void NETSGPClient::sendCommand(const Command command, const uint8_t value, const uint32_t deviceID)
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
    *bufferPointer++ = calcCRC(14);

    mStream.write(&mBuffer[0], 15);
}

bool NETSGPClient::waitForAnswer(const size_t expectedSize)
{
    const uint32_t startTime = millis();
    while (millis() - startTime < 100)
    {
        if (mStream.available())
        {
            return mStream.readBytes(&mBuffer[0], expectedSize) == expectedSize;
        }
        delay(1);
    }
    return false;
}

uint8_t NETSGPClient::calcCRC(const size_t bytes) const
{
    uint8_t crc = 0;
    for (size_t i = 0; i < bytes; ++i)
    {
        crc += mBuffer[i];
    }
    return crc;
}

void NETSGPClient::enableProgramming()
{
    digitalWrite(mProgPin, LOW);
    delay(400);
}

void NETSGPClient::disableProgramming()
{
    digitalWrite(mProgPin, HIGH);
    delay(10);
}

void NETSGPClient::flushRX()
{
    while (mStream.read() != -1) { }
}
