#include "Anemometre.h"

/**
 * @brief Constructor that initializes the serial communication.
 * @param rxPin The RX pin used for receiving data from the anemometer.
 */
Anemometre::Anemometre(int rxPin) : _rxPin(rxPin) {
    Serial1.begin(4800, SERIAL_8N1, _rxPin);
}

/**
 * @brief Initializes the anemometer module.
 */
void Anemometre::initialize() {
    Serial.begin(115200);
    Serial.println("🚀 [Anemometer] Initialization completed!");
}

/**
 * @brief Reads a frame of data from the anemometer and extracts wind speed, wind direction, and temperature.
 */
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

/**
 * @brief Returns the corrected temperature (-6°C).
 * @return Adjusted temperature in Celsius.
 */
float Anemometre::getTemperature() const { 
    return temperature - 6.0; 
}

/**
 * @brief Returns the last recorded wind speed.
 * @return Wind speed in km/h.
 */
float Anemometre::getWindSpeed() const { 
    return windSpeed; 
}

/**
 * @brief Returns the last recorded wind direction.
 * @return Wind direction in degrees.
 */
float Anemometre::getWindDirection() const { 
    return windDirection; 
}

/**
 * @brief Computes the median wind speed from the last three readings.
 * @return Median wind speed in km/h.
 */
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
