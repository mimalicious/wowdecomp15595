#include "WoW/Source/DB/Rec/SoundEntriesFallbacksRec.h"

// STATIC: 0x00C27C10/0x140A1E210
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C27C1C/0x140A1E220
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C27C28/0x140A1E230
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C27C34/0x140A1E240
const DBCMeta SoundEntriesFallbacksRec::s_meta = { "DBFilesClient\\SoundEntriesFallbacks.dbc", 3, 12, SoundEntriesFallbacksRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SoundEntriesFallbacksRec::FixupStrings(void* record, const char* strings) {
}
