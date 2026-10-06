#include "WoW/Source/DB/Rec/SoundAmbienceFlavorRec.h"

// STATIC: 0x00C22208/0x140A16FF8
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C22218/0x140A17008
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C22228/0x140A17018
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C22238/0x140A17028
const DBCMeta SoundAmbienceFlavorRec::s_meta = { "DBFilesClient\\SoundAmbienceFlavor.dbc", 4, 16, SoundAmbienceFlavorRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: ff700163 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SoundAmbienceFlavorRec::FixupStrings(void* record, const char* strings) {
}
