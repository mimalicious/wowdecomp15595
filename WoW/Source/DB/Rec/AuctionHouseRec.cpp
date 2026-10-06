#include "WoW/Source/DB/Rec/AuctionHouseRec.h"

// STATIC: 0x00C26990/0x140A1C9B8
const DBCMeta AuctionHouseRec::s_meta = { "DBFilesClient\\AuctionHouse.dbc", 5, 20, AuctionHouseRec::FixupStrings, 0 };

// PROVEN: a9cdad74 IDENTICAL/NONE
// OFFSET: 0x00A2EC70/0x1407D85F0
void AuctionHouseRec::FixupStrings(void* record, const char* strings) {
    AuctionHouseRec* rec = static_cast<AuctionHouseRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
