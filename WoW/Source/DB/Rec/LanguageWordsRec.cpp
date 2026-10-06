#include "WoW/Source/DB/Rec/LanguageWordsRec.h"

// STATIC: 0x00C23A08/0x140A18DE0
const DBCMeta LanguageWordsRec::s_meta = { "DBFilesClient\\LanguageWords.dbc", 3, 12, LanguageWordsRec::FixupStrings, 0 };

// PROVEN: eebb772d IDENTICAL/NONE
// OFFSET: 0x00A2ECA0/0x1407D8230
void LanguageWordsRec::FixupStrings(void* record, const char* strings) {
    LanguageWordsRec* rec = static_cast<LanguageWordsRec*>(record);
    DBCFixupString(rec->m_word, strings);
}
