#include "WoW/Source/DB/Rec/SpellDescriptionVariablesRec.h"

// STATIC: %0x00B451BC
const DBCMeta SpellDescriptionVariablesRec::s_meta = { "DBFilesClient\\SpellDescriptionVariables.dbc", 2, 8, SpellDescriptionVariablesRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void SpellDescriptionVariablesRec::FixupStrings(void* record, const char* strings) {
    SpellDescriptionVariablesRec* rec = static_cast<SpellDescriptionVariablesRec*>(record);
    rec->m_variables = &strings[reinterpret_cast<uintptr_t>(rec->m_variables)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellDescriptionVariablesRec::ConvertRow(SpellDescriptionVariablesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_variables = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
