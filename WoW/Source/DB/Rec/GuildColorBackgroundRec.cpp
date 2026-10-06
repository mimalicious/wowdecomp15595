#include "WoW/Source/DB/Rec/GuildColorBackgroundRec.h"

// STATIC: 0x00C1D454/0x140A11350
const DBCMeta GuildColorBackgroundRec::s_meta = { "DBFilesClient\\GuildColorBackground.dbc", 4, 8, GuildColorBackgroundRec::FixupStrings, 0 };

// PROVEN: 3380f664 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GuildColorBackgroundRec::FixupStrings(void* record, const char* strings) {
}
