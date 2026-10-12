#include "Corpse.h"
#include "WoW/Source/Object/Object.h"

// STATIC: 0x00D1A1D0/0x140BD61F0
const uint32_t CGCorpse::s_mirrorFields[3] = {
    0x1A, 0x1B, 0x1C
};

// OFFSET: 0x0048DAA0/0x1400B19D0
uint32_t CGCorpse::DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base) {
    int32_t total = CGObject::TotalFieldsSaved();

    return (offset & 3) + 4 * (total + CGCorpse::GetSavedIndexFromDataIndex((offset - base) >> 2, (size + 3) >> 2, local));
}

// OFFSET: 0x00A281C0/0x1407CF9C0
int32_t CGCorpse::TotalFieldsSaved() {
    return CGObject::TotalFieldsSaved() + sizeof(CGCorpse::s_mirrorFields) / sizeof(CGCorpse::s_mirrorFields[0]);
}

// OFFSET: 0x00A281D0/0x1407CF9E0
uint32_t CGCorpse::GetSavedIndexFromDataIndex(uint32_t index, uint32_t count, int32_t local) {
    for (uint32_t i = 0; i < sizeof(CGCorpse::s_mirrorFields) / sizeof(CGCorpse::s_mirrorFields[0]); i++) {
        if (CGCorpse::s_mirrorFields[i] == index) {
            return i;
        }
    }

    return 28;
}
