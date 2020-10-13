#pragma once

#include <Arduino.h>

enum InputActionType {
    none,
    changeFgHue,
    changeBgHue,
    changeY,
    changeFgSat,
    changeBgSat,
    changeX,
    changeFgVal,
    changeBgVal,
    changeColor,
    save,
    load,
    setMode,
    setModeDraw,
    setModeErase,
    setModeHover
};

struct InputAction {
    InputActionType type{none};
    int8_t value{0};

    inline InputAction() __attribute__((always_inline)) {
    }

    inline InputAction(InputActionType itype, int8_t ivalue) __attribute__((always_inline))
    : type(itype), value(ivalue) {
    }

    inline InputAction(InputActionType itype) __attribute__((always_inline))
    : type(itype), value(0) {
    }
};
