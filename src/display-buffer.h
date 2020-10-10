#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "hardware.h"

#define NUM_LEDS 256
#define BRIGHTNESS 128

class DisplayBuffer {
   public:
    DisplayBuffer();
    void init();
    void show();
    CRGB getPixel(uint8_t x, uint8_t y);
    void setPixel(uint8_t x, uint8_t y, CRGB pixel);
    void setPixel(uint8_t x, uint8_t y, CHSV pixel);

   private:
    CRGB leds[NUM_LEDS];
    uint16_t XY(uint8_t x, uint8_t y);
};
