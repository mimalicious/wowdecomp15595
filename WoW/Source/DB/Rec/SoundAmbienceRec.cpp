#include "WoW/Source/DB/Rec/SoundAmbienceRec.h"

// STATIC: 0x00C22290/0x140A17098
const DBCMeta SoundAmbienceRec::s_meta = { "DBFilesClient\\SoundAmbience.dbc", 2, 12, SoundAmbienceRec::FixupStrings, 0 };

// PROVEN: 643bad45 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SoundAmbienceRec::FixupStrings(void* record, const char* strings) {
}
