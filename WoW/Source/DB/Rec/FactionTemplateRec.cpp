#include "WoW/Source/DB/Rec/FactionTemplateRec.h"

// STATIC: 0x00C1E3C8/0x140A125D8
const DBCMeta FactionTemplateRec::s_meta = { "DBFilesClient\\FactionTemplate.dbc", 8, 56, FactionTemplateRec::FixupStrings, 0 };

// PROVEN: c85fd199 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void FactionTemplateRec::FixupStrings(void* record, const char* strings) {
}
