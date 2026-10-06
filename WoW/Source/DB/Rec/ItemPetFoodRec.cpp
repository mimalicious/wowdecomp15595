#include "WoW/Source/DB/Rec/ItemPetFoodRec.h"

// STATIC: 0x00C24128/0x140A196B8
const DBCMeta ItemPetFoodRec::s_meta = { "DBFilesClient\\ItemPetFood.dbc", 2, 8, ItemPetFoodRec::FixupStrings, 0 };

// PROVEN: 5a1d280c IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ItemPetFoodRec::FixupStrings(void* record, const char* strings) {
    ItemPetFoodRec* rec = static_cast<ItemPetFoodRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
