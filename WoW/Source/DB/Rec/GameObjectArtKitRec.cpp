#include "WoW/Source/DB/Rec/GameObjectArtKitRec.h"

// STATIC: %0x00B46A20
const DBCMeta GameObjectArtKitRec::s_meta = { "DBFilesClient\\GameObjectArtKit.dbc", 3, 32, GameObjectArtKitRec::FixupStrings, 0 };

// OFFSET: %0x0097C3D0
void GameObjectArtKitRec::FixupStrings(void* record, const char* strings) {
    GameObjectArtKitRec* rec = static_cast<GameObjectArtKitRec*>(record);
    rec->m_textureVariation[0] = &strings[reinterpret_cast<uintptr_t>(rec->m_textureVariation[0])];
    rec->m_textureVariation[1] = &strings[reinterpret_cast<uintptr_t>(rec->m_textureVariation[1])];
    rec->m_textureVariation[2] = &strings[reinterpret_cast<uintptr_t>(rec->m_textureVariation[2])];
    rec->m_attachModel[0] = &strings[reinterpret_cast<uintptr_t>(rec->m_attachModel[0])];
    rec->m_attachModel[1] = &strings[reinterpret_cast<uintptr_t>(rec->m_attachModel[1])];
    rec->m_attachModel[2] = &strings[reinterpret_cast<uintptr_t>(rec->m_attachModel[2])];
    rec->m_attachModel[3] = &strings[reinterpret_cast<uintptr_t>(rec->m_attachModel[3])];
}

#if defined(WOWDECOMP_ARCH_64)
void GameObjectArtKitRec::ConvertRow(GameObjectArtKitRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_textureVariation[0] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    rec->m_textureVariation[1] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    rec->m_textureVariation[2] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
    rec->m_attachModel[0] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
    rec->m_attachModel[1] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0014])];
    rec->m_attachModel[2] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    rec->m_attachModel[3] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x001C])];
}
#endif
