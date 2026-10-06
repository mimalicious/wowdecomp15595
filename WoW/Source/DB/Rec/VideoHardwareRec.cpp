#include "WoW/Source/DB/Rec/VideoHardwareRec.h"

// STATIC: %0x00B4746C
const DBCMeta VideoHardwareRec::s_meta = { "DBFilesClient\\VideoHardware.dbc", 23, 92, VideoHardwareRec::FixupStrings, 0 };

// OFFSET: %0x0097C4F0
void VideoHardwareRec::FixupStrings(void* record, const char* strings) {
    VideoHardwareRec* rec = static_cast<VideoHardwareRec*>(record);
    rec->m_oglOverrides = &strings[reinterpret_cast<uintptr_t>(rec->m_oglOverrides)];
    rec->m_d3DOverrides = &strings[reinterpret_cast<uintptr_t>(rec->m_d3DOverrides)];
}

#if defined(WOWDECOMP_ARCH_64)
void VideoHardwareRec::ConvertRow(VideoHardwareRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_vendorID, &row[0x0004], sizeof(rec->m_vendorID));
    memcpy(&rec->m_deviceID, &row[0x0008], sizeof(rec->m_deviceID));
    memcpy(&rec->m_farclipIdx, &row[0x000C], sizeof(rec->m_farclipIdx));
    memcpy(&rec->m_terrainLoddistIdx, &row[0x0010], sizeof(rec->m_terrainLoddistIdx));
    memcpy(&rec->m_terrainShadowLod, &row[0x0014], sizeof(rec->m_terrainShadowLod));
    memcpy(&rec->m_detailDoodadDensityIdx, &row[0x0018], sizeof(rec->m_detailDoodadDensityIdx));
    memcpy(&rec->m_detailDoodadAlpha, &row[0x001C], sizeof(rec->m_detailDoodadAlpha));
    memcpy(&rec->m_animatingDoodadIdx, &row[0x0020], sizeof(rec->m_animatingDoodadIdx));
    memcpy(&rec->m_trilinear, &row[0x0024], sizeof(rec->m_trilinear));
    memcpy(&rec->m_numLights, &row[0x0028], sizeof(rec->m_numLights));
    memcpy(&rec->m_specularity, &row[0x002C], sizeof(rec->m_specularity));
    memcpy(&rec->m_waterLodidx, &row[0x0030], sizeof(rec->m_waterLodidx));
    memcpy(&rec->m_particleDensityIdx, &row[0x0034], sizeof(rec->m_particleDensityIdx));
    memcpy(&rec->m_unitDrawDistIdx, &row[0x0038], sizeof(rec->m_unitDrawDistIdx));
    memcpy(&rec->m_smallCullDistIdx, &row[0x003C], sizeof(rec->m_smallCullDistIdx));
    memcpy(&rec->m_resolutionIdx, &row[0x0040], sizeof(rec->m_resolutionIdx));
    memcpy(&rec->m_baseMipLevel, &row[0x0044], sizeof(rec->m_baseMipLevel));
    rec->m_oglOverrides = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0048])];
    rec->m_d3DOverrides = &strings[*reinterpret_cast<const uint32_t*>(&row[0x004C])];
    memcpy(&rec->m_fixLag, &row[0x0050], sizeof(rec->m_fixLag));
    memcpy(&rec->m_multisample, &row[0x0054], sizeof(rec->m_multisample));
    memcpy(&rec->m_atlasdisable, &row[0x0058], sizeof(rec->m_atlasdisable));
}
#endif
