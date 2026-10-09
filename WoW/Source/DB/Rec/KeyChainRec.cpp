#include "WoW/Source/DB/Rec/KeyChainRec.h"

// STATIC: 0x00C27184
static const int32_t s_fieldOffsets[] = { 0, 4 };

// STATIC: 0x00C2718C
static const int32_t s_fieldSizes[] = { 4, 32 };

// STATIC: 0x00C27194
static const int32_t s_fieldTypes[] = { 0, 4 };

// STATIC: 0x00C2719C
const DB2Meta KeyChainRec::s_meta = { "DBFilesClient\\KeyChain.db2", "KeyChain.adb", 2, 36, KeyChainRec::FixupRecord, 0, true, false, s_fieldOffsets, s_fieldSizes, s_fieldTypes, 0x6D8A2694, 0x00000000, 0 };

// PROVEN: 3be3a7e8 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void KeyChainRec::FixupRecord(void* record) {
}
