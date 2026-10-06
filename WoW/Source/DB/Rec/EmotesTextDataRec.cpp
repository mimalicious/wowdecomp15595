#include "WoW/Source/DB/Rec/EmotesTextDataRec.h"

// STATIC: 0x00C25878/0x140A1B500
const DBCMeta EmotesTextDataRec::s_meta = { "DBFilesClient\\EmotesTextData.dbc", 2, 8, EmotesTextDataRec::FixupStrings, 0 };

// PROVEN: c464f2b2 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void EmotesTextDataRec::FixupStrings(void* record, const char* strings) {
    EmotesTextDataRec* rec = static_cast<EmotesTextDataRec*>(record);
    DBCFixupString(rec->m_text, strings);
}
