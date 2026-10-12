#include "WoW/Source/DB/Rec/DeclinedWordRec.h"

// STATIC: 0x00C27264/0x140A1D460
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C2726C/0x140A1D468
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C27274/0x140A1D470
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C2727C/0x140A1D478
const DBCMeta DeclinedWordRec::s_meta = { "DBFilesClient\\DeclinedWord.dbc", 2, 8, DeclinedWordRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 723529cc IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void DeclinedWordRec::FixupStrings(void* record, const char* strings) {
    DeclinedWordRec* rec = static_cast<DeclinedWordRec*>(record);
    DBCFixupString(rec->m_word, strings);
}
