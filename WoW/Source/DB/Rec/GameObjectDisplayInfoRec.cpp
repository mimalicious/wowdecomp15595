#include "WoW/Source/DB/Rec/GameObjectDisplayInfoRec.h"

// STATIC: %0x00B469E8
const DBCMeta GameObjectDisplayInfoRec::s_meta = { "DBFilesClient\\GameObjectDisplayInfo.dbc", 8, 84, GameObjectDisplayInfoRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void GameObjectDisplayInfoRec::FixupStrings(void* record, const char* strings) {
    GameObjectDisplayInfoRec* rec = static_cast<GameObjectDisplayInfoRec*>(record);
    rec->m_modelName = &strings[reinterpret_cast<uintptr_t>(rec->m_modelName)];
}

#if defined(WOWDECOMP_ARCH_64)
void GameObjectDisplayInfoRec::ConvertRow(GameObjectDisplayInfoRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_modelName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(rec->m_sound, &row[0x0008], sizeof(rec->m_sound));
    memcpy(rec->m_geoBoxMin, &row[0x0030], sizeof(rec->m_geoBoxMin));
    memcpy(rec->m_geoBoxMax, &row[0x003C], sizeof(rec->m_geoBoxMax));
    memcpy(&rec->m_objectEffectPackageID, &row[0x0048], sizeof(rec->m_objectEffectPackageID));
    memcpy(&rec->unk_004C, &row[0x004C], sizeof(rec->unk_004C));
    memcpy(&rec->unk_0050, &row[0x0050], sizeof(rec->unk_0050));
}
#endif
