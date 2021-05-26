#include "MicroInverterArduino.h"

constexpr const uint8_t RX_PIN = 16;
constexpr const uint8_t TX_PIN = 17;

MicroInverterArduino inverter(Serial2);

void setup()
{
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
    pinMode(LED_BUILTIN, OUTPUT);
    delay(1000);
    Serial.println("Welcome to Micro Inverter Interface by ATCnetz.de and enwi");
}

long lastSendMillis = 0;
void loop()
{
    const uint32_t currentMillis = millis();
    if (currentMillis - lastSendMillis > 2000)
    {
        lastSendMillis = currentMillis;

        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        Serial.println("");
        Serial.println("Sending request now");

        const MicroInverterArduino::Status status = inverter.getStatus(0x11002793);
        if (status.valid)
        {
            Serial.println("*********************************************");
            Serial.println("Received Inverter Status");
            Serial.print("Device: ");
            Serial.println(status.deviceID, HEX);
            Serial.println("Status: " + String(status.state));
            Serial.println("PowerGrade: " + String(status.powerGrade));
            Serial.println("DC_Voltage: " + String(status.dcVoltage) + "V");
            Serial.println("DC_Current: " + String(status.dcCurrent) + "A");
            Serial.println("DC_Power: " + String(status.dcPower) + "W");
            Serial.println("AC_Voltage: " + String(status.acVoltage) + "V");
            Serial.println("AC_Current: " + String(status.acCurrent) + "A");
            Serial.println("AC_Power: " + String(status.acPower) + "W");
            Serial.println("Power gen total: " + String(status.totalGeneratedPower));
            Serial.println("Temperature: " + String(status.temperature));
        }
    }
}
