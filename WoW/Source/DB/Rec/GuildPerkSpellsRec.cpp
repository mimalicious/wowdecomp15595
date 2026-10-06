#include "WoW/Source/DB/Rec/GuildPerkSpellsRec.h"

// STATIC: 0x00C24B88/0x140A1A448
const DBCMeta GuildPerkSpellsRec::s_meta = { "DBFilesClient\\GuildPerkSpells.dbc", 3, 12, GuildPerkSpellsRec::FixupStrings, 0 };

// PROVEN: f0eb869f IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GuildPerkSpellsRec::FixupStrings(void* record, const char* strings) {
}
