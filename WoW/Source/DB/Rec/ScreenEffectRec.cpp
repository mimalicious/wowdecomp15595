#include "WoW/Source/DB/Rec/ScreenEffectRec.h"

// STATIC: 0x00C2274C/0x140A17670
const DBCMeta ScreenEffectRec::s_meta = { "DBFilesClient\\ScreenEffect.dbc", 8, 44, ScreenEffectRec::FixupStrings, 0 };

// PROVEN: c0557122 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ScreenEffectRec::FixupStrings(void* record, const char* strings) {
    ScreenEffectRec* rec = static_cast<ScreenEffectRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
