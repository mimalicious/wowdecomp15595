#include "WoW/Source/DB/Rec/GuildColorBackgroundRec.h"

// STATIC: %0x00B43BD8
const DBCMeta GuildColorBackgroundRec::s_meta = { "DBFilesClient\\GuildColorBackground.dbc", 4, 8, GuildColorBackgroundRec::FixupStrings, 0 };

// PROVEN: 3380f664 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GuildColorBackgroundRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void GuildColorBackgroundRec::ConvertRow(GuildColorBackgroundRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
}
#endif
