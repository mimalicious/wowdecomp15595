#include "WoW/Source/DB/Rec/CreatureSoundDataRec.h"

// STATIC: 0x00C261D4/0x140A1C038
const DBCMeta CreatureSoundDataRec::s_meta = { "DBFilesClient\\CreatureSoundData.dbc", 33, 160, CreatureSoundDataRec::FixupStrings, 0 };

// PROVEN: bbc7c4c0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CreatureSoundDataRec::FixupStrings(void* record, const char* strings) {
}
