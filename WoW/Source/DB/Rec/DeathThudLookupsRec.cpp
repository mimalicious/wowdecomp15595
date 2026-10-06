#include "WoW/Source/DB/Rec/DeathThudLookupsRec.h"

// STATIC: 0x00C25D70/0x140A1BAF8
const DBCMeta DeathThudLookupsRec::s_meta = { "DBFilesClient\\DeathThudLookups.dbc", 5, 20, DeathThudLookupsRec::FixupStrings, 0 };

// PROVEN: d0a3cad0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void DeathThudLookupsRec::FixupStrings(void* record, const char* strings) {
}
