#pragma once

#include <Arduino.h>
#include "display-buffer.h"
#include "palette.h"

#define BUFFER_SIZE NUM_LEDS >> 1

struct CompactPixel {
    union {
        struct {
            uint8_t colorIndex0 : 4;
            uint8_t colorIndex1 : 4;
        };
        uint8_t compactPixel;
    };
};

class PalettedBuffer {
   public:
    PalettedBuffer(DisplayBuffer* displayBuffer, Palette* palette);
    void render();
    uint8_t getColorIndex(uint8_t x, uint8_t y);
    void setColorIndex(uint8_t x, uint8_t y, uint8_t pixel);
    uint8_t getByOffset(uint8_t offset);
    void setByOffset(uint8_t offset, uint8_t colorIndex);
    CompactPixel* getCompactPixelRef(uint8_t bufferIndex);

   private:
    CompactPixel buffer[BUFFER_SIZE];
    DisplayBuffer* displayBuffer;
    Palette* palette;
    inline uint8_t xyToBufferIndex(uint8_t x, uint8_t y);
    inline uint8_t xyToOffset(uint8_t x, uint8_t y);
};
