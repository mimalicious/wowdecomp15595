#include "WoW/Source/DB/Rec/AnimKitConfigBoneSetRec.h"

// STATIC: 0x00C1EAA0/0x140A12E00
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C1EAB0/0x140A12E10
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C1EAC0/0x140A12E20
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C1EAD0/0x140A12E30
const DBCMeta AnimKitConfigBoneSetRec::s_meta = { "DBFilesClient\\AnimKitConfigBoneSet.dbc", 4, 16, AnimKitConfigBoneSetRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void AnimKitConfigBoneSetRec::FixupStrings(void* record, const char* strings) {
}
