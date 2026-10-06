#include "WoW/Source/DB/Rec/EmotesTextSoundRec.h"

// STATIC: 0x00C257E0/0x140A1B440
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C257F4/0x140A1B458
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C25808/0x140A1B470
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 0 };

// STATIC: 0x00C2581C/0x140A1B488
const DBCMeta EmotesTextSoundRec::s_meta = { "DBFilesClient\\EmotesTextSound.dbc", 5, 20, EmotesTextSoundRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: c4610e4c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void EmotesTextSoundRec::FixupStrings(void* record, const char* strings) {
}
