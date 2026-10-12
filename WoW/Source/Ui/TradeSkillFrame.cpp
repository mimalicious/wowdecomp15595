#include "TradeSkillFrame.h"
#include <cstdio>
#include <cstdlib>
#include <malloc.h>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/GemPropertiesRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SpellCastingRequirementsRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellEquippedItemsRec.h"
#include "WoW/Source/DB/Rec/SpellFocusObjectRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/DB/Rec/SpellReagentsRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellTotemsRec.h"
#include "WoW/Source/DB/Rec/TotemCategoryRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GuildInfo.h"
#include "WoW/Source/Ui/QuestTextParser.h"
#include "WoW/Source/Ui/ScalingStats.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/UIUtil/Tooltip.h"

// STATIC: 0x00C01C68/0x1409F9E50
static const char s_base64Encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// STATIC: 0x00C01CB0
static const uint8_t s_base64Decode[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0,
    0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0,
};

// STATIC: 0x00D145E4/0x140BCCF48
const char* s_tradeLinkColor = "ffffd000";

// STATIC: 0x00D1461C/0x140BCCFB8
uint32_t CGTradeSkillInfo::m_subClassFilter = 0xFFFFFFFF;

// STATIC: 0x00D14620/0x140BCCFBC
uint32_t CGTradeSkillInfo::m_subClassFilterIndex = 0xFFFFFFFF;

// STATIC: 0x00D14624/0x140BCCFC0
uint32_t CGTradeSkillInfo::m_invTypeFilter = 0xFFFFFFFF;

// STATIC: 0x00D14628/0x140BCCFC4
uint32_t CGTradeSkillInfo::m_collapseFilter = 0xFFFFFFFF;

// STATIC: 0x00D14630/0x140BCCFD0
const char* s_invSlotTokens[27] = { "HEADSLOT", "NECKSLOT", "SHOULDERSLOT", "SHIRTSLOT", "CHESTSLOT", "WAISTSLOT", "LEGSSLOT", "FEETSLOT", "WRISTSLOT", "HANDSSLOT", "FINGER0SLOT", "FINGER1SLOT", "TRINKET0SLOT", "TRINKET1SLOT", "BACKSLOT", "MAINHANDSLOT", "SECONDARYHANDSLOT", "RANGEDSLOT", "TABARDSLOT", "BAGSLOT", "BAGSLOT", "BAGSLOT", "BAGSLOT", "ENCHSLOT_WEAPON", "ENCHSLOT_2HWEAPON", "SHIELDSLOT", "NONEQUIPSLOT" };

// STATIC: 0x00F4B1A0/0x140E3F510
WOWGUID CGTradeSkillInfo::m_playerGuid;

// STATIC: 0x00F4B1A8/0x140E3F518
int32_t CGTradeSkillInfo::m_spellID;

// STATIC: 0x00F4B1AC/0x140E3F51C
int32_t CGTradeSkillInfo::m_skillLineID;

// STATIC: 0x00F4B1B0/0x140E3F520
int32_t CGTradeSkillInfo::m_selectedSkill;

// STATIC: 0x00F4B1B4/0x140E3F524
uint32_t CGTradeSkillInfo::m_itemsPending;

// STATIC: 0x00F4B1B8/0x140E3F528
uint32_t CGTradeSkillInfo::m_numDisplayEntries;

// STATIC: 0x00F4B1BC/0x140E3F52C
uint32_t CGTradeSkillInfo::m_numSubClasses;

// STATIC: 0x00F4B1C0/0x140E3F530
int32_t CGTradeSkillInfo::m_filteredSkills;

// STATIC: 0x00F4B1C4/0x140E3F538
char* CGTradeSkillInfo::m_itemNameFilter;

// STATIC: 0x00F4B1C8/0x140E3F534
int32_t CGTradeSkillInfo::m_itemMinLevelFilter;

// STATIC: 0x00F4B1CC/0x140E3F540
int32_t CGTradeSkillInfo::m_itemMaxLevelFilter;

// STATIC: 0x00F4B1D0/0x140E3F544
int32_t CGTradeSkillInfo::m_showOnlyMakeable;

// STATIC: 0x00F4B1D4/0x140E3F548
int32_t CGTradeSkillInfo::m_showOnlySkillUps;

// STATIC: 0x00F4B1D8/0x140E3F54C
uint32_t CGTradeSkillInfo::m_invSlots;

// STATIC: 0x00F4B1DC/0x140E3F550
int32_t CGTradeSkillInfo::m_recastSpellID;

// STATIC: 0x00F4B1E0/0x140E3F554
uint32_t CGTradeSkillInfo::m_recastTimes;

// STATIC: 0x00F4B1E4/0x140E3F558
int32_t CGTradeSkillInfo::m_pendingRecastSpellID;

// STATIC: 0x00F4B1E8/0x140E3F55C
uint32_t CGTradeSkillInfo::m_pendingRecastTimes;

// STATIC: 0x00F4B1EC/0x140E3F560
void* CGTradeSkillInfo::m_linkData;

// STATIC: 0x00F4B1F0/0x140E3F568
int32_t CGTradeSkillInfo::m_skillRank;

// STATIC: 0x00F4B1F4/0x140E3F56C
int32_t CGTradeSkillInfo::m_skillMaxRank;

// STATIC: 0x00F4B1F8/0x140E3F570
bool CGTradeSkillInfo::m_guildMemberSkill;

// STATIC: 0x00F4B1F9/0x140E3F571
bool CGTradeSkillInfo::m_guildSkill;

// STATIC: 0x00F4B1FC/0x140E3F574
int32_t CGTradeSkillInfo::m_lastSkillLineID;

// STATIC: 0x00F4B24C/0x140E3F5D0
TSGrowableArray<TradeSkillInfo*> CGTradeSkillInfo::m_displayList;

// STATIC: 0x00F4B25C/0x140E3F5E8
TSGrowableArray<TradeSkillSubClassInfo*> CGTradeSkillInfo::m_subClassList;

// OFFSET: 0x0098D180/0x14070B640
const char* GetInventorySlotToken(int32_t slot) {
    switch (slot) {
        case 10:
            return "FINGER0SLOT_UNIQUE";
        case 11:
            return "FINGER1SLOT_UNIQUE";
        case 12:
            return "TRINKET0SLOT_UNIQUE";
        case 13:
            return "TRINKET1SLOT_UNIQUE";
        default:
            return s_invSlotTokens[slot];
    }
}

// OFFSET: 0x0098D1D0/0x14070B690
static void TradeSkillLinkNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        CGTradeSkillInfo::SetLink(static_cast<const char*>(param));
    }

    SMemFree(param, __FILE__, __LINE__, 0);
}

// OFFSET: 0x0098D200/0x14070B6D0
static void TradeSkillListItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (CGTradeSkillInfo::m_itemsPending && !--CGTradeSkillInfo::m_itemsPending) {
        CGTradeSkillInfo::RefreshList(1);
    }
}

// OFFSET: 0x0098D220/0x14070B700
void TradeSkillNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_NAME_UPDATE, NULL);
}

// OFFSET: 0x0098D230
void CGTradeSkillInfo::DoTradeSkill(int32_t spellId, uint32_t times) {
    if (spellId == CGTradeSkillInfo::m_recastSpellID) {
        CGTradeSkillInfo::m_recastTimes = times;
    } else {
        CGTradeSkillInfo::m_pendingRecastSpellID = spellId;
        CGTradeSkillInfo::m_pendingRecastTimes = times;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, NULL);
}

// OFFSET: 0x0098D280/0x14070B710
void CGTradeSkillInfo::StopRecast() {
    if (CGTradeSkillInfo::m_recastSpellID || CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, NULL);
    }
}

// OFFSET: 0x0098D2B0/0x14070B750
void CGTradeSkillInfo::StopRecast(int32_t spellId) {
    if (spellId == CGTradeSkillInfo::m_recastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, NULL);
    }

    if (spellId == CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, NULL);
    }
}

