#include "WoW/Source/DB/Rec/DeclinedWordCasesRec.h"

// STATIC: %0x00B47404
const DBCMeta DeclinedWordCasesRec::s_meta = { "DBFilesClient\\DeclinedWordCases.dbc", 4, 16, DeclinedWordCasesRec::FixupStrings, 0 };

// OFFSET: %0x0097C4E0
void DeclinedWordCasesRec::FixupStrings(void* record, const char* strings) {
    DeclinedWordCasesRec* rec = static_cast<DeclinedWordCasesRec*>(record);
    rec->m_declinedWord = &strings[reinterpret_cast<uintptr_t>(rec->m_declinedWord)];
}

#if defined(WOWDECOMP_ARCH_64)
void DeclinedWordCasesRec::ConvertRow(DeclinedWordCasesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_declinedWordID, &row[0x0004], sizeof(rec->m_declinedWordID));
    memcpy(&rec->m_caseIndex, &row[0x0008], sizeof(rec->m_caseIndex));
    rec->m_declinedWord = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
}
#endif
