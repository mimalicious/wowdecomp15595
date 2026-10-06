#include "WoW/Source/DB/Rec/PaperDollItemFrameRec.h"

// STATIC: %0x00B45A84
const DBCMeta PaperDollItemFrameRec::s_meta = { "DBFilesClient\\PaperDollItemFrame.dbc", 3, 16, PaperDollItemFrameRec::FixupStrings, 0 };

// OFFSET: %0x0097C1F0
void PaperDollItemFrameRec::FixupStrings(void* record, const char* strings) {
    PaperDollItemFrameRec* rec = static_cast<PaperDollItemFrameRec*>(record);
    rec->m_itemButtonName = &strings[reinterpret_cast<uintptr_t>(rec->m_itemButtonName)];
    rec->m_slotIcon = &strings[reinterpret_cast<uintptr_t>(rec->m_slotIcon)];
}

#if defined(WOWDECOMP_ARCH_64)
void PaperDollItemFrameRec::ConvertRow(PaperDollItemFrameRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_itemButtonName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    rec->m_slotIcon = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    memcpy(&rec->m_slotNumber, &row[0x000C], sizeof(rec->m_slotNumber));
}
#endif
