#include "WoW/Source/DB/Rec/gtBarberShopCostBaseRec.h"

// STATIC: %0x00B46784
const DBCMeta gtBarberShopCostBaseRec::s_meta = { "DBFilesClient\\gtBarberShopCostBase.dbc", 1, 8, gtBarberShopCostBaseRec::FixupStrings, 0 };

// PROVEN: a2266275 IDENTICAL
// OFFSET: 0x00401000
void gtBarberShopCostBaseRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void gtBarberShopCostBaseRec::ConvertRow(gtBarberShopCostBaseRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_data, &row[0x0004], sizeof(rec->m_data));
}
#endif