// OFFSET: 0x0098D300/0x14070B7A0
void CGTradeSkillInfo::DoRecast(int32_t spellId) {
    if (!CGTradeSkillInfo::m_recastSpellID || !CGTradeSkillInfo::m_recastTimes || CGTradeSkillInfo::m_recastSpellID != spellId) {
        if (CGTradeSkillInfo::m_recastSpellID || CGTradeSkillInfo::m_pendingRecastSpellID) {
            CGTradeSkillInfo::m_recastSpellID = 0;
            CGTradeSkillInfo::m_pendingRecastSpellID = 0;
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, NULL);
        }

        return;
    }

    if (--CGTradeSkillInfo::m_recastTimes) {
        Spell_C_CastSpell(spellId, NULL, WOWGUID(), false);
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, NULL);
    } else {
        CGTradeSkillInfo::m_recastSpellID = 0;
    }
}

// OFFSET: 0x0098D390/0x14070B830
void CGTradeSkillInfo::ActivatePendingRecast(int32_t spellId) {
    if (spellId == CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = CGTradeSkillInfo::m_pendingRecastSpellID;
        CGTradeSkillInfo::m_recastTimes = CGTradeSkillInfo::m_pendingRecastTimes;
    }

    CGTradeSkillInfo::m_pendingRecastSpellID = 0;
    CGTradeSkillInfo::m_pendingRecastTimes = 0;
}

// OFFSET: 0x0098D960/0x14070BEF0
void CGTradeSkillInfo::Close() {
    CGTradeSkillInfo::m_spellID = 0;
    CGTradeSkillInfo::m_skillLineID = 0;
    CGTradeSkillInfo::m_guildMemberSkill = false;
    CGTradeSkillInfo::m_guildSkill = false;

    if (CGTradeSkillInfo::m_recastSpellID || CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, NULL);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_CLOSE, NULL);
}

// OFFSET: 0x0098D9C0/0x14070BF50
SkillLineRec* CGTradeSkillInfo::GetSkillLineRec(int32_t spellId, const uint64_t& guid) {
    WOWGUID requester = WOWGUID::Make(0);
    NameCache* name = g_nameDBCache.GetRecord(guid, requester, NULL, NULL, false);
    SkillLineAbilityRec* ability;

    if (name && (ability = SpellTableLookupAbility(name->m_race, name->m_class, spellId))) {
        return g_skillLineDB.GetRecord(ability->m_skillLine);
    }

    return NULL;
}

// OFFSET: 0x0098DA40/0x14070BFE0
int32_t CGTradeSkillInfo::GetSelectionIndex() {
    if (!CGTradeSkillInfo::m_selectedSkill) {
        return -1;
    }

    uint32_t i;

    for (i = 0; i < CGTradeSkillInfo::m_numDisplayEntries; i++) {
        if (CGTradeSkillInfo::m_displayList.m_data[i]->m_spellId == CGTradeSkillInfo::m_selectedSkill) {
            break;
        }
    }

    if (i == CGTradeSkillInfo::m_numDisplayEntries) {
        return -1;
    }

    return i;
}

// OFFSET: 0x0098DA80/0x14070C030
static int32_t QSortSkills(const void* a, const void* b) {
    const TradeSkillInfo* infoA = *static_cast<TradeSkillInfo* const*>(a);
    const TradeSkillInfo* infoB = *static_cast<TradeSkillInfo* const*>(b);

    if (CGTradeSkillInfo::m_numSubClasses) {
        int32_t shownA = 1;
        int32_t shownB = 1;
        uint32_t indexA = 0;
        uint32_t indexB = 0;

        for (uint32_t i = 0; i < CGTradeSkillInfo::m_numSubClasses; i++) {
            TradeSkillSubClassInfo* subClass = i < CGTradeSkillInfo::m_numSubClasses ? CGTradeSkillInfo::m_subClassList.m_data[i] : NULL;

            if (subClass->m_itemClass == infoA->m_itemClass && subClass->m_itemSubClass == infoA->m_itemSubClass) {
                indexA = i;
                shownA = infoA->m_visible && subClass->m_shown;
            }

            if (subClass->m_itemClass == infoB->m_itemClass && subClass->m_itemSubClass == infoB->m_itemSubClass) {
                indexB = i;
                shownB = infoB->m_visible && subClass->m_shown;
            }
        }

        if (!shownA) {
            if (shownA == shownB) {
                return 0;
            }

            return 2 * (shownB != 0) - 1;
        }

        if (!shownB) {
            if (shownA == shownB) {
                return 0;
            }

            return 2 * (shownB != 0) - 1;
        }

        if (indexB != indexA) {
            return indexB < indexA ? 1 : -1;
        }
    }

    if (infoA->m_spellId == -1 || infoB->m_spellId == -1) {
        return 2 * (infoB->m_spellId == -1) - 1;
    }

    SpellRec* spellA = g_spellDB.GetRecord(infoA->m_spellId);
    SpellRec* spellB = g_spellDB.GetRecord(infoB->m_spellId);

    if (!spellA || !spellB) {
        return 0;
    }

    if (infoA->m_difficulty == infoB->m_difficulty) {
        if (infoA->m_skillLineRank != infoB->m_skillLineRank) {
            return 2 * (infoA->m_skillLineRank < infoB->m_skillLineRank) - 1;
        }

        if (infoA->m_itemLevel != infoB->m_itemLevel) {
            return 2 * (infoA->m_itemLevel < infoB->m_itemLevel) - 1;
        }

        return SStrSortUTF8I(spellA->m_name, spellB->m_name, STORM_MAX_STR);
    }

    return 2 * (infoA->m_difficulty > infoB->m_difficulty) - 1;
}

// OFFSET: 0x0098DC50/0x14070C230
static int32_t QSortSubClasses(const void* a, const void* b) {
    const TradeSkillSubClassInfo* subClassA = *static_cast<TradeSkillSubClassInfo* const*>(a);
    const TradeSkillSubClassInfo* subClassB = *static_cast<TradeSkillSubClassInfo* const*>(b);

    if (subClassA->m_itemClass == subClassB->m_itemClass) {
        ItemSubClassRec* recA = NULL;
        ItemSubClassRec* recB = NULL;

        for (uint32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
            ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

            if (rec->m_classID == subClassA->m_itemClass && rec->m_subClassID == subClassA->m_itemSubClass) {
                recA = rec;

                if (recB) {
                    break;
                }
            }

            if (rec->m_classID == subClassB->m_itemClass && rec->m_subClassID == subClassB->m_itemSubClass) {
                recB = rec;

                if (recA) {
                    break;
                }
            }
        }

        if (!recA || !recB) {
            if (!recA && !recB) {
                return 0;
            }

            return 2 * (recA != NULL) - 1;
        }

        const char* nameA = recA->m_verboseName;

        if (!nameA || !*nameA) {
            nameA = recA->m_displayName;
        }

        const char* nameB = recB->m_verboseName;

        if (!nameB || !*nameB) {
            nameB = recB->m_displayName;
        }

        if (nameA && nameB) {
            return SStrSortUTF8I(nameA, nameB, STORM_MAX_STR);
        }

        return 0;
    }

    return subClassA->m_itemClass > subClassB->m_itemClass ? 1 : -1;
}

