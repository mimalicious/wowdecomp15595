#include "AreaTrigger.h"
#include "WoW/Source/Object/Object.h"

// OFFSET: [0x0048DA60/0x1400B1960]
uint32_t CGAreaTrigger::DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base) {
    int32_t total = CGObject::TotalFieldsSaved();

    return (offset & 3) + 4 * (total + CGAreaTrigger::GetSavedIndexFromDataIndex((offset - base) >> 2, (size + 3) >> 2, local));
}

// PROVEN: 92316639 IDENTICAL/IDENTICAL
// OFFSET: [0x00619630/0x1402A6B50]
uint32_t CGAreaTrigger::GetSavedIndexFromDataIndex(uint32_t index, uint32_t count, int32_t local) {
    return 6;
}

// PROVEN: 3b2d1b77 IDENTICAL/IDENTICAL
// OFFSET: [0x00A281B0/0x1407CFA10]
int32_t CGAreaTrigger::TotalFields() {
    return CGObject::TotalFieldsSaved();
}
