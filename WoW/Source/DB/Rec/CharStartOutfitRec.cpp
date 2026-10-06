#include "WoW/Source/DB/Rec/CharStartOutfitRec.h"

// STATIC: 0x00C1D9CC/0x140A119B8
static const int32_t s_fieldOffsets[10] = { 0, 4, 5, 6, 7, 8, 104, 200, 296, 300 };

// STATIC: 0x00C1D9F4/0x140A119E0
static const int32_t s_fieldSizes[10] = { 4, 1, 1, 1, 1, 96, 96, 96, 4, 4 };

// STATIC: 0x00C1DA1C/0x140A11A08
static const int32_t s_fieldTypes[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1DA44/0x140A11A30
const DBCMeta CharStartOutfitRec::s_meta = { "DBFilesClient\\CharStartOutfit.dbc", 10, 304, CharStartOutfitRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 90cc02d7 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CharStartOutfitRec::FixupStrings(void* record, const char* strings) {
}
