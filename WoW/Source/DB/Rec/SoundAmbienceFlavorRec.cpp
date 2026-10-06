#include "WoW/Source/DB/Rec/SoundAmbienceFlavorRec.h"

// STATIC: 0x00C22238/0x140A17028
const DBCMeta SoundAmbienceFlavorRec::s_meta = { "DBFilesClient\\SoundAmbienceFlavor.dbc", 4, 16, SoundAmbienceFlavorRec::FixupStrings, 0 };

// PROVEN: ff700163 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SoundAmbienceFlavorRec::FixupStrings(void* record, const char* strings) {
}
