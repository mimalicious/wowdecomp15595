#ifndef WOW_SOURCE_UI_TALENTINFO_INL
#define WOW_SOURCE_UI_TALENTINFO_INL

// INLINED
inline uint32_t CGTalentInfo::GetNumTalentGroups(int32_t pet) {
    if (pet) {
        return CGTalentInfo::m_petTalentTab && CGTalentInfo::m_petTabs.m_count > 0;
    }

    return CGTalentInfo::m_numTalentGroups;
}

#endif
