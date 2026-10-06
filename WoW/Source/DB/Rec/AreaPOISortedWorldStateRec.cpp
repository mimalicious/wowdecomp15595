#include "WoW/Source/DB/Rec/AreaPOISortedWorldStateRec.h"

// STATIC: 0x00C26B9C/0x140A1CC38
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C26BA8/0x140A1CC48
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C26BB4/0x140A1CC58
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C26BC0/0x140A1CC68
const DBCMeta AreaPOISortedWorldStateRec::s_meta = { "DBFilesClient\\AreaPOISortedWorldState.dbc", 3, 12, AreaPOISortedWorldStateRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: a0d3284a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AreaPOISortedWorldStateRec::FixupStrings(void* record, const char* strings) {
}
