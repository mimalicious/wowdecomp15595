#include "WoW/Source/DB/Rec/CharStartOutfitRec.h"

// STATIC: %0x00B43F14
const DBCMeta CharStartOutfitRec::s_meta = { "DBFilesClient\\CharStartOutfit.dbc", 10, 304, CharStartOutfitRec::FixupStrings, 0 };

// PROVEN: 90cc02d7 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CharStartOutfitRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void CharStartOutfitRec::ConvertRow(CharStartOutfitRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_raceID, &row[0x0004], sizeof(rec->m_raceID));
    memcpy(&rec->m_classID, &row[0x0005], sizeof(rec->m_classID));
    memcpy(&rec->m_sexID, &row[0x0006], sizeof(rec->m_sexID));
    memcpy(&rec->m_outfitID, &row[0x0007], sizeof(rec->m_outfitID));
    memcpy(rec->m_itemID, &row[0x0008], sizeof(rec->m_itemID));
    memcpy(rec->m_displayItemID, &row[0x0068], sizeof(rec->m_displayItemID));
    memcpy(rec->m_inventoryType, &row[0x00C8], sizeof(rec->m_inventoryType));
    memcpy(&rec->unk_0128, &row[0x0128], sizeof(rec->unk_0128));
    memcpy(&rec->unk_012C, &row[0x012C], sizeof(rec->unk_012C));
}
#endif
