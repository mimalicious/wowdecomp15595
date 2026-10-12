#include "WoW/Source/DB/Rec/ScreenEffectRec.h"

// STATIC: 0x00C226EC/0x140A17610
static const int32_t s_fieldOffsets[8] = { 0, 4, 8, 12, 28, 32, 36, 40 };

// STATIC: 0x00C2270C/0x140A17630
static const int32_t s_fieldSizes[8] = { 4, 4, 4, 16, 4, 4, 4, 4 };

// STATIC: 0x00C2272C/0x140A17650
static const int32_t s_fieldTypes[8] = { 0, 2, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C2274C/0x140A17670
const DBCMeta ScreenEffectRec::s_meta = { "DBFilesClient\\ScreenEffect.dbc", 8, 44, ScreenEffectRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: c0557122 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void ScreenEffectRec::FixupStrings(void* record, const char* strings) {
    ScreenEffectRec* rec = static_cast<ScreenEffectRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
