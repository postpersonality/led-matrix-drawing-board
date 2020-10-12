#include "paletted-buffer.h"

PalettedBuffer::PalettedBuffer(DisplayBuffer* displayBuffer, Palette* palette) {
    this->displayBuffer = displayBuffer;
    this->palette = palette;
    memset(buffer, (PALETTE_BG_COLOR_INDEX << 4) + PALETTE_BG_COLOR_INDEX, BUFFER_SIZE);
}

void PalettedBuffer::render() {
    uint16_t ledIndex = 0;
    uint8_t colorIndex = 0;
    for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
        if (i & 8) {
            colorIndex = buffer[i].colorIndex0;
            this->displayBuffer->leds[ledIndex] = this->palette->colors[colorIndex];
            ledIndex--;
            colorIndex = buffer[i].colorIndex1;
            this->displayBuffer->leds[ledIndex] = this->palette->colors[colorIndex];
            ledIndex--;
            if ((i & 7) == 7) {
                ledIndex += 17;
            }
        } else {
            colorIndex = buffer[i].colorIndex0;
            this->displayBuffer->leds[ledIndex] = this->palette->colors[colorIndex];
            ledIndex++;
            colorIndex = buffer[i].colorIndex1;
            this->displayBuffer->leds[ledIndex] = this->palette->colors[colorIndex];
            ledIndex++;
            if ((i & 7) == 7) {
                ledIndex += 15;
            }
        }
    }
}

uint8_t PalettedBuffer::getColorIndex(uint8_t x, uint8_t y) {
    CompactPixel cpixel = buffer[XY(x, y)];
    return x & 1 ? cpixel.colorIndex1 : cpixel.colorIndex0;
}

void PalettedBuffer::setColorIndex(uint8_t x, uint8_t y, uint8_t colorIndex) {
    this->displayBuffer->setPixel(x, y, this->palette->colors[colorIndex]);
    uint8_t bufferOffset = XY(x, y);
    if (x & 1) {
        buffer[bufferOffset].colorIndex1 = colorIndex;
    } else {
        buffer[bufferOffset].colorIndex0 = colorIndex;
    }
}

// Optimized for 16x16
inline uint8_t PalettedBuffer::XY(uint8_t x, uint8_t y) {
    return (y << 3) + (x >> 1);
}
