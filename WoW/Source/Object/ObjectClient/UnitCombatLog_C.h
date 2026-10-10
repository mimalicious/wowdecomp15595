#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITCOMBATLOG_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITCOMBATLOG_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Net/Types.h"
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/Storm/H/stpl.h"

class ATTACKROUNDINFO;
class AuraData;
class CGUnit_C;
class CDataStore;
class SpellMechanicRec;
class SpellRec;
struct lua_State;
struct PendingCombatLogData;

struct CombatLogEntry : public TSLinkedNode<CombatLogEntry> {
    /* 0008 */ uint32_t m_timestamp;
    /* 000C */ int32_t m_event;
    /* 0010 */ uint32_t m_pendingMask;
    /* 0014 */ bool m_hideCaster;
    /* 0018 */ WOWGUID m_sourceGuid;
    /* 0020 */ const char* m_sourceName;
    /* 0024 */ const char* m_sourceRealm;
    /* 0028 */ uint32_t m_sourceFlags;
    /* 002C */ uint32_t m_sourceRaidFlags;
    /* 0030 */ WOWGUID m_destGuid;
    /* 0038 */ const char* m_destName;
    /* 003C */ const char* m_destRealm;
    /* 0040 */ uint32_t m_destFlags;
    /* 0044 */ uint32_t m_destRaidFlags;
    /* 0048 */ int32_t m_spellId;
    /* 004C */ RCString m_spellName;
    /* 0058 */ uint32_t m_infoFlags;
    /* 005C */ union {
        char* m_text;
        int32_t m_values[7];
        const char* m_names[7];
        uint8_t m_bytes[28];
    } m_info;
    /* 0078 */ uint32_t m_critical : 1;
    /* 0078 */ uint32_t m_glancing : 1;
    /* 0078 */ uint32_t m_crushing : 1;
    /* 0078 */ uint32_t m_isOffHand : 1;
    /* 007C */ uint32_t unk_007C;

    static void PendingCombatLogResolved(int32_t id, const WOWGUID& guid, void* param, bool success);

    CombatLogEntry();
    ~CombatLogEntry();
    void Clear();
    void CancelPendingCallbacks();
    const char* ResolveName(PendingCombatLogData* data, const char** realm, bool pending);
    const char* GetObjectName(const WOWGUID& guid, uint32_t mask, const char** realm);
    int32_t PushEvent(lua_State* L);
    bool MatchesFilter();
    void PackEvent(char* buffer, uint32_t size);
    void SetFailReason(const char* reason);
    void SetAuraValues(const int32_t* values);
    void SetMissInfo(int32_t missType, int32_t amount, bool offHand);
    void SetSource(const WOWGUID& guid);
    void SetExecuteItem(int32_t itemId);
};

struct InvalidatedName : public TSLinkedNode<InvalidatedName> {
    /* 0008 */ char* m_name;

    InvalidatedName();
    ~InvalidatedName();
};

struct PendingCombatLogData : public TSLinkedNode<PendingCombatLogData> {
    /* 0008 */ CombatLogEntry* m_entry;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ int32_t m_id;
    /* 001C */ int32_t m_type;
    /* 0020 */ uint32_t m_mask;
    /* 0024 */ uint32_t unk_0024;
};

struct CombatLogFilter : public TSLinkedNode<CombatLogFilter> {
    /* 0008 */ TSBitField<TSFixedArray<uint32_t> > m_events;
    /* 0018 */ WOWGUID m_sourceGuid;
    /* 0020 */ WOWGUID m_destGuid;
    /* 0028 */ uint32_t m_sourceFlags;
    /* 002C */ uint32_t m_sourceRaidFlags;
    /* 0030 */ uint32_t m_destFlags;
    /* 0034 */ uint32_t m_destRaidFlags;
    /* 0038 */ int32_t m_spellId;
    /* 003C */ RCString m_spellName;

    CombatLogFilter();
    bool SetFilterData(const char* events, const WOWGUID& sourceGuid, uint32_t sourceFlags, uint32_t sourceRaidFlags, const WOWGUID& destGuid, uint32_t destFlags, uint32_t destRaidFlags, int32_t spellId, const char* spellName);
    bool MatchesFilter(int32_t event, const WOWGUID& sourceGuid, uint32_t sourceFlags, uint32_t sourceRaidFlags, const WOWGUID& destGuid, uint32_t destFlags, uint32_t destRaidFlags, int32_t spellId, const char* spellName);
};

class CombatLog {
    public:
    static int32_t HandleSpellLogMiss(CDataStore* msg, int32_t timeOffset);
    static int32_t HandleSpellLogExecute(CDataStore* msg, int32_t timeOffset);
    static int32_t HandleSpellNonMeleeDamageLog(CDataStore* msg, int32_t timeOffset);
    static int32_t HandleSpellHealLog(CDataStore* msg, int32_t timeOffset);
    static int32_t HandleSpellLogEnergizeLog(CDataStore* msg, int32_t timeOffset);
    static int32_t HandleSpellRemoveLog(NETMESSAGE msgId, CDataStore* msg, int32_t timeOffset);
    static int32_t HandlePeriodicAuraLog(CDataStore* msg, int32_t timeOffset);
    static void GetSpellLogLinkInfo(const WOWGUID& source, const WOWGUID& dest, int32_t spellId);
};

extern TSList<CombatLogEntry, TSGetLink<CombatLogEntry> > s_entries;
extern WOWGUID s_combatTextActiveUnit;
extern CombatLogEntry* s_currentEntry;
extern CombatLogEntry* s_pendingEntries;
extern const char* g_spellMissStrings[12];
extern const uint32_t s_environmentalDamageSchools[6];

