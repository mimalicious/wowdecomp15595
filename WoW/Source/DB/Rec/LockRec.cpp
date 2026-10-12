#include "WoW/Source/DB/Rec/LockRec.h"

// STATIC: 0x00C235A0/0x140A18880
static const int32_t s_fieldOffsets[5] = { 0, 4, 36, 68, 100 };

// STATIC: 0x00C235B4/0x140A18898
static const int32_t s_fieldSizes[5] = { 4, 32, 32, 32, 32 };

// STATIC: 0x00C235C8/0x140A188B0
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 0 };

// STATIC: 0x00C235DC/0x140A188C8
const DBCMeta LockRec::s_meta = { "DBFilesClient\\Lock.dbc", 5, 132, LockRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void LockRec::FixupStrings(void* record, const char* strings) {
}
