#include "paletted-buffer.h"

PalettedBuffer::PalettedBuffer(DisplayBuffer* displayBuffer, Palette* palette) {
    this->displayBuffer = displayBuffer;
    this->palette = palette;
    memset(buffer, (PALETTE_BG_COLOR_INDEX << 4) + PALETTE_BG_COLOR_INDEX, BUFFER_SIZE);
}

void PalettedBuffer::render() {
    uint16_t ledIndex = 0;
    uint8_t colorIndex = 0;
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        if (i & 0x10) {
            colorIndex = getByOffset(i);
            this->displayBuffer->leds[ledIndex] = this->palette->colors[colorIndex];
            ledIndex--;
            if ((i & 0x0f) == 0x0f) {
                ledIndex += 17;
            }
        } else {
            colorIndex = getByOffset(i);
            this->displayBuffer->leds[ledIndex] = this->palette->colors[colorIndex];
            ledIndex++;
            if ((i & 0x0f) == 0x0f) {
                ledIndex += 15;
            }
        }
    }
}

uint8_t PalettedBuffer::getColorIndex(uint8_t x, uint8_t y) {
    return getByOffset(xyToOffset(x, y));
}

void PalettedBuffer::setColorIndex(uint8_t x, uint8_t y, uint8_t colorIndex) {
    this->displayBuffer->setPixel(x, y, this->palette->colors[colorIndex]);
    setByOffset(xyToOffset(x, y), colorIndex);
}

uint8_t PalettedBuffer::getByOffset(uint8_t offset) {
    return offset & 1 ? buffer[offset >> 1].colorIndex1 : buffer[offset >> 1].colorIndex0;
}

void PalettedBuffer::setByOffset(uint8_t offset, uint8_t colorIndex) {
    if (offset & 1) {
        buffer[offset >> 1].colorIndex1 = colorIndex;
    } else {
        buffer[offset >> 1].colorIndex0 = colorIndex;
    }
}

CompactPixel* PalettedBuffer::getCompactPixelRef(uint8_t bufferIndex) {
    return &buffer[bufferIndex];
}

inline uint8_t PalettedBuffer::xyToBufferIndex(uint8_t x, uint8_t y) {
    return (y << 3) + (x >> 1);
}
inline uint8_t PalettedBuffer::xyToOffset(uint8_t x, uint8_t y) {
    return (y << 4) | (x & 0x0f);
}
