#include "palette.h"

Palette::Palette() {
    fill_rainbow(palette, NUM_PALETTE, 128, 16);
    for (uint8_t i; i < NUM_PALETTE; i++) {
        palette[i].s = 196;
        palette[i].v = 128;
    }
}

CHSV Palette::get(uint8_t index) {
    return palette[index];
}

void Palette::set(uint8_t index, CHSV color) {
    palette[index] = color;
}

void Palette::set(uint8_t index, uint8_t h, uint8_t s, uint8_t v) {
    palette[index].h = h;
    palette[index].s = s;
    palette[index].v = v;
}
