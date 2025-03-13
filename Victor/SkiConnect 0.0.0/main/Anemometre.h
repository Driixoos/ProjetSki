#ifndef ANEMOMETRE_H
#define ANEMOMETRE_H

#include <Arduino.h>

class Anemometre {
public:
    Anemometre(int rxPin);
    void initialiser();
    void lireTrame();
    float getTemperature() const;
    float getVitesseVent() const;
    float getDirectionVent() const;

private:
    int _rxPin;
    float temperature = 0.0;
    float vitesseVent = 0.0;
    float directionVent = 0.0;
};

#endif