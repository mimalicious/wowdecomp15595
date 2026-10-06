#include "WoW/Source/DB/Rec/PageTextMaterialRec.h"

// STATIC: %0x00B45ABC
const DBCMeta PageTextMaterialRec::s_meta = { "DBFilesClient\\PageTextMaterial.dbc", 2, 8, PageTextMaterialRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void PageTextMaterialRec::FixupStrings(void* record, const char* strings) {
    PageTextMaterialRec* rec = static_cast<PageTextMaterialRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void PageTextMaterialRec::ConvertRow(PageTextMaterialRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
