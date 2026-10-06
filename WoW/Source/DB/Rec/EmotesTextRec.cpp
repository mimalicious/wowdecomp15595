#include "WoW/Source/DB/Rec/EmotesTextRec.h"

// STATIC: 0x00C2579C/0x140A1B3E0
const DBCMeta EmotesTextRec::s_meta = { "DBFilesClient\\EmotesText.dbc", 4, 76, EmotesTextRec::FixupStrings, 0 };

// PROVEN: 66060229 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void EmotesTextRec::FixupStrings(void* record, const char* strings) {
    EmotesTextRec* rec = static_cast<EmotesTextRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
