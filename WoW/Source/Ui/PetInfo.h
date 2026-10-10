#ifndef WOW_SOURCE_UI_PETINFO_H
#define WOW_SOURCE_UI_PETINFO_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

class CDataStore;
class CTerrainClickEvent;
struct lua_State;

class PetAction {
    public:
    /* 0000 */ uint32_t m_data;

    void SetAutocast(bool enable);
};

class CGPetInfo {
    public:
    static uint32_t m_petMode;
    static uint32_t m_expirationTime;
    static WOWGUID m_pet;
    static uint32_t m_petBarState;
    static int32_t m_attackingTarget;
    static int32_t m_possessSpell;
    static uint8_t m_comboPoints;
    static bool m_petOrderMoveTo;
    static WOWGUID m_comboTarget;
    static PetAction m_actions[10];

    static uint32_t GetPetMode();
    static void SetPetMode(uint32_t mode);
    static void SetPetOrders(uint32_t orders);
    static const char* GetModeToken(uint32_t mode);
    static const char* GetOrdersToken(uint32_t order);
    static void ShowGrid();
    static void HideGrid();
    static void UpdateCooldowns();
    static void SetIsAttackingTarget(int32_t attacking);
    static void SetPossessedSpell(int32_t spellId);
    static void SetPetOrderMoveTo();
    static void ClearPetOrderMoveTo();
    static int32_t OnPetBroken(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnPetRenameable(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static void LeaveWorld();
    static void ShutdownGame();
    static WOWGUID GetPet(uint32_t index);
    static WOWGUID GetTarget();
    static PetAction* GetPetSpellAction(int32_t spellId);
    static void UpdatePetSpellAction(const PetAction* action);
    static void SetComboPoints(WOWGUID target, uint8_t points);
    static uint8_t GetComboPoints(WOWGUID target, bool anyTarget = false);
    static PetAction* FindSpellByID(int32_t spellId);
    static int32_t OnPetSpellChanges(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnPetMode(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnPetActionFeedback(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnPetUpdateComboPoints(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static const char* GetPetActionTexture(const PetAction& action);
    static bool PetActionIsActive(const PetAction& action);
    static bool PetActionsUsable(bool ignoreControl, bool ignoreControl2 = false);
    static int32_t PetActionSlotUsable(const PetAction* action);
    static int32_t IsAttackAction(uint32_t index);
    static void SendPetAction(const PetAction& action, const WOWGUID& target, int32_t useGiven);
    static void PetFollow();
    static void PetAttackTarget(const WOWGUID& target, int32_t useGiven);
    static void PetClearTarget();
    static void PetDismiss();
    static void PetAbandon();
    static void PetRename(const char* name, const char* declinedNames);
    static int32_t IsPetBarUsed();
    static void UpdatePetTempBar();
    static int32_t HandleMoveToTerrainRay(const CTerrainClickEvent& evt);
    static bool HandleTerrainClick(const CTerrainClickEvent& evt);
    static void SetPetCount(uint32_t count);
    static int32_t SetAction(uint32_t index, PetAction& action, int32_t send);
    static void SetAutocast(uint32_t index, int32_t enable);
    static void SetSpellAutocast(int32_t spellId, int32_t enable);
    static void PlaceAction(PetAction& action, uint32_t index);
    static void FindPossessedSpell();
    static int32_t OnPetGUIDs(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static void PickupAction(uint32_t index, int32_t send);
    static void PlaceCursorAction(PetAction action, uint32_t index);
    static void SetPet(WOWGUID pet, uint32_t expiration, int32_t family);
    static int32_t OnPetSpells(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static void InitializeGame();
    static void EnterWorld();
};

extern TSGrowableArray<WOWGUID> s_pets;

void PetInfoRegisterScriptFunctions();
void PetInfoUnregisterScriptFunctions();

#endif
