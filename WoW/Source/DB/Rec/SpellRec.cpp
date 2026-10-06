#include "WoW/Source/DB/Rec/SpellRec.h"

// STATIC: 0x00C21954/0x140A16590
const DBCMeta SpellRec::s_meta = { "DBFilesClient\\Spell.dbc", 47, 192, SpellRec::FixupStrings, 0 };

// OFFSET: 0x00A2E9E0/0x1407D7F10
void SpellRec::FixupStrings(void* record, const char* strings) {
    SpellRec* rec = static_cast<SpellRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_nameSubtext, strings);
    DBCFixupString(rec->m_description, strings);
    DBCFixupString(rec->m_auraDescription, strings);
}
