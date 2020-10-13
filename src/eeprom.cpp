#include "eeprom.h"
#include <EEPROM.h>

void Eeprom::save(PalettedBuffer* buffer, uint16_t numberOfColorIndices, Palette* palette, uint8_t paletteSize) {
    uint16_t address = 0;

    // Save palette
    for (uint8_t i = 0; i < paletteSize; i++) {
        auto ref = palette->getRef(i);
        EEPROM.update(address++, ref->h);
        EEPROM.update(address++, ref->s);
        EEPROM.update(address++, ref->v);
    }

    uint16_t compressedSize = estimateCompressedSize(buffer, numberOfColorIndices);
    if (compressedSize >= (numberOfColorIndices >> 1)) {
        // Save uncompressed flag
        EEPROM.update(address++, 0);
        // Save uncompressed board
        for (uint8_t i = 0; i < (numberOfColorIndices >> 1); i++) {
            EEPROM.update(address++, buffer->getCompactPixelRef(i)->compactPixel);
        }
    } else {
        // Save compressed length
        EEPROM.update(address++, (uint8_t)compressedSize);
        // Save compressed board
        address = saveCompressedBoard(buffer, numberOfColorIndices, address);
    }
}

void Eeprom::load(PalettedBuffer* buffer, uint16_t numberOfColorIndices, Palette* palette, uint8_t paletteSize) {
    uint16_t address = 0;

    // Palette
    for (uint8_t i = 0; i < paletteSize; i++) {
        auto ref = palette->getRef(i);
        ref->h = EEPROM.read(address++);
        ref->s = EEPROM.read(address++);
        ref->v = EEPROM.read(address++);
    }

    uint8_t compressedSize = EEPROM.read(address++);
    if (compressedSize) {
        // Compressed
        address = loadCompressedBoard(buffer, compressedSize, address);
    } else {
        // Uncompressed
        for (uint8_t i = 0; i < (numberOfColorIndices >> 1); i++) {
            buffer->getCompactPixelRef(i)->compactPixel = EEPROM.read(address++);
        }
    }
}

uint16_t Eeprom::estimateCompressedSize(PalettedBuffer* buffer, uint16_t numberOfColorIndices) {
    uint8_t colorIndexPrev = -1;
    uint8_t colorIndex = 0;
    uint8_t runCount = 0;
    uint16_t recordCount = 0;
    for (uint16_t i = 0; i < numberOfColorIndices; i++) {
        colorIndex = buffer->getByOffset(i);
        if (colorIndex != colorIndexPrev || runCount == 16) {
            recordCount++;
            runCount = 0;
        }
        runCount++;
        colorIndexPrev = colorIndex;
    }
    return recordCount;
}

uint16_t Eeprom::saveCompressedBoard(PalettedBuffer* buffer, uint16_t numberOfColorIndices, uint16_t address) {
    uint8_t colorIndexPrev = -1;
    uint8_t colorIndex = 0;
    uint8_t runCount = 0;
    for (uint16_t i = 0; i < numberOfColorIndices; i++) {
        colorIndex = buffer->getByOffset(i);
        if (colorIndex != colorIndexPrev || runCount == 16) {
            if (runCount != 0) {
                EEPROM.update(address++, (colorIndexPrev << 4) | ((runCount - 1) & 0x0f));
            }
            runCount = 0;
        }
        runCount++;
        colorIndexPrev = colorIndex;
    }
    EEPROM.update(address++, (colorIndexPrev << 4) | ((runCount - 1) & 0x0f));
    return address;
}

uint16_t Eeprom::loadCompressedBoard(PalettedBuffer* buffer, uint8_t compressedSize, uint16_t address) {
    uint8_t rle;
    uint8_t runCount;
    uint8_t colorIndex;
    uint8_t offset = 0;
    for (uint8_t i = 0; i < compressedSize; i++) {
        rle = EEPROM.read(address++);
        runCount = (rle & 0x0f) + 1;
        colorIndex = rle >> 4;
        for (uint8_t j = 0; j < runCount; j++) {
            buffer->setByOffset(offset++, colorIndex);
        }
    }
    return address;
}
