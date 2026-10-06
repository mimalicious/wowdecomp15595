#include "WoW/Source/DB/Rec/SpellDispelTypeRec.h"

// STATIC: 0x00C21620/0x140A16208
const DBCMeta SpellDispelTypeRec::s_meta = { "DBFilesClient\\SpellDispelType.dbc", 5, 20, SpellDispelTypeRec::FixupStrings, 0 };

// PROVEN: 95edf69d IDENTICAL/NONE
// OFFSET: 0x00A2EA60/0x1407D7ED0
void SpellDispelTypeRec::FixupStrings(void* record, const char* strings) {
    SpellDispelTypeRec* rec = static_cast<SpellDispelTypeRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_internalName, strings);
}
