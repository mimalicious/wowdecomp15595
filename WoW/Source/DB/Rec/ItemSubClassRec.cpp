#include "WoW/Source/DB/Rec/ItemSubClassRec.h"

// STATIC: 0x00C1E1BC/0x140A12360
static const int32_t s_fieldOffsets[12] = { 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48 };

// STATIC: 0x00C1E1EC/0x140A12390
static const int32_t s_fieldSizes[12] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1E21C/0x140A123C0
static const int32_t s_fieldTypes[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2 };

// STATIC: 0x00C1E24C/0x140A123F0
const DBCMeta ItemSubClassRec::s_meta = { "DBFilesClient\\ItemSubClass.dbc", 12, 52, ItemSubClassRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 0ae3df63 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2E870/0x1407D79F0
void ItemSubClassRec::FixupStrings(void* record, const char* strings) {
    ItemSubClassRec* rec = static_cast<ItemSubClassRec*>(record);
    DBCFixupString(rec->m_displayName, strings);
    DBCFixupString(rec->m_verboseName, strings);
}
