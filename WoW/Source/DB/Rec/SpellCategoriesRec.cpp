#include "WoW/Source/DB/Rec/SpellCategoriesRec.h"

// STATIC: 0x00C21DD8/0x140A16AB8
const DBCMeta SpellCategoriesRec::s_meta = { "DBFilesClient\\SpellCategories.dbc", 7, 28, SpellCategoriesRec::FixupStrings, 0 };

// PROVEN: e212f9cf IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellCategoriesRec::FixupStrings(void* record, const char* strings) {
}
