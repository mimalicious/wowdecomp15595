#include "WoW/Source/DB/Rec/BarberShopStyleRec.h"

// STATIC: %0x00B470FC
const DBCMeta BarberShopStyleRec::s_meta = { "DBFilesClient\\BarberShopStyle.dbc", 8, 32, BarberShopStyleRec::FixupStrings, 0 };

// OFFSET: %0x0097C460
void BarberShopStyleRec::FixupStrings(void* record, const char* strings) {
    BarberShopStyleRec* rec = static_cast<BarberShopStyleRec*>(record);
    rec->m_displayName = &strings[reinterpret_cast<uintptr_t>(rec->m_displayName)];
    rec->m_description = &strings[reinterpret_cast<uintptr_t>(rec->m_description)];
}

#if defined(WOWDECOMP_ARCH_64)
void BarberShopStyleRec::ConvertRow(BarberShopStyleRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_type, &row[0x0004], sizeof(rec->m_type));
    rec->m_displayName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    rec->m_description = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
    memcpy(&rec->m_costModifier, &row[0x0010], sizeof(rec->m_costModifier));
    memcpy(&rec->m_race, &row[0x0014], sizeof(rec->m_race));
    memcpy(&rec->m_sex, &row[0x0018], sizeof(rec->m_sex));
    memcpy(&rec->m_data, &row[0x001C], sizeof(rec->m_data));
}
#endif
