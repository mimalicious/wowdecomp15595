#ifndef WOW_SOURCE_UI_STABLEINFO_H
#define WOW_SOURCE_UI_STABLEINFO_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

class CDataStore;
class ClientPetAdded;
class ClientPetSlotUpdated;

struct StablePetInfo {
    /* 0000 */ uint32_t m_petNumber;
    /* 0004 */ uint32_t m_level;
    /* 0008 */ int32_t m_entry;
    /* 000C */ char m_name[80];
    /* 005C */ uint8_t m_flags;
    /* 005D */ uint8_t unk_005D[3];
};

class CGStableInfo {
    public:
    static StablePetInfo m_pets[25];
    static WOWGUID m_stableMaster;
    static uint32_t m_numPets;
    static uint8_t m_slotChangePending;

    static void InitializeGame();
    static void ShutdownGame();
    static void EnterWorld();
    static void LeaveWorld();
    static void CloseStables(bool force);
    static StablePetInfo* GetPetInfo(uint32_t index);
    static int32_t GetNumActivePets();
    static void SetPetSlot(uint32_t index, uint32_t slot);
    static int32_t FindPetIndexBySpellID(int32_t spellId, int32_t effectIndex);
    static void UpdatePetName(uint32_t petNumber, const char* name);
    static int32_t OnStableList(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnStableCommandResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static bool OnPetSlotUpdated(ClientPetSlotUpdated* msg);
    static bool OnPetAdded(ClientPetAdded* msg);
};

void StableInfoRegisterScriptFunctions();
void StableInfoUnregisterScriptFunctions();

#endif
