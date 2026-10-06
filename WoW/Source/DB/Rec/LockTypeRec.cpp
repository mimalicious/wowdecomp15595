#include "WoW/Source/DB/Rec/LockTypeRec.h"

// STATIC: 0x00C23568/0x140A18830
const DBCMeta LockTypeRec::s_meta = { "DBFilesClient\\LockType.dbc", 5, 20, LockTypeRec::FixupStrings, 0 };

// PROVEN: 059e0598 IDENTICAL/NONE
// OFFSET: 0x00A2EAB0/0x1407D8150
void LockTypeRec::FixupStrings(void* record, const char* strings) {
    LockTypeRec* rec = static_cast<LockTypeRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_resourceName, strings);
    DBCFixupString(rec->m_verb, strings);
    DBCFixupString(rec->m_cursorName, strings);
}
