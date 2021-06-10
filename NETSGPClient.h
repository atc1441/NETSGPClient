#pragma once

#include <Stream.h>

/// @brief LC12S 2.4GHz RF module specific stuff
namespace LC12S
{
    /// @brief RF power setting in dBm
    enum RFPower
    {
        DBM_12 = 0, // 12 dBm
        DBM_10 = 1, // 10 dBm
        DBM_09 = 2, //  9 dBm
        DBM_08 = 3, //  8 dBm
        DBM_06 = 4, //  6 dBm
        DBM_03 = 5, //  3 dBm
        DBM_00 = 6, //  0 dBm
        DBM_N_02 = 7, //   -2 dBm
        DBM_N_05 = 8, //   -5 dBm
        DBM_N_10 = 9, //  -10 dBm
        DBM_N_15 = 10, // -15 dBm
        DBM_N_20 = 11, // -20 dBm
        DBM_N_25 = 12, // -25 dBm
        DBM_N_30 = 13, // -30 dBm
        DBM_N_35 = 14, // -35 dBm
    };

    /// @brief Baudrate setting in baud
    enum Baudrate
    {
        BPS_600 = 0,
        BPS_1200 = 1,
        BPS_2400 = 2,
        BPS_4800 = 3,
        BPS_9600 = 4,
        BPS_19200 = 5,
        BPS_38400 = 6,
    };

    /// @brief LC12S module settings
    struct Settings
    {
        uint16_t moduleID; /// Unique module identifier
        uint16_t networkID; /// Network identifier
        LC12S::RFPower rfPower; /// RF power
        LC12S::Baudrate baudrate; /// Baudrate
        uint8_t rfChannel; /// RF channel
        bool valid; /// Is this settings object valid (true) or not (false)

        bool operator==(const Settings& rhs) const
        {
            return networkID == rhs.networkID && rfPower == rhs.rfPower && baudrate == rhs.baudrate
                && rfChannel == rhs.rfChannel && valid == rhs.valid;
        }
        bool operator!=(const Settings& rhs) const { return !operator==(rhs); }
    };

    constexpr const Settings DEFAULT_SETTINGS = {
        .moduleID = 0x58AF,
        .networkID = 0x0000,
        .rfPower = RFPower::DBM_12,
        .baudrate = Baudrate::BPS_9600,
        .rfChannel = 0x64,
        .valid = true,
    };

} // namespace LC12S

/// @brief Class for micro inverter communication
class NETSGPClient
{
public:
    /// @brief Contains status information of a specific inverter
    struct InverterStatus
    {
        uint32_t deviceID; /// Unique inverter identifier

        uint8_t state; /// Inverter state (not reversed)
        uint8_t temperature; /// Inverter temperature (not reversed)

        bool valid; /// Validity of the contained data

        float totalGeneratedPower; /// Total generated power (kWh? not reversed)

        float dcVoltage; /// DC voltage in Volts (panel voltage)
        float dcCurrent; /// DC current in Amperes (panel current)
        float dcPower; /// DC power in Watts (panel power)

        float acVoltage; /// AC voltage in Volts
        float acCurrent; /// AC current in Amperes
        float acPower; /// AC power in Watts
    };

public:
    /// @brief Construct a new NETSGPClient object.
    ///
    /// @param stream Stream to communicate with the RF module
    /// @param progPin Programming enable pin of RF module (active low)
    NETSGPClient(Stream& stream, const uint8_t progPin);

    /// @brief Destroy the NETSGPClient object
    ~NETSGPClient();

    /// @brief Get the status of the given device.
    ///
    /// @param deviceID Unique device identifier
    /// @return InverterStatus Status of the inverter (InverterStatus.valid == true) or empty status
    /// (InverterStatus.valid == false)
    InverterStatus getStatus(const uint32_t deviceID);

    /// @brief Read the settings of the RF module
    LC12S::Settings readRFModuleSettings();

    /// @brief Change the settings of the RF module to the provided ones.
    ///
    /// @param settings Settings to write to the RF module
    /// @return true If settings were written successfully
    /// @return false If not
    bool writeRFModuleSettings(const LC12S::Settings& settings);

    /// @brief Set the RF module to its default settings if needed.
    ///
    /// This function will read the RF module settings and then compare these with the default ones and if they
    /// mismatch will write the default config
    /// @return true If settings are correct or written successfully
    /// @return false If settings could not be written
    bool setDefaultRFSettings();

protected:
    /// @brief All known commands
    enum Command
    {
        STATUS = 0xC0, /// Get status command
        CONTROL = 0xC1, /// Control command
        POWER_GRADE = 0xC3, /// Set power grade command
    };

protected:
    /// @brief Send a specific command to a specific inverter with a specific value.
    ///
    /// @param command Command to send
    /// @param value Value to send
    /// @param deviceID Recipient inverter identifier
    void sendCommand(const Command command, const uint8_t value, const uint32_t deviceID);

    /// @brief Wait for a message with a timeout of 1 second
    ///
    /// @return true If stream contains a message within timeout
    /// @return false If not
    bool waitForMessage();

    /// @brief Try to find a status message in the stream and if present read it into mBuffer
    ///
    /// @return true If message was found and read into mBuffer
    /// @return false If not
    bool findAndReadStatusMessage();

    /// @brief Calculate the checksum for a message inside the buffer.
    ///
    /// @param bytes The amount of bytes to calculate the checksum for
    /// @return uint8_t CRC
    uint8_t calcCRC(const size_t bytes) const;

    /// @brief Enable programming mode of the RF module.
    ///
    /// This function will delay code execution for 400ms
    void enableProgramming();

    /// @brief Disable programming mode of the RF module.
    ///
    /// This function will delay code execution for 10ms
    void disableProgramming();

    /// @brief Fill the given inverter status from the given buffer
    ///
    /// @param buffer Bufffer containing raw inverter status data, must be at least 27 bytes in size
    /// @param status Inverter status to fill
    /// @return true If checksum is valid
    /// @return false If checksum is invalid
    bool fillInverterStatusFromBuffer(const uint8_t* buffer, InverterStatus& status);

protected:
    Stream& mStream; /// Stream for communication
    uint8_t mProgPin; /// Programming enable pin of RF module (active low)
    uint8_t mBuffer[32] = {0}; /// Inernal buffer
    static const uint8_t MAGIC_BYTE = 0x43; /// Magic byte indicating start of messages
    static const uint8_t STATUS_HEADER[2]; /// Start of status message containing magic byte and status command byte
};
