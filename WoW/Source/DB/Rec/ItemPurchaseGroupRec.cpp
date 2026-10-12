#include "WoW/Source/DB/Rec/ItemPurchaseGroupRec.h"

// STATIC: 0x00C2403C/0x140A19590
static const int32_t s_fieldOffsets[3] = { 0, 4, 36 };

// STATIC: 0x00C24048/0x140A195A0
static const int32_t s_fieldSizes[3] = { 4, 32, 4 };

// STATIC: 0x00C24054/0x140A195B0
static const int32_t s_fieldTypes[3] = { 0, 0, 2 };

// STATIC: 0x00C24060/0x140A195C0
const DBCMeta ItemPurchaseGroupRec::s_meta = { "DBFilesClient\\ItemPurchaseGroup.dbc", 3, 40, ItemPurchaseGroupRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EB70/0x1407D8310
void ItemPurchaseGroupRec::FixupStrings(void* record, const char* strings) {
    ItemPurchaseGroupRec* rec = static_cast<ItemPurchaseGroupRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
