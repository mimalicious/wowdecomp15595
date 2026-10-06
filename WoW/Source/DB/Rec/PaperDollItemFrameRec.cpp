#include "WoW/Source/DB/Rec/PaperDollItemFrameRec.h"

// STATIC: 0x00C2309C/0x140A18208
const DBCMeta PaperDollItemFrameRec::s_meta = { "DBFilesClient\\PaperDollItemFrame.dbc", 3, 16, PaperDollItemFrameRec::FixupStrings, 0 };

// PROVEN: a639cb66 IDENTICAL/NONE
// OFFSET: 0x00A2E9B0/0x1407D8110
void PaperDollItemFrameRec::FixupStrings(void* record, const char* strings) {
    PaperDollItemFrameRec* rec = static_cast<PaperDollItemFrameRec*>(record);
    DBCFixupString(rec->m_itemButtonName, strings);
    DBCFixupString(rec->m_slotIcon, strings);
}
