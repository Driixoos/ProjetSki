#include "Anemometre.h"

Anemometre::Anemometre(int rxPin) : _rxPin(rxPin) {
    Serial1.begin(4800, SERIAL_8N1, _rxPin);
}

void Anemometre::initialiser() {
    Serial.begin(115200);
    Serial.println("🚀 [Anémomètre] Initialisation terminée !");
}

void Anemometre::lireTrame() {
    if (Serial1.available()) {
        String trame = Serial1.readStringUntil('\n'); // Correction du double échappement
        trame.trim();

        if (trame.startsWith("$IIMWV")) {
            sscanf(trame.c_str(), "$IIMWV,%f,R,%f,N,A", &directionVent, &vitesseVent);
        } else if (trame.startsWith("$WIXDR")) {
            sscanf(trame.c_str(), "$WIXDR,C,%f,C", &temperature);
        }
    }
}

float Anemometre::getTemperature() const { return temperature; }
float Anemometre::getVitesseVent() const { return vitesseVent; }
float Anemometre::getDirectionVent() const { return directionVent; }