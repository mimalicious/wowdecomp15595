#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"

// STATIC: %0x00B43E04
const DBCMeta CreatureDisplayInfoRec::s_meta = { "DBFilesClient\\CreatureDisplayInfo.dbc", 15, 68, CreatureDisplayInfoRec::FixupStrings, 0 };

// OFFSET: %0x0097C080
void CreatureDisplayInfoRec::FixupStrings(void* record, const char* strings) {
    CreatureDisplayInfoRec* rec = static_cast<CreatureDisplayInfoRec*>(record);
    rec->m_textureVariation[0] = &strings[reinterpret_cast<uintptr_t>(rec->m_textureVariation[0])];
    rec->m_textureVariation[1] = &strings[reinterpret_cast<uintptr_t>(rec->m_textureVariation[1])];
    rec->m_textureVariation[2] = &strings[reinterpret_cast<uintptr_t>(rec->m_textureVariation[2])];
    rec->m_portraitTextureName = &strings[reinterpret_cast<uintptr_t>(rec->m_portraitTextureName)];
}

#if defined(WOWDECOMP_ARCH_64)
void CreatureDisplayInfoRec::ConvertRow(CreatureDisplayInfoRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_modelID, &row[0x0004], sizeof(rec->m_modelID));
    memcpy(&rec->m_soundID, &row[0x0008], sizeof(rec->m_soundID));
    memcpy(&rec->m_extendedDisplayInfoID, &row[0x000C], sizeof(rec->m_extendedDisplayInfoID));
    memcpy(&rec->m_creatureModelScale, &row[0x0010], sizeof(rec->m_creatureModelScale));
    memcpy(&rec->m_creatureModelAlpha, &row[0x0014], sizeof(rec->m_creatureModelAlpha));
    rec->m_textureVariation[0] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    rec->m_textureVariation[1] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x001C])];
    rec->m_textureVariation[2] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0020])];
    rec->m_portraitTextureName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0024])];
    memcpy(&rec->m_sizeClass, &row[0x0028], sizeof(rec->m_sizeClass));
    memcpy(&rec->m_bloodID, &row[0x002C], sizeof(rec->m_bloodID));
    memcpy(&rec->m_npcsoundID, &row[0x0030], sizeof(rec->m_npcsoundID));
    memcpy(&rec->m_particleColorID, &row[0x0034], sizeof(rec->m_particleColorID));
    memcpy(&rec->m_creatureGeosetData, &row[0x0038], sizeof(rec->m_creatureGeosetData));
    memcpy(&rec->m_objectEffectPackageID, &row[0x003C], sizeof(rec->m_objectEffectPackageID));
    memcpy(&rec->unk_0040, &row[0x0040], sizeof(rec->unk_0040));
}
#endif
