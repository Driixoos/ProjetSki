#ifndef ANEMOMETRE_H
#define ANEMOMETRE_H

#include <Arduino.h>

class Anemometre {
public:
    Anemometre(int rxPin);
    void initialize();
    void readFrame();
    float getTemperature() const;
    float getWindSpeed() const;
    float getWindDirection() const;
    float getMedianWindSpeed() const; // Returns median wind speed

private:
    int _rxPin;
    float temperature = 0.0;
    float windSpeed = 0.0;
    float windDirection = 0.0;

    float lastSpeeds[3] = {0.0, 0.0, 0.0}; // Stores last 3 wind speed values
    int speedIndex = 0; // Circular index for updating values
};

#endif // ANEMOMETRE_H
