#include "WoW/Source/DB/Rec/ItemBagFamilyRec.h"

// STATIC: 0x00C24690/0x140A19DF8
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C24698/0x140A19E00
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C246A0/0x140A19E08
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C246A8/0x140A19E10
const DBCMeta ItemBagFamilyRec::s_meta = { "DBFilesClient\\ItemBagFamily.dbc", 2, 8, ItemBagFamilyRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void ItemBagFamilyRec::FixupStrings(void* record, const char* strings) {
    ItemBagFamilyRec* rec = static_cast<ItemBagFamilyRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
