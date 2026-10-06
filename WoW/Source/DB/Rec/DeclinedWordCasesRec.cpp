#include "WoW/Source/DB/Rec/DeclinedWordCasesRec.h"

// STATIC: 0x00C27224/0x140A1D408
const DBCMeta DeclinedWordCasesRec::s_meta = { "DBFilesClient\\DeclinedWordCases.dbc", 4, 16, DeclinedWordCasesRec::FixupStrings, 0 };

// PROVEN: f53f460c IDENTICAL/NONE
// OFFSET: 0x00A2ECE0/0x1407D8380
void DeclinedWordCasesRec::FixupStrings(void* record, const char* strings) {
    DeclinedWordCasesRec* rec = static_cast<DeclinedWordCasesRec*>(record);
    DBCFixupString(rec->m_declinedWord, strings);
}