// OFFSET: 0x0098DD70/0x14070C3A0
int32_t CGTradeSkillInfo::GetSubClassIndexFromSkill(uint32_t index) {
    if (index >= CGTradeSkillInfo::m_numDisplayEntries) {
        return -1;
    }

    TradeSkillInfo* info = CGTradeSkillInfo::m_displayList.m_data[index];

    if (info->m_spellId >= 0) {
        return -1;
    }

    for (uint32_t i = 0; i < CGTradeSkillInfo::m_numSubClasses; i++) {
        TradeSkillSubClassInfo* subClass = CGTradeSkillInfo::m_subClassList.m_data[i];

        if (subClass->m_itemClass == info->m_itemClass && subClass->m_itemSubClass == info->m_itemSubClass) {
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x0098DDD0/0x14070C400
int32_t CGTradeSkillInfo::IsCollapsedHeader(uint32_t index) {
    if (index >= CGTradeSkillInfo::m_numDisplayEntries) {
        return 0;
    }

    TradeSkillInfo* info = CGTradeSkillInfo::m_displayList.m_data[index];

    if (info->m_spellId >= 0) {
        return 0;
    }

    for (uint32_t i = 0; i < CGTradeSkillInfo::m_numSubClasses; i++) {
        TradeSkillSubClassInfo* subClass = CGTradeSkillInfo::m_subClassList.m_data[i];

        if (subClass->m_itemClass == info->m_itemClass && subClass->m_itemSubClass == info->m_itemSubClass) {
            return (CGTradeSkillInfo::m_collapseFilter & (1 << i)) == 0;
        }
    }

    return 0;
}

// OFFSET: 0x0098EAA0
uint32_t CGTradeSkillInfo::GetInvSlotFilter() {
    TradeSkillSubClassInfo* subClass;
    uint32_t mask;

    if (CGTradeSkillInfo::m_subClassFilterIndex < CGTradeSkillInfo::m_numSubClasses && (subClass = CGTradeSkillInfo::m_subClassList.m_data[CGTradeSkillInfo::m_subClassFilterIndex])) {
        mask = subClass->m_invTypeMask;
    } else {
        mask = 0xFFFFFFFF;
    }

    return CGTradeSkillInfo::m_invSlots & mask;
}

// OFFSET: 0x0098EAD0/0x14070D460
int32_t CGTradeSkillInfo::GetSkillMaxRank() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || CGTradeSkillInfo::m_playerGuid != player->m_obj->m_guid) {
        return CGTradeSkillInfo::m_skillMaxRank;
    }

    return player->GetSkillMaxRank(player->GetSkillIndex(CGTradeSkillInfo::m_skillLineID));
}

// OFFSET: 0x0098EB30/0x14070D4D0
int32_t CGTradeSkillInfo::GetSkillModifier() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || CGTradeSkillInfo::m_playerGuid != player->m_obj->m_guid) {
        return 0;
    }

    return player->GetSkillModifier(player->GetSkillIndex(CGTradeSkillInfo::m_skillLineID));
}

// OFFSET: 0x0098EBC0/0x14070D540
bool CGTradeSkillInfo::GetTradeSkillListLink(int32_t spellId, char* buffer, uint32_t bufferSize) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return false;
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

    if (!effect || effect->m_effect != 47) {
        return false;
    }

    uint64_t guid = player->m_obj->m_guid.m_guid;
    SkillLineRec* skillLine = CGTradeSkillInfo::GetSkillLineRec(spellId, guid);

    if (!skillLine || !skillLine->m_canLink) {
        return false;
    }

    const SkillLineAbilityRec* ability = NULL;
    uint32_t count = 0;

    if (!SpellTableGetSkillLineLinkInfo(skillLine->m_id, ability, count)) {
        return false;
    }

    int32_t index = player->GetSkillIndex(skillLine->m_id);
    uint32_t value = player->GetSkillValue(index);
    uint32_t maxRank = player->GetSkillMaxRank(index);
    SStrPrintf(buffer, bufferSize, "|c%s|Htrade:%d:%d:%d:%llX:", s_tradeLinkColor, spellId, value, maxRank, player->m_obj->m_guid.m_guid);

    uint32_t chars = count / 6;

    if (6 * chars != count) {
        chars++;
    }

    if (strlen(buffer) + 1 + chars >= bufferSize) {
        return false;
    }

    int32_t race = player->m_unit->m_unitFieldBytes0[0];
    int32_t classId = player->m_unit->m_unitFieldBytes0[1];
    uint8_t bits = 0;
    uint8_t bit = 0;
    char* out = &buffer[strlen(buffer)];

    for (uint32_t i = 0; i < count; i++) {
        if (player->IsSpellKnown(ability->m_spell) && SpellTableMatchRaceClass(race, classId, ability->m_raceMask, ability->m_classMask, 0, 0)) {
            bits |= 1 << bit;
        }

        if (++bit >= 6) {
            *out++ = s_base64Encode[bits];
            bit = 0;
            bits = 0;
        }

        ability++;
    }

    if (bit) {
        *out++ = s_base64Encode[bits];
    }

    *out = 0;
    SStrPrintf(out, buffer + bufferSize - out, "|h[%s]|h|r", static_cast<const char*>(spell->m_name));
    return true;
}

// OFFSET: 0x0098F190/0x14070DC50
void CGTradeSkillInfo::ShutdownGame() {
    for (uint32_t i = 0; i < CGTradeSkillInfo::m_displayList.Count(); i++) {
        TradeSkillInfo* info = CGTradeSkillInfo::m_displayList.m_data[i];

        if (info) {
            SMemFree(info, "delete", -1, 0);
        }
    }

    for (uint32_t i = 0; i < CGTradeSkillInfo::m_subClassList.Count(); i++) {
        TradeSkillSubClassInfo* subClass = CGTradeSkillInfo::m_subClassList.m_data[i];

        if (subClass) {
            SMemFree(subClass, "delete", -1, 0);
        }
    }

    CGTradeSkillInfo::m_playerGuid = WOWGUID::Make(0);
    CGTradeSkillInfo::m_spellID = 0;
    CGTradeSkillInfo::m_skillLineID = 0;
    CGTradeSkillInfo::m_displayList.Clear();
    CGTradeSkillInfo::m_subClassList.Clear();
    CGTradeSkillInfo::m_numDisplayEntries = 0;
    CGTradeSkillInfo::m_filteredSkills = 0;
    CGTradeSkillInfo::m_numSubClasses = 0;

    if (CGTradeSkillInfo::m_itemNameFilter) {
        SMemFree(CGTradeSkillInfo::m_itemNameFilter, __FILE__, __LINE__, 0);
    }

    CGTradeSkillInfo::m_itemNameFilter = NULL;
    CGTradeSkillInfo::m_lastSkillLineID = 0;
    CGTradeSkillInfo::m_recastSpellID = 0;
    CGTradeSkillInfo::m_pendingRecastSpellID = 0;

    if (CGTradeSkillInfo::m_linkData) {
        SMemFree(CGTradeSkillInfo::m_linkData, __FILE__, __LINE__, 0);
    }

    CGTradeSkillInfo::m_linkData = NULL;
    CGTradeSkillInfo::m_skillRank = 0;
    CGTradeSkillInfo::m_skillMaxRank = 0;
    CGTradeSkillInfo::m_guildMemberSkill = false;
    CGTradeSkillInfo::m_guildSkill = false;
}

