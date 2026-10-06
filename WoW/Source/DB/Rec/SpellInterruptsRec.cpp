#include "WoW/Source/DB/Rec/SpellInterruptsRec.h"

// STATIC: 0x00C2129C/0x140A15D98
const DBCMeta SpellInterruptsRec::s_meta = { "DBFilesClient\\SpellInterrupts.dbc", 4, 24, SpellInterruptsRec::FixupStrings, 0 };

// PROVEN: a816a06e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellInterruptsRec::FixupStrings(void* record, const char* strings) {
}
