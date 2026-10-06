#include "WoW/Source/DB/Rec/ItemCurrencyCostRec.h"

// STATIC: %0x00B46308
const DBCMeta ItemCurrencyCostRec::s_meta = { "DBFilesClient\\ItemCurrencyCost.dbc", 2, 8, ItemCurrencyCostRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void ItemCurrencyCostRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void ItemCurrencyCostRec::ConvertRow(ItemCurrencyCostRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
}
#endif
