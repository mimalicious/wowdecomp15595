#include "WoW/Source/DB/Rec/DeclinedWordRec.h"

// STATIC: 0x00C2727C/0x140A1D478
const DBCMeta DeclinedWordRec::s_meta = { "DBFilesClient\\DeclinedWord.dbc", 2, 8, DeclinedWordRec::FixupStrings, 0 };

// PROVEN: 723529cc IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void DeclinedWordRec::FixupStrings(void* record, const char* strings) {
    DeclinedWordRec* rec = static_cast<DeclinedWordRec*>(record);
    DBCFixupString(rec->m_word, strings);
}