// OFFSET: 0x0098F700/0x14070E3A0
bool Profession_FilterSpell(int32_t spellId, int32_t minLevel, int32_t maxLevel, const char* name) {
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return false;
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);
    ItemRecSparse* item;

    if (effect) {
        WOWGUID requester = WOWGUID::Make(0);
        item = g_itemSparseDB.GetRecord(effect->m_effectItemType, requester, NULL, NULL, false);

        if (item && ((minLevel && item->m_requiredLevel < minLevel) || (maxLevel && item->m_requiredLevel > maxLevel))) {
            return false;
        }
    } else {
        item = NULL;
    }

    if (!name) {
        return true;
    }

    char text[1024];
    char format[1024];
    char schoolName[1024];
    char token[32];
    char colorName[32];

    if (item) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        for (uint32_t i = 0; i < 10; i++) {
            int32_t value = CalculateItemBonusFromPercent(player->m_unit->m_unitFieldMaxitemlevel, item->m_itemLevel, item->m_statModifierBonusAmount[i], item->m_statPercentEditor[i], item->m_statPercentageOfSocket[i], item->m_quality, item->m_inventoryType);

            if (value <= 0 || item->m_statModifierBonusStat[i] < 0) {
                continue;
            }

            FrameScript_GetText(s_itemModTokens[item->m_statModifierBonusStat[i]], format, sizeof(format));
            SStrPrintf(text, sizeof(text), format, value > 0 ? '+' : '-', abs(value));
            CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

            if (SStrStr(text, name)) {
                return true;
            }
        }

        int32_t resist = 0;
        int32_t percent = 0;
        uint32_t resistCount = 0;
        bool mixed = false;

        for (uint32_t i = 0; i < 10; i++) {
            int32_t stat = item->m_statModifierBonusStat[i];

            if (stat < 51 || stat > 56) {
                continue;
            }

            if (percent && item->m_statPercentEditor[i] != percent) {
                mixed = true;
                break;
            }

            percent = item->m_statPercentEditor[i];
            resistCount++;
            resist = CalculateItemBonusFromPercent(player->m_unit->m_unitFieldMaxitemlevel, item->m_itemLevel, item->m_statModifierBonusAmount[i], percent, item->m_statPercentageOfSocket[i], item->m_quality, item->m_inventoryType);
        }

        if (!mixed && resistCount == 5) {
            if (resist) {
                FrameScript_GetText("ITEM_RESIST_ALL", format, sizeof(format));
                SStrPrintf(text, sizeof(text), format, resist > 0 ? '+' : '-', abs(resist));
                CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

                if (SStrStr(text, name)) {
                    return true;
                }
            }
        } else {
            for (uint32_t i = 0; i < 10; i++) {
                int32_t stat = item->m_statModifierBonusStat[i];

                if (stat < 51 || stat > 56) {
                    continue;
                }

                int32_t value = CalculateItemBonusFromPercent(player->m_unit->m_unitFieldMaxitemlevel, item->m_itemLevel, item->m_statModifierBonusAmount[i], item->m_statPercentEditor[i], item->m_statPercentageOfSocket[i], item->m_quality, item->m_inventoryType);
                SStrPrintf(token, 64, "SPELL_SCHOOL%d_CAP", GetDamageClassFromBonusStat(stat));
                SStrCopy(schoolName, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), sizeof(schoolName));
                SStrCopy(format, FrameScript_GetText("ITEM_RESIST_SINGLE", -1, GENDER_NOT_APPLICABLE), sizeof(format));
                SStrPrintf(text, sizeof(text), format, value > 0 ? '+' : '-', abs(value), schoolName);
                CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

                if (SStrStr(text, name)) {
                    return true;
                }
            }
        }

        for (uint32_t i = 0; i < 5; i++) {
            SpellRec* itemSpell = g_spellDB.GetRecord(item->m_spellID[i]);

            if (!itemSpell) {
                continue;
            }

            SpellParserParseText(itemSpell, format, sizeof(format), 0, 0, 0, 0, 1, 0, NULL);

            switch (item->m_spellTrigger[i]) {
                case 0:
                    SStrPrintf(text, sizeof(text), "%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONUSE", -1, GENDER_NOT_APPLICABLE), format);
                    break;
                case 1:
                    SStrPrintf(text, sizeof(text), "%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONEQUIP", -1, GENDER_NOT_APPLICABLE), format);
                    break;
                case 2:
                    SStrPrintf(text, sizeof(text), "%s %s", FrameScript_GetText("ITEM_SPELL_TRIGGER_ONPROC", -1, GENDER_NOT_APPLICABLE), format);
                    break;
                default:
                    SStrCopy(text, format, sizeof(text));
                    break;
            }

            CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

            if (SStrStr(text, name)) {
                return true;
            }
        }

        for (uint32_t i = 0; i < 15; i++) {
            if (i < 2 || i > 4 || !item->m_socketColor[i - 2]) {
                continue;
            }

            SStrCopy(colorName, GetSocketIdentifier(item->m_socketColor[i - 2]), sizeof(colorName));
            SStrUpper(colorName);
            SStrPrintf(format, sizeof(format), "%s%s", "EMPTY_SOCKET_", colorName);
            SStrPrintf(text, sizeof(text), FrameScript_GetText(format, -1, GENDER_NOT_APPLICABLE));
            CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

            if (SStrStr(text, name)) {
                return true;
            }
        }

        GemPropertiesRec* gem;
        SpellItemEnchantmentRec* enchant;

        if (item->m_gemProperties && (gem = g_gemPropertiesDB.GetRecord(item->m_gemProperties)) && (enchant = g_spellItemEnchantmentDB.GetRecord(abs(gem->m_enchantID)))) {
            SStrCopy(text, enchant->m_name, sizeof(text));
            CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

            if (SStrStr(text, name)) {
                return true;
            }
        }

        if (item->m_description && *item->m_description) {
            SStrCopy(text, item->m_description, sizeof(text));
            CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

            if (SStrStr(text, name)) {
                return true;
            }
        }
    }

    SStrCopy(text, spell->m_name, sizeof(text));
    CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

    if (SStrStr(text, name)) {
        return true;
    }

    SpellReagentsRec* reagents;

    if (spell->m_spellReagentsID && (reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID))) {
        for (uint32_t i = 0; i < 8; i++) {
            if (!reagents->m_reagent[i] || !reagents->m_reagentCount[i]) {
                continue;
            }

            WOWGUID requester = WOWGUID::Make(0);
            ItemRecSparse* reagent = g_itemSparseDB.GetRecord(reagents->m_reagent[i], requester, NULL, NULL, false);

            if (reagent) {
                SStrCopy(text, reagent->GetName(0), sizeof(text));
                CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

                if (SStrStr(text, name)) {
                    return true;
                }
            }
        }
    }

    SpellTotemsRec* totems;

    if (spell->m_spellTotemsID && (totems = g_spellTotemsDB.GetRecord(spell->m_spellTotemsID))) {
        for (uint32_t i = 0; i < 2; i++) {
            if (!totems->m_totem[i]) {
                continue;
            }

            WOWGUID requester = WOWGUID::Make(0);
            ItemRecSparse* totem = g_itemSparseDB.GetRecord(totems->m_totem[i], requester, NULL, NULL, false);

            if (totem) {
                SStrCopy(text, totem->GetName(0), sizeof(text));
                CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

                if (SStrStr(text, name)) {
                    return true;
                }
            }
        }

        for (uint32_t i = 0; i < 2; i++) {
            TotemCategoryRec* category;

            if (totems->m_requiredTotemCategoryID[i] && (category = g_totemCategoryDB.GetRecord(totems->m_requiredTotemCategoryID[i]))) {
                SStrCopy(text, category->m_name, sizeof(text));
                CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

                if (SStrStr(text, name)) {
                    return true;
                }
            }
        }
    }

    SpellCastingRequirementsRec* requirements;
    SpellFocusObjectRec* focus;

    if (spell->m_spellCastingRequirementsID && (requirements = g_spellCastingRequirementsDB.GetRecord(spell->m_spellCastingRequirementsID)) && requirements->m_requiresSpellFocus && (focus = g_spellFocusObjectDB.GetRecord(requirements->m_requiresSpellFocus))) {
        SStrCopy(text, focus->m_name, sizeof(text));
        CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

        if (SStrStr(text, name)) {
            return true;
        }
    }

    if (spell->m_description && *spell->m_description) {
        SpellParserParseText(spell, text, sizeof(text), 0, 0, 0, 0, 1, 0, NULL);
        CaseAccentInsensitiveParse(text, sizeof(text), s_locale);

        if (SStrStr(text, name)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x00990280/0x14070F2D0
void CGTradeSkillInfo::ClearItemCallbacks() {
    if (!CGTradeSkillInfo::m_itemsPending) {
        return;
    }

    for (uint32_t i = 0; i < CGTradeSkillInfo::m_numDisplayEntries; i++) {
        const SpellEffectRec* effect = SpellGetEffect(CGTradeSkillInfo::m_displayList.m_data[i]->m_spellId, 0);

        if (effect) {
            g_itemSparseDB.CancelCallback(effect->m_effectItemType, TradeSkillListItemCallback, NULL);
        }
    }

    CGTradeSkillInfo::m_itemsPending = 0;
}

// OFFSET: 0x009902E0/0x14070F400
void CGTradeSkillInfo::FilterAndSortSkills(int32_t resetSubClasses) {
    CGTradeSkillInfo::m_filteredSkills = CGTradeSkillInfo::m_numDisplayEntries;

    if (resetSubClasses) {
        for (uint32_t i = 0; i < CGTradeSkillInfo::m_numSubClasses; i++) {
            CGTradeSkillInfo::m_subClassList.m_data[i]->m_shown = (CGTradeSkillInfo::m_subClassFilter & (1 << i)) != 0;
            CGTradeSkillInfo::m_subClassList.m_data[i]->m_collapsed = (CGTradeSkillInfo::m_collapseFilter & (1 << i)) == 0;
        }
    }

    for (uint32_t i = 0; i < CGTradeSkillInfo::m_numDisplayEntries; i++) {
        CGTradeSkillInfo::m_displayList.m_data[i]->m_visible = 1;
        TradeSkillInfo* info = CGTradeSkillInfo::m_displayList.m_data[i];

        if (info->m_spellId < 0) {
            continue;
        }

        if (CGTradeSkillInfo::m_numSubClasses) {
            uint32_t index = 0;

            for (uint32_t j = 0; j < CGTradeSkillInfo::m_numSubClasses; j++) {
                TradeSkillSubClassInfo* subClass = CGTradeSkillInfo::m_subClassList.m_data[j];

                if (info->m_itemClass == subClass->m_itemClass && info->m_itemSubClass == subClass->m_itemSubClass) {
                    index = j;
                    break;
                }
            }

            if (!CGTradeSkillInfo::m_subClassList.m_data[index]->m_shown) {
                info->m_visible = 0;
                CGTradeSkillInfo::m_filteredSkills--;
                continue;
            }
        }

        if (!(CGTradeSkillInfo::m_invTypeFilter & info->m_invTypeMask)) {
            info->m_visible = 0;
            CGTradeSkillInfo::m_filteredSkills--;
        } else if (CGTradeSkillInfo::m_showOnlyMakeable && !info->m_numAvailable) {
            info->m_visible = 0;
            CGTradeSkillInfo::m_filteredSkills--;
        } else if (CGTradeSkillInfo::m_showOnlySkillUps && info->m_difficulty > 2) {
            info->m_visible = 0;
            CGTradeSkillInfo::m_filteredSkills--;
        } else if ((CGTradeSkillInfo::m_itemNameFilter || CGTradeSkillInfo::m_itemMinLevelFilter || CGTradeSkillInfo::m_itemMaxLevelFilter) && !Profession_FilterSpell(CGTradeSkillInfo::m_displayList.m_data[i]->m_spellId, CGTradeSkillInfo::m_itemMinLevelFilter, CGTradeSkillInfo::m_itemMaxLevelFilter, CGTradeSkillInfo::m_itemNameFilter)) {
            CGTradeSkillInfo::m_displayList.m_data[i]->m_visible = 0;
            CGTradeSkillInfo::m_filteredSkills--;
        }
    }

    if (CGTradeSkillInfo::m_numSubClasses) {
        for (uint32_t i = 0; i < CGTradeSkillInfo::m_numDisplayEntries; i++) {
            if (CGTradeSkillInfo::m_displayList.m_data[i]->m_spellId >= 0) {
                continue;
            }

            int32_t subClass = CGTradeSkillInfo::GetSubClassIndexFromSkill(i);
            int32_t any = 0;

            for (uint32_t j = 0; j < CGTradeSkillInfo::m_numDisplayEntries; j++) {
                if (i == j) {
                    continue;
                }

                TradeSkillInfo* info = CGTradeSkillInfo::m_displayList.m_data[j];
                TradeSkillInfo* header = CGTradeSkillInfo::m_displayList.m_data[i];

                if (header->m_itemClass == info->m_itemClass && header->m_itemSubClass == info->m_itemSubClass && info->m_visible) {
                    any = 1;

                    if (CGTradeSkillInfo::m_subClassList.m_data[subClass]->m_collapsed) {
                        info->m_visible = 0;
                        CGTradeSkillInfo::m_filteredSkills--;
                    }
                }
            }

            if (!any) {
                CGTradeSkillInfo::m_displayList.m_data[i]->m_visible = 0;
                CGTradeSkillInfo::m_filteredSkills--;
            }
        }
    }

    qsort(CGTradeSkillInfo::m_subClassList.m_data, CGTradeSkillInfo::m_numSubClasses, sizeof(TradeSkillSubClassInfo*), QSortSubClasses);
    qsort(CGTradeSkillInfo::m_displayList.m_data, CGTradeSkillInfo::m_numDisplayEntries, sizeof(TradeSkillInfo*), QSortSkills);
}

// OFFSET: 0x00990590
void CGTradeSkillInfo::SetSubClassFilter(uint32_t filter, uint32_t index) {
    CGTradeSkillInfo::m_subClassFilter = filter;
    CGTradeSkillInfo::m_subClassFilterIndex = index;
    CGTradeSkillInfo::FilterAndSortSkills(1);
    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_UPDATE, NULL);
}

// OFFSET: 0x009905C0
void CGTradeSkillInfo::SetInvSlotFilter(uint32_t filter) {
    CGTradeSkillInfo::m_invTypeFilter = filter;
    CGTradeSkillInfo::FilterAndSortSkills(1);
    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_UPDATE, NULL);
}

