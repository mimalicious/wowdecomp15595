#include "WoW/Source/DB/Rec/ItemSetRec.h"

// STATIC: 0x00C23E30/0x140A192F8
static const int32_t s_fieldOffsets[7] = { 0, 4, 8, 76, 108, 140, 144 };

// STATIC: 0x00C23E4C/0x140A19318
static const int32_t s_fieldSizes[7] = { 4, 4, 68, 32, 32, 4, 4 };

// STATIC: 0x00C23E68/0x140A19338
static const int32_t s_fieldTypes[7] = { 0, 2, 0, 0, 0, 0, 0 };

// STATIC: 0x00C23E84/0x140A19358
const DBCMeta ItemSetRec::s_meta = { "DBFilesClient\\ItemSet.dbc", 7, 148, ItemSetRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 3f782e43 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ItemSetRec::FixupStrings(void* record, const char* strings) {
    ItemSetRec* rec = static_cast<ItemSetRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
