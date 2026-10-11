#include "Item.h"
#include "WoW/Source/Object/Object.h"

// STATIC: 0x00D18E60
const uint32_t CGItem::s_mirrorFields[56] = {
    0x0, 0x1, 0x8, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
    0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3F, 0x40
};

// OFFSET: 0x00A27CE0/0x1407CF490
uint32_t CGItem::GetSavedIndexFromDataIndex(uint32_t index, uint32_t count, int32_t local) {
    for (uint32_t i = 0; i < sizeof(CGItem::s_mirrorFields) / sizeof(CGItem::s_mirrorFields[0]); i++) {
        if (CGItem::s_mirrorFields[i] == index) {
            return i;
        }
    }

    return 66;
}

// OFFSET: 0x0048D9E0/0x1400B1880
uint32_t CGItem::DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base) {
    int32_t total = CGObject::TotalFieldsSaved();

    return (offset & 3) + 4 * (total + CGItem::GetSavedIndexFromDataIndex((offset - base) >> 2, (size + 3) >> 2, local));
}

// OFFSET: 0x00A27CD0/0x1407CF470
int32_t CGItem::TotalFieldsSaved() {
    return CGObject::TotalFieldsSaved() + sizeof(CGItem::s_mirrorFields) / sizeof(CGItem::s_mirrorFields[0]);
}
