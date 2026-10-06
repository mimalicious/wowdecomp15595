#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"

// STATIC: 0x00C1D294
const DBCMeta ItemDisplayInfoRec::s_meta = { "DBFilesClient\\ItemDisplayInfo.dbc", 12, 100, ItemDisplayInfoRec::FixupStrings, 0 };

// OFFSET: %0x0097C000
void ItemDisplayInfoRec::FixupStrings(void* record, const char* strings) {
    ItemDisplayInfoRec* rec = static_cast<ItemDisplayInfoRec*>(record);
    rec->m_modelName[0] = &strings[reinterpret_cast<uintptr_t>(rec->m_modelName[0])];
    rec->m_modelName[1] = &strings[reinterpret_cast<uintptr_t>(rec->m_modelName[1])];
    rec->m_modelTexture[0] = &strings[reinterpret_cast<uintptr_t>(rec->m_modelTexture[0])];
    rec->m_modelTexture[1] = &strings[reinterpret_cast<uintptr_t>(rec->m_modelTexture[1])];
    rec->m_inventoryIcon[0] = &strings[reinterpret_cast<uintptr_t>(rec->m_inventoryIcon[0])];
    rec->m_inventoryIcon[1] = &strings[reinterpret_cast<uintptr_t>(rec->m_inventoryIcon[1])];
    rec->m_texture[0] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[0])];
    rec->m_texture[1] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[1])];
    rec->m_texture[2] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[2])];
    rec->m_texture[3] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[3])];
    rec->m_texture[4] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[4])];
    rec->m_texture[5] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[5])];
    rec->m_texture[6] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[6])];
    rec->m_texture[7] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[7])];
}

#if defined(WOWDECOMP_ARCH_64)
void ItemDisplayInfoRec::ConvertRow(ItemDisplayInfoRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_modelName[0] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    rec->m_modelName[1] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    rec->m_modelTexture[0] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
    rec->m_modelTexture[1] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
    rec->m_inventoryIcon[0] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0014])];
    rec->m_inventoryIcon[1] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    memcpy(rec->m_geosetGroup, &row[0x001C], sizeof(rec->m_geosetGroup));
    memcpy(&rec->m_flags, &row[0x0028], sizeof(rec->m_flags));
    memcpy(&rec->m_spellVisualID, &row[0x002C], sizeof(rec->m_spellVisualID));
    memcpy(&rec->m_groupSoundIndex, &row[0x0030], sizeof(rec->m_groupSoundIndex));
    memcpy(rec->m_helmetGeosetVisID, &row[0x0034], sizeof(rec->m_helmetGeosetVisID));
    rec->m_texture[0] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x003C])];
    rec->m_texture[1] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0040])];
    rec->m_texture[2] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0044])];
    rec->m_texture[3] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0048])];
    rec->m_texture[4] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x004C])];
    rec->m_texture[5] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0050])];
    rec->m_texture[6] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0054])];
    rec->m_texture[7] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0058])];
    memcpy(&rec->m_itemVisual, &row[0x005C], sizeof(rec->m_itemVisual));
    memcpy(&rec->m_particleColorID, &row[0x0060], sizeof(rec->m_particleColorID));
}
#endif
