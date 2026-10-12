#include "WoW/Source/DB/Rec/SpellCastingRequirementsRec.h"

// STATIC: 0x00C21E98/0x140A16BB0
static const int32_t s_fieldOffsets[7] = { 0, 4, 8, 12, 16, 20, 24 };

// STATIC: 0x00C21EB4/0x140A16BD0
static const int32_t s_fieldSizes[7] = { 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C21ED0/0x140A16BF0
static const int32_t s_fieldTypes[7] = { 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C21EEC/0x140A16C10
const DBCMeta SpellCastingRequirementsRec::s_meta = { "DBFilesClient\\SpellCastingRequirements.dbc", 7, 28, SpellCastingRequirementsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellCastingRequirementsRec::FixupStrings(void* record, const char* strings) {
}
