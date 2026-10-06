#include "WoW/Source/DB/Rec/AuctionHouseRec.h"

// STATIC: %0x00B47168
const DBCMeta AuctionHouseRec::s_meta = { "DBFilesClient\\AuctionHouse.dbc", 5, 20, AuctionHouseRec::FixupStrings, 0 };

// OFFSET: %0x0097C480
void AuctionHouseRec::FixupStrings(void* record, const char* strings) {
    AuctionHouseRec* rec = static_cast<AuctionHouseRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void AuctionHouseRec::ConvertRow(AuctionHouseRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_factionID, &row[0x0004], sizeof(rec->m_factionID));
    memcpy(&rec->m_depositRate, &row[0x0008], sizeof(rec->m_depositRate));
    memcpy(&rec->m_consignmentRate, &row[0x000C], sizeof(rec->m_consignmentRate));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
}
#endif
