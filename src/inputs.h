#pragma once

#include <Arduino.h>
#include <Atmega328p-io.h>
#include <JC_Button.h>
#include <Rotary.h>
#include "hardware.h"
#include "input-action.h"

#define DEBOUNCE_MS 20

class Inputs {
   public:
    Rotary enc1{PIN_ENC1_1, PIN_ENC1_2};
    Rotary enc2{PIN_ENC2_1, PIN_ENC2_2};
    Rotary enc3{PIN_ENC3_1, PIN_ENC3_2};
    // Button btn_enc1{PIN_ENC1_B, DEBOUNCE_MS, false, false};
    // Button btn_enc2{PIN_ENC2_B, DEBOUNCE_MS, false, false};
    // Button btn_enc3{PIN_ENC3_B, DEBOUNCE_MS, false, false};
    Button btn1{PIN_BTN1, DEBOUNCE_MS, false, false};
    Button btn2{PIN_BTN2, DEBOUNCE_MS, false, false};
    Button btn3{PIN_BTN3, DEBOUNCE_MS, false, false};
    Button btn4{PIN_BTN4, DEBOUNCE_MS, false, false};
    Button btn5{PIN_BTN5, DEBOUNCE_MS, false, false};

    int8_t enc1d{0};
    int8_t enc2d{0};
    int8_t enc3d{0};

    bool inputsChanged = true;

    void init();
    InputAction checkInputs();
    InputAction processInputs();

    void pcint0Handler();
    void pcint1Handler();
    void pcint2Handler();

   private:
    uint32_t ms{0};
};
