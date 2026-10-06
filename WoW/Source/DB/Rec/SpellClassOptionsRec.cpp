#include "WoW/Source/DB/Rec/SpellClassOptionsRec.h"

// STATIC: 0x00C21A48/0x140A166C8
const DBCMeta SpellClassOptionsRec::s_meta = { "DBFilesClient\\SpellClassOptions.dbc", 5, 28, SpellClassOptionsRec::FixupStrings, 0 };

// PROVEN: 2e9884e3 IDENTICAL/NONE
// OFFSET: 0x00A2E920/0x1407D7F90
void SpellClassOptionsRec::FixupStrings(void* record, const char* strings) {
    SpellClassOptionsRec* rec = static_cast<SpellClassOptionsRec*>(record);
    DBCFixupString(rec->unk_0018, strings);
}
