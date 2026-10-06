#include "WoW/Source/DB/Rec/ItemPurchaseGroupRec.h"

// STATIC: 0x00C24060/0x140A195C0
const DBCMeta ItemPurchaseGroupRec::s_meta = { "DBFilesClient\\ItemPurchaseGroup.dbc", 3, 40, ItemPurchaseGroupRec::FixupStrings, 0 };

// PROVEN: 17bc1f86 IDENTICAL/NONE
// OFFSET: 0x00A2EB70/0x1407D8310
void ItemPurchaseGroupRec::FixupStrings(void* record, const char* strings) {
    ItemPurchaseGroupRec* rec = static_cast<ItemPurchaseGroupRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
