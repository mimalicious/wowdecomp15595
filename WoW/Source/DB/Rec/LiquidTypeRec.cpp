#include "WoW/Source/DB/Rec/LiquidTypeRec.h"

// STATIC: %0x00B40AC8
const DBCMeta LiquidTypeRec::s_meta = { "DBFilesClient\\LiquidType.dbc", 19, 180, LiquidTypeRec::FixupStrings, 0 };

// OFFSET: %0x0097BC80
void LiquidTypeRec::FixupStrings(void* record, const char* strings) {
    LiquidTypeRec* rec = static_cast<LiquidTypeRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_texture[0] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[0])];
    rec->m_texture[1] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[1])];
    rec->m_texture[2] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[2])];
    rec->m_texture[3] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[3])];
    rec->m_texture[4] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[4])];
    rec->m_texture[5] = &strings[reinterpret_cast<uintptr_t>(rec->m_texture[5])];
}

#if defined(WOWDECOMP_ARCH_64)
void LiquidTypeRec::ConvertRow(LiquidTypeRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_flags, &row[0x0008], sizeof(rec->m_flags));
    memcpy(&rec->m_soundBank, &row[0x000C], sizeof(rec->m_soundBank));
    memcpy(&rec->m_soundID, &row[0x0010], sizeof(rec->m_soundID));
    memcpy(&rec->m_spellID, &row[0x0014], sizeof(rec->m_spellID));
    memcpy(&rec->m_maxDarkenDepth, &row[0x0018], sizeof(rec->m_maxDarkenDepth));
    memcpy(&rec->m_fogDarkenIntensity, &row[0x001C], sizeof(rec->m_fogDarkenIntensity));
    memcpy(&rec->m_ambDarkenIntensity, &row[0x0020], sizeof(rec->m_ambDarkenIntensity));
    memcpy(&rec->m_dirDarkenIntensity, &row[0x0024], sizeof(rec->m_dirDarkenIntensity));
    memcpy(&rec->m_lightID, &row[0x0028], sizeof(rec->m_lightID));
    memcpy(&rec->m_particleScale, &row[0x002C], sizeof(rec->m_particleScale));
    memcpy(&rec->m_particleMovement, &row[0x0030], sizeof(rec->m_particleMovement));
    memcpy(&rec->m_particleTexSlots, &row[0x0034], sizeof(rec->m_particleTexSlots));
    memcpy(&rec->m_materialID, &row[0x0038], sizeof(rec->m_materialID));
    rec->m_texture[0] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x003C])];
    rec->m_texture[1] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0040])];
    rec->m_texture[2] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0044])];
    rec->m_texture[3] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0048])];
    rec->m_texture[4] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x004C])];
    rec->m_texture[5] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0050])];
    memcpy(rec->m_color, &row[0x0054], sizeof(rec->m_color));
    memcpy(rec->m_float, &row[0x005C], sizeof(rec->m_float));
    memcpy(rec->m_int, &row[0x00A4], sizeof(rec->m_int));
}
#endif
