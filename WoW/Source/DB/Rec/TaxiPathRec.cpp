#include "WoW/Source/DB/Rec/TaxiPathRec.h"

// STATIC: 0x00C200E8/0x140A14860
const DBCMeta TaxiPathRec::s_meta = { "DBFilesClient\\TaxiPath.dbc", 4, 16, TaxiPathRec::FixupStrings, 0 };

// PROVEN: 008a31da IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void TaxiPathRec::FixupStrings(void* record, const char* strings) {
}
