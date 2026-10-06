#include "WoW/Source/DB/Rec/TransportPhysicsRec.h"

// STATIC: 0x00C1FEB4/0x140A145A8
static const int32_t s_fieldOffsets[11] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

// STATIC: 0x00C1FEE0/0x140A145D8
static const int32_t s_fieldSizes[11] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1FF0C/0x140A14608
static const int32_t s_fieldTypes[11] = { 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };

// STATIC: 0x00C1FF38/0x140A14638
const DBCMeta TransportPhysicsRec::s_meta = { "DBFilesClient\\TransportPhysics.dbc", 11, 44, TransportPhysicsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 0416542b IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void TransportPhysicsRec::FixupStrings(void* record, const char* strings) {
}