// OFFSET: 0x009905F0/0x14070F740
void CGTradeSkillInfo::SetItemNameFilter(const char* filter) {
    if (filter && !*filter) {
        filter = NULL;
    }

    if (!filter && !CGTradeSkillInfo::m_itemNameFilter) {
        return;
    }

    if (filter && CGTradeSkillInfo::m_itemNameFilter && !SStrCmpUTF8I(filter, CGTradeSkillInfo::m_itemNameFilter, STORM_MAX_STR)) {
        return;
    }

    if (CGTradeSkillInfo::m_itemNameFilter) {
        SMemFree(CGTradeSkillInfo::m_itemNameFilter, __FILE__, __LINE__, 0);
    }

    if (filter) {
        char buffer[1024];
        SStrCopy(buffer, filter, sizeof(buffer));
        CaseAccentInsensitiveParse(buffer, sizeof(buffer), s_locale);
        CGTradeSkillInfo::m_itemNameFilter = SStrDupA(buffer, __FILE__, __LINE__);
    } else {
        CGTradeSkillInfo::m_itemNameFilter = NULL;
    }

    CGTradeSkillInfo::FilterAndSortSkills(1);
    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_FILTER_UPDATE, NULL);
}

// OFFSET: 0x009906E0
void CGTradeSkillInfo::SetItemLevelFilter(int32_t minLevel, int32_t maxLevel) {
    if (minLevel != CGTradeSkillInfo::m_itemMinLevelFilter || maxLevel != CGTradeSkillInfo::m_itemMaxLevelFilter) {
        CGTradeSkillInfo::m_itemMinLevelFilter = minLevel;
        CGTradeSkillInfo::m_itemMaxLevelFilter = maxLevel;
        CGTradeSkillInfo::FilterAndSortSkills(1);
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_UPDATE, NULL);
    }
}

// OFFSET: 0x00990C50/0x14070FDE0
void CGTradeSkillInfo::LeaveWorld() {
    CGTradeSkillInfo::ClearItemCallbacks();
}

