#include "display-buffer.h"

DisplayBuffer::DisplayBuffer() {
    memset8(leds, 0, NUM_LEDS);
}

void DisplayBuffer::init() {
    FastLED.addLeds<WS2812B, PIN_LED_MATRIX, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.setDither(DISABLE_DITHER);
}
void DisplayBuffer::show() {
    FastLED.show();
}
CRGB DisplayBuffer::getPixel(uint8_t x, uint8_t y) {
    return leds[XY(x, y)];
}
void DisplayBuffer::setPixel(uint8_t x, uint8_t y, CRGB pixel) {
    leds[XY(x, y)] = pixel;
}
void DisplayBuffer::setPixel(uint8_t x, uint8_t y, CHSV pixel) {
    leds[XY(x, y)] = pixel;
}

// Optimized for 16x16
uint16_t DisplayBuffer::XY(uint8_t x, uint8_t y) {
    return y & 0x01 ? (y << 4) + 15 - x : (y << 4) + x;
}
