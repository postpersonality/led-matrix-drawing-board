#pragma once

#include <Arduino.h>
#include <FastLED.h>

// Fixed to 16 strictly
#define NUM_PALETTE 16

class Palette {
   private:
    CHSV palette[NUM_PALETTE];

   public:
    Palette();
    CHSV get(uint8_t index);
    void set(uint8_t index, CHSV color);
    void set(uint8_t index, uint8_t h, uint8_t s, uint8_t v);
};
