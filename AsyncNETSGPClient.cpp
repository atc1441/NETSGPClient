#include <Arduino.h>

#include "AsyncNETSGPClient.h"

AsyncNETSGPClient::AsyncNETSGPClient(Stream& stream, const uint8_t progPin, const uint8_t interval)
    : NETSGPClient(stream, progPin), mIntervalMS(1000 * interval)
{ }

void AsyncNETSGPClient::update()
{
    const uint32_t currentMillis = millis();

    // Send comands at mIntervalMS
    if (currentMillis - mLastUpdateMS >= mIntervalMS)
    {
        mLastUpdateMS = currentMillis;

        for (const uint32_t deviceID : mDevices)
        {
            sendCommand(Command::STATUS, 0x00, deviceID);
        }
    }

    // Check for answers
    while (mStream.available() >= 27)
    {
        // Search for a magic byte and command
        const uint8_t header[2] = {MAGIC_BYTE, Command::STATUS};
        if (!mStream.find(&header[0], 2))
        {
            continue;
        }

        // Read rest of message
        if (mStream.readBytes(&mBuffer[2], 25) != 25)
        {
            continue;
        }

        if (mCallback)
        {
            InverterStatus status;
            status.valid = true; // TODO maybe use checksum for this
            fillInverterStatusFromBuffer(&mBuffer[0], status);
            mCallback(status);
        }
    }
}