#include <FastLED.h>

#include "display-buffer.h"
#include "draw-mode.h"
#include "eeprom.h"
#include "inputs.h"
#include "palette.h"
#include "paletted-buffer.h"

DisplayBuffer displayBuffer;
Palette palette;
PalettedBuffer palettedBuffer(&displayBuffer, &palette);
Inputs inputs;

uint8_t cursorColorIndex;
uint8_t fgColorIndex = 0;
uint8_t bgColorIndex = PALETTE_BG_COLOR_INDEX;

uint8_t x = 7;
uint8_t y = 7;
uint8_t cx = 7;
uint8_t cy = 7;

bool inputsChanged = true;
bool displayChanged = true;
bool redrawRequired = true;
bool cursorChanged = true;
#define CURSOR_BLINK_TICKS_MAX 15
uint8_t cursorBlinkTicks = CURSOR_BLINK_TICKS_MAX;
DrawMode drawMode = DrawMode::draw;

uint8_t count = 0;
uint8_t countPrev = 0;

inline void restoreCursor() { palettedBuffer.setColorIndex(cx, cy, cursorColorIndex); }

inline void storeCursor() {
    cx = x;
    cy = y;
    cursorColorIndex = palettedBuffer.getColorIndex(cx, cy);
}

void draw() {
    switch (drawMode) {
        case DrawMode::hover:
            cursorBlinkTicks = CURSOR_BLINK_TICKS_MAX >> 1;
            displayChanged = true;
            if (cursorChanged) {
                restoreCursor();
                storeCursor();
                cursorChanged = false;
            }
            palettedBuffer.setColorIndex(x, y, fgColorIndex);
            return;
        case DrawMode::draw:
            cursorBlinkTicks = CURSOR_BLINK_TICKS_MAX;
            displayChanged = true;
            if (cursorChanged) {
                cursorColorIndex = fgColorIndex;
                restoreCursor();
                storeCursor();
                cursorChanged = false;
            }
            palettedBuffer.setColorIndex(x, y, fgColorIndex);
            return;
        case DrawMode::erase:
            cursorBlinkTicks = 1;
            displayChanged = true;
            if (cursorChanged) {
                cursorColorIndex = bgColorIndex;
                restoreCursor();
                storeCursor();
                cursorChanged = false;
            }
            palettedBuffer.setColorIndex(x, y, bgColorIndex);
            return;
    }
}

inline uint8_t encunl(uint8_t x, uint8_t d, uint8_t result) {
    if (result == DIR_NONE) {
        return x;
    } else if (result == DIR_CW) {
        return x - d;
    } else {
        return x + d;
    }
}
inline uint8_t dec(uint8_t x, uint8_t d) { return x > d ? x - d : 0; }
inline uint8_t inc(uint8_t x, uint8_t d) {
    uint8_t z = x + d;
    return z < x || z < d ? 255 : z;
}
inline uint8_t sum(uint8_t x, int8_t d) {
    if (d > 0) {
        return inc(x, d);
    } else {
        return dec(x, -d);
    }
}

void processInputs() {
    InputAction action = inputs.checkInputs();
    CHSV* color;
    switch (action.type) {
        case InputActionType::none:
            return;
        case InputActionType::changeX:
            cx = x;
            x = (x + action.value) & 0x0f;
            cursorChanged = true;
            break;
        case InputActionType::changeY:
            cy = y;
            y = (y - action.value) & 0x0f;
            cursorChanged = true;
            break;
        case InputActionType::changeColor:
            fgColorIndex = (fgColorIndex + action.value) % (PALETTE_SIZE - 1);
            break;
        case InputActionType::changeFgHue:
            color = palette.getRef(fgColorIndex);
            color->h -= action.value * 4;
            redrawRequired = true;
            break;
        case InputActionType::changeBgHue:
            color = palette.getRef(bgColorIndex);
            color->h -= action.value * 4;
            redrawRequired = true;
            break;
        case InputActionType::changeFgSat:
            color = palette.getRef(fgColorIndex);
            color->s = sum(color->s, action.value * 10);
            redrawRequired = true;
            break;
        case InputActionType::changeBgSat:
            color = palette.getRef(bgColorIndex);
            color->s = sum(color->s, action.value * 10);
            redrawRequired = true;
            break;
        case InputActionType::changeFgVal:
            color = palette.getRef(fgColorIndex);
            color->v = sum(color->v, action.value * 10);
            redrawRequired = true;
            break;
        case InputActionType::changeBgVal:
            color = palette.getRef(bgColorIndex);
            color->v = sum(color->v, action.value * 10);
            redrawRequired = true;
            break;
        case InputActionType::setMode:
            drawMode = (DrawMode)action.value;
            break;
        case InputActionType::save:
            Eeprom::save(&palettedBuffer, NUM_LEDS, &palette, PALETTE_SIZE);
            break;
        case InputActionType::load:
            Eeprom::load(&palettedBuffer, NUM_LEDS, &palette, PALETTE_SIZE);
            displayChanged = true;
            redrawRequired = true;
            storeCursor();
            break;
        default:
            return;
    }
    draw();
}

void setup() {
    displayBuffer.init();
    inputs.init();
    storeCursor();
}

void loop() {
    processInputs();

    // Cursor blinking
    countPrev = count;
    count = (count + 1) & CURSOR_BLINK_TICKS_MAX;
    if (count >= cursorBlinkTicks && countPrev < cursorBlinkTicks) {
        palettedBuffer.setColorIndex(x, y, bgColorIndex);
        displayChanged = true;
    } else if (count < cursorBlinkTicks && countPrev >= cursorBlinkTicks) {
        palettedBuffer.setColorIndex(x, y, fgColorIndex);
        displayChanged = true;
    }

    if (displayChanged) {
        if (redrawRequired) {
            palettedBuffer.render();
            redrawRequired = false;
        }
        displayBuffer.show();
        displayChanged = false;
    }
    delay(DEBOUNCE_MS);
}

ISR(PCINT0_vect) {
    inputs.pcint0Handler();
}

ISR(PCINT1_vect) {
    inputs.pcint1Handler();
}

ISR(PCINT2_vect) {
    inputs.pcint2Handler();
}
