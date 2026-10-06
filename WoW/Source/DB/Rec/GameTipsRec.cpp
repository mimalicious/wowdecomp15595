#include "WoW/Source/DB/Rec/GameTipsRec.h"

// STATIC: 0x00C1D564/0x140A11498
const DBCMeta GameTipsRec::s_meta = { "DBFilesClient\\GameTips.dbc", 4, 16, GameTipsRec::FixupStrings, 0 };

// PROVEN: 621e5ad2 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void GameTipsRec::FixupStrings(void* record, const char* strings) {
    GameTipsRec* rec = static_cast<GameTipsRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
}