// OFFSET: 0x00990C60/0x14070FDF0
void CGTradeSkillInfo::RefreshList(int32_t full) {
    CGTradeSkillInfo::ClearItemCallbacks();
    CGTradeSkillInfo::m_numDisplayEntries = 0;

    uint32_t savedCount = CGTradeSkillInfo::m_numSubClasses;
    uint32_t saveSize = CGTradeSkillInfo::m_subClassList.Count();

    if (CGTradeSkillInfo::m_numSubClasses < saveSize) {
        saveSize = CGTradeSkillInfo::m_numSubClasses;
    }

    TradeSkillSubClassInfo* saved = static_cast<TradeSkillSubClassInfo*>(alloca(sizeof(TradeSkillSubClassInfo) * saveSize));

    if (full) {
        for (uint32_t i = 0; i < CGTradeSkillInfo::m_numSubClasses; i++) {
            saved[i].m_itemClass = CGTradeSkillInfo::m_subClassList.m_data[i]->m_itemClass;
            saved[i].m_itemSubClass = CGTradeSkillInfo::m_subClassList.m_data[i]->m_itemSubClass;
            saved[i].m_shown = CGTradeSkillInfo::m_subClassList.m_data[i]->m_shown;
            saved[i].m_collapsed = CGTradeSkillInfo::m_subClassList.m_data[i]->m_collapsed;
        }

        CGTradeSkillInfo::m_numSubClasses = 0;
        CGTradeSkillInfo::m_subClassFilter = 0xFFFFFFFF;
        CGTradeSkillInfo::m_collapseFilter = 0xFFFFFFFF;
    }

    if (!CGTradeSkillInfo::m_skillLineID) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    WOWGUID requester = WOWGUID::Make(0);
    NameCache* name = g_nameDBCache.GetRecord(CGTradeSkillInfo::m_playerGuid.m_guid, requester, NULL, NULL, false);

    if (!name && !CGTradeSkillInfo::m_guildMemberSkill && !CGTradeSkillInfo::m_guildSkill) {
        return;
    }

    uint32_t count = 0;
    TSGrowableArray<int32_t>* spells = NULL;
    TSGrowableArray<const SkillLineAbilityRec*> abilities;
    SkillLine skillLine = { 0, 0, 0 };

    if (CGTradeSkillInfo::m_guildMemberSkill || CGTradeSkillInfo::m_guildSkill) {
        uint8_t data[300];
        memset(data, 0, sizeof(data));
        int32_t size = sizeof(data);

        if (CGTradeSkillInfo::m_guildMemberSkill) {
            CGGuildInfo::GetGuildMemberRecipes(skillLine, data);
        } else {
            CGGuildInfo::GetGuildKnownRecipes(CGTradeSkillInfo::m_skillLineID, data, size);
        }

        for (int32_t i = 0; i < g_skillLineAbilityDB.m_numRecords; i++) {
            SkillLineAbilityRec* ability = g_skillLineAbilityDB.GetRecordByIndex(i);

            if (!ability || !ability->m_uniqueBit || ability->m_skillLine != CGTradeSkillInfo::m_skillLineID || !ability->m_spell) {
                continue;
            }

            SpellRec* spell = g_spellDB.GetRecord(ability->m_spell);

            if (!spell || !(spell->m_attributes & 0x20)) {
                continue;
            }

            uint32_t byte = ability->m_uniqueBit / 8;

            if (byte < sizeof(data) && (static_cast<uint8_t>(1 << (ability->m_uniqueBit % 8)) & data[byte])) {
                if (count == abilities.Count()) {
                    abilities.SetCount(count + 32);
                }

                abilities.m_data[count++] = ability;
            }
        }
    } else if (CGTradeSkillInfo::m_linkData) {
        const SkillLineAbilityRec* ability = NULL;
        uint32_t linkCount = 0;

        if (!SpellTableGetSkillLineLinkInfo(CGTradeSkillInfo::m_skillLineID, ability, linkCount)) {
            return;
        }

        uint8_t bit = 0;
        int32_t offset = 0;

        for (uint32_t i = 0; i < linkCount; i++) {
            if (static_cast<uint8_t>(1 << bit) & s_base64Decode[static_cast<const uint8_t*>(CGTradeSkillInfo::m_linkData)[offset]]) {
                SpellRec* spell = g_spellDB.GetRecord(ability->m_spell);

                if (spell && (spell->m_attributes & 0x20)) {
                    if (count == abilities.Count()) {
                        abilities.SetCount(count + 32);
                    }

                    abilities.m_data[count++] = ability;
                }
            }

            if (++bit >= 6) {
                bit = 0;
                offset++;
            }

            ability++;
        }
    } else {
        if (!player) {
            return;
        }

        spells = player->GetTradeSkills(CGTradeSkillInfo::m_skillLineID);

        if (!spells) {
            return;
        }

        count = spells->Count();
    }

    if (CGTradeSkillInfo::m_displayList.Count() < count) {
        uint32_t missing = count - CGTradeSkillInfo::m_displayList.Count();

        do {
            TradeSkillInfo* info = static_cast<TradeSkillInfo*>(SMemAlloc(sizeof(TradeSkillInfo), __FILE__, __LINE__, 0));
            CGTradeSkillInfo::m_displayList.New(info);
        } while (--missing);
    }

    CGTradeSkillInfo::m_numDisplayEntries = count;

    int32_t skillRank;

    if (CGTradeSkillInfo::m_guildMemberSkill) {
        skillRank = skillLine.m_skillRank;
    } else {
        CGPlayer_C* activePlayer = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (activePlayer && CGTradeSkillInfo::m_playerGuid == activePlayer->m_obj->m_guid) {
            skillRank = activePlayer->m_player->m_playerSkillRank0[activePlayer->GetSkillIndex(CGTradeSkillInfo::m_skillLineID)];
        } else {
            skillRank = CGTradeSkillInfo::m_skillRank;
        }
    }

    for (uint32_t i = 0; i < CGTradeSkillInfo::m_numDisplayEntries; i++) {
        const SkillLineAbilityRec* ability = NULL;
        int32_t spellId;

        if (spells) {
            spellId = spells->m_data[i];

            if (name) {
                ability = SpellTableLookupAbility(name->m_race, name->m_class, spellId);
            }
        } else {
            ability = abilities.m_data[i];
            spellId = ability->m_spell;
        }

        CGTradeSkillInfo::m_displayList.m_data[i]->m_spellId = spellId;
        CGTradeSkillInfo::m_displayList.m_data[i]->m_difficulty = 0;
        CGTradeSkillInfo::m_displayList.m_data[i]->m_skillLineRank = ability->m_trivialSkillLineRankHigh;

        int32_t low = ability->m_trivialSkillLineRankLow;
        int32_t high = ability->m_trivialSkillLineRankHigh;

        if (!low) {
            low = high - 25 > 0 ? high - 25 : 0;
        }

        if (skillRank < low) {
            CGTradeSkillInfo::m_displayList.m_data[i]->m_difficulty = 0;
        } else if (skillRank < (high + low) / 2) {
            CGTradeSkillInfo::m_displayList.m_data[i]->m_difficulty = 1;
        } else if (skillRank < high) {
            CGTradeSkillInfo::m_displayList.m_data[i]->m_difficulty = 2;
        } else {
            CGTradeSkillInfo::m_displayList.m_data[i]->m_difficulty = 3;
        }

        CGTradeSkillInfo::m_displayList.m_data[i]->m_numSkillUps = ability->m_characterPoints;

        int32_t numAvailable = -1;
        SpellRec* spell = g_spellDB.GetRecord(spellId);

        if (spell) {
            SpellReagentsRec* reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID);

            if (reagents) {
                for (uint32_t m = 0; m < 8 && numAvailable; m++) {
                    if (!reagents->m_reagent[m] || !reagents->m_reagentCount[m]) {
                        continue;
                    }

                    int32_t available = player ? player->m_bag.GetItemTypeCount(reagents->m_reagent[m], 0) / reagents->m_reagentCount[m] : 0;

                    if (numAvailable == -1 || numAvailable >= available) {
                        numAvailable = available;
                    }
                }
            }

            const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

            if (effect && (effect->m_effect == 24 || effect->m_effect == 157 || effect->m_effect == 59) && effect->m_effectItemType) {
                WOWGUID itemRequester = WOWGUID::Make(spellId | 0x1FE0000000000000ULL);
                ItemRecSparse* sparse = g_itemSparseDB.GetRecord(effect->m_effectItemType, itemRequester, TradeSkillListItemCallback, NULL, false);
                ItemRec* item = NULL;

                if (!sparse || !(item = sparse->GetNonSparse())) {
                    CGTradeSkillInfo::m_itemsPending++;
                }

                if (CGTradeSkillInfo::m_itemsPending) {
                    CGTradeSkillInfo::m_displayList.m_data[i]->m_numAvailable = numAvailable;
                    continue;
                }

                CGTradeSkillInfo::m_displayList.m_data[i]->m_itemClass = item->m_classID;
                CGTradeSkillInfo::m_displayList.m_data[i]->m_itemSubClass = item->m_subclassID;
                CGTradeSkillInfo::m_displayList.m_data[i]->m_itemLevel = sparse->m_itemLevel;
                CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask = s_inventoryTypeSlots[sparse->m_inventoryType];

                if (sparse->m_inventoryType == 18) {
                    CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask = 0x80000;
                } else if (sparse->m_inventoryType == 11) {
                    CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask = 0x400;
                } else if (sparse->m_inventoryType == 12) {
                    CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask = 0x1000;
                }

                if (!CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask) {
                    CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask = 0x4000000;
                }

                CGTradeSkillInfo::m_invSlots |= CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask;
            } else {
                CGTradeSkillInfo::m_displayList.m_data[i]->m_itemClass = -1;
                CGTradeSkillInfo::m_displayList.m_data[i]->m_itemSubClass = -1;
                CGTradeSkillInfo::m_displayList.m_data[i]->m_itemLevel = 0;
                CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask = 0x4000000;

                SpellEquippedItemsRec* equipped;

                if (spell->m_spellEquippedItemsID && (equipped = g_spellEquippedItemsDB.GetRecord(spell->m_spellEquippedItemsID))) {
                    if (equipped->m_equippedItemClass == 2) {
                        if (equipped->m_equippedItemSubclass & 1) {
                            CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask = 0x800000;
                        } else {
                            CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask = 0x1000000;
                        }
                    } else if (equipped->m_equippedItemClass == 4) {
                        if (equipped->m_equippedItemInvTypes) {
                            CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask = 0;

                            for (uint32_t slot = 0; slot < 29; slot++) {
                                if (!((1 << slot) & equipped->m_equippedItemInvTypes)) {
                                    continue;
                                }

                                CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask = s_inventoryTypeSlots[slot];
                                TradeSkillInfo* info = CGTradeSkillInfo::m_displayList.m_data[i];

                                if (info->m_invTypeMask & 0x80000) {
                                    info->m_invTypeMask &= 0xFF8FFFFF;
                                } else if (info->m_invTypeMask & 0x400) {
                                    info->m_invTypeMask &= ~0x800;
                                } else if (info->m_invTypeMask & 0x1000) {
                                    info->m_invTypeMask &= ~0x2000;
                                }

                                break;
                            }

                            if (!CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask) {
                                CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask = 0x4000000;
                            }
                        } else if (equipped->m_equippedItemSubclass & 0x40) {
                            CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask = 0x2000000;
                        }
                    }
                }

                CGTradeSkillInfo::m_invSlots |= CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask;
            }

            if (!CGTradeSkillInfo::m_itemsPending && full) {
                uint32_t j = 0;

                for (; j < CGTradeSkillInfo::m_numSubClasses; j++) {
                    TradeSkillSubClassInfo* subClass = CGTradeSkillInfo::m_subClassList.m_data[j];
                    TradeSkillInfo* info = CGTradeSkillInfo::m_displayList.m_data[i];

                    if (subClass->m_itemClass == info->m_itemClass && subClass->m_itemSubClass == info->m_itemSubClass) {
                        break;
                    }
                }

                if (j == CGTradeSkillInfo::m_numSubClasses) {
                    if (j >= CGTradeSkillInfo::m_subClassList.Count()) {
                        TradeSkillSubClassInfo* subClass = static_cast<TradeSkillSubClassInfo*>(SMemAlloc(sizeof(TradeSkillSubClassInfo), __FILE__, __LINE__, 0));
                        CGTradeSkillInfo::m_subClassList.New(subClass);
                    }

                    CGTradeSkillInfo::m_subClassList.m_data[j]->m_itemClass = CGTradeSkillInfo::m_displayList.m_data[i]->m_itemClass;
                    CGTradeSkillInfo::m_subClassList.m_data[j]->m_itemSubClass = CGTradeSkillInfo::m_displayList.m_data[i]->m_itemSubClass;
                    CGTradeSkillInfo::m_subClassList.m_data[j]->m_shown = 1;
                    CGTradeSkillInfo::m_subClassList.m_data[j]->m_collapsed = 0;
                    CGTradeSkillInfo::m_subClassList.m_data[j]->m_invTypeMask = 0;
                    CGTradeSkillInfo::m_numSubClasses++;
                }

                CGTradeSkillInfo::m_subClassList.m_data[j]->m_invTypeMask |= CGTradeSkillInfo::m_displayList.m_data[i]->m_invTypeMask;
            }
        }

        CGTradeSkillInfo::m_displayList.m_data[i]->m_numAvailable = numAvailable;
    }

    if (CGTradeSkillInfo::m_itemsPending) {
        return;
    }

    if (full) {
        for (uint32_t i = 0; i < savedCount; i++) {
            for (uint32_t j = 0; j < CGTradeSkillInfo::m_numSubClasses; j++) {
                TradeSkillSubClassInfo* subClass = CGTradeSkillInfo::m_subClassList.m_data[j];

                if (subClass->m_itemClass == saved[i].m_itemClass && subClass->m_itemSubClass == saved[i].m_itemSubClass) {
                    CGTradeSkillInfo::m_subClassList.m_data[j]->m_shown = saved[i].m_shown;
                    CGTradeSkillInfo::m_subClassList.m_data[j]->m_collapsed = saved[i].m_collapsed;
                    break;
                }
            }
        }
    }

    for (uint32_t i = CGTradeSkillInfo::m_displayList.Count(); i < CGTradeSkillInfo::m_numSubClasses + CGTradeSkillInfo::m_numDisplayEntries; i++) {
        TradeSkillInfo* info = static_cast<TradeSkillInfo*>(SMemAlloc(sizeof(TradeSkillInfo), __FILE__, __LINE__, 0));
        CGTradeSkillInfo::m_displayList.New(info);
    }

    for (uint32_t i = 0; i < CGTradeSkillInfo::m_numSubClasses; i++) {
        CGTradeSkillInfo::m_displayList.m_data[i + CGTradeSkillInfo::m_numDisplayEntries]->m_spellId = -1;
        CGTradeSkillInfo::m_displayList.m_data[i + CGTradeSkillInfo::m_numDisplayEntries]->m_itemClass = CGTradeSkillInfo::m_subClassList.m_data[i]->m_itemClass;
        CGTradeSkillInfo::m_displayList.m_data[i + CGTradeSkillInfo::m_numDisplayEntries]->m_itemSubClass = CGTradeSkillInfo::m_subClassList.m_data[i]->m_itemSubClass;
    }

    CGTradeSkillInfo::m_numDisplayEntries += CGTradeSkillInfo::m_numSubClasses;
    CGTradeSkillInfo::FilterAndSortSkills(0);

    if (full) {
        for (uint32_t i = 0; i < CGTradeSkillInfo::m_numSubClasses; i++) {
            if (!CGTradeSkillInfo::m_subClassList.m_data[i]->m_shown) {
                CGTradeSkillInfo::m_subClassFilter &= ~(1 << i);
            }

            if (CGTradeSkillInfo::m_subClassList.m_data[i]->m_collapsed) {
                CGTradeSkillInfo::m_collapseFilter &= ~(1 << i);
            }
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_UPDATE, NULL);
}

