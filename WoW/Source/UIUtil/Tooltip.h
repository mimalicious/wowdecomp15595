#ifndef WOW_SOURCE_UIUTIL_TOOLTIP_H
#define WOW_SOURCE_UIUTIL_TOOLTIP_H

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameScript/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Common/WowTime.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/UIUtil/Types.h"

class AuraData;
class CAaRect;
class CGItemStatsSummary;
class CGItem_C;
class CGPlayer_C;
class CGTalentGroup_C;
class CGTooltip;
class CGUnit_C;
class CImVector;
class CSimpleFontString;
class CSimpleStatusBar;
class CSimpleTexture;
class CStatus;
class CurrencyTypesRec;
class ItemRecSparse;
class ItemSetRec;
class ScalingStatValuesRec;
class SpellItemEnchantmentRec;
class SpellRec;
class TalentRec;
class linkObject;
struct PetAction;
struct ScriptAuraInfo;
struct lua_State;

class TooltipExtendedItemInfo {
    public:
    /* 0000 */ uint32_t m_itemId;
    /* 0004 */ int32_t m_enchants [15];
    /* 0040 */ uint32_t m_enchantDurations [15];
    /* 007C */ int32_t m_enchantCharges [15];
    /* 00B8 */ uint32_t m_cooldown;
    /* 00BC */ int32_t m_proposedEnchant;
    /* 00C0 */ WOWGUID m_creator;
    /* 00C8 */ WOWGUID m_giftCreator;
    /* 00D0 */ uint32_t m_propertySeed;
    /* 00D4 */ int32_t m_randomPropertiesId;
    /* 00D8 */ int32_t m_spellCharges;
    /* 00DC */ uint32_t m_wrapped;
    /* 00E0 */ uint32_t m_lockId;
    /* 00E4 */ int32_t m_maxDurability;
    /* 00E8 */ uint32_t m_durability;
    /* 00EC */ int32_t m_level;
    /* 00F0 */ int32_t m_stackCount;
    /* 00F4 */ int32_t m_maxStackCount;
    /* 00F8 */ uint8_t m_soulbound;

    void Clear();
    int32_t GetItemIdFromLink(const char* link);
};

struct FrameStackInfo {
    /* 0000 */ char m_name [1024];
    /* 0400 */ int32_t m_strata;
    /* 0404 */ int32_t m_level;
    /* 0408 */ bool m_mouseEnabled;
    /* 040C */ uint32_t m_visible;
};

struct ItemSetTooltipData {
    /* 0000 */ CGTooltip* m_tooltip;
    /* 0004 */ const ItemSetRec* m_set;
    /* 0008 */ ItemRecSparse* m_items [17];
    /* 004C */ int32_t* m_equipped;
    /* 0050 */ uint8_t m_slotUsed [19];
};

