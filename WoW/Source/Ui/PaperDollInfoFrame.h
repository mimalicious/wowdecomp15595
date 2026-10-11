#ifndef WOW_SOURCE_UI_PAPERDOLLINFOFRAME_H
#define WOW_SOURCE_UI_PAPERDOLLINFOFRAME_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

class CGPlayer_C;
class CGItem_C;
class ItemRecSparse;

enum INV_ALERT_STATUS {
    INV_ALERT_NONE = 0,
    INV_ALERT_YELLOW = 1,
    INV_ALERT_RED = 2,
};

struct InspectArenaTeam {
    /* 0000 */ uint32_t m_teamId;
    /* 0004 */ uint32_t m_rating;
    /* 0008 */ uint32_t m_weekGames;
    /* 000C */ uint32_t m_weekWins;
    /* 0010 */ uint32_t m_seasonGames;
    /* 0014 */ uint32_t m_personalRating;
};

class CGCharacterInfo {
    public:
    static TSFixedArray<INV_ALERT_STATUS> m_invAlertStatus;
    static void AverageItemLevelDirty();
    static int32_t m_hasHonorData;
    static int32_t m_honorRequestPending;
    static uint32_t m_inspectLifetimeHonorableKills;
    static uint16_t m_inspectYesterdayHonorableKills;
    static uint8_t m_inspectLifetimeHighestRank;
    static uint16_t m_inspectTodayHonorableKills;
    static InspectArenaTeam m_inspectArenaTeams[3];
    static int32_t m_avgItemLevelPending;
    static float m_avgItemLevel;
    static float m_avgItemLevelEquipped;
    static uint32_t m_inspectRatedBGPlayed;
    static uint32_t m_inspectRatedBGRating;
    static uint32_t m_inspectRatedBGWon;
    static uint8_t m_avgItemLevelDirty;
    static INV_ALERT_STATUS CalculateInventoryAlertStatus(CGPlayer_C* player, uint32_t slot);
    static float CalculateAverageItemLevel(bool equippedOnly);
    static void RegisterMirrorHandlers(WOWGUID guid);
    static WOWGUID m_inspectTarget;
    static bool CanInspect(const CGPlayer_C* unit);
    static int32_t GetActivePlayerAmmoID(const CGPlayer_C* player);
    static void PickupInventoryItem(int32_t slot);
    static int32_t PutItemInBackpack(uint32_t slot);
    static void EnterWorld();
    static void LeaveWorld();
    static void SetInspectTarget(const WOWGUID& guid);
    static void InitializeGame();
    static void ShutdownGame();
    static void UpdateInventoryAlerts();
    static void UpdateItem(WOWGUID guid);
    static void UseItem(int32_t slot, WOWGUID* target);
    static void PickupBag(uint32_t slot);
    static void SocketInventoryItem(int32_t slot);
    static void RequestInspectHonorData();
    static int32_t CanEquipItemInSlot(const ItemRecSparse* stats, uint32_t slot);
    static int32_t CanEquipItem(CGItem_C* item, uint32_t slot);
};

void GuildNameCallback(const uint64_t& key, const WOWGUID& guid, void* param, bool success);
void CharacterInfoRegisterScriptFunctions();
void CharacterInfoUnregisterScriptFunctions();

class CDataStore;

int32_t Packet_MSG_INSPECT_HONOR_STATS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_INSPECT_ARENA_TEAMS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

int32_t PlayerCharacterPointsUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);
int32_t PlayerEquipmentChangedHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);

void UnitInventoryChangedCallback(int32_t id, const WOWGUID& guid, void* param, bool success);

#endif
