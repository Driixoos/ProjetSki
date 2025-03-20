#ifndef ANEMOMETRE_H
#define ANEMOMETRE_H

#include <Arduino.h>

/**
 * @brief Anemometer class for measuring wind speed, direction, and temperature.
 */
class Anemometre {
public:
    /**
     * @brief Constructor to initialize the anemometer with the RX pin.
     * @param rxPin Pin used for serial communication with the anemometer.
     */
    explicit Anemometre(int rxPin);

    /**
     * @brief Initializes the anemometer.
     */
    void initialize();

    /**
     * @brief Reads a data frame from the anemometer.
     */
    void readFrame();

    /**
     * @brief Returns the current temperature (corrected by -6°C).
     * @return Temperature in degrees Celsius.
     */
    float getTemperature() const;

    /**
     * @brief Returns the current wind speed.
     * @return Wind speed in km/h.
     */
    float getWindSpeed() const;

    /**
     * @brief Returns the wind direction.
     * @return Wind direction in degrees.
     */
    float getWindDirection() const;

    /**
     * @brief Calculates the median wind speed based on the last three readings.
     * @return Median wind speed in km/h.
     */
    float getMedianWindSpeed() const;

private:
    int _rxPin;  // RX pin for communication

    float temperature = 0.0;
    float windSpeed = 0.0;
    float windDirection = 0.0;

    float lastSpeeds[3] = {0.0, 0.0, 0.0};  // Stores last 3 wind speed values
    int speedIndex = 0;  // Circular index for updating values
};

#endif // ANEMOMETRE_H
