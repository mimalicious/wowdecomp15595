#include "WoW/Source/DB/Rec/ItemLimitCategoryRec.h"

// STATIC: 0x00C24208/0x140A197D8
const DBCMeta ItemLimitCategoryRec::s_meta = { "DBFilesClient\\ItemLimitCategory.dbc", 4, 16, ItemLimitCategoryRec::FixupStrings, 0 };

// PROVEN: 00ba9b0f IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ItemLimitCategoryRec::FixupStrings(void* record, const char* strings) {
    ItemLimitCategoryRec* rec = static_cast<ItemLimitCategoryRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
