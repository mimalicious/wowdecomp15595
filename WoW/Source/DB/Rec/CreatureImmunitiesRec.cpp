#include "WoW/Source/DB/Rec/CreatureImmunitiesRec.h"

// STATIC: 0x00C262F4/0x140A1C188
const DBCMeta CreatureImmunitiesRec::s_meta = { "DBFilesClient\\CreatureImmunities.dbc", 10, 104, CreatureImmunitiesRec::FixupStrings, 0 };

// PROVEN: be2468c0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CreatureImmunitiesRec::FixupStrings(void* record, const char* strings) {
}
