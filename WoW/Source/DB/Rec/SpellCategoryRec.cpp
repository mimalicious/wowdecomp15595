#include "WoW/Source/DB/Rec/SpellCategoryRec.h"

// STATIC: 0x00C21D40/0x140A169F8
const DBCMeta SpellCategoryRec::s_meta = { "DBFilesClient\\SpellCategory.dbc", 4, 16, SpellCategoryRec::FixupStrings, 0 };

// PROVEN: 11e161fb IDENTICAL/NONE
// OFFSET: 0x00A2ECE0/0x1407D8380
void SpellCategoryRec::FixupStrings(void* record, const char* strings) {
    SpellCategoryRec* rec = static_cast<SpellCategoryRec*>(record);
    DBCFixupString(rec->unk_000C, strings);
}
