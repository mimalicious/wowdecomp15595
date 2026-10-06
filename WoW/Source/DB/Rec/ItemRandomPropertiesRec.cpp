#include "WoW/Source/DB/Rec/ItemRandomPropertiesRec.h"

// STATIC: 0x00C23FF8/0x140A19530
const DBCMeta ItemRandomPropertiesRec::s_meta = { "DBFilesClient\\ItemRandomProperties.dbc", 4, 32, ItemRandomPropertiesRec::FixupStrings, 0 };

// PROVEN: 9d677c96 IDENTICAL/NONE
// OFFSET: 0x00A2EB50/0x1407D82D0
void ItemRandomPropertiesRec::FixupStrings(void* record, const char* strings) {
    ItemRandomPropertiesRec* rec = static_cast<ItemRandomPropertiesRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_nameLang, strings);
}
