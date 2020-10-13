#include <Atmega328p-io.h>
#include <FastLED.h>
#include <JC_Button.h>
#include <Rotary.h>
#include "display-buffer.h"
#include "encoder.h"
#include "hardware.h"
#include "palette.h"
#include "paletted-buffer.h"

#define DEBOUNCE_MS 20

DisplayBuffer displayBuffer;
Palette palette;
PalettedBuffer palettedBuffer(&displayBuffer, &palette);

Rotary enc1(PIN_ENC1_1, PIN_ENC1_2);
Rotary enc2(PIN_ENC2_1, PIN_ENC2_2);
Rotary enc3(PIN_ENC3_1, PIN_ENC3_2);
// Button btn_enc1(PIN_ENC1_B, DEBOUNCE_MS, false, false);
// Button btn_enc2(PIN_ENC2_B, DEBOUNCE_MS, false, false);
// Button btn_enc3(PIN_ENC3_B, DEBOUNCE_MS, false, false);
Button btn1(PIN_BTN1, DEBOUNCE_MS, false, false);
Button btn2(PIN_BTN2, DEBOUNCE_MS, false, false);
Button btn3(PIN_BTN3, DEBOUNCE_MS, false, false);
Button btn4(PIN_BTN4, DEBOUNCE_MS, false, false);
Button btn5(PIN_BTN5, DEBOUNCE_MS, false, false);

uint8_t cursorColorIndex;
uint8_t fgColorIndex = 0;
uint8_t bgColorIndex = PALETTE_BG_COLOR_INDEX;

int8_t enc1d = 0;
int8_t enc2d = 0;
int8_t enc3d = 0;

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
#define MODE_HOVER 0
#define MODE_DRAW 1
#define MODE_ERASE 2
uint8_t drawMode = MODE_DRAW;

uint8_t count = 0;
uint8_t countPrev = 0;

inline void restoreCursor() { palettedBuffer.setColorIndex(cx, cy, cursorColorIndex); }

inline void storeCursor() {
    cx = x;
    cy = y;
    cursorColorIndex = palettedBuffer.getColorIndex(cx, cy);
}

ISR(PCINT2_vect) {
    uint8_t result =
        enc1.process(digitalState(PIN_ENC1_1), digitalState(PIN_ENC1_2));
    if (result == DIR_NONE) {
    } else if (result == DIR_CW) {
        enc1d++;
    } else {
        enc1d--;
    }
    result = enc2.process(digitalState(PIN_ENC2_1), digitalState(PIN_ENC2_2));
    if (result == DIR_NONE) {
    } else if (result == DIR_CW) {
        enc2d++;
    } else {
        enc2d--;
    }
    inputsChanged = true;
}

ISR(PCINT0_vect) {
    uint8_t result =
        enc3.process(digitalState(PIN_ENC3_1), digitalState(PIN_ENC3_2));
    if (result == DIR_NONE) {
    } else if (result == DIR_CW) {
        enc3d++;
    } else {
        enc3d--;
    }
    inputsChanged = true;
}

ISR(PCINT1_vect) { inputsChanged = true; }

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
inline uint8_t enc(uint8_t x, uint8_t d, uint8_t result) {
    if (result == DIR_NONE) {
        return x;
    } else if (result == DIR_CW) {
        return dec(x, d);
    } else {
        return inc(x, d);
    }
}

void processInputs() {
    CHSV* color;
    if (enc1d != 0) {
        if (btn4.isPressed()) {
            color = palette.getRef(fgColorIndex);
            color->h -= enc1d * 4;
            redrawRequired = true;
        } else if (btn5.isPressed()) {
            color = palette.getRef(bgColorIndex);
            color->h -= enc1d * 4;
            redrawRequired = true;
        } else {
            cy = y;
            y = (y - enc1d) & 0x0f;
            cursorChanged = true;
        }
        enc1d = 0;
    }
    if (enc2d != 0) {
        if (btn4.isPressed()) {
            color = palette.getRef(fgColorIndex);
            color->s = sum(color->s, enc2d * 10);
            redrawRequired = true;
        } else if (btn5.isPressed()) {
            color = palette.getRef(bgColorIndex);
            color->s = sum(color->s, enc2d * 10);
            redrawRequired = true;
        } else {
            cx = x;
            x = (x + enc2d) & 0x0f;
            cursorChanged = true;
        }
        enc2d = 0;
    }
    if (enc3d != 0) {
        if (btn4.isPressed()) {
            color = palette.getRef(fgColorIndex);
            color->v = sum(color->v, enc3d * 10);
            redrawRequired = true;
        } else if (btn5.isPressed()) {
            color = palette.getRef(bgColorIndex);
            color->v = sum(color->v, enc3d * 10);
            redrawRequired = true;
        } else {
            fgColorIndex = (fgColorIndex + enc3d) % (PALETTE_SIZE - 1);
        }
        enc3d = 0;
    }
    if (btn1.isPressed()) {
        drawMode = MODE_DRAW;
    }
    if (btn2.isPressed()) {
        drawMode = MODE_ERASE;
    }
    if (btn3.isPressed()) {
        drawMode = MODE_HOVER;
    }
}

void draw() {
    switch (drawMode) {
        case MODE_HOVER:
            cursorBlinkTicks = CURSOR_BLINK_TICKS_MAX >> 1;
            displayChanged = true;
            if (cursorChanged) {
                restoreCursor();
                storeCursor();
                cursorChanged = false;
            }
            palettedBuffer.setColorIndex(x, y, fgColorIndex);
            return;
        case MODE_DRAW:
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
        case MODE_ERASE:
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

void setup() {
    // btn_enc1.begin();
    // btn_enc2.begin();
    // btn_enc3.begin();
    btn1.begin();
    btn2.begin();
    btn3.begin();
    btn4.begin();
    btn5.begin();
    displayBuffer.init();
    PCICR |= (1 << PCIE0) | (1 << PCIE1) | (1 << PCIE2);
    PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20);  // ENC1
    PCMSK2 |= (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23);  // ENC2
    PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2);     // ENC3
    PCMSK0 |= (1 << PCINT4);                                     // BTN1
    PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10) |
              (1 << PCINT11);  // BTN2-5
    sei();

    storeCursor();
}

uint32_t ms = 0;
void checkInputs() {
    ms = millis();
    btn1.read(ms);
    btn2.read(ms);
    btn3.read(ms);
    btn4.read(ms);
    btn5.read(ms);
    processInputs();
}

void loop() {
    if (inputsChanged) {
        checkInputs();
        draw();
        inputsChanged = false;
    }

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
    delay(20);
}
