#include <FastLED.h>
#include <atmega328p-io.h>
#include <Rotary.h>
#include <JC_Button-fast.h>

#define PIN_ENC1_1 2
#define PIN_ENC1_2 3
#define PIN_ENC1_B 4
#define PIN_ENC2_1 5
#define PIN_ENC2_2 6
#define PIN_ENC2_B 7
#define PIN_ENC3_1 8
#define PIN_ENC3_2 9
#define PIN_ENC3_B 10
#define PIN_BTN1 A0
#define PIN_BTN2 12
#define PIN_BTN3 A1
#define PIN_BTN4 A2
#define PIN_BTN5 A3
#define PIN_LED_MATRIX 11

#define DEBOUNCE_MS 20

Rotary enc1(PIN_ENC1_1, PIN_ENC1_2);
Rotary enc2(PIN_ENC2_1, PIN_ENC2_2);
Rotary enc3(PIN_ENC3_1, PIN_ENC3_2);
Button btn_enc1(PIN_ENC1_B, DEBOUNCE_MS, false, false);
Button btn_enc2(PIN_ENC2_B, DEBOUNCE_MS, false, false);
Button btn_enc3(PIN_ENC3_B, DEBOUNCE_MS, false, false);
Button btn1(PIN_BTN1, DEBOUNCE_MS, false, false);
Button btn2(PIN_BTN2, DEBOUNCE_MS, false, false);
Button btn3(PIN_BTN3, DEBOUNCE_MS, false, false);
Button btn4(PIN_BTN4, DEBOUNCE_MS, false, false);
Button btn5(PIN_BTN5, DEBOUNCE_MS, false, false);

struct 

#define NUM_LEDS 256
CRGB leds[NUM_LEDS];
CRGB cursorColor;

// Fixed to 16!
#define NUM_PALETTE 16
CHSV palette[NUM_PALETTE]; 

#define BRIGHTNESS 128

void setup() {
  btn_enc1.begin();
  btn_enc2.begin();
  btn_enc3.begin();
  btn1.begin();
  btn2.begin();
  btn3.begin();
  btn4.begin();
  btn5.begin();
  FastLED.addLeds<WS2812B, PIN_LED_MATRIX, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(DISABLE_DITHER);
  PCICR |= (1 << PCIE0) | (1 << PCIE1) | (1 << PCIE2);
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20); // ENC1
  PCMSK2 |= (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23); // ENC2
  PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2);    // ENC3
  PCMSK0 |= (1 << PCINT4); // BTN1
  PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10) | (1 << PCINT11); // BTN2-5
  sei();

  generatePalette();
  loadColor();
  storeCursor();
}

int8_t enc1d = 0;
int8_t enc2d = 0;
int8_t enc3d = 0;

uint8_t h = 128;
uint8_t s = 196;
uint8_t v = 196;
uint8_t bh = 0;
uint8_t bs = 0;
uint8_t bv = 0;
uint8_t c = 0;
uint8_t x = 7;
uint8_t y = 7;
uint8_t cx = 7;
uint8_t cy = 7;

bool inputsChanged = true; 
bool displayChanged = true;
bool cursorChanged = true;
#define MODE_HOVER 0
#define MODE_DRAW 1
#define MODE_ERASE 2
uint8_t drawMode = MODE_DRAW;

uint8_t count = 0;
void loop() {
  if (inputsChanged) {
    checkInputs();
    draw();
    inputsChanged = false;
  }
  count = (count + 1) & 7;
  if (!(count & 3)) {
    if (count & 4) {
      leds[XY(x, y)] = CHSV(bh, bs, bv);
    } else {
      leds[XY(x, y)] = palette[c];
    }
    displayChanged = true;
  }
  if (displayChanged) {
    FastLED.show();
    displayChanged = false;
  }
  delay(20);
}

void processInputs() {
  if (enc1d != 0) {
    if (btn4.isPressed()) {
      h = h - enc1d * 4;
    } else if (btn5.isPressed()) {
      bh = bh - enc1d * 4;
    } else {
      cy = y;
      y = (y - enc1d) & 0x0f;
      cursorChanged = true;
    }
    enc1d = 0;
  }
  if (enc2d != 0) {
    if (btn4.isPressed()) {
      s = sum(s, enc2d * 10);
    } else if (btn5.isPressed()) {
      bs = sum(bs, enc2d * 10);
    } else {
      cx = x;
      x = (x + enc2d) & 0x0f;
      cursorChanged = true;
    }
    enc2d = 0;
  }
  if (enc3d != 0) {
    if (btn4.isPressed()) {
      v = sum(v, enc3d * 10);
    } else if (btn5.isPressed()) {
      bv = sum(bv, enc3d * 10);
    } else {
      c = (c + enc3d) % NUM_PALETTE;
      loadColor();
    }
    enc3d = 0;
  }
  if(btn1.isPressed()) {
    drawMode = MODE_DRAW;
  }
  if(btn2.isPressed()) {
    drawMode = MODE_ERASE;
  }
  if(btn3.isPressed()) {
    drawMode = MODE_HOVER;
  }
}

void draw() {
  switch (drawMode) {
    case MODE_HOVER:
      displayChanged = true;
      if (cursorChanged) {
        restoreCursor();
        storeCursor();
        cursorChanged = false;
      }
      leds[XY(x, y)] = palette[c];
      return;
    case MODE_DRAW:
      displayChanged = true;
      if (cursorChanged) {
        cursorColor = CHSV(h, s, v);
        restoreCursor();
        storeCursor();
        cursorChanged = false;
      }
      palette[c] = CHSV(h, s, v);
      leds[XY(x, y)] = palette[c];
      return;
    case MODE_ERASE:
      displayChanged = true;
      if (cursorChanged) {
        cursorColor = CHSV(bh, bs, bv);
        restoreCursor();
        storeCursor();
        cursorChanged = false;
      }
      leds[XY(x, y)] = CHSV(bh, bs, bv);
      return;
  }
}

inline void restoreCursor() {
  leds[XY(cx, cy)] = cursorColor;
}

inline void storeCursor() {
  cx = x;
  cy = y;
  cursorColor = leds[XY(cx, cy)];
}

inline void loadColor() {
  h = palette[c].h;
  s = palette[c].s;
  v = palette[c].v;
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

ISR(PCINT2_vect) {
  uint8_t result = enc1.process(digitalState(PIN_ENC1_1), digitalState(PIN_ENC1_2));
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
  uint8_t result = enc3.process(digitalState(PIN_ENC3_1), digitalState(PIN_ENC3_2));
  if (result == DIR_NONE) {
  } else if (result == DIR_CW) {
    enc3d++;
  } else {
    enc3d--;
  }
  inputsChanged = true;
}

ISR(PCINT1_vect) {
  inputsChanged = true;
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
inline uint8_t encunl(uint8_t x, uint8_t d, uint8_t result) {
  if (result == DIR_NONE) {
    return x;
  } else if (result == DIR_CW) {
    return x - d;
  } else {
    return x + d;
  }
}
inline uint8_t dec(uint8_t x, uint8_t d) {
  return x > d ? x - d : 0;
}
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

uint16_t XY(uint8_t x, uint8_t y)
{
  return y & 0x01 
    ? (y << 4) + 15 - x
    : (y << 4) + x;
}

void generatePalette() {
  fill_rainbow(palette, NUM_PALETTE, BRIGHTNESS, 16);
  for (uint8_t i; i < NUM_PALETTE; i++) {
    palette[i].s = 128;
    palette[i].v = 128;
  }
}
