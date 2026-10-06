#include "WoW/Source/DB/Rec/MovieVariationRec.h"

// STATIC: 0x00C1E0BC/0x140A12218
const DBCMeta MovieVariationRec::s_meta = { "DBFilesClient\\MovieVariation.dbc", 3, 12, MovieVariationRec::FixupStrings, 0 };

// PROVEN: bbc864c4 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void MovieVariationRec::FixupStrings(void* record, const char* strings) {
}
