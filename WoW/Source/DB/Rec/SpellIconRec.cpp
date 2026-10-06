#include "WoW/Source/DB/Rec/SpellIconRec.h"

// STATIC: 0x00C212F0/0x140A15E08
const DBCMeta SpellIconRec::s_meta = { "DBFilesClient\\SpellIcon.dbc", 2, 8, SpellIconRec::FixupStrings, 0 };

// PROVEN: f5647291 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void SpellIconRec::FixupStrings(void* record, const char* strings) {
    SpellIconRec* rec = static_cast<SpellIconRec*>(record);
    DBCFixupString(rec->m_textureFilename, strings);
}
