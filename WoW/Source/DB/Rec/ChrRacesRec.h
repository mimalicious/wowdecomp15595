#ifndef WOW_SOURCE_DB_REC_CHRRACESREC_H
#define WOW_SOURCE_DB_REC_CHRRACESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ChrRacesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ int32_t m_factionID;
    /* 000C */ int32_t m_explorationSoundID;
    /* 0010 */ int32_t m_maleDisplayID;
    /* 0014 */ int32_t m_femaleDisplayID;
    /* 0018 */ const char* m_clientPrefix;
    /* 001C */ int32_t m_baseLanguage;
    /* 0020 */ int32_t m_creatureType;
    /* 0024 */ int32_t m_resSicknessSpellID;
    /* 0028 */ int32_t m_splashSoundID;
    /* 002C */ const char* m_clientFileString;
    /* 0030 */ int32_t m_cinematicSequenceID;
    /* 0034 */ int32_t m_alliance;
    /* 0038 */ const char* m_name;
    /* 003C */ const char* m_nameFemale;
    /* 0040 */ const char* m_nameMale;
    /* 0044 */ const char* m_facialHairCustomization [2];
    /* 004C */ const char* m_hairCustomization;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t unk_0054;
    /* 0058 */ int32_t unk_0058;
    /* 005C */ int32_t unk_005C;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(ChrRacesRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
