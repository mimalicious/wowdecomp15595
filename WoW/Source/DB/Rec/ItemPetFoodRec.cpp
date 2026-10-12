#include "WoW/Source/DB/Rec/ItemPetFoodRec.h"

// STATIC: 0x00C24110/0x140A196A0
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C24118/0x140A196A8
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C24120/0x140A196B0
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C24128/0x140A196B8
const DBCMeta ItemPetFoodRec::s_meta = { "DBFilesClient\\ItemPetFood.dbc", 2, 8, ItemPetFoodRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void ItemPetFoodRec::FixupStrings(void* record, const char* strings) {
    ItemPetFoodRec* rec = static_cast<ItemPetFoodRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
