#include "WoW/Source/DB/Rec/CinematicSequencesRec.h"

// STATIC: 0x00C2633C/0x140A1C1E8
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C26348/0x140A1C1F8
static const int32_t s_fieldSizes[3] = { 4, 4, 32 };

// STATIC: 0x00C26354/0x140A1C208
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C26360/0x140A1C218
const DBCMeta CinematicSequencesRec::s_meta = { "DBFilesClient\\CinematicSequences.dbc", 3, 40, CinematicSequencesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: a392b303 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CinematicSequencesRec::FixupStrings(void* record, const char* strings) {
}
