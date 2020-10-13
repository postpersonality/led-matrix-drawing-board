#pragma once

#include <Arduino.h>

enum struct InputActionType {
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
    setDrawMode,
    toggleSymModeVer,
    toggleSymModeHor
};

struct InputAction {
    InputActionType type{InputActionType::none};
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
