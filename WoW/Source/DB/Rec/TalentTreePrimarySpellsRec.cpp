#include "WoW/Source/DB/Rec/TalentTreePrimarySpellsRec.h"

// STATIC: 0x00C20270/0x140A14A50
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C20280/0x140A14A60
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C20290/0x140A14A70
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C202A0/0x140A14A80
const DBCMeta TalentTreePrimarySpellsRec::s_meta = { "DBFilesClient\\TalentTreePrimarySpells.dbc", 4, 16, TalentTreePrimarySpellsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void TalentTreePrimarySpellsRec::FixupStrings(void* record, const char* strings) {
}
