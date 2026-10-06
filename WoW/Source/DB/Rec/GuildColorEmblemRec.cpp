#include "WoW/Source/DB/Rec/GuildColorEmblemRec.h"

// STATIC: 0x00C1D368/0x140A11230
const DBCMeta GuildColorEmblemRec::s_meta = { "DBFilesClient\\GuildColorEmblem.dbc", 4, 8, GuildColorEmblemRec::FixupStrings, 0 };

// PROVEN: 6cd5771c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GuildColorEmblemRec::FixupStrings(void* record, const char* strings) {
}
