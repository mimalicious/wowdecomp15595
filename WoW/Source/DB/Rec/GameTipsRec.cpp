#include "WoW/Source/DB/Rec/GameTipsRec.h"

// STATIC: %0x00B43C7C
const DBCMeta GameTipsRec::s_meta = { "DBFilesClient\\GameTips.dbc", 4, 16, GameTipsRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void GameTipsRec::FixupStrings(void* record, const char* strings) {
    GameTipsRec* rec = static_cast<GameTipsRec*>(record);
    rec->unk_0004 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0004)];
}

#if defined(WOWDECOMP_ARCH_64)
void GameTipsRec::ConvertRow(GameTipsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->unk_0004 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
}
#endif
