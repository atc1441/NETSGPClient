#include "AsyncNETSGPClient.h"

constexpr const uint8_t PROG_PIN = 4; /// Programming enable pin of RF module
constexpr const uint8_t RX_PIN = 16; /// RX pin of RF module
constexpr const uint8_t TX_PIN = 17; /// TX pin of RF module
constexpr const uint32_t inverterID = 0x11002793; /// Identifier of your inverter (see label on inverter)

AsyncNETSGPClient client(Serial2, PROG_PIN); // Defaults to update every 2 seconds
// AsyncNETSGPClient client(Serial2, PROG_PIN, 10); // Update every 10 seconds

void onInverterStatus(const AsyncNETSGPClient::InverterStatus& status)
{
    // We do not need to check status.valid, because only valid ones are anounced
    Serial.println("*********************************************");
    Serial.println("Received Inverter Status");
    Serial.print("Device: ");
    Serial.println(status.deviceID, HEX);
    Serial.println("Status: " + String(status.state));
    Serial.println("DC_Voltage: " + String(status.dcVoltage) + "V");
    Serial.println("DC_Current: " + String(status.dcCurrent) + "A");
    Serial.println("DC_Power: " + String(status.dcPower) + "W");
    Serial.println("AC_Voltage: " + String(status.acVoltage) + "V");
    Serial.println("AC_Current: " + String(status.acCurrent) + "A");
    Serial.println("AC_Power: " + String(status.acPower) + "W");
    Serial.println("Power gen total: " + String(status.totalGeneratedPower));
    Serial.println("Temperature: " + String(status.temperature));
}

void setup()
{
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
    pinMode(LED_BUILTIN, OUTPUT);
    delay(1000);
    Serial.println("Welcome to Micro Inverter Interface by ATCnetz.de and enwi");

    // Make sure the RF module is set to the correct settings
    if (!client.setDefaultRFSettings())
    {
        Serial.println("Could not set RF module to default settings");
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
