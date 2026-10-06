#include "WoW/Source/DB/Rec/MovieFileDataRec.h"

// STATIC: 0x00C1E114/0x140A12288
const DBCMeta MovieFileDataRec::s_meta = { "DBFilesClient\\MovieFileData.dbc", 2, 8, MovieFileDataRec::FixupStrings, 0 };

// PROVEN: 9faf1e1c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void MovieFileDataRec::FixupStrings(void* record, const char* strings) {
}
