#include "GameObject.h"
#include "WoW/Source/Object/Object.h"

// STATIC: 0x00D1A1B8/0x140BD61D8
uint32_t CGGameObject::s_mirrorFieldIndex[4] = {
    0x3, 0x8, 0xB, 0xC
};

// OFFSET: 0x0048DA20/0x1400B18F0
uint32_t CGGameObject::DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base) {
    int32_t total = CGObject::TotalFieldsSaved();

    return (offset & 3) + 4 * (total + CGGameObject::MirrorIndexFromFieldIndex((offset - base) >> 2, (size + 3) >> 2, local));
}

// PROVEN: ab7e0567 IDENTICAL/IDENTICAL
// OFFSET: 0x00A27DD0/0x1407CF5D0
uint32_t CGGameObject::MirrorIndexFromFieldIndex(uint32_t fieldIndex, uint32_t count, int32_t local) {
    for (uint32_t i = 0; i < 4; i++) {
        if (s_mirrorFieldIndex[i] == fieldIndex) {
            return i;
        }
    }

    return 12;
}

// PROVEN: bd0517a2 IDENTICAL/IDENTICAL
// OFFSET: 0x00A27DC0/0x1407CF5B0
int32_t CGGameObject::TotalFieldsSaved() {
    return CGObject::TotalFieldsSaved() + sizeof(CGGameObject::s_mirrorFieldIndex) / sizeof(CGGameObject::s_mirrorFieldIndex[0]);
}
