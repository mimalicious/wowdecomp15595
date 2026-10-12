#include "WoW/Source/DB/Rec/EmotesTextRec.h"

// STATIC: 0x00C2576C/0x140A1B3B0
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C2577C/0x140A1B3C0
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 64 };

// STATIC: 0x00C2578C/0x140A1B3D0
static const int32_t s_fieldTypes[4] = { 0, 2, 0, 0 };

// STATIC: 0x00C2579C/0x140A1B3E0
const DBCMeta EmotesTextRec::s_meta = { "DBFilesClient\\EmotesText.dbc", 4, 76, EmotesTextRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void EmotesTextRec::FixupStrings(void* record, const char* strings) {
    EmotesTextRec* rec = static_cast<EmotesTextRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
