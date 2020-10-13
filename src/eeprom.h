#pragma once

#include <Arduino.h>
#include "paletted-buffer.h"

class Eeprom {
   public:
    static void save(PalettedBuffer* buffer, uint16_t numberOfColorIndices, Palette* palette, uint8_t paletteSize);
    static void load(PalettedBuffer* buffer, uint16_t numberOfColorIndices, Palette* palette, uint8_t paletteSize);

   private:
    static uint16_t estimateCompressedSize(PalettedBuffer* buffer, uint16_t numberOfColorIndices);
    static uint16_t saveCompressedBoard(PalettedBuffer* buffer, uint16_t numberOfColorIndices, uint16_t address);
    static uint16_t loadCompressedBoard(PalettedBuffer* buffer, uint8_t compressedSize, uint16_t address);
};
