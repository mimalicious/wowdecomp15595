#include "WoW/Source/DB/Rec/GameObjectDisplayInfoRec.h"

// STATIC: 0x00C25474/0x140A1AFF0
const DBCMeta GameObjectDisplayInfoRec::s_meta = { "DBFilesClient\\GameObjectDisplayInfo.dbc", 8, 84, GameObjectDisplayInfoRec::FixupStrings, 0 };

// PROVEN: 706d2a61 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void GameObjectDisplayInfoRec::FixupStrings(void* record, const char* strings) {
    GameObjectDisplayInfoRec* rec = static_cast<GameObjectDisplayInfoRec*>(record);
    DBCFixupString(rec->m_modelName, strings);
}
