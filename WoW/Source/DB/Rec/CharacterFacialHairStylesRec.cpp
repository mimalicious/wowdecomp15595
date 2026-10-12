#include "WoW/Source/DB/Rec/CharacterFacialHairStylesRec.h"

// STATIC: 0x00C1E72C/0x140A129D0
static const int32_t s_fieldOffsets[4] = { 4, 8, 12, 16 };

// STATIC: 0x00C1E73C/0x140A129E0
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 20 };

// STATIC: 0x00C1E74C/0x140A129F0
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C1E75C/0x140A12A00
const DBCMeta CharacterFacialHairStylesRec::s_meta = { "DBFilesClient\\CharacterFacialHairStyles.dbc", 4, 36, CharacterFacialHairStylesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void CharacterFacialHairStylesRec::FixupStrings(void* record, const char* strings) {
}
