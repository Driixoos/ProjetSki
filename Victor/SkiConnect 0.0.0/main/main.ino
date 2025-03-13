#include <EzLoRaWAN_CayenneLPP.h>
#include <EzLoRaWAN.h>
#include "LoRaWAN_config.h"
#include "Anemometre.h"

const char* devEui = config_devEui;
const char* appEui = config_appEui;
const char* appKey = config_appKey;

EzLoRaWAN ttn;
EzLoRaWAN_CayenneLPP lpp;
Anemometre anemometre(16); // Initialisation avec le bon port RX

#ifndef AUTO_PIN_MAP
#include "board_config.h"
#endif

void message(const uint8_t* payload, size_t size, uint8_t port, int rssi)
{
    Serial.println("-- MESSAGE");
    Serial.printf("Received %d bytes on port %d (RSSI=%ddB) :", size, port, rssi);
    for (int i = 0; i < size; i++)
    {
        Serial.printf(" %02X", payload[i]);
    }
    Serial.println();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting");

#ifndef AUTO_PIN_MAP
    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);
#endif

    anemometre.initialiser(); // Initialisation de l'anémomètre

    ttn.begin();
    ttn.onMessage(message);
    ttn.join(devEui, appEui, appKey);

    Serial.print("Joining TTN ");
    while (!ttn.isJoined())
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\njoined !");
    ttn.showStatus();
}

void loop()
{
    anemometre.lireTrame(); // Lecture des données de l'anémomètre

    float temperature = anemometre.getTemperature();
    float windSpeed = anemometre.getVitesseVent();
    float windDirection = anemometre.getDirectionVent();

    lpp.reset();
    lpp.addTemperature(1, temperature);
    lpp.addAnalogInput(2, windSpeed);
    lpp.addAnalogInput(3, windDirection);

    if (ttn.sendBytes(lpp.getBuffer(), lpp.getSize()))
    {
        Serial.printf("Sent to TTN: Temp=%f°C, Wind Speed=%f m/s, Wind Dir=%f°\n", temperature, windSpeed, windDirection);
    }

    delay(10000); // Envoi toutes les 10 secondes
}