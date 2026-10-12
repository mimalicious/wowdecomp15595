#include "WoW/Source/DB/Rec/AnimReplacementRec.h"

// STATIC: 0x00C1DAE8/0x140A11B08
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C1DAF8/0x140A11B18
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C1DB08/0x140A11B28
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C1DB18/0x140A11B38
const DBCMeta AnimReplacementRec::s_meta = { "DBFilesClient\\AnimReplacement.dbc", 4, 16, AnimReplacementRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void AnimReplacementRec::FixupStrings(void* record, const char* strings) {
}
