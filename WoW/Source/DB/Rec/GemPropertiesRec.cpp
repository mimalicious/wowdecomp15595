#include "WoW/Source/DB/Rec/GemPropertiesRec.h"

// STATIC: 0x00C25368/0x140A1AEA8
const DBCMeta GemPropertiesRec::s_meta = { "DBFilesClient\\GemProperties.dbc", 6, 24, GemPropertiesRec::FixupStrings, 0 };

// PROVEN: eb3c871c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GemPropertiesRec::FixupStrings(void* record, const char* strings) {
}
