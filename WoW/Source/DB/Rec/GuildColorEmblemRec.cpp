#include "WoW/Source/DB/Rec/GuildColorEmblemRec.h"

// STATIC: %0x00B43B64
const DBCMeta GuildColorEmblemRec::s_meta = { "DBFilesClient\\GuildColorEmblem.dbc", 4, 8, GuildColorEmblemRec::FixupStrings, 0 };

// PROVEN: 6cd5771c IDENTICAL
// OFFSET: 0x00401000
void GuildColorEmblemRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void GuildColorEmblemRec::ConvertRow(GuildColorEmblemRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
}
#endif
