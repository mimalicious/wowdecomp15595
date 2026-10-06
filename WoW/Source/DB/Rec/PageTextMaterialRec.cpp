#include "WoW/Source/DB/Rec/PageTextMaterialRec.h"

// STATIC: 0x00C230F8/0x140A18280
const DBCMeta PageTextMaterialRec::s_meta = { "DBFilesClient\\PageTextMaterial.dbc", 2, 8, PageTextMaterialRec::FixupStrings, 0 };

// PROVEN: 69dd75d1 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void PageTextMaterialRec::FixupStrings(void* record, const char* strings) {
    PageTextMaterialRec* rec = static_cast<PageTextMaterialRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
