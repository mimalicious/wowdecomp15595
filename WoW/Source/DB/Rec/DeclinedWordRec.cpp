#include "WoW/Source/DB/Rec/DeclinedWordRec.h"

// STATIC: %0x00B47438
const DBCMeta DeclinedWordRec::s_meta = { "DBFilesClient\\DeclinedWord.dbc", 2, 8, DeclinedWordRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void DeclinedWordRec::FixupStrings(void* record, const char* strings) {
    DeclinedWordRec* rec = static_cast<DeclinedWordRec*>(record);
    rec->m_word = &strings[reinterpret_cast<uintptr_t>(rec->m_word)];
}

#if defined(WOWDECOMP_ARCH_64)
void DeclinedWordRec::ConvertRow(DeclinedWordRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_word = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
