#pragma once

#include <set>

#include "NETSGPClient.h"

/// @brief Async version of NETSGPClient
class AsyncNETSGPClient : public NETSGPClient
{
    /// @brief Callback function type definition for inverter status updates
    typedef void (*InverterStatusCallback)(const NETSGPClient::InverterStatus&);

public:
    /// @brief Construct a new AsyncNETSGPClient object.
    ///
    /// @param stream Stream to communicate with the RF module
    /// @param progPin Programming enable pin of RF module (active low)
    /// @param interval The update interval in seconds, default is 2 seconds
    AsyncNETSGPClient(Stream& stream, const uint8_t progPin, const uint8_t interval = 2);

    /// @brief Set the callback for inverter status updates
    ///
    /// @param callback Callback that gets called on updates, may be nullptr
    void setStatusCallback(InverterStatusCallback callback) { mCallback = callback; }

    /// @brief Register a new inverter to receive status updates
    ///
    /// @param deviceID The device identifier of the inverter
    void registerInverter(const uint32_t deviceID) { mDevices.insert(deviceID); }

    /// @brief Deregister an inverter to not receive status updates
    ///
    /// @param deviceID The device identifier of the inverter
    void deregisterInverter(const uint32_t deviceID) { mDevices.erase(deviceID); }

    /// @brief Update the internal state
    ///
    /// @note Needs to be called inside loop()
    void update();

private:
    uint16_t mIntervalMS; /// Update interval in milliseconds
    uint32_t mLastUpdateMS; /// Last update time in milliseconds
    uint32_t mLastSendMS; /// Makes sure we do not send to often
    bool mCanSend = true; /// Can the next message be sent?
    std::set<uint32_t> mDevices; /// All devices to poll
    std::set<uint32_t>::iterator mDeviceIte; /// Set iterator to know which device to poll
    InverterStatusCallback mCallback = nullptr; /// Callback for status updates
};
