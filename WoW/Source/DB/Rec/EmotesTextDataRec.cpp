#include "WoW/Source/DB/Rec/EmotesTextDataRec.h"

// STATIC: 0x00C25860/0x140A1B4E8
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C25868/0x140A1B4F0
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C25870/0x140A1B4F8
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C25878/0x140A1B500
const DBCMeta EmotesTextDataRec::s_meta = { "DBFilesClient\\EmotesTextData.dbc", 2, 8, EmotesTextDataRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void EmotesTextDataRec::FixupStrings(void* record, const char* strings) {
    EmotesTextDataRec* rec = static_cast<EmotesTextDataRec*>(record);
    DBCFixupString(rec->m_text, strings);
}
