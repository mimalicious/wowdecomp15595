#include "WoW/Source/DB/Rec/PlayerConditionRec.h"

// STATIC: 0x00C22F6C/0x140A18068
const DBCMeta PlayerConditionRec::s_meta = { "DBFilesClient\\PlayerCondition.dbc", 2, 8, PlayerConditionRec::FixupStrings, 0 };

// PROVEN: 4eb054b4 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void PlayerConditionRec::FixupStrings(void* record, const char* strings) {
    PlayerConditionRec* rec = static_cast<PlayerConditionRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
}
