#include "WoW/Source/DB/Rec/Cfg_ConfigsRec.h"

// STATIC: 0x00C1E950/0x140A12C58
const DBCMeta Cfg_ConfigsRec::s_meta = { "DBFilesClient\\Cfg_Configs.dbc", 4, 20, Cfg_ConfigsRec::FixupStrings, 0 };

// PROVEN: b6023f3b IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void Cfg_ConfigsRec::FixupStrings(void* record, const char* strings) {
}
