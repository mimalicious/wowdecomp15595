#ifndef WOW_SOURCE_MAGIC_MAGICCLIENT_SPELL_C_H
#define WOW_SOURCE_MAGIC_MAGICCLIENT_SPELL_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Magic/SpellCast.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/Ui/Types.h"

struct lua_State;
class SpellWorldObject;
class CSoftFloat;
class SpellEffectRec;
class SpellCategoryRec;
class GameObjectDisplayInfoRec;
class SpellAuraVisibilityRec;
class CGItem;
struct MissileCollisionData;
class CGGameObject_C;
class CGCorpse_C;
class CDataStore;
class SpellRec;
class SpellItemEnchantmentRec;
class SpellVisualRec;
class SpellVisualKitRec;
class ItemSubClassRec;

class CMapEntity;
class ItemStats_C;
class ItemRecSparse;
class OverrideSpellDataRec;
class CGItem_C;
class CGObject_C;
class CGPlayer_C;
class CGUnit_C;
class SpellScalingRec;
class CM2Model;
class CTerrainClickEvent;
class PendingSpellCast : public TSLinkedNode<PendingSpellCast> {
    public:
    /* 0008 */ SpellCast m_data;
    /* 0148 */ uint8_t m_hidden : 1;
    /* 0148 */ uint8_t m_trajectoryLaunched : 1;
    /* 0149 */ uint8_t unk_0149[7];
    /* 0150 */ WOWGUID m_petGuid;
    /* 0158 */ int32_t m_baseSpellId;
    /* 015C */ int32_t m_cooldownDelay;
    /* 0160 */ TSGrowableArray<WOWGUID> m_otherTargets;

    PendingSpellCast();
    PendingSpellCast& operator=(const PendingSpellCast& other);
    void AddOtherTarget(WOWGUID target);
    void OnSpellStartPacket(CDataStore* msg);
    void OnSpellGo(CDataStore* msg);
    bool IsCharmedUnitCast() const;
    bool HasValidTransports() const;
};

struct SPELLHISTORY : public TSLinkedNode<SPELLHISTORY> {
    /* 0008 */ int32_t m_spellId;
    /* 000C */ int32_t m_itemId;
    /* 0010 */ uint32_t m_time;
    /* 0014 */ uint32_t m_recoveryTime;
    /* 0018 */ int32_t m_category;
    /* 001C */ uint32_t m_categoryTime;
    /* 0020 */ uint32_t m_categoryRecoveryTime;
    /* 0024 */ uint8_t m_onHold;
    /* 0025 */ uint8_t unk_0025[3];
    /* 0028 */ int32_t m_startRecoveryCategory;
    /* 002C */ uint32_t m_startRecoveryTime;
    /* 0030 */ uint8_t m_castCount;
    /* 0031 */ uint8_t unk_0031[3];
};

struct WEEKLYSPELLUSE : public TSLinkedNode<WEEKLYSPELLUSE> {
    /* 0008 */ JamWeeklySpellUse m_usage;
};

struct SpellCastTargetPoint {
    /* 0000 */ C3Vector m_position;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ WOWGUID m_transport;
    /* 0018 */ C3Vector m_worldPosition;
    /* 0024 */ C3Vector unk_0024;

    SpellCastTargetPoint();
};

struct NotifyDestLocSpellCastData {
    /* 0000 */ WOWGUID m_caster;
    /* 0008 */ WOWGUID m_destTarget;
    /* 0010 */ int32_t m_spellId;
    /* 0014 */ C3Vector m_destLocation;
    /* 0020 */ C3Vector m_destPosition;
    /* 002C */ float m_elevation;
    /* 0030 */ float unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint8_t unk_0038;
    /* 0039 */ uint8_t m_castCount;
    /* 003A */ uint8_t unk_003A[2];
};

