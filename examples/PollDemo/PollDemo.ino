#include "NETSGPClient.h"

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
// On D1 Mini connect RF module to pins marked RX and TX and use D4 for debug output
#define debugSerial Serial1
#define clientSerial Serial
#endif

NETSGPClient client(clientSerial, PROG_PIN); /// NETSGPClient instance

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
}

uint32_t lastSendMillis = 0;
void loop()
{
    const uint32_t currentMillis = millis();
    if (currentMillis - lastSendMillis > 2000)
    {
        lastSendMillis = currentMillis;

        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        debugSerial.println("");
        debugSerial.println("Sending request now");

        const NETSGPClient::InverterStatus status = client.getStatus(inverterID);
        if (status.valid)
        {
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
    }
}
