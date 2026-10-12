#include "WoW/Source/DB/Rec/SoundFilterElemRec.h"

// STATIC: 0x00C1DE84/0x140A11F30
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C1DE98/0x140A11F48
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 36 };

// STATIC: 0x00C1DEAC/0x140A11F60
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 3 };

// STATIC: 0x00C1DEC0/0x140A11F78
const DBCMeta SoundFilterElemRec::s_meta = { "DBFilesClient\\SoundFilterElem.dbc", 5, 52, SoundFilterElemRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SoundFilterElemRec::FixupStrings(void* record, const char* strings) {
}
