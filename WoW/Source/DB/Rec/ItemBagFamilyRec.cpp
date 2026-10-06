#include "WoW/Source/DB/Rec/ItemBagFamilyRec.h"

// STATIC: 0x00C246A8/0x140A19E10
const DBCMeta ItemBagFamilyRec::s_meta = { "DBFilesClient\\ItemBagFamily.dbc", 2, 8, ItemBagFamilyRec::FixupStrings, 0 };

// PROVEN: 60894894 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ItemBagFamilyRec::FixupStrings(void* record, const char* strings) {
    ItemBagFamilyRec* rec = static_cast<ItemBagFamilyRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
