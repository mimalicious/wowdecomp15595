#include "WoW/Source/DB/Rec/LiquidMaterialRec.h"

// STATIC: %0x00B40B34
const DBCMeta LiquidMaterialRec::s_meta = { "DBFilesClient\\LiquidMaterial.dbc", 3, 12, LiquidMaterialRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void LiquidMaterialRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void LiquidMaterialRec::ConvertRow(LiquidMaterialRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_lvf, &row[0x0004], sizeof(rec->m_lvf));
    memcpy(&rec->m_flags, &row[0x0008], sizeof(rec->m_flags));
}
#endif
