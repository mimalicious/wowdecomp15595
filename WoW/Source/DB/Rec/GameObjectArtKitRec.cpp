#include "WoW/Source/DB/Rec/GameObjectArtKitRec.h"

// STATIC: 0x00C254DC/0x140A1B080
const DBCMeta GameObjectArtKitRec::s_meta = { "DBFilesClient\\GameObjectArtKit.dbc", 3, 32, GameObjectArtKitRec::FixupStrings, 0 };

// PROVEN: 3df658e9 IDENTICAL/NONE
// OFFSET: 0x00A2EBB0/0x1407D83A0
void GameObjectArtKitRec::FixupStrings(void* record, const char* strings) {
    GameObjectArtKitRec* rec = static_cast<GameObjectArtKitRec*>(record);
    DBCFixupString(rec->m_textureVariation[0], strings);
    DBCFixupString(rec->m_textureVariation[1], strings);
    DBCFixupString(rec->m_textureVariation[2], strings);
    DBCFixupString(rec->m_attachModel[0], strings);
    DBCFixupString(rec->m_attachModel[1], strings);
    DBCFixupString(rec->m_attachModel[2], strings);
    DBCFixupString(rec->m_attachModel[3], strings);
}
