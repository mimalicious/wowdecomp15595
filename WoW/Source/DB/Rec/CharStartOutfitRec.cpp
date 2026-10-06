#include "WoW/Source/DB/Rec/CharStartOutfitRec.h"

// STATIC: 0x00C1DA44/0x140A11A30
const DBCMeta CharStartOutfitRec::s_meta = { "DBFilesClient\\CharStartOutfit.dbc", 10, 304, CharStartOutfitRec::FixupStrings, 0 };

// PROVEN: 90cc02d7 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CharStartOutfitRec::FixupStrings(void* record, const char* strings) {
}
