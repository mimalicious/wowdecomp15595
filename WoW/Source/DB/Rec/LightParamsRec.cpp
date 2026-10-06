#include "WoW/Source/DB/Rec/LightParamsRec.h"

// STATIC: 0x00C283EC/0x140A1EBA0
const DBCMeta LightParamsRec::s_meta = { "DBFilesClient\\LightParams.dbc", 10, 40, LightParamsRec::FixupStrings, 0 };

// PROVEN: ff8f362a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LightParamsRec::FixupStrings(void* record, const char* strings) {
}
