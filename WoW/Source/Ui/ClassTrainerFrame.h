#ifndef WOW_SOURCE_UI_CLASSTRAINERFRAME_H
#define WOW_SOURCE_UI_CLASSTRAINERFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

struct TrainerServiceInfo {
    /* 0000 */ int32_t m_spellID;
    /* 0004 */ int32_t m_skillLine;
    /* 0008 */ int32_t m_moneyCost;
    /* 000C */ uint8_t m_levelReq;
    /* 000D */ uint8_t unk_000D[3];
    /* 0010 */ int32_t m_skillReq;
    /* 0014 */ uint32_t m_skillRankReq;
    /* 0018 */ int32_t m_abilityReq[3];
    /* 0024 */ int32_t m_talentTabReq;
    /* 0028 */ uint8_t m_state;
    /* 0029 */ uint8_t unk_0029[3];
    /* 002C */ int32_t m_visible;
};

class CGClassTrainer {
    public:
    static char m_greetingText[2048];
    static WOWGUID m_trainer;
    static int32_t m_trainerID;
    static int32_t m_trainerType;
    static int32_t m_currentSelection;
    static uint32_t m_numServices;
    static uint32_t m_filteredServices;
    static TSGrowableArray<TrainerServiceInfo*> m_services;
    static int32_t m_stepIndex;

    static void EnterWorld();
    static void SetTrainer(const WOWGUID& trainer, int32_t trainerID, int32_t type);
    static int32_t GetSelectionIndex();
    static TrainerServiceInfo* GetService(uint32_t index);
    static void LeaveWorld();
    static void FilterAndSortServices();
    static void BuyService(uint32_t index);
    static void BuyAllServices();
    static void SetServiceTypeFilter(int32_t filter);
    static void ShutdownGame();
    static void AddServices(uint32_t count, const int32_t* spellIds, const int32_t* moneyCosts, const uint8_t* levelReqs, const int32_t* skillReqs, const uint32_t* skillRankReqs, const int32_t* const* abilityReqs, const int32_t* talentTabReqs, const uint8_t* states, const char* greeting);
    static void RefreshList();
};

void TrainerItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
int32_t GetServiceTypeFromString(const char* type);
int32_t GetServiceSkillLine(int32_t spellId);

void ClassTrainerRegisterScriptFunctions();
void ClassTrainerUnregisterScriptFunctions();

#endif
