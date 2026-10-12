#include "WoW/Source/DB/Rec/AnimKitPriorityRec.h"

// STATIC: 0x00C1DC64/0x140A11CC0
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C1DC6C/0x140A11CC8
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C1DC74/0x140A11CD0
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C1DC7C/0x140A11CD8
const DBCMeta AnimKitPriorityRec::s_meta = { "DBFilesClient\\AnimKitPriority.dbc", 2, 8, AnimKitPriorityRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void AnimKitPriorityRec::FixupStrings(void* record, const char* strings) {
}
