#include "NETSGPClient.h"

#include <Arduino.h>

NETSGPClient::NETSGPClient(Stream& stream, const uint8_t progPin) : mStream(stream), mProgPin(progPin)
{
    pinMode(mProgPin, OUTPUT);
    disableProgramming();
}

NETSGPClient::~NETSGPClient() { }

NETSGPClient::InverterStatus NETSGPClient::getStatus(const uint32_t deviceID)
{
    sendCommand(deviceID, Command::STATUS);
    InverterStatus status;
    if (waitForMessage() && findAndReadReply(Command::STATUS))
    {
        fillInverterStatusFromBuffer(&mBuffer[0], status);
    }
    else
    {
        status.valid = false;
    }
    return status;
}

bool NETSGPClient::setPowerGrade(const uint32_t deviceID, const PowerGrade pg)
{
    return sendCommandAndValidate(deviceID, Command::POWER_GRADE, pg);
}

bool NETSGPClient::activate(const uint32_t deviceID, const bool activate)
{
    return sendCommandAndValidate(deviceID, Command::CONTROL, activate ? Control::ACTIVATE : Control::DEACTIVATE);
}

bool NETSGPClient::reboot(const uint32_t deviceID)
{
    return sendCommandAndValidate(deviceID, Command::CONTROL, Control::REBOOT);
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

void NETSGPClient::sendCommand(const uint32_t deviceID, const Command command, const uint8_t value)
{
    uint8_t* bufferPointer = &mBuffer[0];

    *bufferPointer++ = MAGIC_BYTE;
    *bufferPointer++ = command;
    *bufferPointer++ = 0x00; // data box ID
    *bufferPointer++ = 0x00; // data box ID
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

bool NETSGPClient::sendCommandAndValidate(const uint32_t deviceID, const Command command, const uint8_t value)
{
    sendCommand(deviceID, command, value);
    if (waitForMessage() && findAndReadReply(command))
    {
        const bool crc = mBuffer[14] == calcCRC(14);
        const bool valid = mBuffer[13] == value;

        DEBUGF("[sendCommandAndValidate] CRC %s & value %s\n", crc ? "valid" : "invalid", valid ? "valid" : "invalid");

        return crc && valid;
    }

    return false;
}

bool NETSGPClient::waitForMessage()
{
    const uint32_t startTime = millis();
    while (millis() - startTime < 1000)
    {
        if (mStream.available())
        {
            return true;
        }
        delay(1);
    }
    DEBUGLN("[waitForMessage] Timeout");
    return false;
}

bool NETSGPClient::findAndReadReply(const Command command)
{
    // Search for a reply header consisting of magic byte and one of the command bytes
    const char header[2] = {MAGIC_BYTE, command};
    if (!mStream.find(&header[0], 2))
    {
        DEBUGLN("[findAndReadReply] Could not find header");
        return false;
    }

    size_t bytesToRead;
    switch (command)
    {
    case Command::STATUS:
        // whole message is 27 bytes
        bytesToRead = 25;
        break;
    case Command::CONTROL:
    case Command::POWER_GRADE:
        // whole message is 15 bytes
        bytesToRead = 13;
        break;
    default:
        bytesToRead = 0;
        DEBUGLN("[findAndReadReply] Unknown command");
        break;
    }

    if (bytesToRead)
    {
        const size_t bytesRead = mStream.readBytes(&mBuffer[2], bytesToRead);
        dumpBuffer(2 + bytesRead);
        return bytesRead == bytesToRead;
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

bool NETSGPClient::fillInverterStatusFromBuffer(const uint8_t* buffer, InverterStatus& status)
{
    status.deviceID = buffer[6] << 24 | buffer[7] << 16 | buffer[8] << 8 | (buffer[9] & 0xFF);

    const uint32_t tempTotal = buffer[10] << 24 | buffer[11] << 16 | buffer[12] << 8 | (buffer[13] & 0xFF);
    status.totalGeneratedPower = *((float*)&tempTotal);

    status.dcVoltage = (buffer[15] << 8 | buffer[16]) / 100.0f;
    status.dcCurrent = (buffer[17] << 8 | buffer[18]) / 100.0f;
    status.dcPower = status.dcVoltage * status.dcCurrent;

    status.acVoltage = (buffer[19] << 8 | buffer[20]) / 100.0f;
    status.acCurrent = (buffer[21] << 8 | buffer[22]) / 100.0f;
    status.acPower = status.acVoltage * status.acCurrent;

    status.state = buffer[25]; // not fully reversed

    status.temperature = buffer[26]; // not fully reversed

    status.valid = buffer[14] == calcCRC(14);

    DEBUGF("CRC %s\n", status.valid ? "valid" : "invalid");

    return status.valid;
}

void NETSGPClient::dumpBuffer(const size_t bytes)
{
#ifdef DEBUG_SERIAL
    if (bytes <= BUFFER_SIZE)
    {
        for (uint8_t i = 0; i < bytes; ++i)
        {
            DEBUGF("%02X", mBuffer[i]);
        }
        DEBUGLN();
    }
#endif
}