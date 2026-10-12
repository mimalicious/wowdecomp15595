#include "WoW/Source/DB/Rec/Cfg_CategoriesRec.h"

// STATIC: 0x00C1E994/0x140A12CB8
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 20 };

// STATIC: 0x00C1E9AC/0x140A12CD0
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1E9C4/0x140A12CE8
static const int32_t s_fieldTypes[6] = { 0, 0, 0, 0, 0, 2 };

// STATIC: 0x00C1E9DC/0x140A12D00
const DBCMeta Cfg_CategoriesRec::s_meta = { "DBFilesClient\\Cfg_Categories.dbc", 6, 24, Cfg_CategoriesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBF0/0x1407D84B0
void Cfg_CategoriesRec::FixupStrings(void* record, const char* strings) {
    Cfg_CategoriesRec* rec = static_cast<Cfg_CategoriesRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
