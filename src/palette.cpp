#include "palette.h"

Palette::Palette() {
    fill_rainbow(colors, PALETTE_SIZE, 128, PALETTE_SIZE - 1);
    for (uint8_t i = 0; i < PALETTE_SIZE - 1; i++) {
        colors[i].s = 160;
        colors[i].v = 128;
    }
    colors[PALETTE_BG_COLOR_INDEX] = CHSV(0, 0, 16);
}

CHSV Palette::get(uint8_t index) {
    return colors[index];
}

CHSV* Palette::getRef(uint8_t index) {
    return &colors[index];
}

void Palette::set(uint8_t index, CHSV color) {
    colors[index] = color;
}

void Palette::set(uint8_t index, uint8_t h, uint8_t s, uint8_t v) {
    colors[index].h = h;
    colors[index].s = s;
    colors[index].v = v;
}
