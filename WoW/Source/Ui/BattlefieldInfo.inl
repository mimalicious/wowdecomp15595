#ifndef WOW_SOURCE_UI_BATTLEFIELDINFO_INL
#define WOW_SOURCE_UI_BATTLEFIELDINFO_INL

// INLINED
inline int32_t CGBattlefieldInfo::GetArenaOpponentIndex(const WOWGUID& guid, int32_t* index) {
    for (int32_t i = 0; i < 5; i++) {
        if (CGBattlefieldInfo::m_arenaOpponents[i] == guid) {
            *index = i;
            return 1;
        }
    }

    return 0;
}

#endif
