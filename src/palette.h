#pragma once

#include <Arduino.h>
#include <FastLED.h>

// Fixed to 16 strictly
#define PALETTE_BG_COLOR_INDEX 15
#define PALETTE_SIZE 16

class Palette {
   private:
    CHSV colors[PALETTE_SIZE];

   public:
    Palette();
    CHSV get(uint8_t index);
    CHSV* getRef(uint8_t index);
    void set(uint8_t index, CHSV color);
    void set(uint8_t index, uint8_t h, uint8_t s, uint8_t v);
    friend class PalettedBuffer;
};
