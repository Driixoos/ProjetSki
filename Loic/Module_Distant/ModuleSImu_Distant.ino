#include <Preferences.h>
#include <LoRa.h>
#include <SPI.h>

#define SERIAL_BAUD 115200
#define TIMEOUT_MS 20000
#define MAX_ERRORS 5
#define LORA_FREQ 868E6
#define SEND_INTERVAL 7000

Preferences preferences;
const char* nvs_namespace = "module_config";
const char* nvs_key = "module_id";
int module_id = -1;

unsigned long lastSend = 0;
int idVoisin = -1;

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
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(100);
  Serial.println("\n[BOOT] Démarrage du module simulateur de données...");

  lireID();

  LoRa.setPins(18, 14, 26);
  delay(500);
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("[ERREUR] Initialisation LoRa échouée !");
    while (1);
  }
  Serial.println("[LoRa] Initialisé avec succès.");

  idVoisin = module_id - 1;
  if (idVoisin <= 0) {
    Serial.println("[AVERTISSEMENT] Aucun voisin inférieur possible (ID trop bas).");
  }
}

void loop() {
  if (LoRa.parsePacket()) {
    String message = "";
    while (LoRa.available()) {
      message += (char)LoRa.read();
    }

    if (message.startsWith("To=") && message.indexOf("From=") >= 0) {
      int toID = message.substring(message.indexOf("To=") + 3, message.indexOf("From=") - 1).toInt();
      int fromID = message.substring(message.indexOf("From=") + 5, message.indexOf(';')).toInt();
      String data = message.substring(message.indexOf(';') + 1);

      if (toID == module_id && fromID == module_id + 1) {
        Serial.printf("[P2P] Message reçu depuis le voisin ID=%d → Données : %s\n", fromID, data.c_str());
      } else {
        Serial.printf("[P2P] Message ignoré (pas de voisin direct) : reçu de ID=%d vers ID=%d\n", fromID, toID);
      }
    } else {
      Serial.println("[P2P] Message non valide reçu (format incorrect)");
    }
  }

  if (millis() - lastSend >= SEND_INTERVAL) {
    lastSend = millis();

    float temperature = random(150, 300) / 10.0;
    float windSpeed = random(0, 150) / 10.0;
    float windDir = random(0, 360);

    if (idVoisin > 0) {
      String msg = "To=" + String(idVoisin) + " From=" + String(module_id);
      msg += ";T=" + String(temperature);
      msg += ";V=" + String(windSpeed);
      msg += ";D=" + String(windDir);

      LoRa.beginPacket();
      LoRa.print(msg);
      LoRa.endPacket();

      Serial.printf("[P2P] Données simulées envoyées à ID=%d → T=%.2f°C, V=%.2f km/h, D=%.2f°\n", idVoisin, temperature, windSpeed, windDir);
    } else {
      Serial.println("[INFO] Aucun module inférieur détecté ou ID trop bas.");
    }
  }
}
