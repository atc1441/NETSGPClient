#pragma once

#include <Stream.h>

// To enable debug output uncomment one of the below lines
// #define DEBUG_SERIAL Serial
// #define DEBUG_SERIAL Serial1

#ifdef DEBUG_SERIAL
#define DEBUG(s) DEBUG_SERIAL.print(s)
#define DEBUGLN(s) DEBUG_SERIAL.println(s)
#if defined(__cplusplus) && (__cplusplus > 201703L)
#define DEBUGF(format, ...) DEBUG_SERIAL.printf_P(PSTR(format), __VA_OPT__(, ) __VA_ARGS__)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define DEBUGF(format, ...) DEBUG_SERIAL.printf_P(PSTR(format), ##__VA_ARGS__)
#endif
#else
#define DEBUG(s)
#define DEBUGLN(s)
#define DEBUGF(format, ...)
#endif

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

    /// @brief All possible power grades from 0% up to 100%
    enum PowerGrade
    {
        PG0,
        PG1,
        PG2,
        PG3,
        PG4,
        PG5,
        PG6,
        PG7,
        PG8,
        PG9,
        PG10,
        PG11,
        PG12,
        PG13,
        PG14,
        PG15,
        PG16,
        PG17,
        PG18,
        PG19,
        PG20,
        PG21,
        PG22,
        PG23,
        PG24,
        PG25,
        PG26,
        PG27,
        PG28,
        PG29,
        PG30,
        PG31,
        PG32,
        PG33,
        PG34,
        PG35,
        PG36,
        PG37,
        PG38,
        PG39,
        PG40,
        PG41,
        PG42,
        PG43,
        PG44,
        PG45,
        PG46,
        PG47,
        PG48,
        PG49,
        PG50,
        PG51,
        PG52,
        PG53,
        PG54,
        PG55,
        PG56,
        PG57,
        PG58,
        PG59,
        PG60,
        PG61,
        PG62,
        PG63,
        PG64,
        PG65,
        PG66,
        PG67,
        PG68,
        PG69,
        PG70,
        PG71,
        PG72,
        PG73,
        PG74,
        PG75,
        PG76,
        PG77,
        PG78,
        PG79,
        PG80,
        PG81,
        PG82,
        PG83,
        PG84,
        PG85,
        PG86,
        PG87,
        PG88,
        PG89,
        PG90,
        PG91,
        PG92,
        PG93,
        PG94,
        PG95,
        PG96,
        PG97,
        PG98,
        PG99,
        PG100,
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

    /// @brief Set the power grade of the given inverter
    ///
    /// @param deviceID Unique device identifier
    /// @param pg Power grade from 0-100% to set
    /// @return true if power grade was set successfully
    /// @return false if not
    bool setPowerGrade(const uint32_t deviceID, const PowerGrade pg);

    /// @brief Activate or deactivate the given inverter
    ///
    /// @param deviceID Unique device identifier
    /// @param activate True to activate, false to deactivate
    /// @return true if inverter was activated/deactivated
    /// @return false if not
    bool activate(const uint32_t deviceID, const bool activate);

    /// @brief Reboot the given inverter
    ///
    /// @param deviceID Unique device identifier
    /// @return true True if inverter will reboot
    /// @return false if not
    bool reboot(const uint32_t deviceID);

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
        STATUS = 0xC0, /// Get status command (0xC0)
        CONTROL = 0xC1, /// Control command (0xC1)
        POWER_GRADE = 0xC3, /// Set power grade command (0xC3)
    };

    /// @brief All known control values
    enum Control
    {
        ACTIVATE = 0x01, /// Activate inverter
        DEACTIVATE = 0x02, /// Deactivate inverter
        REBOOT = 0x03, /// Reboot inverter
    };

protected:
    /// @brief Send a specific command to a specific inverter with a specific value.
    ///
    /// @param deviceID Recipient inverter identifier
    /// @param command Command to send
    /// @param value Optional value to send
    void sendCommand(const uint32_t deviceID, const Command command, const uint8_t value = 0x00);

    /// @brief Send a specific command to a specific inverter with a specific value and validate the reply
    ///
    /// @param deviceID  Recipient inverter identifier
    /// @param command Command to send
    /// @param value Optional value to send
    /// @return true If command was sent and validated
    /// @return false not
    bool sendCommandAndValidate(const uint32_t deviceID, const Command command, const uint8_t value = 0x00);

    /// @brief Wait for a message with a timeout of 1 second
    ///
    /// @return true If stream contains a message within timeout
    /// @return false If not
    bool waitForMessage();

    /// @brief Try to find a reply in the stream and if present read it into mBuffer
    ///
    /// @param command Expected command of reply
    /// @return true If reply was found and read into mBuffer
    /// @return false If not
    bool findAndReadReply(const Command command);

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

    /// @brief Dump the buffer contents to debug serial
    ///
    /// @param bytes Amount of bytes to dump
    void dumpBuffer(const size_t bytes = BUFFER_SIZE);

protected:
    constexpr static const size_t BUFFER_SIZE = 32;
    constexpr static const uint8_t MAGIC_BYTE = 0x43; /// Magic byte indicating start of messages
    Stream& mStream; /// Stream for communication
    uint8_t mProgPin; /// Programming enable pin of RF module (active low)
    uint8_t mBuffer[BUFFER_SIZE] = {0}; /// Inernal buffer
};
