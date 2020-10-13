#include "inputs.h"
#include "draw-mode.h"

void Inputs::pcint2Handler() {
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

void Inputs::pcint0Handler() {
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

void Inputs::pcint1Handler() {
    inputsChanged = true;
}

InputAction Inputs::processInputs() {
    if (enc1d != 0) {
        if (btn4.isPressed()) {
            return InputAction(InputActionType::changeFgHue, enc1d);
        } else if (btn5.isPressed()) {
            return InputAction(InputActionType::changeBgHue, enc1d);
        } else {
            return InputAction(InputActionType::changeY, enc1d);
        }
    }
    if (enc2d != 0) {
        if (btn4.isPressed()) {
            return InputAction(InputActionType::changeFgSat, enc2d);
        } else if (btn5.isPressed()) {
            return InputAction(InputActionType::changeBgSat, enc2d);
        } else {
            return InputAction(InputActionType::changeX, enc2d);
        }
    }
    if (enc3d != 0) {
        if (btn4.isPressed()) {
            return InputAction(InputActionType::changeFgVal, enc3d);
        } else if (btn5.isPressed()) {
            return InputAction(InputActionType::changeBgVal, enc3d);
        } else {
            return InputAction(InputActionType::changeColor, enc3d);
        }
    }
    if (btn5.isPressed()) {
        if (btn4.isPressed()) {
            if (btn1.isPressed()) {
                return InputAction(InputActionType::save);
            } else if (btn2.isPressed()) {
                return InputAction(InputActionType::load);
            }
        }
    } else {
        if (btn1.isPressed()) {
            return InputAction(InputActionType::setMode, (int8_t)DrawMode::draw);
        }
        if (btn2.isPressed()) {
            return InputAction(InputActionType::setMode, (int8_t)DrawMode::erase);
        }
        if (btn3.isPressed()) {
            return InputAction(InputActionType::setMode, (int8_t)DrawMode::hover);
        }
    }
    return InputAction();
}

InputAction Inputs::checkInputs() {
    if (!inputsChanged) {
        return InputAction();
    }

    ms = millis();
    // btn_enc1.read(ms);
    // btn_enc2.read(ms);
    // btn_enc3.read(ms);
    btn1.read(ms);
    btn2.read(ms);
    btn3.read(ms);
    btn4.read(ms);
    btn5.read(ms);
    InputAction action = processInputs();
    if (action.type != InputActionType::none) {
        enc1d = 0;
        enc2d = 0;
        enc3d = 0;
    }
    return action;
}

void Inputs::init() {
    // btn_enc1.begin();
    // btn_enc2.begin();
    // btn_enc3.begin();
    btn1.begin();
    btn2.begin();
    btn3.begin();
    btn4.begin();
    btn5.begin();
    PCICR |= (1 << PCIE0) | (1 << PCIE1) | (1 << PCIE2);
    PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20);  // ENC1
    PCMSK2 |= (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23);  // ENC2
    PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2);     // ENC3
    PCMSK0 |= (1 << PCINT4);                                     // BTN1
    PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10) |
              (1 << PCINT11);  // BTN2-5
    sei();
}
