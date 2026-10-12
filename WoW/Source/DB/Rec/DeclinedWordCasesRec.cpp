#include "WoW/Source/DB/Rec/DeclinedWordCasesRec.h"

// STATIC: 0x00C271F4/0x140A1D3D8
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C27204/0x140A1D3E8
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C27214/0x140A1D3F8
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 2 };

// STATIC: 0x00C27224/0x140A1D408
const DBCMeta DeclinedWordCasesRec::s_meta = { "DBFilesClient\\DeclinedWordCases.dbc", 4, 16, DeclinedWordCasesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2ECE0/0x1407D8380
void DeclinedWordCasesRec::FixupStrings(void* record, const char* strings) {
    DeclinedWordCasesRec* rec = static_cast<DeclinedWordCasesRec*>(record);
    DBCFixupString(rec->m_declinedWord, strings);
}
