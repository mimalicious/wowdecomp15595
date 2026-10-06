#include "WoW/Source/DB/Rec/GlueScreenEmoteRec.h"

// STATIC: 0x00C1D4F8/0x140A11410
const DBCMeta GlueScreenEmoteRec::s_meta = { "DBFilesClient\\GlueScreenEmote.dbc", 8, 32, GlueScreenEmoteRec::FixupStrings, 0 };

// PROVEN: 3d296330 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GlueScreenEmoteRec::FixupStrings(void* record, const char* strings) {
}
