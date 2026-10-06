#include "WoW/Source/DB/Rec/ItemClassRec.h"

// STATIC: 0x00C1E2C4/0x140A12490
const DBCMeta ItemClassRec::s_meta = { "DBFilesClient\\ItemClass.dbc", 5, 24, ItemClassRec::FixupStrings, 0 };

// PROVEN: 20783a7b IDENTICAL/NONE
// OFFSET: 0x00A2EBF0/0x1407D84B0
void ItemClassRec::FixupStrings(void* record, const char* strings) {
    ItemClassRec* rec = static_cast<ItemClassRec*>(record);
    DBCFixupString(rec->m_className, strings);
}
