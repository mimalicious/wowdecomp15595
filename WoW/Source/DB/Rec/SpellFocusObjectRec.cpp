#include "WoW/Source/DB/Rec/SpellFocusObjectRec.h"

// STATIC: 0x00C2134C/0x140A15E80
const DBCMeta SpellFocusObjectRec::s_meta = { "DBFilesClient\\SpellFocusObject.dbc", 2, 8, SpellFocusObjectRec::FixupStrings, 0 };

// PROVEN: c4456c63 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void SpellFocusObjectRec::FixupStrings(void* record, const char* strings) {
    SpellFocusObjectRec* rec = static_cast<SpellFocusObjectRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
