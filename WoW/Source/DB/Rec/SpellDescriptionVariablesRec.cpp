#include "WoW/Source/DB/Rec/SpellDescriptionVariablesRec.h"

// STATIC: 0x00C216C8/0x140A162E8
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C216D0/0x140A162F0
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C216D8/0x140A162F8
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C216E0/0x140A16300
const DBCMeta SpellDescriptionVariablesRec::s_meta = { "DBFilesClient\\SpellDescriptionVariables.dbc", 2, 8, SpellDescriptionVariablesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 548bfd96 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void SpellDescriptionVariablesRec::FixupStrings(void* record, const char* strings) {
    SpellDescriptionVariablesRec* rec = static_cast<SpellDescriptionVariablesRec*>(record);
    DBCFixupString(rec->m_variables, strings);
}
