#include <Arduino.h>

#include "AsyncNETSGPClient.h"

AsyncNETSGPClient::AsyncNETSGPClient(Stream& stream, const uint8_t progPin, const uint8_t interval)
    : NETSGPClient(stream, progPin), mIntervalMS(1000 * interval), mDeviceIte(mDevices.begin())
{ }

void AsyncNETSGPClient::update()
{
    const uint32_t currentMillis = millis();

    // Send comands at mIntervalMS
    if (currentMillis - mLastUpdateMS >= mIntervalMS && !mCanSend)
    {
        mLastUpdateMS = currentMillis;
        mCanSend = true;
    }

    if (mCanSend && currentMillis - mLastSendMS >= 1010)
    {
        if (mDeviceIte != mDevices.end())
        {
            mLastSendMS = currentMillis;
            mLastUpdateMS = currentMillis;
            sendCommand(Command::STATUS, 0x00, *mDeviceIte);
            // Serial1.printf("Sent to %#04x\n", *mDeviceIte);
            mCanSend = false;
            ++mDeviceIte;
        }
        else
        {
            mCanSend = false; // make sure we only poll every mIntervalMS
            mDeviceIte = mDevices.begin();
        }
    }

    // Check for answers
    while (mStream.available() >= 27)
    {
        // Search for a read status message
        if (findAndReadStatusMessage() && mCallback)
        {
            InverterStatus status;
            if (fillInverterStatusFromBuffer(&mBuffer[0], status))
            {
                status.valid = true;
                mCallback(status);
                mCanSend = true;
            }
        }
    }
}