#include "WoW/Source/DB/Rec/SpellAuraRestrictionsRec.h"

// STATIC: 0x00C2201C/0x140A16D98
static const int32_t s_fieldOffsets[9] = { 0, 4, 8, 12, 16, 20, 24, 28, 32 };

// STATIC: 0x00C22040/0x140A16DC0
static const int32_t s_fieldSizes[9] = { 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C22064/0x140A16DE8
static const int32_t s_fieldTypes[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C22088/0x140A16E10
const DBCMeta SpellAuraRestrictionsRec::s_meta = { "DBFilesClient\\SpellAuraRestrictions.dbc", 9, 36, SpellAuraRestrictionsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: e266f51d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellAuraRestrictionsRec::FixupStrings(void* record, const char* strings) {
}
