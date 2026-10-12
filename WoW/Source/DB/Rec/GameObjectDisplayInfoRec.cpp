#include "WoW/Source/DB/Rec/GameObjectDisplayInfoRec.h"

// STATIC: 0x00C25414/0x140A1AF90
static const int32_t s_fieldOffsets[8] = { 0, 4, 8, 48, 60, 72, 76, 80 };

// STATIC: 0x00C25434/0x140A1AFB0
static const int32_t s_fieldSizes[8] = { 4, 4, 40, 12, 12, 4, 4, 4 };

// STATIC: 0x00C25454/0x140A1AFD0
static const int32_t s_fieldTypes[8] = { 0, 2, 0, 3, 3, 0, 3, 3 };

// STATIC: 0x00C25474/0x140A1AFF0
const DBCMeta GameObjectDisplayInfoRec::s_meta = { "DBFilesClient\\GameObjectDisplayInfo.dbc", 8, 84, GameObjectDisplayInfoRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void GameObjectDisplayInfoRec::FixupStrings(void* record, const char* strings) {
    GameObjectDisplayInfoRec* rec = static_cast<GameObjectDisplayInfoRec*>(record);
    DBCFixupString(rec->m_modelName, strings);
}
