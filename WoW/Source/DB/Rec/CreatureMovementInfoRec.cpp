#include "WoW/Source/DB/Rec/CreatureMovementInfoRec.h"

// STATIC: 0x00C26234/0x140A1C0B0
const DBCMeta CreatureMovementInfoRec::s_meta = { "DBFilesClient\\CreatureMovementInfo.dbc", 2, 8, CreatureMovementInfoRec::FixupStrings, 0 };

// PROVEN: 4ca884c1 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CreatureMovementInfoRec::FixupStrings(void* record, const char* strings) {
}
