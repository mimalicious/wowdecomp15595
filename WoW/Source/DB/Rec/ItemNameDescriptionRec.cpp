#include "WoW/Source/DB/Rec/ItemNameDescriptionRec.h"

// STATIC: 0x00C24170/0x140A19718
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C2417C/0x140A19728
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C24188/0x140A19738
static const int32_t s_fieldTypes[3] = { 0, 2, 0 };

// STATIC: 0x00C24194/0x140A19748
const DBCMeta ItemNameDescriptionRec::s_meta = { "DBFilesClient\\ItemNameDescription.dbc", 3, 12, ItemNameDescriptionRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 9aa008b1 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ItemNameDescriptionRec::FixupStrings(void* record, const char* strings) {
    ItemNameDescriptionRec* rec = static_cast<ItemNameDescriptionRec*>(record);
    DBCFixupString(rec->m_description, strings);
}