// OFFSET: 0x00991960/0x140710D60
void CGTradeSkillInfo::SetTradeSkill(int32_t spellId, const uint64_t& guid, const char* link, int32_t skillRank, int32_t skillMaxRank) {
    SkillLineRec* skillLine = CGTradeSkillInfo::GetSkillLineRec(spellId, guid);

    if (!skillLine) {
        return;
    }

    if (link) {
        if (!skillLine->m_canLink) {
            return;
        }

        const SkillLineAbilityRec* ability = NULL;
        uint32_t count = 0;

        if (!SpellTableGetSkillLineLinkInfo(skillLine->m_id, ability, count)) {
            return;
        }

        uint32_t chars = count / 6;

        if (6 * chars != count) {
            chars++;
        }

        if (chars != SStrLen(link)) {
            return;
        }
    }

    if (CGTradeSkillInfo::m_recastSpellID || CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, NULL);
    }

    CGTradeSkillInfo::ClearItemCallbacks();

    if (!link && !CGTradeSkillInfo::m_linkData && skillLine->m_id == CGTradeSkillInfo::m_skillLineID) {
        CGTradeSkillInfo::Close();
        return;
    }

    CGTradeSkillInfo::m_playerGuid = WOWGUID::Make(guid);
    CGTradeSkillInfo::m_spellID = spellId;
    CGTradeSkillInfo::m_skillLineID = skillLine->m_id;
    CGTradeSkillInfo::m_skillRank = skillRank;
    CGTradeSkillInfo::m_skillMaxRank = skillMaxRank;

    if (CGTradeSkillInfo::m_linkData) {
        SMemFree(CGTradeSkillInfo::m_linkData, __FILE__, __LINE__, 0);
    }

    CGTradeSkillInfo::m_linkData = link ? SStrDupA(link, __FILE__, __LINE__) : NULL;

    if (CGTradeSkillInfo::m_lastSkillLineID != CGTradeSkillInfo::m_skillLineID) {
        if (CGTradeSkillInfo::m_itemNameFilter) {
            SMemFree(CGTradeSkillInfo::m_itemNameFilter, __FILE__, __LINE__, 0);
        }

        CGTradeSkillInfo::m_itemNameFilter = NULL;
        CGTradeSkillInfo::m_itemMinLevelFilter = 0;
        CGTradeSkillInfo::m_itemMaxLevelFilter = 0;
        CGTradeSkillInfo::m_showOnlyMakeable = 0;
        CGTradeSkillInfo::m_subClassFilter = 0xFFFFFFFF;
        CGTradeSkillInfo::m_invTypeFilter = 0xFFFFFFFF;
        CGTradeSkillInfo::m_collapseFilter = 0xFFFFFFFF;
        CGTradeSkillInfo::m_invSlots = 0;
        CGTradeSkillInfo::m_selectedSkill = 0;
    }

    CGTradeSkillInfo::m_guildMemberSkill = false;
    CGTradeSkillInfo::m_guildSkill = false;
    CGTradeSkillInfo::RefreshList(1);
    CGTradeSkillInfo::m_lastSkillLineID = CGTradeSkillInfo::m_skillLineID;

    if (CGTradeSkillInfo::m_numDisplayEntries > 0) {
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_SHOW, NULL);
    }
}

