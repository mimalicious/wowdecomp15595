#include "WoW/Source/DB/Rec/GuildColorBorderRec.h"

// STATIC: %0x00B43B9C
const DBCMeta GuildColorBorderRec::s_meta = { "DBFilesClient\\GuildColorBorder.dbc", 4, 8, GuildColorBorderRec::FixupStrings, 0 };

// PROVEN: 7c943553 IDENTICAL/NONE
// OFFSET: 0x00401000
void GuildColorBorderRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void GuildColorBorderRec::ConvertRow(GuildColorBorderRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
}
#endif
