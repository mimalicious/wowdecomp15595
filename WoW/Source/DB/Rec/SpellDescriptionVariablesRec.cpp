#include "WoW/Source/DB/Rec/SpellDescriptionVariablesRec.h"

// STATIC: 0x00C216E0/0x140A16300
const DBCMeta SpellDescriptionVariablesRec::s_meta = { "DBFilesClient\\SpellDescriptionVariables.dbc", 2, 8, SpellDescriptionVariablesRec::FixupStrings, 0 };

// PROVEN: 548bfd96 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void SpellDescriptionVariablesRec::FixupStrings(void* record, const char* strings) {
    SpellDescriptionVariablesRec* rec = static_cast<SpellDescriptionVariablesRec*>(record);
    DBCFixupString(rec->m_variables, strings);
}
