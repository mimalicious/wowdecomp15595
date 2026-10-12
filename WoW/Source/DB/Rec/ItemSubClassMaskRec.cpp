#include "WoW/Source/DB/Rec/ItemSubClassMaskRec.h"

// STATIC: 0x00C23DD0/0x140A19270
static const int32_t s_fieldOffsets[3] = { 4, 8, 12 };

// STATIC: 0x00C23DDC/0x140A19280
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C23DE8/0x140A19290
static const int32_t s_fieldTypes[3] = { 0, 0, 2 };

// STATIC: 0x00C23DF4/0x140A192A0
const DBCMeta ItemSubClassMaskRec::s_meta = { "DBFilesClient\\ItemSubClassMask.dbc", 3, 16, ItemSubClassMaskRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 852bdd1f IDENTICAL/DIFFERENT
// OFFSET: 0x00A2ECE0/0x1407D8380
void ItemSubClassMaskRec::FixupStrings(void* record, const char* strings) {
    ItemSubClassMaskRec* rec = static_cast<ItemSubClassMaskRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
