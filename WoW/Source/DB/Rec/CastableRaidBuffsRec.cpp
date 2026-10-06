#include "WoW/Source/DB/Rec/CastableRaidBuffsRec.h"

// STATIC: 0x00C265AC/0x140A1C508
const DBCMeta CastableRaidBuffsRec::s_meta = { "DBFilesClient\\CastableRaidBuffs.dbc", 3, 12, CastableRaidBuffsRec::FixupStrings, 0 };

// PROVEN: 7cf93d9b IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CastableRaidBuffsRec::FixupStrings(void* record, const char* strings) {
}
