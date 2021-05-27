#pragma once

#include <Stream.h>

/// @brief Class for micro inverter communication
class MicroInverterArduino
{
public:
    /// @brief Contains status information of a specific inverter
    struct Status
    {
        uint32_t deviceID; /// Unique inverter identifier

        int state; /// Inverter state (not reversed)
        int powerGrade; /// Power grade (not reversed)

        int totalGeneratedPower; /// Total generated power (kWh? not reversed)
        int temperature; /// Inverter temperature (not reversed)

        bool valid; /// Validity of the contained data

        float dcVoltage; /// DC voltage in Volts (panel voltage)
        float dcCurrent; /// DC current in Amperes (panel current)
        float dcPower; /// DC power in Watts (panel power)

        float acVoltage; /// AC voltage in Volts
        float acCurrent; /// AC current in Amperes
        float acPower; /// AC power in Watts
    };

public:
    /// @brief Construct a new Micro Inverter Arduino object
    ///
    /// @param stream Stream to communicate with the wireless module
    /// @param progPin Programming enable pin of wireless module (active low)
    MicroInverterArduino(Stream& stream, const uint8_t progPin);

    /// @brief Destroy the Micro Inverter Arduino object
    ~MicroInverterArduino();

    /// @brief Get the status of the given device
    ///
    /// @param deviceID Unique device identifier
    /// @return Status of the device (Status.valid == true) or empty status (Status.valid == false)
    Status getStatus(const uint32_t deviceID);

private:
    /// @brief All known commands
    enum Command
    {
        STATUS = 0xC0, /// Get status command
        CONTROL = 0xC1, /// Control command
        POWER_GRADE = 0xC3, /// Set power grade command
    };

private:
    /// @brief Send a specific command to a specific inverter with a specific value
    ///
    /// @param command Command to send
    /// @param value Value to send
    /// @param deviceID Recipient inverter identifier
    void sendCommand(const Command command, const uint8_t value, const uint32_t deviceID);

    /// @brief Wait for an answer with the given expected size
    ///
    /// @param expectedSize Expected answer size in bytes
    /// @return true If an answer with correct lenght was received
    /// @return false If not
    bool waitForAnswer(const size_t expectedSize);

    /// @brief Calculate the checksum for a message inside the buffer
    ///
    /// @return uint8_t CRC
    uint8_t calcCRC() const;

    /// @brief Enable programming mode of the wireless module
    void enableProgramming();

    /// @brief Disable programming mode of the wireless module
    void disableProgramming();

    /// @brief Flush the receive buffer of the stream
    void flushRX();

private:
    Stream& mStream; /// Stream for communication
    uint8_t mProgPin; /// Programming enable pin of wireless module (active low)
    uint8_t mBuffer[32] = {0}; /// Inernal buffer
};
