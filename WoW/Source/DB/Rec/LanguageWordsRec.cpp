#include "WoW/Source/DB/Rec/LanguageWordsRec.h"

// STATIC: 0x00C239E4/0x140A18DB0
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C239F0/0x140A18DC0
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C239FC/0x140A18DD0
static const int32_t s_fieldTypes[3] = { 0, 0, 2 };

// STATIC: 0x00C23A08/0x140A18DE0
const DBCMeta LanguageWordsRec::s_meta = { "DBFilesClient\\LanguageWords.dbc", 3, 12, LanguageWordsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2ECA0/0x1407D8230
void LanguageWordsRec::FixupStrings(void* record, const char* strings) {
    LanguageWordsRec* rec = static_cast<LanguageWordsRec*>(record);
    DBCFixupString(rec->m_word, strings);
}