class CGTooltip : public CSimpleFrame {
    public:
    static WOWGUID m_vehicleStatUnit;
    static CDataAllocator s_tooltipHeap;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);
    static const CImVector* GetThreatStateColor(uint32_t status);
    static const CImVector* GetItemQualityColor(uint32_t quality);
    static const char* GetItemQualityColorString(uint32_t quality);
    static const char* GetTalentHyperlinkString(const TalentRec* talent, const char* name, int32_t rank);
    static int32_t GetScalingStatItemBudget(const ItemRecSparse* item, const ScalingStatValuesRec* values);
    static int32_t GetScalingStatArmor(const ItemRecSparse* item, const ScalingStatValuesRec* values);
    static bool GetScalingStatWeaponDPS(const ItemRecSparse* item, const ScalingStatValuesRec* values, float* minDamage, float* maxDamage);
    static bool DurationIsInt(float time, float fraction);
    static void GetDurationString(char* buffer, uint32_t size, uint64_t time, const char* token, const char* prefix, int32_t roundUp, bool seconds);
    static void GetFloatDurationString(char* buffer, uint32_t size, float time, const char* token, const char* prefix);
    static void __stdcall FormatStringForXML(const char* text, char* buffer);
    static const char* GetCurrencyHyperlinkString(int32_t currencyId);
    static const char* GetSpellHyperlinkString(int32_t spellId);
    static const char* GetAchievementHyperlink(int32_t achievementId, WOWGUID guid, bool completed, WowTime time, uint32_t criteria1, uint32_t criteria2, uint32_t criteria3, uint32_t criteria4);
    static const char* GetGlyphHyperlinkString(int32_t glyphId);
    static const char* GetInstanceLockHyperlink(int32_t mapId, int32_t difficulty, uint32_t encounterMask);
    static void GetItemDurationString(char* buffer, uint32_t size, uint64_t time, const char* token);
    static void GetSummonedByString(const CGUnit_C* unit, char* buffer, uint32_t size);
    static const char* GetItemHyperlinkString(int32_t itemId, uint32_t quality, int32_t enchant, const int32_t* gems, int32_t randomPropertiesId, int32_t seed, int32_t reforge);
    static const char* GetItemHyperlinkString(int32_t itemId, uint32_t quality, int32_t enchant, int32_t randomPropertiesId, int32_t seed, int32_t reforge);
    static const char* GetItemHyperlinkString(const CGItem_C* item);
    static const char* GetQuestDifficultyColorString(int32_t level);
    static const char* GetQuestHyperlinkString(int32_t questId, int32_t level);
    static int32_t GetDislayedScalingItemLevel(const ItemRecSparse* item, const TooltipExtendedItemInfo* info, WOWGUID target, int32_t minimumLevel);

    /* 01F0 */ CSimpleFrame* m_owner;
    /* 01F4 */ int32_t m_anchorType;
    /* 01F8 */ uint32_t m_numLines;
    /* 01FC */ uint32_t m_maxLines;
    /* 0200 */ TSFixedArray<CSimpleFontString*> m_leftStrings;
    /* 020C */ TSFixedArray<CSimpleFontString*> m_rightStrings;
    /* 0218 */ TSFixedArray<int32_t> m_lineWrap;
    /* 0224 */ CSimpleStatusBar* m_statusBar;
    /* 0228 */ CSimpleTexture* m_textures [10];
    /* 0250 */ uint32_t m_textureLines [10];
    /* 0278 */ uint32_t m_numTextures;
    /* 027C */ uint32_t unk_027C;
    /* 0280 */ WOWGUID m_unitGuid;
    /* 0288 */ WOWGUID m_objectGuid;
    /* 0290 */ WOWGUID m_unitAuraGuid;
    /* 0298 */ WOWGUID m_itemGuid;
    /* 02A0 */ WOWGUID m_corpseGuid;
    /* 02A8 */ WOWGUID m_corpseOwnerGuid;
    /* 02B0 */ WOWGUID m_healthGuid;
    /* 02B8 */ int32_t m_itemId;
    /* 02BC */ int32_t m_currencyId;
    /* 02C0 */ int32_t m_spellId;
    /* 02C4 */ int32_t m_questId;
    /* 02C8 */ int32_t m_achievementId;
    /* 02CC */ int32_t m_nextRankSpellId;
    /* 02D0 */ WOWGUID m_achievementGuid;
    /* 02D8 */ bool m_achievementCompleted;
    /* 02DC */ WowTime m_achievementTime;
    /* 02FC */ uint32_t m_achievementCriteria [4];
    /* 030C */ int32_t m_itemNameOnly;
    /* 0310 */ int32_t m_itemSocketPreview;
    /* 0314 */ int32_t m_itemCompare;
    /* 0318 */ WOWGUID m_itemOwner;
    /* 0320 */ int32_t m_pendingQueries;
    /* 0324 */ uint32_t m_fading;
    /* 0328 */ float m_fadeTime;
    /* 032C */ float m_padding;
    /* 0330 */ float m_minimumWidth;
    /* 0334 */ uint32_t m_minimumWidthForce;
    /* 0338 */ float m_offsetX;
    /* 033C */ float m_offsetY;
    /* 0340 */ TooltipExtendedItemInfo m_itemLinkInfo;
    /* 0440 */ int32_t m_itemUseLinkInfo;
    /* 0444 */ int32_t m_itemHideCharges;
    /* 0448 */ int32_t m_itemMinimumLevel;
    /* 044C */ int32_t m_itemSkipSellPrice;
    /* 0450 */ int32_t m_itemDisplayedLevel;
    /* 0454 */ FrameScript_Object::ScriptIx m_onTooltipSetDefaultAnchor;
    /* 0455 */ FrameScript_Object::ScriptIx m_onTooltipCleared;
    /* 0456 */ FrameScript_Object::ScriptIx m_onTooltipAddMoney;
    /* 0457 */ FrameScript_Object::ScriptIx m_onTooltipSetUnit;
    /* 0458 */ FrameScript_Object::ScriptIx m_onTooltipSetItem;
    /* 0459 */ FrameScript_Object::ScriptIx m_onTooltipSetSpell;
    /* 045A */ FrameScript_Object::ScriptIx m_onTooltipSetQuest;
    /* 045B */ FrameScript_Object::ScriptIx m_onTooltipSetAchievement;
    /* 045C */ FrameScript_Object::ScriptIx m_onTooltipSetEquipmentSet;
    /* 045D */ FrameScript_Object::ScriptIx m_onTooltipSetFrameStack;

    ~CGTooltip();
    int32_t GetScriptMetaTable() override;
    FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) override;
    bool IsA(int32_t type) override;
    bool IsA(const char* typeName) override;
    const char* GetObjectTypeName() override;
    void GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) override;
    void PostLoadXML(linkObject* node, CStatus* status) override;
    void OnLayerUpdate(float elapsedSec) override;
    void HideThis() override;
    void ShowThis(bool loadPending) override;

    CGTooltip(CSimpleFrame* parent);
    void ResetPosition(int32_t force);
    void FadeOut();
    void AddMoney(int32_t money, int32_t maxMoney);
    void RunOnTooltipSetUnit();
    void ClearTooltip();
    void AddTexture(const char* texture, const CAaRect& coords, const CImVector& color);
    void CalculateSize();
    void SetOwner(CSimpleFrame* owner, TOOLTIP_ANCHORPOINT anchorType, float offsetX, float offsetY);
    void AppendText(const char* text);
    void AddFontStrings(CSimpleFontString* left, CSimpleFontString* right);
    void AddLine(const char* left, const char* right, const CImVector& leftColor, const CImVector& rightColor, int32_t wrap);
    void AddLine(const char* left, const char* right, int32_t wrap);
    void AddEmpathyText(CGUnit_C* unit, char* buffer, uint32_t size);
    void AddSkinnableText(CGUnit_C* unit, char* buffer, uint32_t size);
    void AddItemNameDescription(int32_t descriptionId);
    void SetCorpse(const WOWGUID& guid);
    int32_t AddTalentPrereqs(const TalentRec* talent, const CGTalentGroup_C* group, int32_t isInspect, int32_t isPet, int32_t usePreview);
    void AddTalentLearnText(const TalentRec* talent, const CGTalentGroup_C* group, int32_t prereqsMet, int32_t rank, int32_t maxRank, int32_t a7, int32_t a8, int32_t isPet, int32_t usePreview);
    void SetCurrency(const CurrencyTypesRec* currency, WOWGUID owner);
    int32_t SetQuest(int32_t questId);
    int32_t SetAchievement(int32_t achievementId, WOWGUID guid, bool completed, WowTime time, uint32_t criteria1, uint32_t criteria2, uint32_t criteria3, uint32_t criteria4);
    void SetGlyph(int32_t glyphId, int32_t hideRemove, int32_t unit);
    int32_t SetEquipmentSet(int32_t setId);
    int32_t SetSpellFlyout(int32_t flyoutId, bool showDescription);
    void SetInstanceLockCompletedEncounters(WOWGUID guid, int32_t mapId, uint32_t difficulty, uint32_t encounterMask);
    void SetFrameStack(bool showHidden);
    int32_t SetItemNoStats();
    void AddDeltaHeader(int32_t& first);
    void AddColoredItemStat(int32_t value, const char* name, int32_t& first);
    void SetBuff(int32_t spellId, int32_t casterLevel, int32_t stacks, uint8_t a5, const AuraData* aura);
    int32_t SetItemDelta(const CGItemStatsSummary& summary, int32_t append);
    int32_t SetUnit(const WOWGUID& guid, int32_t showHealth, const WOWGUID& healthGuid, int32_t a4);
    void AddSpellReagents(const SpellRec* spell, const char* token, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success));
    int32_t SetSpell(int32_t spellId, int32_t brief, uint32_t cooldown, int32_t isPet, int32_t showSubtext, int32_t isInspect, int32_t hideCallPetName, const TalentRec* talent, const CGTalentGroup_C* talentGroup, int32_t append, int32_t usePreview, int32_t a12, int32_t rank, int32_t maxRank, int32_t a15, int32_t noCastTime, int32_t noCost);
    void SetObject(const WOWGUID& guid);
    void SetTalent(const TalentRec* talent, const CGTalentGroup_C* group, int32_t isInspect, int32_t a4, int32_t forcedRank, int32_t isPet, int32_t usePreview);
    int32_t SetPetAction(const PetAction& action);
    int32_t SetItem(int32_t itemId, const WOWGUID& requester, const WOWGUID& itemGuid, int32_t nameOnly, int32_t compare, int32_t useLinkInfo, int32_t append, WOWGUID owner, int32_t socketPreview, FILE* stream, int32_t hideCharges, const TooltipExtendedItemInfo* info, int32_t minimumLevel, int32_t skipLock, int32_t skipSellPrice, const WOWGUID* transmogrify);
    int32_t SetMacro(uint32_t macroId);
    void SetDefaultAnchor();
};

