#ifndef WOW_SOURCE_UI_SPELLBOOKFRAME_H
#define WOW_SOURCE_UI_SPELLBOOKFRAME_H

#include <cstdint>
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;
class CGUnit_C;
class CGObject_C;
class AuraData;
class CGPlayer_C;
class SpellRec;

struct SpellUsableInfo {
    /* 0000 */ int32_t m_spellId;
    /* 0004 */ uint8_t m_usable;
    /* 0005 */ uint8_t m_noMana;
    /* 0006 */ uint8_t unk_0006[2];
};

enum SPELLBOOK_ITEM_TYPE {
    SPELLBOOK_ITEM_SPELL = 1,
    SPELLBOOK_ITEM_FUTURESPELL = 2,
    SPELLBOOK_ITEM_PETACTION = 3,
    SPELLBOOK_ITEM_FLYOUT = 4,
};

struct SpellBookItem {
    /* 0000 */ int32_t m_type;
    /* 0004 */ int32_t m_id;
    /* 0008 */ int32_t m_level;
    /* 000C */ int32_t m_skillLine;
    /* 0010 */ uint8_t m_inFlyout;
};

struct SpellBookSkillInfo {
    /* 0000 */ int32_t m_skillLine;
    /* 0004 */ uint32_t m_count;
    /* 0008 */ uint32_t m_numVisible;
    /* 000C */ uint32_t m_numFuture;
    /* 0010 */ uint32_t m_numFlyouts;
    /* 0014 */ uint8_t m_isProfession;
    /* 0015 */ uint8_t m_isGuild;
};

struct SpellBookIntroSlot {
    /* 0000 */ int32_t m_spellId;
    /* 0004 */ int32_t m_slot;
};

struct SPELLRANKNODE : public TSLinkedNode<SPELLRANKNODE> {
    /* 0008 */ int32_t m_spellId;
    /* 000C */ int32_t m_petSpell;
    /* 0010 */ RCString m_subtext;
    /* 001C */ int32_t m_rank;
};

struct PASSIVESPELLNODE : public TSLinkedNode<PASSIVESPELLNODE> {
    /* 0008 */ int32_t m_spellId;
};

struct SPELLSBYNAME : public TSHashObject<SPELLSBYNAME, HASHKEY_UTF8I> {
    /* 0018 */ TSList<SPELLRANKNODE> m_spells;
};

class CGSpellBook {
    public:
    static int32_t m_autoRangedSpell;
    static int32_t m_summonFriendSpell;
    static int32_t m_stuckSpell;
    static int32_t m_duelSpell;
    static uint32_t m_interruptSchoolMask;
    static uint32_t m_interruptSpecialSchoolMask;
    static uint32_t m_silenceSchoolMask;
    static uint32_t m_silenceSpecialSchoolMask;
    static TSFixedArray<int32_t> m_languageSpells;
    static TSGrowableArray<int32_t> m_companions[2];
    static TSGrowableArray<int32_t> m_unlearnedSpells;
    static TSGrowableArray<int32_t> m_totemSpells[4];
    static TSList<PASSIVESPELLNODE> s_passiveSpells;