struct SpellInTransit : public TSLinkedNode<SpellInTransit> {
    /* 0008 */ WOWGUID m_caster;
    /* 0010 */ int32_t m_spellId;
    /* 0014 */ uint32_t m_serverTime;
    /* 0018 */ uint32_t m_arrivalTime;
    /* 001C */ uint32_t unk_001C;
};

class SpellHistory {
    public:
    /* 0000 */ TSList<SPELLHISTORY, TSGetLink<SPELLHISTORY>> m_history;
    /* 000C */ TSList<SPELLHISTORY, TSGetLink<SPELLHISTORY>> m_free;

    ~SpellHistory();
    int32_t RecentlyCast(int32_t spellId, int32_t itemId);
    int32_t IsOnHold(int32_t itemId);
    int32_t IsOnHold(int32_t spellId, int32_t itemId);
    void ClearOnHold(int32_t spellId, uint32_t time, bool remove);
    void DelayCooldown(int32_t spellId, int32_t delay);
    void ClearAll();
    void GarbageCollect(uint32_t time);
    void RemoveStartCooldown(int32_t spellId, uint8_t castCount);
    void AddHistory(int32_t spellId, int32_t itemId, uint32_t time, uint32_t recoveryTime, int32_t category, uint32_t categoryTime, uint32_t categoryRecoveryTime, bool onHold, int32_t startRecoveryCategory, uint32_t startRecoveryTime, uint8_t castCount);
    bool GetCooldown(int32_t spellId, int32_t itemId, uint32_t* duration, uint32_t* start, uint32_t* enable, int32_t* remaining);
    bool RefreshCategoryRecovery(int32_t category);
};

struct AmmoSearch {
    /* 0000 */ int32_t m_ammoType;
    /* 0004 */ int32_t m_ammoId;
    /* 0008 */ const CGItem_C* m_item;
};

struct PeriodicClientTrigger : public TSLinkedNode<PeriodicClientTrigger> {
    /* 0008 */ int32_t m_auraSpellId;
    /* 000C */ int32_t m_spellId;
    /* 0010 */ uint32_t m_nextTime;
    /* 0014 */ uint32_t m_period;
    /* 0018 */ WOWGUID m_caster;
};

struct DESTLOCSPELLCASTINDEXHASHNODE : public TSHashObject<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID> {
    /* 0020 */ uint8_t m_castCount;
    /* 0021 */ uint8_t unk_0021[3];
    /* 0024 */ uint32_t m_time;
};

struct ITEMCOOLDOWNHASHNODE : public TSHashObject<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE> {
    /* 0018 */ int32_t m_spellId;
    /* 001C */ uint32_t m_time;
    /* 0020 */ bool m_onHold;
    /* 0021 */ uint8_t unk_0021[3];
};

extern PendingSpellCast* s_targetingSpell;
extern uint32_t s_needTargets;