class CGItemStatsSummary {
    public:
    static int32_t GetItemComparison(CGTooltip* first, CGTooltip* second, CGItemStatsSummary& result);

    /* 0000 */ float m_dps;
    /* 0004 */ int32_t m_stats [89];
    /* 0168 */ uint32_t m_flags;

    void Clear();
    void AddStatMod(int32_t stat, int32_t amount);
    void GetSocketFlags(const ItemRecSparse* item);
    void MakeStatNamesPretty(int32_t stat, const int32_t* group, uint32_t count, int32_t merge);
    void CleanStatNames();
    void ParseSpellRec(const SpellRec* spell);
    void ScanEnchantSpells(const ItemRecSparse* item);
    CGItemStatsSummary operator-(const CGItemStatsSummary& other) const;
    static char* GetStatIntGlobalString(uint32_t stat, char* buffer, uint32_t size);
    void AddEnchantEffect(int32_t type, int32_t arg, int32_t amount);
    void ParseWeaponDPS(const ItemRecSparse* item);
    void ParseResistances(const ItemRecSparse* item);
    void ParseBasicStats(const ItemRecSparse* item);
    void FillOutScalingItem(const ItemRecSparse* item, int32_t level);
    void AddEnchantment(const SpellItemEnchantmentRec* enchant);
    void AddEnchantment(const SpellItemEnchantmentRec* enchant, int32_t amount);
    void FillOutRandomProperties(int32_t randomPropertiesId, int32_t seed);
    int32_t Create(const ItemRecSparse* item, const TooltipExtendedItemInfo* info, int32_t level);
    int32_t Create(const CGItem_C* item, int32_t level);
    int32_t Create(const char* link);
    int32_t CreateFromTooltip(CGTooltip* tooltip);
};

extern const char* s_inventoryTypeTokens[29];
extern const char* s_itemModTokens[57];

const char* GetSocketIdentifier(int32_t color);
int32_t FormatString(const char* token, int32_t count, FRAMESCRIPT_GENDER gender, char* buffer, uint32_t size);
int32_t GetEnchantmentConditionText(CGPlayer_C* player, const SpellItemEnchantmentRec* enchant, char* buffer, uint32_t size, int32_t newline);
void TooltipObjectCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void TooltipCorpseCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void TooltipSpellCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void TooltipUnitNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void TooltipQuestCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void TooltipAchievementCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void TooltipItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
int32_t SetTooltipUnitAura(CGTooltip* tooltip, ScriptAuraInfo* info);
bool ResolveItemSetItem(ItemSetTooltipData* data, int32_t pass, int32_t itemId, int32_t slot);
int32_t SetCompareItem(CGTooltip* tooltip, int32_t itemId, int32_t offset, int32_t shift, CGTooltip* compare);

#include "WoW/Source/UIUtil/Tooltip.inl"

#endif