    static int32_t GetCompanionType(const SpellRec* spell);
    static void SendCompanionUpdate(int32_t type);
    static void UpdateSelection(uint8_t cancelled);
    static int32_t GetSpellRankFromSubtext(const SpellRec* spell);
    static SpellBookSkillInfo* GetSkillInfo(uint32_t index);
    static uint32_t GetSkillInfoIndex(int32_t skillLine);
    static uint32_t GetSkillLineOffset(uint32_t index);
    static void UpdateSilenceAndInterruptsForSpell(const SpellRec* spell);
    static void UpdateCompanions();
    static void UpdateCooldowns();
    static SpellBookItem* GetSpellBookItem(int32_t slot, int32_t petSpell);
    static int32_t GetSkinningSpell(const CGObject_C* object);
    static int32_t IsTrackingSpell(int32_t spellId);
    static int32_t IsShapeshiftSpell(int32_t spellId);
    static int32_t IsAuraCaster(WOWGUID guid, const WOWGUID* caster);
    static int32_t FindRaidDispel(int32_t dispelType);
    static int32_t FindSlotBySpellID(int32_t spellId, int32_t petSpell);
    static int32_t GetSpellProgressionRank(const SpellRec* spell);
    static void GetAppropriateSpellRank(int32_t& spellId, const CGUnit_C* caster, const CGUnit_C* target);
    static bool IsUnlearnedSpell(int32_t spellId);
    static int32_t GetRealSpellBookSkillLine(int32_t spellId, bool* isProfession);
    static uint32_t GetSpellSkillInfoIndex(int32_t spellId);
    static void UpdateSilenceAndInterrupts();
    static void UpdateUsable();
    static void UpdateUsablePower();
    static int32_t IsSelectedSlot(int32_t slot, int32_t petSpell);
    static uint32_t GetShapeshiftForm(int32_t requireForm);
    static int32_t CanShowAura(const SpellRec* spell, const WOWGUID* caster, bool helpful);
    static int32_t CanStealAura(const CGUnit_C* unit, const AuraData* aura, const SpellRec* spell);
    static int32_t CanApplyAura(int32_t spellId, const WOWGUID* target);
    static int32_t CanApplyAura(const AuraData* aura);
    static int32_t FindRaidBuff(int32_t spellId);
    static int32_t FindCompanionSlotByID(int32_t spellId, int32_t type);
    static void ClearPetSpells();
    static void AddPetActions();
    static uint32_t AddSkillInfo(int32_t skillLine, bool isProfession);
    static void InitFutureSpells();
    static void UpdateFutureSpells();
    static void AddFlyout(int32_t flyoutId, int32_t spellId);
    static int32_t GetOverrideSpell(int32_t spellId);
    static void SetCursorSpell(int32_t spellId);
    static void PickupSpellBookItem(int32_t slot, int32_t petSpell);
    static void PickupSpell(int32_t spellId);
    static void PickupCompanion(int32_t type, uint32_t index);
    static void CastSpell(int32_t slot, int32_t petSpell, WOWGUID target);
    static int32_t GetSpell(int32_t slot, int32_t petSpell);
    static int32_t FindSpellByName(const char* name, const char* subtext, int32_t& petSpell);
    static const char* GetSpellTexture(int32_t spellId);
    static const char* GetSpellBookItemTexture(int32_t slot, int32_t petSpell);
    static void SetBarSlotFromIntro(int32_t slot);
    static void DelSpellFromNameCache(int32_t spellId, int32_t petSpell);
    static void UpdateSpells(int32_t spells, int32_t petSpells, int32_t silenceAndInterrupts);
    static int32_t FindSlotByName(const char* name, const char* subtext, int32_t& petSpell);
    static int32_t FindSpellByName(const char* name, int32_t& petSpell);
    static void ClearSpells();
    static void DelKnownSpell(int32_t spellId, int32_t notify);
    static int32_t FindSlotByName(const char* name, int32_t& petSpell);
    static void InitializeGame();
    static void ShutdownGame();
    static void AddSpellToNameCache(int32_t spellId, int32_t petSpell);
    static void AddKnownSpell(int32_t spellId, int32_t notify, int32_t replaceSpell);
    static void ReplaceSpell(int32_t oldSpell, int32_t newSpell);
    static void AddPetSpell(int32_t spellId);
    static void MakeKnownSpellModelsLocal();
    static void CompanionCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static void LeaveWorld(bool reload);
};

extern const char* s_companionTypes[2];
extern TSGrowableArray<SpellUsableInfo> s_shapeshiftSpells;
extern uint32_t s_numSkillInfos;
extern TSGrowableArray<SpellBookItem*> s_spellBookItems;
extern TSGrowableArray<SpellBookItem*> s_petSpellBookItems;
extern TSGrowableArray<SpellBookSkillInfo*> s_skillInfos;
extern TSGrowableArray<int32_t> s_trackingSpells;
extern TSBitField<TSFixedArray<uint32_t> > s_knownSpellBits;
extern TSGrowableArray<int32_t> s_openLockSpells;

bool HasPetSpell(int32_t spellId);
void PrefetchSpellVisuals(int32_t spellId, int32_t wait);
int32_t GetActionBarPageOffset(const CGPlayer_C* player, const SpellRec* spell);

void SpellBookRegisterScriptFunctions();
void SpellBookUnregisterScriptFunctions();


#include "WoW/Source/Ui/SpellBookFrame.inl"

#endif
