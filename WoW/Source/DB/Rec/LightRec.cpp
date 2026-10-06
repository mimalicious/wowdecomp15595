#include "WoW/Source/DB/Rec/LightRec.h"

// STATIC: 0x00C280C4/0x140A1E7D8
const DBCMeta LightRec::s_meta = { "DBFilesClient\\Light.dbc", 8, 60, LightRec::FixupStrings, 0 };

// PROVEN: 12f22067 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LightRec::FixupStrings(void* record, const char* strings) {
}
