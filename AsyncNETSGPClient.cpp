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
        // Search for a read status message
        if (findAndReadStatusMessage() && mCallback)
        {
            InverterStatus status;
            fillInverterStatusFromBuffer(&mBuffer[0], status);
            status.valid = true; // TODO maybe use checksum for this
            mCallback(status);
        }
    }
}