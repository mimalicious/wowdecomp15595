#include "WoW/Source/DB/Rec/Cfg_ConfigsRec.h"

// STATIC: 0x00C1E920/0x140A12C28
static const int32_t s_fieldOffsets[4] = { 4, 8, 12, 16 };

// STATIC: 0x00C1E930/0x140A12C38
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C1E940/0x140A12C48
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C1E950/0x140A12C58
const DBCMeta Cfg_ConfigsRec::s_meta = { "DBFilesClient\\Cfg_Configs.dbc", 4, 20, Cfg_ConfigsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: b6023f3b IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void Cfg_ConfigsRec::FixupStrings(void* record, const char* strings) {
}
