#include <Preferences.h>
#include <EzLoRaWAN_CayenneLPP.h>
#include <EzLoRaWAN.h>
#include "LoRaWAN_config.h"
#include <CV7.h>

#define SERIAL_BAUD 115200
#define TIMEOUT_MS 20000
#define MAX_ERRORS 5
#define RX_PIN 16

// TTN credentials from config
const char* devEui = config_devEui;
const char* appEui = config_appEui;
const char* appKey = config_appKey;

// TTN objects
EzLoRaWAN ttn;
EzLoRaWAN_CayenneLPP lpp;

// Preferences for ID
Preferences preferences;
const char* nvs_namespace = "module_config";
const char* nvs_key = "module_id";
int module_id = -1;
int remontee = -1;

// CV7 sensor instance
CV7 sensor(RX_PIN);

// TTN message handler
void message(const uint8_t* payload, size_t size, uint8_t port, int rssi) {
  Serial.println("-- MESSAGE");
  Serial.printf("Received %d bytes on port %d (RSSI=%ddB) :", size, port, rssi);
  for (int i = 0; i < size; i++) {
    Serial.printf(" %02X", payload[i]);
  }
  Serial.println();
}

// Identification routine
void lireID() {
  preferences.begin(nvs_namespace, false);
  module_id = preferences.getInt(nvs_key, -1);
  Serial.printf("[INFO] Identifiant actuel dans NVS : %d\n", module_id);

  int error_count = 0;
  Serial.println("Voulez-vous définir ou modifier l'identifiant du module ? (o/n)");
  Serial.println("[DEBUG] Attente de réponse série...");

  unsigned long start_time = millis();
  while (millis() - start_time < TIMEOUT_MS) {
    if (Serial.available()) {
      char response = Serial.read();
      Serial.printf("[DEBUG] Réponse reçue : %c\n", response);

      if (response == 'o' || response == 'O') {
        Serial.println("[INFO] Entrée en mode modification d'identifiant...");

        while (error_count < MAX_ERRORS) {
          Serial.println("Entrez le nouvel identifiant numérique :");
          while (!Serial.available());
          String input = Serial.readStringUntil('\n');
          input.trim();

          Serial.printf("[DEBUG] Valeur saisie : %s\n", input.c_str());

          if (input.length() > 0 && input.toInt() > 0) {
            module_id = input.toInt();
            preferences.putInt(nvs_key, module_id);
            Serial.printf("[SUCCESS] Identifiant %d enregistré avec succès !\n", module_id);
            break;
          } else {
            Serial.println("[ERROR] Entrée invalide, veuillez entrer un nombre valide.");
            error_count++;
            if (error_count >= MAX_ERRORS) {
              Serial.println("[ERROR] Trop d'erreurs, redémarrage du module...");
              ESP.restart();
            }
          }
        }
        break;
      } else {
        Serial.println("[INFO] Démarrage normal...");
        break;
      }
    }
  }

  Serial.printf("[INFO] Identifiant actuel : %d\n", module_id);
  preferences.end();
  remontee = ((module_id - 1) / 10) + 1;
}

unsigned long lastSend = 0;
bool isFirstPylone = false;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(100);
  Serial.println("\n[BOOT] Démarrage du module ESP32 avec CV7...");

  lireID();
  Serial.printf("[INFO] Remontée calculée : %d\n", remontee);

  isFirstPylone = ((module_id - 1) % 10 == 0);

  sensor.initialize();

  if (isFirstPylone) {
#ifndef AUTO_PIN_MAP
    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);
#endif

    ttn.begin();
    ttn.onMessage(message);
    ttn.join(devEui, appEui, appKey);
    Serial.print("Joining TTN ");
    while (!ttn.isJoined()) {
      Serial.print(".");
      delay(500);
    }
    Serial.println("\njoined !");
    ttn.showStatus();
  } else {
    Serial.println("[INFO] Ce module n'est pas un premier pylône. Aucune transmission TTN.");
  }
}

void loop() {
  sensor.readFrame();  // lire en continu

  if (millis() - lastSend >= 5000) {
    lastSend = millis();

    float temperature = sensor.getTemperature();
    float windSpeed = sensor.getWindSpeed();
    float windDir = sensor.getWindDirection();

    Serial.printf("[CV7] Temp: %.2f°C | Vent: %.2f km/h | Dir: %.2f°\n", temperature, windSpeed, windDir);

    if (isFirstPylone) {
      lpp.reset();
      lpp.addTemperature(1, temperature);
      lpp.addAnalogInput(2, windSpeed);
      lpp.addAnalogInput(3, windDir);
      lpp.addAnalogInput(4, module_id);
      lpp.addAnalogInput(5, remontee);

      if (ttn.sendBytes(lpp.getBuffer(), lpp.getSize())) {
        Serial.println("[TTN] Données envoyées avec succès");
      } else {
        Serial.println("[TTN] Échec de l'envoi des données");
      }
    }
  }
}
