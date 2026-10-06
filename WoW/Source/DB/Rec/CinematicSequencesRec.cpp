#include "WoW/Source/DB/Rec/CinematicSequencesRec.h"

// STATIC: 0x00C26360/0x140A1C218
const DBCMeta CinematicSequencesRec::s_meta = { "DBFilesClient\\CinematicSequences.dbc", 3, 40, CinematicSequencesRec::FixupStrings, 0 };

// PROVEN: a392b303 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CinematicSequencesRec::FixupStrings(void* record, const char* strings) {
}
