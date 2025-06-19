#include <Preferences.h>
#include <EzLoRaWAN_CayenneLPP.h>
#include <EzLoRaWAN.h>
#include "LoRaWAN_config.h"
#include <CV7.h>

#define SERIAL_BAUD 115200
#define TIMEOUT_MS 20000
#define MAX_ERRORS 5
#define RX_PIN 16
#define SEND_INTERVAL 10000

// TTN credentials
const char* devEui = config_devEui;
const char* appEui = config_appEui;
const char* appKey = config_appKey;

EzLoRaWAN ttn;
EzLoRaWAN_CayenneLPP lpp;
Preferences preferences;

const char* nvs_namespace = "module_config";
const char* nvs_key = "module_id";
int module_id = -1;
int simu1_id = -1;
int simu2_id = -1;
int remontee = -1;
bool isFirstPylone = false;

CV7 sensor(RX_PIN);

void message(const uint8_t* payload, size_t size, uint8_t port, int rssi) {
  Serial.println("-- MESSAGE");
  Serial.printf("Received %d bytes on port %d (RSSI=%ddB) :", size, port, rssi);
  for (int i = 0; i < size; i++) Serial.printf(" %02X", payload[i]);
  Serial.println();
}

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
            Serial.println("[ERROR] Entrée invalide.");
            if (++error_count >= MAX_ERRORS) ESP.restart();
          }
        }
        break;
      } else {
        Serial.println("[INFO] Démarrage normal...");
        break;
      }
    }
  }

  preferences.end();
  simu1_id = module_id + 1;
  simu2_id = module_id + 2;
  remontee = ((module_id - 1) / 10) + 1;
  isFirstPylone = ((module_id - 1) % 10 == 0);
  Serial.printf("[INFO] Remontée : %d | CV7 ID: %d | Simu1 ID: %d | Simu2 ID: %d | Premier pylône : %s\n",
                remontee, module_id, simu1_id, simu2_id, isFirstPylone ? "OUI" : "NON");
}

unsigned long lastSend = 0;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(100);
  Serial.println("\n[BOOT] Démarrage du module CV7 + Simu1 + Simu2...");

  lireID();
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
    Serial.println("\n[TTN] Connecté !");
    ttn.showStatus();
  } else {
    Serial.println("[INFO] Ce module n'est pas un premier pylône. Aucune transmission TTN.");
  }
}

void loop() {
  sensor.readFrame();

  if (!isFirstPylone) return;
  if (millis() - lastSend >= SEND_INTERVAL) {
    lastSend = millis();

    // CV7 réel
    float tempCV7 = sensor.getTemperature();
    float windCV7 = sensor.getWindSpeed();
    float dirCV7 = sensor.getWindDirection();

    // Simu 1
    float tempSimu1 = random(150, 300) / 10.0;
    float windSimu1 = random(0, 150) / 10.0;
    float dirSimu1 = random(0, 360);

    // Simu 2
    float tempSimu2 = random(150, 300) / 10.0;
    float windSimu2 = random(0, 150) / 10.0;
    float dirSimu2 = random(0, 360);

    // Debug
    Serial.println("------ Mesures envoyées ------");
    Serial.printf("[SIMU1] Temp: %.1f | Vent: %.1f | Dir: %.1f | ID: %d\n", tempSimu1, windSimu1, dirSimu1, simu1_id);
    Serial.printf("[SIMU2] Temp: %.1f | Vent: %.1f | Dir: %.1f | ID: %d\n", tempSimu2, windSimu2, dirSimu2, simu2_id);
    Serial.printf("[CV7  ] Temp: %.1f | Vent: %.1f | Dir: %.1f | ID: %d\n", tempCV7, windCV7, dirCV7, module_id);

    lpp.reset();

    lpp.reset();
		lpp.addAnalogInput(1, module_id);
		lpp.addTemperature(2, tempCV7);
		lpp.addAnalogInput(3, windCV7);
		lpp.addAnalogInput(4, dirCV7);
		lpp.addAnalogInput(5, simu1_id);
		lpp.addTemperature(6, tempSimu1);
		lpp.addAnalogInput(7, windSimu1);
		lpp.addAnalogInput(8, dirSimu1);
		lpp.addAnalogInput(9, simu2_id);
		lpp.addTemperature(10, tempSimu2);
		lpp.addAnalogInput(11, windSimu2);
		lpp.addAnalogInput(12, dirSimu2);
		lpp.addAnalogInput(13, remontee);       // D'abord remontée


    if (ttn.sendBytes(lpp.getBuffer(), lpp.getSize())) {
      Serial.println("[TTN] Données envoyées avec succès");
    } else {
      Serial.println("[TTN] Échec de l'envoi des données");
    }
  }
}
