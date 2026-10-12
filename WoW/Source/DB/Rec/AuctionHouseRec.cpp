#include "WoW/Source/DB/Rec/AuctionHouseRec.h"

// STATIC: 0x00C26954/0x140A1C970
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C26968/0x140A1C988
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C2697C/0x140A1C9A0
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 2 };

// STATIC: 0x00C26990/0x140A1C9B8
const DBCMeta AuctionHouseRec::s_meta = { "DBFilesClient\\AuctionHouse.dbc", 5, 20, AuctionHouseRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EC70/0x1407D85F0
void AuctionHouseRec::FixupStrings(void* record, const char* strings) {
    AuctionHouseRec* rec = static_cast<AuctionHouseRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
