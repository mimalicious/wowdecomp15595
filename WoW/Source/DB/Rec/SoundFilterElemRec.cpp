#include "WoW/Source/DB/Rec/SoundFilterElemRec.h"

// STATIC: 0x00C1DEC0/0x140A11F78
const DBCMeta SoundFilterElemRec::s_meta = { "DBFilesClient\\SoundFilterElem.dbc", 5, 52, SoundFilterElemRec::FixupStrings, 0 };

// PROVEN: b4917fee IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SoundFilterElemRec::FixupStrings(void* record, const char* strings) {
}
