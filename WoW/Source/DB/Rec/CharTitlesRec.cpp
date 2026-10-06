#include "WoW/Source/DB/Rec/CharTitlesRec.h"

// STATIC: 0x00C26544/0x140A1C478
const DBCMeta CharTitlesRec::s_meta = { "DBFilesClient\\CharTitles.dbc", 6, 24, CharTitlesRec::FixupStrings, 0 };

// PROVEN: 810be4ac IDENTICAL/NONE
// OFFSET: 0x00A2EC50/0x1407D8590
void CharTitlesRec::FixupStrings(void* record, const char* strings) {
    CharTitlesRec* rec = static_cast<CharTitlesRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_name1, strings);
}
