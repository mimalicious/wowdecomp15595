#include "WoW/Source/DB/Rec/TransportAnimationRec.h"

// STATIC: 0x00C1FF80/0x140A14698
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 24 };

// STATIC: 0x00C1FF94/0x140A146B0
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 12, 4 };

// STATIC: 0x00C1FFA8/0x140A146C8
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 3, 0 };

// STATIC: 0x00C1FFBC/0x140A146E0
const DBCMeta TransportAnimationRec::s_meta = { "DBFilesClient\\TransportAnimation.dbc", 5, 28, TransportAnimationRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 480c9e45 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void TransportAnimationRec::FixupStrings(void* record, const char* strings) {
}