// OFFSET: 0x00991B50/0x140710FA0
void CGTradeSkillInfo::OpenGuildMemberTradeSkill(const uint64_t& guid, const SkillLine& skillLine) {
    CGTradeSkillInfo::m_playerGuid = WOWGUID::Make(guid);
    CGTradeSkillInfo::m_spellID = 0;
    CGTradeSkillInfo::m_skillLineID = skillLine.m_skillLineID;
    CGTradeSkillInfo::m_skillRank = skillLine.m_skillRank;
    CGTradeSkillInfo::m_skillMaxRank = 75 * skillLine.m_skillStep;

    if (CGTradeSkillInfo::m_lastSkillLineID != CGTradeSkillInfo::m_skillLineID) {
        if (CGTradeSkillInfo::m_itemNameFilter) {
            SMemFree(CGTradeSkillInfo::m_itemNameFilter, __FILE__, __LINE__, 0);
        }

        CGTradeSkillInfo::m_itemNameFilter = NULL;
        CGTradeSkillInfo::m_itemMinLevelFilter = 0;
        CGTradeSkillInfo::m_itemMaxLevelFilter = 0;
        CGTradeSkillInfo::m_showOnlyMakeable = 0;
        CGTradeSkillInfo::m_subClassFilter = 0xFFFFFFFF;
        CGTradeSkillInfo::m_invTypeFilter = 0xFFFFFFFF;
        CGTradeSkillInfo::m_collapseFilter = 0xFFFFFFFF;
        CGTradeSkillInfo::m_invSlots = 0;
        CGTradeSkillInfo::m_selectedSkill = 0;
    }

    if (CGTradeSkillInfo::m_linkData) {
        SMemFree(CGTradeSkillInfo::m_linkData, __FILE__, __LINE__, 0);
    }

    CGTradeSkillInfo::m_linkData = NULL;
    CGTradeSkillInfo::m_guildMemberSkill = true;
    CGTradeSkillInfo::m_guildSkill = false;
    CGTradeSkillInfo::RefreshList(1);
    CGTradeSkillInfo::m_lastSkillLineID = CGTradeSkillInfo::m_skillLineID;

    if (CGTradeSkillInfo::m_numDisplayEntries > 0) {
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_SHOW, NULL);
    }
}

// OFFSET: 0x00991C50/0x1407110B0
void CGTradeSkillInfo::OpenGuildTradeSkill(int32_t skillLineID) {
    CGTradeSkillInfo::m_playerGuid = WOWGUID::Make(0);
    CGTradeSkillInfo::m_spellID = 0;
    CGTradeSkillInfo::m_skillLineID = skillLineID;
    CGTradeSkillInfo::m_skillRank = 0;
    CGTradeSkillInfo::m_skillMaxRank = 0;

    if (CGTradeSkillInfo::m_lastSkillLineID != skillLineID) {
        if (CGTradeSkillInfo::m_itemNameFilter) {
            SMemFree(CGTradeSkillInfo::m_itemNameFilter, __FILE__, __LINE__, 0);
        }

        CGTradeSkillInfo::m_itemNameFilter = NULL;
        CGTradeSkillInfo::m_itemMinLevelFilter = 0;
        CGTradeSkillInfo::m_itemMaxLevelFilter = 0;
        CGTradeSkillInfo::m_showOnlyMakeable = 0;
        CGTradeSkillInfo::m_subClassFilter = 0xFFFFFFFF;
        CGTradeSkillInfo::m_invTypeFilter = 0xFFFFFFFF;
        CGTradeSkillInfo::m_collapseFilter = 0xFFFFFFFF;
        CGTradeSkillInfo::m_invSlots = 0;
        CGTradeSkillInfo::m_selectedSkill = 0;
    }

    if (CGTradeSkillInfo::m_linkData) {
        SMemFree(CGTradeSkillInfo::m_linkData, __FILE__, __LINE__, 0);
    }

    CGTradeSkillInfo::m_linkData = NULL;
    CGTradeSkillInfo::m_guildMemberSkill = false;
    CGTradeSkillInfo::m_guildSkill = true;
    CGTradeSkillInfo::RefreshList(1);
    CGTradeSkillInfo::m_lastSkillLineID = CGTradeSkillInfo::m_skillLineID;

    if (CGTradeSkillInfo::m_numDisplayEntries > 0) {
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_SHOW, NULL);
    }
}

// OFFSET: 0x00991D30/0x1407111B0
void CGTradeSkillInfo::SetLink(const char* link) {
    if (!link || !*link) {
        return;
    }

    int32_t spellId = SStrToInt(link);

    if (!spellId) {
        return;
    }

    const char* colon = SStrChr(link, ':');

    if (!colon) {
        return;
    }

    const char* data = colon + 1;
    int32_t skillRank = 0;
    int32_t skillMaxRank = 0;
    uint64_t guid = 0;
    sscanf(data, "%d:%d:%llX:", &skillRank, &skillMaxRank, &guid);

    if (!guid) {
        return;
    }

    char* copy = SStrDupA(link, __FILE__, __LINE__);
    WOWGUID requester = WOWGUID::Make(0);

    if (g_nameDBCache.GetRecord(guid, requester, TradeSkillLinkNameCallback, copy, true)) {
        SMemFree(copy, __FILE__, __LINE__, 0);
        const char* bits = strrchr(data, ':');

        if (bits) {
            CGTradeSkillInfo::SetTradeSkill(spellId, guid, bits + 1, skillRank, skillMaxRank);
        }
    }
}
