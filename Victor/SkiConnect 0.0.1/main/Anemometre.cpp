#include "Anemometre.h"

Anemometre::Anemometre(int rxPin) : _rxPin(rxPin) {
    Serial1.begin(4800, SERIAL_8N1, _rxPin);
}

void Anemometre::initialize() {
    Serial.begin(115200);
    Serial.println("🚀 [Anemometer] Initialization completed!");
}

void Anemometre::readFrame() {
    if (Serial1.available()) {
        String frame = Serial1.readStringUntil('\n');
        frame.trim();

        if (frame.startsWith("$IIMWV")) {
            float newWindSpeed;
            sscanf(frame.c_str(), "$IIMWV,%f,R,%f,N,A", &windDirection, &newWindSpeed);

            // Convert m/s to km/h
            newWindSpeed *= 3.6;

            // Circular update of the last three values
            lastSpeeds[speedIndex] = newWindSpeed;
            speedIndex = (speedIndex + 1) % 3;

            windSpeed = newWindSpeed; // Latest wind speed value

            Serial.printf("📡 Parsed Frame: Wind Speed = %.2f km/h, Wind Direction = %.2f°\n", windSpeed, windDirection);
        } 
        else if (frame.startsWith("$WIXDR")) {
            sscanf(frame.c_str(), "$WIXDR,C,%f,C", &temperature);
            
            // ✅ Adjust the displayed temperature (-6°C)
            Serial.printf("📡 Parsed Frame: Temperature = %.2f°C\n", temperature - 6.0);
        }
    }
}

// ✅ Ensure the temperature returned is already corrected
float Anemometre::getTemperature() const { return temperature - 6.0; }

float Anemometre::getWindSpeed() const { return windSpeed; }
float Anemometre::getWindDirection() const { return windDirection; }

float Anemometre::getMedianWindSpeed() const {
    float sortedValues[3] = {lastSpeeds[0], lastSpeeds[1], lastSpeeds[2]};

    // Bubble Sort to find the median
    for (int i = 0; i < 2; i++) {
        for (int j = i + 1; j < 3; j++) {
            if (sortedValues[i] > sortedValues[j]) {
                float temp = sortedValues[i];
                sortedValues[i] = sortedValues[j];
                sortedValues[j] = temp;
            }
        }
    }

    return sortedValues[1]; // Return the median value
}
