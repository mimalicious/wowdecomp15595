#include "WoW/Source/DB/Rec/ChrRacesRec.h"

// STATIC: %0x00B43E34
const DBCMeta ChrRacesRec::s_meta = { "DBFilesClient\\ChrRaces.dbc", 23, 96, ChrRacesRec::FixupStrings, 0 };

// PROVEN: fc0c3538 IDENTICAL
// OFFSET: 0x00A2E890
void ChrRacesRec::FixupStrings(void* record, const char* strings) {
    ChrRacesRec* rec = static_cast<ChrRacesRec*>(record);
    rec->m_clientPrefix = &strings[reinterpret_cast<uintptr_t>(rec->m_clientPrefix)];
    rec->m_clientFileString = &strings[reinterpret_cast<uintptr_t>(rec->m_clientFileString)];
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_nameFemale = &strings[reinterpret_cast<uintptr_t>(rec->m_nameFemale)];
    rec->m_nameMale = &strings[reinterpret_cast<uintptr_t>(rec->m_nameMale)];
    rec->m_facialHairCustomization[0] = &strings[reinterpret_cast<uintptr_t>(rec->m_facialHairCustomization[0])];
    rec->m_facialHairCustomization[1] = &strings[reinterpret_cast<uintptr_t>(rec->m_facialHairCustomization[1])];
    rec->m_hairCustomization = &strings[reinterpret_cast<uintptr_t>(rec->m_hairCustomization)];
}

#if defined(WOWDECOMP_ARCH_64)
void ChrRacesRec::ConvertRow(ChrRacesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_flags, &row[0x0004], sizeof(rec->m_flags));
    memcpy(&rec->m_factionID, &row[0x0008], sizeof(rec->m_factionID));
    memcpy(&rec->m_explorationSoundID, &row[0x000C], sizeof(rec->m_explorationSoundID));
    memcpy(&rec->m_maleDisplayID, &row[0x0010], sizeof(rec->m_maleDisplayID));
    memcpy(&rec->m_femaleDisplayID, &row[0x0014], sizeof(rec->m_femaleDisplayID));
    rec->m_clientPrefix = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    memcpy(&rec->m_baseLanguage, &row[0x001C], sizeof(rec->m_baseLanguage));
    memcpy(&rec->m_creatureType, &row[0x0020], sizeof(rec->m_creatureType));
    memcpy(&rec->m_resSicknessSpellID, &row[0x0024], sizeof(rec->m_resSicknessSpellID));
    memcpy(&rec->m_splashSoundID, &row[0x0028], sizeof(rec->m_splashSoundID));
    rec->m_clientFileString = &strings[*reinterpret_cast<const uint32_t*>(&row[0x002C])];
    memcpy(&rec->m_cinematicSequenceID, &row[0x0030], sizeof(rec->m_cinematicSequenceID));
    memcpy(&rec->m_alliance, &row[0x0034], sizeof(rec->m_alliance));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0038])];
    rec->m_nameFemale = &strings[*reinterpret_cast<const uint32_t*>(&row[0x003C])];
    rec->m_nameMale = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0040])];
    rec->m_facialHairCustomization[0] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0044])];
    rec->m_facialHairCustomization[1] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0048])];
    rec->m_hairCustomization = &strings[*reinterpret_cast<const uint32_t*>(&row[0x004C])];
    memcpy(&rec->unk_0050, &row[0x0050], sizeof(rec->unk_0050));
    memcpy(&rec->unk_0054, &row[0x0054], sizeof(rec->unk_0054));
    memcpy(&rec->unk_0058, &row[0x0058], sizeof(rec->unk_0058));
    memcpy(&rec->unk_005C, &row[0x005C], sizeof(rec->unk_005C));
}
#endif
