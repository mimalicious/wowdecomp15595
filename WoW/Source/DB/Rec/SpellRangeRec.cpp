#include "WoW/Source/DB/Rec/SpellRangeRec.h"

// STATIC: 0x00C20DD8/0x140A157E0
const DBCMeta SpellRangeRec::s_meta = { "DBFilesClient\\SpellRange.dbc", 6, 32, SpellRangeRec::FixupStrings, 0 };

// PROVEN: 09fa2c9e IDENTICAL/NONE
// OFFSET: 0x00A2E990/0x1407D7E20
void SpellRangeRec::FixupStrings(void* record, const char* strings) {
    SpellRangeRec* rec = static_cast<SpellRangeRec*>(record);
    DBCFixupString(rec->m_displayName, strings);
    DBCFixupString(rec->m_displayNameShort, strings);
}
