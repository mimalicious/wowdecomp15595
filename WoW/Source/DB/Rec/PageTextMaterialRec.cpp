#include "WoW/Source/DB/Rec/PageTextMaterialRec.h"

// STATIC: 0x00C230E0/0x140A18268
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C230E8/0x140A18270
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C230F0/0x140A18278
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C230F8/0x140A18280
const DBCMeta PageTextMaterialRec::s_meta = { "DBFilesClient\\PageTextMaterial.dbc", 2, 8, PageTextMaterialRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 69dd75d1 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void PageTextMaterialRec::FixupStrings(void* record, const char* strings) {
    PageTextMaterialRec* rec = static_cast<PageTextMaterialRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