uint32_t SetTrajectoryCheckBit(uint32_t value, uint32_t shift);
bool Spell_C_IsRestrictedAbility(const SpellRec* spell);
bool SpellIsWeaponDamageEffect(int32_t effect);
const char* GetSpellFailedResult(int32_t reason);
void ClearSpellCastState(CGUnit_C* unit, PendingSpellCast* cast);
bool GetEffectModifierType(int32_t effect, bool* periodic, int32_t aura, bool* rounded);
CGUnit_C* Spell_C_TargetImpliedTarget(CGUnit_C* target, const SpellRec* spell, uint32_t needTargets, CGUnit_C* caster);
bool Spell_C_IsValidTarget(const CGUnit_C* caster, const SpellRec* spell, const CGUnit_C* target);
bool Spell_C_IsTargeting();
int32_t Spell_C_GetTargetingSpell();
PendingSpellCast* Spell_C_GetTargetingSpellCast();
bool Spell_C_CanTargetUnits();
bool Spell_C_CanTargetParty();
bool Spell_C_CanTargetRaid();
bool Spell_C_CanTargetFriends();
bool Spell_C_CanTargetEnemies();
bool Spell_C_CanTargetAlive();
bool Spell_C_CanTargetEnemyCorpses();
bool Spell_C_CanTargetObjects();
bool Spell_C_CanTargetObject(const CGGameObject_C* object);
bool Spell_C_CanTargetTerrain();
int32_t Spell_C_CanTargetItems();
bool Spell_C_CanTargetNonCombatPet();
SpellWorldObject* Spell_C_GetCursorWorldObject();
void Spell_C_SetCursorWorldObjectPosition(const C3Vector& position);
int32_t Packet_SMSG_GAMEOBJECT_RESET_STATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
void Spell_C_PetTameFailure(uint8_t reason);
int32_t Packet_SMSG_FEIGN_DEATH_RESISTED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t OnRemoteMissileTrajectoryCollision(WOWGUID guid, void* param);
int32_t Packet_Group_32(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
uint32_t Spell_C_GetPowerDisplayMod(uint32_t powerType);
int32_t Spell_C_GetAutoRepeatingSpell();
void Spell_C_SetAutoRangedCombatSpell(int32_t spellId);
int32_t Spell_C_GetAutoRangedCombatSpell();
void Spell_C_ClearLastAutoRangedCombatError();
void SpellSetFeedPetSpell(int32_t spellId);
bool SpellVisualKitHasChainEffect(const SpellVisualKitRec* kit);
const char* Spell_C_GetPowerTypeToken(uint32_t powerType);
PendingSpellCast* Spell_C_GetDelayedTrajectoryCast();
void Spell_C_SetMissileTrajectoryNextSequenceIndex(uint32_t index);
uint32_t Spell_C_EncodeSequenceIntoMissileTrajectory(uint32_t* pitch, uint32_t* speed, uint32_t* source, uint32_t* dest);
int32_t Spell_C_IsOverride(int32_t spellId, const OverrideSpellDataRec* rec);
float Spell_C_GetSpellScalingMultiplier(const CGUnit_C* unit, const SpellScalingRec* scaling, bool ignoreCoefficient);
bool Spell_C_SpellScalingCastTime(const CGUnit_C* unit, const SpellScalingRec* scaling, int32_t& castTime);
int32_t Spell_C_GetEnchantmentSlot(int32_t slot, const SpellItemEnchantmentRec* enchant);
int32_t Spell_C_GetMaxStartRecoveryOffset();
void Spell_C_SetMaxStartRecoveryOffset(int32_t offset);
int32_t CompareSpellAuraVisibility(const void* a, const void* b);
PendingSpellCast* GetPendingSpellCast(uint8_t castCount, int32_t spellId, const WOWGUID& caster);
ItemSubClassRec* FindAnyItemSubclassRec(int32_t classId, int32_t subclassMask);
void SpellClearMacroGlobalCooldownWait(const SpellRec* spell);
void StartSpellCastVisuals(CGUnit_C* unit, PendingSpellCast* cast, const SpellRec* spell, int32_t castTime, bool hidden);
void HandleSpellFailedOrGo(CGUnit_C* unit, PendingSpellCast* cast, const SpellRec* spell, int32_t reason);
bool Spell_C_TargetMatchesAutoAttack(const SpellRec* spell, const PendingSpellCast* cast, int32_t spellId, int32_t keepTarget);
void GetCustomSpellFailedString(char* buffer, uint32_t size, int32_t customError);
int32_t Spell_C_GetCategoryResetTime(int32_t cooldown, int32_t daily, int32_t gameTime);
void Spell_C_ModifyCategoryRecovery(int32_t& recovery, int32_t category, int32_t daily);
CGUnit_C* Spell_C_GetStatTargetUnit(int32_t pet, int32_t vehicle);
uint32_t Spell_C_GetSpellLevel(const SpellRec* spell, int32_t isPet, int32_t isVehicle);
int32_t Spell_C_GetSpellLevelBonus(const SpellRec* spell, int32_t level);
bool Spell_C_SpellUsesReagent(const SpellRec* spell, int32_t itemId);
int32_t Spell_C_GetSpellReagentUses(const SpellRec* spell);
bool Spell_C_IsModal(const WOWGUID& item);
bool Spell_C_IsSpellPending(int32_t spellId);
bool Spell_C_HasOtherPendingCast(const WOWGUID& caster, int32_t spellId, const PendingSpellCast* exclude);
bool Spell_C_IsCurrentItem(int32_t itemId);
bool Spell_C_CheckItemLevel(const PendingSpellCast* cast, const SpellRec* spell, const CGObject_C* item, const ItemRecSparse* stats, int32_t* reason);
CGUnit_C* Spell_C_GetImplicitSpellTarget(const CGUnit_C* caster, const SpellRec* spell, const WOWGUID& target);
bool Spell_C_IsActivePlayerCasting();
PendingSpellCast* Spell_C_GetActivePlayerPendingCast();
bool Spell_C_IsCastingInterruptedByCombat(const WOWGUID& guid);
void GameObjectStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
bool Spell_C_CanCastWhileCasting(const SpellRec* spell, const CGUnit_C* unit, int32_t* remaining);
bool Spell_C_CanTargetMe();
bool Spell_C_CanTargetDead();
bool Spell_C_CanTargetFriendCorpses();
bool Spell_C_CanTargetCorpse(const CGCorpse_C* corpse);
bool Spell_C_CanTargetPossessedFriends();
float Spell_C_GetConeAngle(const SpellRec* spell);
bool Spell_C_TargetingSpellAllowsTerrain();
int32_t Packet_SMSG_SPELL_DELAYED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_CHANNEL_START(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_CHANNEL_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
void CreateMissilesForTargets(const TSStackArray<WOWGUID>& targets, CGUnit_C* unit, const SpellRec* spell, const SpellVisualRec* visual, uint32_t ammoDisplayId, uint32_t ammoInventoryType, SpellCast* data, const TSStackArray<uint32_t>* missReasons, const TSStackArray<uint32_t>* reflectResults, uint32_t delay);
void CreateMissilesForTargetPoints(CGUnit_C* unit, const SpellRec* spell, const SpellVisualRec* visual, uint32_t ammoDisplayId, uint32_t ammoInventoryType, SpellCast* data, const TSStackArray<SpellCastTargetPoint>& points, uint32_t delay);
void LaunchMissileVisual(const TSStackArray<WOWGUID>& hits, const TSStackArray<WOWGUID>& misses, CGUnit_C* unit, const SpellRec* spell, const SpellVisualRec* visual, uint32_t ammoDisplayId, uint32_t ammoInventoryType, SpellCast* data, const TSStackArray<uint32_t>& missReasons, const TSStackArray<uint32_t>& reflectResults, uint32_t delay);
int32_t Packet_SMSG_PET_TAME_FAILURE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELL_UPDATE_CHAIN_TARGETS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SET_PROJECTILE_POSITION(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
bool OnPlaySpellVisualKit(ClientPlaySpellVisualKit* msg);
int32_t Packet_SMSG_RESUME_CAST_BAR(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
void Spell_C_SetPlayerClass(int32_t classId);
void Spell_C_CancelChannelSpell(int32_t spellId);
bool SpellHasEffect(const SpellRec* spell, int32_t effect);
bool SpellHasAuraEffect(const SpellRec* spell, int32_t aura);
bool SpellHasNonStanceShapeshiftAuraEffect(const SpellRec* spell);
bool SpellMatchesSpellClass(const SpellRec* spell, int32_t classSet, const int32_t* classMask);
bool SpellHasApplyAuraEffect(const SpellRec* spell);
int32_t SpellIsTrackingAura(const SpellRec* spell);
bool SpellIsCallCompanion(const SpellRec* spell);
bool SpellCancelsAura(const SpellRec* spell, const SpellRec* aura, const CGUnit_C* unit, uint32_t auraEffect, bool a5);
void Spell_C_SetAutoRepeatingSpell(int32_t spellId);
bool OnCancelCombat();
int32_t SpellFaction(const SpellRec* spell);
uint32_t GetRequiredAuraVisionBitmask(const SpellRec* spell);
bool Spell_C_SpellVisualHasChainEffect(const SpellVisualRec* visual);
SpellVisualKitRec* Spell_C_GetAppropriateImpactKit(const SpellVisualRec* visual, int32_t caster);
bool Spell_C_HasNoReagentCost(CGPlayer_C* player, const SpellRec* spell);
bool SpellIsPhysicalAttack(const SpellRec* spell);
void Spell_C_PlayMissileTrajectoryCastVisuals(CGUnit_C* unit, PendingSpellCast* cast, const SpellRec* spell, const SpellVisualRec* visual, const SpellVisualKitRec* kit, const TSStackArray<CGUnit_C*>* targets, float time);
void Spell_C_PlayDestLocVisualsWithoutCaster(NotifyDestLocSpellCastData* data);
bool Spell_C_SpellHasShapeshiftMask(const SpellRec* spell, int32_t form);
bool Spell_C_SpellHasShapeshiftExclude(const SpellRec* spell, int32_t form);
int32_t SpellGetSpellIDForDifficulty(int32_t spellId, int32_t difficulty);
bool Spell_C_CannotBeResurrected();
int32_t Spell_C_GetSpellClassSet(const SpellRec* spell);
int32_t SpellScalingGetClassIndex(const SpellScalingRec* scaling);
bool SpellAuraVisibilityHasTalentTab(int32_t auraVisibility, int32_t talentTab);
void Spell_C_GetDefaultMinRange(const SpellRec* spell, float* minRange);
int32_t Spell_C_GetSpellReagentUses(int32_t spellId);
bool Spell_C_IsActiveSpell(const SpellRec* spell, const CGUnit_C* unit);
void Spell_C_CancelAura(const SpellRec* spell, CGUnit_C* unit, int32_t auraIndex);
void PlayImpactVisual(const SpellRec* spell, const SpellVisualRec* visual, const TSStackArray<WOWGUID>& targets, CGUnit_C* caster);
void PlayImpactVisualOnTarget(const SpellRec* spell, const SpellVisualRec* visual, CGUnit_C* target, CGUnit_C* caster);
bool Spell_C_GetSpellModifiers(const SpellRec* spell, int32_t modifier, float& flat, float& pct);
bool Spell_C_ModifySpellValue(const SpellRec* spell, int32_t& value, int32_t modifier);
bool Spell_C_ModifySpellValue(const SpellRec* spell, float& value, int32_t modifier);
bool Spell_C_ModifySpellValue(const SpellRec* spell, CSoftFloat& value, int32_t modifier);
bool Spell_C_ModifyHealingAndDamage(const SpellRec* spell, CSoftFloat& minValue, CSoftFloat& maxValue, bool healing);
bool SpellMatchesAuraSpellClass(const SpellRec* spell, const SpellRec* aura, int32_t effectIndex);
bool Spell_C_CancelsAuraEffect(const CGUnit_C* unit, const SpellRec* spell, int32_t auraType, int32_t* reason);
bool Spell_C_CancelsStun(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason);
bool Spell_C_CancelsSilence(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason);
bool Spell_C_CancelsPacify(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason);
bool Spell_C_CancelsNoActions(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason);
bool Spell_C_CancelsFear(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason);
bool Spell_C_CancelsConfuse(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason);
bool Spell_C_CancelsCharm(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason);
int32_t Spell_C_GetAuraPeriod(const SpellRec* spell, uint32_t effectIndex);
int32_t Spell_C_SpellGetSpellIDForDifficulty(int32_t spellId);
int32_t Spell_C_WasSpellRecentlyUsed(int32_t spellId, int32_t pet);
void Spell_C_ModifyCooldown(int32_t index, int32_t spellId, int32_t delay);
void Spell_C_ClearSpellHistory(int32_t index);
int32_t Spell_C_GetPowerCostPerSecond(const SpellRec* spell, const CGUnit_C* unit);
void Spell_C_GetMinMaxPoints(const SpellRec* spell, uint32_t effectIndex, float* minPoints, float* maxPoints, int32_t spellLevel, int32_t isPet, int32_t isVehicle, int32_t noModifiers, int32_t noBonus);
bool Spell_C_IsCurrentSpell(int32_t spellId);
bool Spell_C_TargetsAuraStateValid(const CGUnit_C* caster, const SpellRec* spell, uint32_t count, CGUnit_C* const* targets, int32_t* reason, bool skipPrimary);
bool Spell_C_AuraStateValid(const CGUnit_C* caster, const SpellRec* spell, const CGUnit_C* target);
float Spell_C_GetEffectRadius(const SpellRec* spell, uint32_t level);
int32_t Packet_Group_31(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_COOLDOWN_CHEAT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
bool OnClearCooldowns(ClientClearCooldowns* msg);
int32_t Spell_C_GetSpellCategoryWeeklyUses(const SpellRec* spell, uint8_t& used, uint8_t& maximum);
bool Spell_C_HasSpellModifiers(const SpellRec* spell, int32_t modifier);
void Spell_C_ClearPetCooldowns();
int32_t Spell_C_IsSpellInTransit(WOWGUID caster, uint32_t spellId, uint32_t castTime);
PendingSpellCast* CreatePendingSpellCast(bool assignCastCount);
PendingSpellCast* FindPendingSpellCast(const WOWGUID& castItem, PendingSpellCast* fallback, uint8_t castCount, int32_t spellId, const WOWGUID& caster);
void Spell_C_GetMinMaxPoints(const SpellRec* spell, uint32_t effectIndex, int32_t* minPoints, int32_t* maxPoints, int32_t spellLevel, int32_t isPet, int32_t isVehicle, int32_t noModifiers);
float Spell_C_GetSpellRadius();
void Spell_C_PetSpellFailedUpdateCooldowns(int32_t spellId);
void Spell_C_AddToInTransitList(WOWGUID caster, const SpellRec* spell, uint32_t serverTime, uint32_t arrivalTime);
void Spell_C_ClearInTransitList();
bool OnWeeklySpellUsageUpdate(ClientWeeklySpellUsageUpdate* msg);
void Spell_C_ClearCooldowns();
void Spell_C_UpdateInTransitList();
void Spell_C_CleanUpProcessedDestLocSpellCasts(uint32_t time);
int32_t Packet_SMSG_ITEM_COOLDOWN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_RESET_RANGED_COMBAT_TIMER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
bool OnWeeklySpellUsage(ClientWeeklySpellUsage* msg);
void Spell_C_CancelTriggeredAuras(WOWGUID caster, const SpellRec* spell);
void ReleasePendingSpellCast(PendingSpellCast* cast);
void SpellMissingItemCallback(int32_t itemId, const WOWGUID& guid, void* param, bool success);
void SpellMissingItemsCallback(int32_t itemId, const WOWGUID& guid, void* param, bool success);
void FinishPendingSpellCast(PendingSpellCast* cast, CGUnit_C* unit, const SpellRec* spell, int32_t reason);
void Spell_C_FailPendingCast(PendingSpellCast* cast, const SpellRec* spell, int32_t reason);
UIErrors Spell_C_GetSpellFailedError(const SpellRec* spell, int32_t reason, int32_t misc);
void Spell_C_PetSpellFailed(PendingSpellCast* cast, const SpellRec* spell, int32_t reason, int32_t misc1, int32_t misc2);
void Spell_C_ModifyCategoryRecovery(int32_t& recovery, const SpellRec* spell, int32_t pet, int32_t daily);
void Spell_C_SetCooldownLeft(int32_t spellId, uint32_t itemId, uint32_t category, int32_t cooldownLeft, int32_t categoryCooldownLeft, bool onHold, int32_t pet);
void Spell_C_PendingItemCooldownCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
bool Spell_C_HasCooldownOnEvent(const SpellRec* spell, WOWGUID guid);
void Spell_C_StartSpellCooldown(const SpellRec* spell, CGUnit_C* unit, uint8_t castCount, int32_t delay);
bool Spell_C_GetSpellCooldown(int32_t spellId, int32_t petSpell, uint32_t* duration, uint32_t* start, uint32_t* enable, int32_t* remaining = NULL);
bool Spell_C_GetItemCooldown(int32_t itemId, uint32_t* duration, uint32_t* start, uint32_t* enable);
bool Spell_C_GetItemCooldown(CGItem_C* item, uint32_t* duration, uint32_t* start, uint32_t* enable);
int32_t Spell_C_WasItemRecentlyUsed(int32_t itemId);
int32_t Spell_C_IsActionOnHold(const int32_t* spellId, int32_t index);
int32_t Spell_C_GetPowerCost(const SpellRec* spell, const CGUnit_C* unit);
int32_t Spell_C_GetCastTime(const SpellRec* spell, int32_t isPet, int32_t isVehicle, int32_t allowNegative);
void Spell_C_GetMinMaxRange(CGUnit_C* caster, const SpellRec* spell, float* minRange, float* maxRange, int32_t friendly, CGObject_C* target);
bool Spell_C_CheckEnchantItemLevel(const PendingSpellCast* cast, const SpellRec* spell, WOWGUID itemGuid, int32_t* reason);
bool Spell_C_HaveSpellPower(CGUnit_C* unit, const SpellRec* spell, int32_t& powerType, int32_t& requirement);
void Spell_C_CancelSpell(PendingSpellCast* cast, bool stopRecast, bool notifyServer, int32_t reason);
void Spell_C_CancelSpellByCaster(const WOWGUID& caster);
void Spell_C_CancelSpellByTarget(const WOWGUID& target, int32_t reason);
void Spell_C_CancelSpellBySpellID(int32_t spellId);
void Spell_C_CancelMeleeSpells();
void Spell_C_CancelRangedSpells();
void Spell_C_CancelOtherRangedSpells(int32_t spellId);
void Spell_C_CancelCastingSpellByMovement(CGUnit_C* unit);
int32_t Packet_SMSG_SPELL_FAILURE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_CAST_FAILED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELL_COOLDOWN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
void Spell_C_CancelAutoRepeat(bool notifyServer);
void Spell_C_SpellFailed(PendingSpellCast* cast, const SpellRec* spell, int32_t reason, int32_t misc1, int32_t misc2, bool resetCooldown);
void ItemCheckCooldownCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
int32_t Spell_C_NeedsCooldownEvent(int32_t itemId);
void Spell_C_RefreshCategoryCooldowns(int32_t category);
void Spell_C_GetMinMaxRange(CGUnit_C* caster, const SpellRec* spell, float* minRange, float* maxRange, CGObject_C* target);
bool Spell_C_HaveSpellTokens(CGUnit_C* unit, const SpellRec* spell, bool report, PendingSpellCast* cast);
bool Spell_C_HaveEquippedSpellItems(CGUnit_C* unit, const SpellRec* spell, bool report, PendingSpellCast* cast);
void Spell_C_StopTargeting();
void Spell_C_StopCasting();
void Spell_C_CancelPlayerSpells();
int32_t Packet_SMSG_CAST_FAILED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELL_FAILED_OTHER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
void Spell_C_Destroy();
int32_t Spell_C_PrepareMissileTrajectoryCast(CGUnit_C* caster, PendingSpellCast* cast, const SpellRec* spell, const TSStackArray<CGUnit_C*>* targets, float* pitch, float* speed);
void Spell_C_GetMinMaxRange(CGUnit_C* caster, int32_t spellId, float* minRange, float* maxRange, CGObject_C* target);
bool Spell_C_IsUsableAction(CGUnit_C* unit, const SpellRec* spell, const WOWGUID& target, const ItemRecSparse* item, bool ignoreControl, bool ignorePetControl = false, bool ignoreCost = false);
bool RangeCheck(CGUnit_C* caster, int32_t spellId, CGObject_C* target, uint8_t* tooFar);
bool Spell_C_RangeCheckSelected(CGUnit_C* caster, const SpellRec* spell, WOWGUID target, uint8_t* inRange, bool report, PendingSpellCast* cast);
bool Spell_C_HandleSpriteClick(CGObject_C* object, PendingSpellCast* cast);
bool Spell_C_HandleTerrainClick(const CTerrainClickEvent& evt);
bool Spell_C_HandleGlyphClick(int32_t socket);
bool Spell_C_HandleSpriteRay(const WOWGUID& guid, bool checkRange);
int32_t Spell_C_HandleTerrainRay(const CTerrainClickEvent& evt, bool checkRange);
int32_t Spell_C_TargetTradeItem();
bool Spell_C_BeginCast(CGUnit_C* caster, const SpellRec* spell, WOWGUID target, PendingSpellCast* cast, bool confirmed);
bool Spell_C_CastSpell(CGUnit_C* caster, int32_t spellId, const CGItem_C* item, WOWGUID target, PendingSpellCast* cast, bool confirmed, bool a7, int32_t baseSpell, int32_t ignoreCost);
bool Spell_C_CastSpell(int32_t spellId, const CGItem_C* item, WOWGUID target, bool confirmed, bool a5 = false, int32_t baseSpell = 0, int32_t ignoreCost = 0);
void Spell_C_ClickSpell(int32_t spellId, const CGItem_C* item, WOWGUID target, int32_t baseSpell = 0, int32_t ignoreCost = 0);
bool Spell_C_HandleItemDrop(CGItem_C* item, CGUnit_C* unit);
int32_t Spell_C_Idle(const void* data, void* param);
int32_t Spell_C_SendDelayedMissileTrajectoryCast(CGUnit_C* unit);
void Spell_C_AddPendingItemCooldown(int32_t itemId, int32_t spellId, uint32_t time, bool onHold);
void Spell_C_AddPeriodicClientTrigger(WOWGUID caster, int32_t auraSpellId, int32_t spellId, uint32_t period);
void Spell_C_RemovePeriodicClientTrigger(WOWGUID caster, int32_t auraSpellId);
void Spell_C_AddProcessedDestLocSpellCast(WOWGUID caster, uint8_t count, uint32_t time);
int32_t Packet_Group_30(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_NOTIFY_DEST_LOC_SPELL_CAST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
void Spell_C_Initialize();
void Spell_C_SendPendingCast(PendingSpellCast* cast, bool hideVisuals);
float GetGameTableValue(int32_t table, int32_t index, int32_t a3);
void UnitCombatLogCastStart(const WOWGUID& caster, const WOWGUID& target, int32_t spellId);
void SpellTableInitialize();
void SpellRegisterScriptFunctions();
void SpellUnregisterScriptFunctions();
const SpellVisualRec* GetSpellVisual(const SpellRec* spell, const CGUnit_C* unit = NULL);
bool CanPlaySpellVisualForPlayer(uint64_t caster, uint64_t target, int32_t casterIsPlayer, int32_t targetIsPlayer);
bool CanPlaySpellVisual(uint64_t caster, uint64_t target, const SpellVisualRec* visual);
bool CanPlaySpellVisualKit(uint64_t caster, uint64_t target, const SpellVisualKitRec* kit, const SpellVisualRec* visual);
void SpellVisualsHandleCastStart(CGUnit_C* unit, const SpellCast* cast, const SpellRec* spell);
const char* GetPowerString(uint32_t powerType);
int32_t Spell_C_GetDuration(const SpellRec* spell, int32_t isPet, int32_t isUnit, int32_t noModifiers, int32_t applyHaste);
float Spell_C_GetPointsPerCombo(const CGUnit_C* unit, const SpellRec* spell, int32_t effectIndex);

#endif