char* GUIDToString(WOWGUID guid, char* buffer);
WOWGUID StringToGUID(const char* string);
bool CombatLogFlagsMatch(uint32_t flags, uint32_t mask, uint32_t raidFlags, uint32_t raidMask);
void __stdcall CheckToSanitizeUnitLowGuid(WOWGUID* guid);
bool IsCombatTextActiveUnit(WOWGUID guid);
void SendToCombatText(int32_t type, const char* text, int32_t amount);
void SendToCombatText(int32_t type, const char* text, int32_t amount, int32_t overflow);
void SendToCombatText(int32_t type, int32_t amount, int32_t overflow);
void SendToCombatText(int32_t type, int32_t amount);
void SendToCombatText(int32_t type, float amount);
void SendToCombatText(int32_t type, const char* text);
void SendToCombatText(int32_t type);
uint32_t GetCombatLogObjectRaidFlags(const WOWGUID& guid);
uint32_t GetCombatLogObjectFlags(const WOWGUID& guid);
bool SpellHasLearnSpellEffect(const SpellRec* spell);
bool SpellHasOpenLockEffect(const SpellRec* spell);
void UnitCombatLogProcGo(const WOWGUID& casterGuid, const SpellRec* spell);
void UnitCombatLogSpellMechanic(const WOWGUID& unitGuid, const AuraData* aura, const SpellRec* spell, uint32_t effectMask);
PendingCombatLogData* GetPendingCombatLogData(CombatLogEntry* entry, const WOWGUID& guid, uint32_t mask);
void UnitCombatLogInvalidateName(const char* name);
void RecycleCombatLogEntry(CombatLogEntry* entry);
CombatLogEntry* GetCombatLogEntry(const WOWGUID& source, const WOWGUID& dest, int32_t event, const SpellRec* spell);
void QueueCombatLogEntry(CombatLogEntry* entry, uint32_t timeOffset);
void SendCombatLogEvents();
void UnitCombatLog(const WOWGUID& guid, int32_t type, int32_t damage, int32_t absorbed, int32_t resisted);
void UnitCombatLog(const ATTACKROUNDINFO& info);
void UnitCombatLogCastStart(const WOWGUID& caster, const WOWGUID& target, const SpellRec* spell);
void UnitCombatLogCastGo(const WOWGUID& caster, const WOWGUID& target, const SpellRec* spell);
void UnitCombatLogSpellFail(const WOWGUID& caster, const SpellRec* spell, const char* reason);
void UnitCombatLogSpellMissed(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, uint32_t missType, bool damageShield, bool periodic, int32_t timeOffset);
void UnitCombatLogSpellDamage(const WOWGUID& source, const WOWGUID& dest, const SpellRec* spell, int32_t damage, int32_t overkill, int32_t school, int32_t absorbed, int32_t resisted, int32_t blocked, int32_t hitInfo, bool periodic, int32_t timeOffset);
void UnitCombatLogHeal(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, int32_t amount, int32_t overheal, int32_t absorbed, bool crit, bool periodic, int32_t timeOffset);
void UnitCombatLogEnergize(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, int32_t powerType, int32_t amount, bool periodic, int32_t timeOffset);
void UnitCombatLogPowerDrain(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, int32_t powerType, int32_t amount, float multiplier, bool periodic, int32_t timeOffset, bool leech);
void UnitCombatLogInstakill(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell);
void UnitCombatLogSummon(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell);
void UnitCombatLogResurrect(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell);
void UnitCombatLogInterrupt(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, const SpellRec* interrupted);
void UnitCombatLogExtraAttacks(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, int32_t count, int32_t timeOffset);
void UnitCombatLogDurabilityDamage(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, int32_t itemId);
void UnitCombatLogDurabilityDamageAll(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell);
void UnitCombatLogAuraAddedOrRemoved(CGUnit_C* unit, AuraData* aura, const SpellRec* spell, bool added);
void UnitCombatLogAuraModifiedApplications(CGUnit_C* unit, WOWGUID& caster, const SpellRec* spell, int32_t oldStack, int32_t newStack, bool harmful);
void UnitCombatLogDispelled(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, const SpellRec* dispelled, bool harmful, bool stolen, int32_t timeOffset);
void UnitCombatLogAuraBroken(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, const SpellRec* breaker, bool harmful);
void UnitCombatLogAuraRefreshed(CGUnit_C* unit, AuraData* aura, const SpellRec* spell);
void UnitCombatLogDamageShield(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, int32_t damage, int32_t overkill, int32_t school, int32_t absorbed);
void UnitCombatLogSplitDamage(const WOWGUID& source, const WOWGUID& dest, const SpellRec* spell, int32_t damage, int32_t overkill, int32_t school, int32_t absorbed, int32_t resisted, int32_t blocked, int32_t hitInfo, int32_t timeOffset);
void UnitCombatLogUnitDead(CGUnit_C* unit);
void UnitCombatLogBuildingDamage(const WOWGUID& caster, const WOWGUID& owner, const WOWGUID& building, int32_t damage, const SpellRec* spell);
void UnitCombatLogBuildingHeal(const WOWGUID& caster, const WOWGUID& owner, const WOWGUID& building, int32_t amount, const SpellRec* spell);
void UnitCombatLogReset();
void UnitCombatLogInitialize();
void UnitCombatLogShutdown();
bool ShouldShowFeedback(const ATTACKROUNDINFO& info);
void UnitCombatLogRegisterScriptFunctions();
void UnitCombatLogUnregisterScriptFunctions();

#endif
