#include "WoW/Source/DB/Rec/ItemSubClassRec.h"

// STATIC: 0x00C1E24C/0x140A123F0
const DBCMeta ItemSubClassRec::s_meta = { "DBFilesClient\\ItemSubClass.dbc", 12, 52, ItemSubClassRec::FixupStrings, 0 };

// PROVEN: 0ae3df63 IDENTICAL/NONE
// OFFSET: 0x00A2E870/0x1407D79F0
void ItemSubClassRec::FixupStrings(void* record, const char* strings) {
    ItemSubClassRec* rec = static_cast<ItemSubClassRec*>(record);
    DBCFixupString(rec->m_displayName, strings);
    DBCFixupString(rec->m_verboseName, strings);
}
