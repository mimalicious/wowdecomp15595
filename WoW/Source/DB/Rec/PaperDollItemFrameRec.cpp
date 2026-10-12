#include "WoW/Source/DB/Rec/PaperDollItemFrameRec.h"

// STATIC: 0x00C23078/0x140A181D8
static const int32_t s_fieldOffsets[3] = { 4, 8, 12 };

// STATIC: 0x00C23084/0x140A181E8
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C23090/0x140A181F8
static const int32_t s_fieldTypes[3] = { 2, 2, 0 };

// STATIC: 0x00C2309C/0x140A18208
const DBCMeta PaperDollItemFrameRec::s_meta = { "DBFilesClient\\PaperDollItemFrame.dbc", 3, 16, PaperDollItemFrameRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: a639cb66 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2E9B0/0x1407D8110
void PaperDollItemFrameRec::FixupStrings(void* record, const char* strings) {
    PaperDollItemFrameRec* rec = static_cast<PaperDollItemFrameRec*>(record);
    DBCFixupString(rec->m_itemButtonName, strings);
    DBCFixupString(rec->m_slotIcon, strings);
}
