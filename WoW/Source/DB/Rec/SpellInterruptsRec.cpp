#include "WoW/Source/DB/Rec/SpellInterruptsRec.h"

// STATIC: 0x00C2126C/0x140A15D68
static const int32_t s_fieldOffsets[4] = { 0, 4, 12, 20 };

// STATIC: 0x00C2127C/0x140A15D78
static const int32_t s_fieldSizes[4] = { 4, 8, 8, 4 };

// STATIC: 0x00C2128C/0x140A15D88
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C2129C/0x140A15D98
const DBCMeta SpellInterruptsRec::s_meta = { "DBFilesClient\\SpellInterrupts.dbc", 4, 24, SpellInterruptsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: a816a06e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellInterruptsRec::FixupStrings(void* record, const char* strings) {
}
