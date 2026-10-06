#include "WoW/Source/DB/Rec/LanguageWordsRec.h"

// STATIC: %0x00B45EA0
const DBCMeta LanguageWordsRec::s_meta = { "DBFilesClient\\LanguageWords.dbc", 3, 12, LanguageWordsRec::FixupStrings, 0 };

// OFFSET: %0x0097C380
void LanguageWordsRec::FixupStrings(void* record, const char* strings) {
    LanguageWordsRec* rec = static_cast<LanguageWordsRec*>(record);
    rec->m_word = &strings[reinterpret_cast<uintptr_t>(rec->m_word)];
}

#if defined(WOWDECOMP_ARCH_64)
void LanguageWordsRec::ConvertRow(LanguageWordsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_languageID, &row[0x0004], sizeof(rec->m_languageID));
    rec->m_word = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
}
#endif
