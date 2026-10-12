#include "WoW/Source/DB/Rec/ItemLimitCategoryRec.h"

// STATIC: 0x00C241D8/0x140A197A8
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C241E8/0x140A197B8
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C241F8/0x140A197C8
static const int32_t s_fieldTypes[4] = { 0, 2, 0, 0 };

// STATIC: 0x00C24208/0x140A197D8
const DBCMeta ItemLimitCategoryRec::s_meta = { "DBFilesClient\\ItemLimitCategory.dbc", 4, 16, ItemLimitCategoryRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void ItemLimitCategoryRec::FixupStrings(void* record, const char* strings) {
    ItemLimitCategoryRec* rec = static_cast<ItemLimitCategoryRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
