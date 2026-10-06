#include "WoW/Source/DB/Rec/GuildPerkSpellsRec.h"

// STATIC: 0x00C24B64/0x140A1A418
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C24B70/0x140A1A428
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C24B7C/0x140A1A438
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C24B88/0x140A1A448
const DBCMeta GuildPerkSpellsRec::s_meta = { "DBFilesClient\\GuildPerkSpells.dbc", 3, 12, GuildPerkSpellsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: f0eb869f IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GuildPerkSpellsRec::FixupStrings(void* record, const char* strings) {
}
