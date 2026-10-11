#ifndef WOW_SOURCE_UI_CUFPROFILES_H
#define WOW_SOURCE_UI_CUFPROFILES_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class ClientLoadCufProfiles;
struct JamCliCUFProfile;
struct lua_State;

struct HASHKEY_CONSTUTF8I {
    /* 0000 */ const char* m_str;

    bool operator==(const char* str);
};

struct CUFFRAMEOPTIONS {
    /* 0000 */ uint16_t m_frameHeight;
    /* 0002 */ uint16_t m_frameWidth;
    /* 0004 */ uint16_t m_sortBy : 4;
    uint16_t m_healthText : 4;
    uint16_t m_keepGroupsTogether : 1;
    uint16_t m_displayPets : 1;
    uint16_t m_displayMainTankAndAssist : 1;
    uint16_t m_displayHealPrediction : 1;
    uint16_t m_displayAggroHighlight : 1;
    uint16_t m_displayOnlyDispellableDebuffs : 1;
    uint16_t m_displayPowerBar : 1;
    uint16_t m_displayBorder : 1;
    /* 0006 */ uint16_t m_useClassColors : 1;
    uint16_t m_displayNonBossDebuffs : 1;
    uint16_t m_horizontalGroups : 1;
};

struct CUFPOSITION {
    /* 0000 */ uint16_t m_dynamic : 1;
    uint16_t m_topPoint : 1;
    uint16_t m_bottomPoint : 1;
    uint16_t m_leftPoint : 2;
    /* 0002 */ uint16_t m_topOffset;
    /* 0004 */ uint16_t m_bottomOffset;
    /* 0006 */ uint16_t m_leftOffset;
};

struct CUFSTATE {
    /* 0000 */ uint16_t m_locked : 1;
    uint16_t m_shown : 1;
};

struct CUFAUTOACTIVATE {
    /* 0000 */ uint8_t m_2Players : 1;
    uint8_t m_3Players : 1;
    uint8_t m_5Players : 1;
    uint8_t m_10Players : 1;
    uint8_t m_15Players : 1;
    uint8_t m_25Players : 1;
    uint8_t m_40Players : 1;
    uint8_t m_spec1 : 1;
    /* 0001 */ uint8_t m_spec2 : 1;
    uint8_t m_pvp : 1;
    uint8_t m_pve : 1;
};

class CUFProfileInfo {
    public:
    /* 0000 */ uint32_t m_version;
    /* 0004 */ char m_name[128];
    /* 0084 */ CUFFRAMEOPTIONS m_options;
    /* 008C */ CUFPOSITION m_position;
    /* 0094 */ CUFSTATE m_state;
    /* 0096 */ CUFAUTOACTIVATE m_autoActivate;

    CUFProfileInfo();
    CUFProfileInfo(const char* name);
    void CopySettings(const CUFProfileInfo* source);
    void SetDefaults();
    void FromJam(const JamCliCUFProfile& jam);
    JamCliCUFProfile* ToJam(JamCliCUFProfile* jam);
};

class CGCUFProfiles {
    public:
    struct PROFILENODE : public TSHashObject<PROFILENODE, HASHKEY_CONSTUTF8I> {
        /* 0018 */ CUFProfileInfo* m_profile;
    };

    struct OPTIONENTRY : public TSHashObject<OPTIONENTRY, HASHKEY_CONSTSTR> {
        /* 0018 */ void (*m_get)(CUFProfileInfo* profile, lua_State* L);
        /* 001C */ void (*m_set)(CUFProfileInfo* profile, lua_State* L, int32_t index);
    };

    static char s_savedName[128];
    static CUFAUTOACTIVATE s_savedAutoActivate;
    static bool s_dirty;
    static bool s_loaded;
    static CUFFRAMEOPTIONS s_savedOptions;
    static TSGrowableArray<CUFProfileInfo*> s_profiles;
    static TSHashTable<PROFILENODE, HASHKEY_CONSTUTF8I> s_profileHash;
    static TSHashTable<OPTIONENTRY, HASHKEY_CONSTSTR> s_options[4];

    static void InitializeGame();
    static void EnterWorld(bool reload);
    static void ShutdownGame();
    static void GetProfilesPath(char* path, uint32_t size);
    static void SaveProfiles();
    static CUFProfileInfo* GetProfile(const char* name);
    static bool HasUnsavedChanges();
    static void SendProfiles(bool logout);
    static void PushOptions(CUFProfileInfo* profile, lua_State* L, int32_t table, int32_t index);
    static void DeleteProfile(CUFProfileInfo* profile);
    static void SaveProfileCopy(CUFProfileInfo* profile);
    static void RestoreProfileCopy();
    static void AddProfile(CUFProfileInfo* profile);
    static bool LoadProfilesHandler(ClientLoadCufProfiles* msg);
    static void CreateProfile(const char* name, CUFProfileInfo* base);
};

#include "WoW/Source/Ui/CUFProfiles.inl"

#endif
