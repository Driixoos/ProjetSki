#include <EzLoRaWAN_CayenneLPP.h>
#include <EzLoRaWAN.h>
#include "LoRaWAN_config.h"
#include "Anemometre.h"

const char* devEui = config_devEui;
const char* appEui = config_appEui;
const char* appKey = config_appKey;

EzLoRaWAN ttn;
EzLoRaWAN_CayenneLPP lpp;
Anemometre anemometre(16); // Initialize with correct RX pin

#ifndef AUTO_PIN_MAP
#include "board_config.h"
#endif

// Timer for 10-second data acquisition
unsigned long lastDataSentTime = 0;
const unsigned long dataInterval = 10000; // 10 seconds

void message(const uint8_t* payload, size_t size, uint8_t port, int rssi)
{
    Serial.println("-- MESSAGE RECEIVED --");
    Serial.printf("Received %d bytes on port %d (RSSI=%ddB):", size, port, rssi);
    for (int i = 0; i < size; i++)
    {
        Serial.printf(" %02X", payload[i]);
    }
    Serial.println();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("🚀 Starting LoRaWAN Anemometer...");

#ifndef AUTO_PIN_MAP
    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);
#endif

    anemometre.initialize(); // Initialize anemometer

    ttn.begin();
    ttn.onMessage(message);
    ttn.join(devEui, appEui, appKey);

    Serial.print("Connecting to TTN ");
    while (!ttn.isJoined())
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\n✅ Connected to TTN!");

    ttn.showStatus();

    // ✅ Block all data acquisition during the 30s stabilization period
    Serial.println("⏳ Waiting 30 seconds for stable sensor readings...");
    delay(30000);  // No data acquisition during this period
    Serial.println("✅ Sensor stabilization completed.");
}

void loop()
{
    anemometre.readFrame(); // Read anemometer data

    float temperature = anemometre.getTemperature();
    float medianWindSpeed_kmh = anemometre.getMedianWindSpeed();  // Median wind speed in km/h
    float windDirection = anemometre.getWindDirection();

    // ✅ Send data every 10 seconds
    if (millis() - lastDataSentTime >= dataInterval) {
        lastDataSentTime = millis(); // Update timer

        // Display processed data instantly in the serial monitor
        Serial.println("📡 Processed Data:");
        Serial.printf(" - Temperature: %.2f°C\n", temperature);
        Serial.printf(" - Median Wind Speed: %.2f km/h\n", medianWindSpeed_kmh);
        Serial.printf(" - Wind Direction: %.2f°\n", windDirection);
        Serial.println("----------------------------------------");

        // Send data to TTN
        lpp.reset();
        lpp.addTemperature(1, temperature);
        lpp.addAnalogInput(2, medianWindSpeed_kmh);  // Median wind speed goes in Analog 2
        lpp.addAnalogInput(3, windDirection);

        if (ttn.sendBytes(lpp.getBuffer(), lpp.getSize()))
        {
            Serial.println("📤 Data successfully sent to TTN!");
        }
        else
        {
            Serial.println("⚠️ Failed to send data to TTN!");
        }
    }
}
