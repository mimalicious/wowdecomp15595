#include "WoW/Source/DB/Rec/ItemSetRec.h"

// STATIC: 0x00C23E84/0x140A19358
const DBCMeta ItemSetRec::s_meta = { "DBFilesClient\\ItemSet.dbc", 7, 148, ItemSetRec::FixupStrings, 0 };

// PROVEN: 3f782e43 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ItemSetRec::FixupStrings(void* record, const char* strings) {
    ItemSetRec* rec = static_cast<ItemSetRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
