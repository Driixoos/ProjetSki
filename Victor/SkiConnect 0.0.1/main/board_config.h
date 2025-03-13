#pragma once

#define BRD_sx1262_radio 1
#define RADIO_SCLK_PIN  5
#define RADIO_MISO_PIN  3
#define RADIO_MOSI_PIN  6
#define RADIO_CS_PIN    7
#define RADIO_DIO1_PIN  33
#define RADIO_RST_PIN   8

const lmic_pinmap lmic_pins = {
    .nss  = RADIO_CS_PIN,
    .rst  = RADIO_RST_PIN,
    .dio  = {LMIC_UNUSED_PIN, RADIO_DIO1_PIN, LMIC_UNUSED_PIN}
};
