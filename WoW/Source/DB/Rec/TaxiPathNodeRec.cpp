#include "WoW/Source/DB/Rec/TaxiPathNodeRec.h"

// STATIC: 0x00C20128/0x140A148B8
static const int32_t s_fieldOffsets[9] = { 0, 4, 8, 12, 16, 28, 32, 36, 40 };

// STATIC: 0x00C2014C/0x140A148E0
static const int32_t s_fieldSizes[9] = { 4, 4, 4, 4, 12, 4, 4, 4, 4 };

// STATIC: 0x00C20170/0x140A14908
static const int32_t s_fieldTypes[9] = { 0, 0, 0, 0, 3, 0, 0, 0, 0 };

// STATIC: 0x00C20194/0x140A14930
const DBCMeta TaxiPathNodeRec::s_meta = { "DBFilesClient\\TaxiPathNode.dbc", 9, 44, TaxiPathNodeRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 5bacbe39 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void TaxiPathNodeRec::FixupStrings(void* record, const char* strings) {
}
