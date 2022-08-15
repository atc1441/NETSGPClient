#include "AsyncNETSGPClient.h"

constexpr const uint8_t PROG_PIN = 4; /// Programming enable pin of RF module
constexpr const uint8_t RX_PIN = 16; /// RX pin of ESP32 connect to TX of RF module
constexpr const uint8_t TX_PIN = 17; /// TX pin of ESP32 connect to RX of RF module
constexpr const uint32_t inverterID = 0x11002793; /// Identifier of your inverter (see label on inverter)

#if defined(ESP32)
// On ESP32 debug output is on Serial and RF module connects to Serial2
#define debugSerial Serial
#define clientSerial Serial2
#else
// On ESP8266 or other debug output is on Serial1 and RF module connects to Serial
// On D1 Mini these are the pins marked RX and TX
#define debugSerial Serial1
#define clientSerial Serial
#endif

AsyncNETSGPClient client(clientSerial, PROG_PIN); // Defaults to fetch status every 2 seconds
// AsyncNETSGPClient client(clientSerial, PROG_PIN, 10); // Fetch status every 10 seconds

void onInverterStatus(const AsyncNETSGPClient::InverterStatus& status)
{
    // We do not need to check status.valid, because only valid ones are announced
    debugSerial.println("*********************************************");
    debugSerial.println("Received Inverter Status");
    debugSerial.print("Device: ");
    debugSerial.println(status.deviceID, HEX);
    debugSerial.println("Status: " + String(status.state));
    debugSerial.println("DC_Voltage: " + String(status.dcVoltage) + "V");
    debugSerial.println("DC_Current: " + String(status.dcCurrent) + "A");
    debugSerial.println("DC_Power: " + String(status.dcPower) + "W");
    debugSerial.println("AC_Voltage: " + String(status.acVoltage) + "V");
    debugSerial.println("AC_Current: " + String(status.acCurrent) + "A");
    debugSerial.println("AC_Power: " + String(status.acPower) + "W");
    debugSerial.println("Power gen total: " + String(status.totalGeneratedPower));
    debugSerial.println("Temperature: " + String(status.temperature));
}

void setup()
{
    debugSerial.begin(115200);
if defined(ESP32)
    clientSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
#else
    clientSerial.begin(9600);
#endif
    pinMode(LED_BUILTIN, OUTPUT);
    delay(1000);
    debugSerial.println("Welcome to Micro Inverter Interface by ATCnetz.de and enwi.one");

    // Make sure the RF module is set to the correct settings
    if (!client.setDefaultRFSettings())
    {
        debugSerial.println("Could not set RF module to default settings");
    }

    // Make sure you set your callback to receive status updates
    client.setStatusCallback(onInverterStatus);
    // Register the inverter whose status should be read
    // This function can be called throughout your program to add inverters as you like
    // To remove an inverter whose status should not be updated anymore you can call
    // client.deregisterInverter(inverterID);
    client.registerInverter(inverterID);
}

void loop()
{
    // The AsyncNETSGPClient needs to be actively updated
    client.update();
}
