#include "RuneInfo.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellRuneCostRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
// STATIC: 0x00F35FFC/0x000E29388
int32_t CGRuneInfo::m_runes[6];

// STATIC: 0x00F36014/0x000E293A0
int32_t CGRuneInfo::m_baseRunes[6];

// STATIC: 0x00F3602C/0x140E293B8
uint32_t CGRuneInfo::m_runeCooldowns[6];

// STATIC: 0x00F36044/0x140E293D0
uint32_t CGRuneInfo::m_runeLastUsed[6];

// STATIC: 0x00F3605C/0x140E293E8
uint32_t CGRuneInfo::m_numRunes;

// STATIC: 0x00F36060/0x140E293EC
uint32_t CGRuneInfo::m_predictedRunes;


// PROVEN: 4305e805 IDENTICAL/DIFFERENT
// OFFSET: 0x0093C370/0x1406A7A00
void CGRuneInfo::InitializeGame() {
    for (uint32_t i = 0; i < 6; i++) {
        CGRuneInfo::m_runeLastUsed[i] = 0;
    }

    for (uint32_t i = 0; i < 6; i++) {
        CGRuneInfo::m_runeCooldowns[i] = 0;
    }

    CGRuneInfo::m_runes[0] = 0;
    CGRuneInfo::m_baseRunes[0] = 0;
    CGRuneInfo::m_runes[1] = 0;
    CGRuneInfo::m_baseRunes[1] = 0;
    CGRuneInfo::m_runes[2] = 1;
    CGRuneInfo::m_baseRunes[2] = 1;
    CGRuneInfo::m_runes[3] = 1;
    CGRuneInfo::m_baseRunes[3] = 1;
    CGRuneInfo::m_predictedRunes = 0xFFFFFFFF;
    CGRuneInfo::m_numRunes = 6;
    CGRuneInfo::m_runes[4] = 2;
    CGRuneInfo::m_baseRunes[4] = 2;
    CGRuneInfo::m_runes[5] = 2;
    CGRuneInfo::m_baseRunes[5] = 2;
}

// PROVEN: 16988e2e IDENTICAL/DIFFERENT
// OFFSET: 0x0093C4C0/0x1406A7B60
void CGRuneInfo::EnterWorld() {
    for (uint32_t i = 0; i < 6; i++) {
        CGRuneInfo::m_runeLastUsed[i] = 0;
    }

    for (uint32_t i = 0; i < 6; i++) {
        CGRuneInfo::m_runeCooldowns[i] = 0;
    }

    CGRuneInfo::m_predictedRunes = 0xFFFFFFFF;

    if (!CGGameUI::m_currentlyReloadingUI) {
        CGRuneInfo::m_runes[0] = 0;
        CGRuneInfo::m_baseRunes[0] = 0;
        CGRuneInfo::m_runes[1] = 0;
        CGRuneInfo::m_baseRunes[1] = 0;
        CGRuneInfo::m_runes[2] = 1;
        CGRuneInfo::m_baseRunes[2] = 1;
        CGRuneInfo::m_runes[3] = 1;
        CGRuneInfo::m_baseRunes[3] = 1;
        CGRuneInfo::m_runes[4] = 2;
        CGRuneInfo::m_baseRunes[4] = 2;
        CGRuneInfo::m_runes[5] = 2;
        CGRuneInfo::m_baseRunes[5] = 2;
    }
}

// PROVEN: 65522d23 IDENTICAL/IDENTICAL
// OFFSET: 0x0093C430/0x1406A7AE0
bool CGRuneInfo::IsRuneIndexValid(int32_t index) {
    if (!(static_cast<uint32_t>(index) > 5)) {
        if (CGRuneInfo::m_numRunes) {
            return index < static_cast<int32_t>(CGRuneInfo::m_numRunes);
        }

        return index < 6;
    }

    return false;
}

// PROVEN: 4cf7ce2d IDENTICAL/IDENTICAL
// OFFSET: 0x0093C460/0x1406A7B10
int32_t CGRuneInfo::GetRuneTypeByIndex(int32_t index, bool base) {
    if (base) {
        return CGRuneInfo::m_baseRunes[index];
    }

    return CGRuneInfo::m_runes[index];
}

// PROVEN: c2bc9c5f IDENTICAL/DIFFERENT
// OFFSET: 0x0093C640/0x1406A7D40
uint32_t CGRuneInfo::GetRuneRechargeTime(int32_t index) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    if (((1 << index) & CGRuneInfo::m_predictedRunes)) {
        return 0;
    }

    return CGRuneInfo::m_runeCooldowns[index];
}

// OFFSET: 0x0093C8C0/0x1406A8100
int32_t CGRuneInfo::CountPredictedRunesOfType(int32_t type) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    int32_t count = 0;

    for (uint32_t i = 0; i < CGRuneInfo::m_numRunes; i++) {
        if (CGRuneInfo::m_runes[i] == type && ((1 << i) & CGRuneInfo::m_predictedRunes)) {
            count++;
        }
    }

    return count;
}

// OFFSET: 0x0093CBA0/0x1406A84B0
bool CGRuneInfo::HasRunesForSpell(int32_t spellId, int32_t* needed) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    SpellRec* rec = g_spellDB.GetRecord(spellId);

    if (!rec) {
        return false;
    }

    SpellRec spell = *rec;

    int32_t school = spell.m_schoolMask;
    int32_t pct = player->GetSpellCostModifier(school) + 100;
    pct = static_cast<int32_t>(player->GetSpellCostMultiplier(school) * pct);
    Spell_C_ModifySpellValue(&spell, pct, 14);

    if (!pct || spell.m_powerType != 5) {
        return true;
    }

    SpellRuneCostRec* cost = g_spellRuneCostDB.GetRecord(spell.m_runeCostID);

    if (!cost) {
        return true;
    }

    int32_t missing = 0;
    bool short1 = false;
    bool short2 = false;

    if (cost->m_blood) {
        int32_t have = CGRuneInfo::CountPredictedRunesOfType(0);
        int32_t need = cost->m_blood;

        if (pct > 100) {
            need = pct * need / 100;
        }

        if (have < need) {
            missing = need - have;
            short1 = true;

            if (needed) {
                needed[0] = need - have;
            }
        }
    }

    if (cost->m_unholy) {
        int32_t have = CGRuneInfo::CountPredictedRunesOfType(1);
        int32_t need = cost->m_unholy;

        if (pct > 100) {
            need = pct * need / 100;
        }

        if (have < need) {
            missing += need - have;
            short1 = true;

            if (needed) {
                needed[1] = need - have;
            }
        }
    }

    if (cost->m_frost) {
        int32_t have = CGRuneInfo::CountPredictedRunesOfType(2);
        int32_t need = cost->m_frost;

        if (pct > 100) {
            need = pct * need / 100;
        }

        if (have < need) {
            missing += need - have;
            short2 = true;

            if (needed) {
                needed[2] = need - have;
            }
        }
    }

    if (!short1 && !short2) {
        return true;
    }

    if (CGRuneInfo::CountPredictedRunesOfType(3) >= missing) {
        return true;
    }

    if (needed) {
        needed[3] = missing - CGRuneInfo::CountPredictedRunesOfType(3);
    }

    return false;
}

// OFFSET: 0x0093CDC0/0x1406A8770
bool CGRuneInfo::ComputeRuneSpellCooldown(const SpellRec* spell, uint32_t* cooldown) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return false;
    }

    SpellRuneCostRec* cost = g_spellRuneCostDB.GetRecord(spell->m_runeCostID);
    int32_t needed[4] = { 0, 0, 0, 0 };

    if (!cost || CGRuneInfo::HasRunesForSpell(spell->m_id, needed)) {
        return false;
    }

    uint32_t latest = 0;
    int32_t latestIndex = 0;
    uint32_t times[6] = { 0, 0, 0, 0, 0, 0 };

    for (int32_t i = 0; i < 6; i++) {
        int32_t type = CGRuneInfo::m_runes[i];

        if (type != 3 && needed[type]) {
            uint32_t time = CGRuneInfo::GetRuneRechargeTime(i);
            times[i] = time;

            if (!latest || static_cast<int32_t>(time - latest) >= 0) {
                latest = time;
                latestIndex = i;
            }
        }
    }

    for (int32_t i = 0; i < 6; i++) {
        if (CGRuneInfo::m_runes[i] != 3 || !ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__) || ((1 << i) & CGRuneInfo::m_predictedRunes) || !CGRuneInfo::m_runeCooldowns[i]) {
            continue;
        }

        uint32_t time = CGRuneInfo::GetRuneRechargeTime(i);

        if (!latest) {
            latest = time;
            latestIndex = i;
            times[i] = time;
        } else if (time < latest) {
            times[latestIndex] = CGRuneInfo::GetRuneRechargeTime(i);
            latestIndex = 0;
            latest = 0;

            for (int32_t j = 0; j < 6; j++) {
                if (CGRuneInfo::m_runes[j] != 3 && (!latest || static_cast<int32_t>(times[j] - latest) >= 0)) {
                    latest = times[j];
                    latestIndex = j;
                }
            }
        }
    }

    uint32_t blood = 0;

    if (needed[0] == 1) {
        if (!times[0] || (times[1] && times[0] >= times[1])) {
            blood = times[1];
        } else {
            blood = times[0];
        }
    } else if (needed[0] == 2) {
        blood = times[0] <= times[1] ? times[1] : times[0];
    }

    uint32_t unholy = 0;

    if (needed[1] == 1) {
        unholy = times[2] && (!times[3] || times[2] < times[3]) ? times[2] : times[3];
    } else if (needed[1] == 2) {
        unholy = times[2] > times[3] ? times[2] : times[3];
    }

    uint32_t frost = 0;

    if (needed[2] == 1) {
        frost = times[4] && (!times[5] || times[4] < times[5]) ? times[4] : times[5];
    } else if (needed[2] == 2) {
        frost = times[4] > times[5] ? times[4] : times[5];
    }

    uint32_t other = unholy <= frost ? frost : unholy;
    *cooldown = blood <= other ? other : blood;
    return true;
}

// PROVEN: 31a366a2 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGRuneInfo::ShutdownGame() {
}

// PROVEN: 656c969a IDENTICAL/UNPROVEN
// OFFSET: 0x0093C410/0x1406A7AD0
void CGRuneInfo::SetRuneLastUsedTime(int32_t index, uint32_t time) {
    CGRuneInfo::m_runeLastUsed[index] = time;
}

// PROVEN: 3e1d1939 IDENTICAL/DIFFERENT
// OFFSET: 0x0093C560/0x1406A7C30
uint32_t CGRuneInfo::GetRuneLastUsedTime(int32_t index) {
    if (!static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        return 0;
    }

    return CGRuneInfo::m_runeLastUsed[index];
}

// OFFSET: 0x0093C590/0x1406A7C80
void CGRuneInfo::ComputeRuneRechargeTime(int32_t index, float progress) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    float rate = player->m_player->m_playerRuneRegen1[CGRuneInfo::m_runes[index]];

    if (!(rate >= 0.0099999998f)) {
        CGRuneInfo::m_runeCooldowns[index] = 0;
    } else {
        CGRuneInfo::m_runeCooldowns[index] = OsGetAsyncTimeMs() - static_cast<int32_t>(1.0f / rate * (1.0f - progress) * -1000.0f);
    }
}

// OFFSET: 0x0093C490/0x1406A7B30
void CGRuneInfo::ConvertRune(uint8_t rune, uint8_t type) {
    CGRuneInfo::m_runes[rune] = type;
    FrameScript_SignalEvent(SCRIPT_EVENT_RUNE_TYPE_UPDATE, "%d", rune + 1);
    CGSpellBook::UpdateUsable();
}

// OFFSET: 0x0093C690
void CGRuneInfo::SetPredictedRunes(uint8_t available, uint8_t used, const uint8_t* cooldowns, bool force) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    CGRuneInfo::m_predictedRunes = used;

    for (int32_t i = 0; i < 6; i++) {
        int32_t bit = 1 << i;

        if (bit & used) {
            if (!(bit & available) || ((bit & CGRuneInfo::m_predictedRunes) && CGRuneInfo::GetRuneLastUsedTime(i))) {
                CGRuneInfo::m_runeLastUsed[i] = 0;
                CGRuneInfo::m_runeCooldowns[i] = 0;
                FrameScript_SignalEvent(SCRIPT_EVENT_RUNE_POWER_UPDATE, "%d", i + 1);
            }
        } else if (force || !CGRuneInfo::GetRuneLastUsedTime(i)) {
            float duration = 0.0f;
            float progress = 0.0f;

            if (cooldowns) {
                progress = cooldowns[i] * 0.0039215689f;

                if (progress > 1.0f) {
                    progress = 1.0f;
                }
            }

            float rate = player->m_player->m_playerRuneRegen1[CGRuneInfo::m_runes[i]];

            if (rate >= 0.0099999998f) {
                duration = 1.0f / rate;
            }

            int32_t pair = (i & 1) ? i - 1 : i + 1;

            if (cooldowns) {
                uint8_t pairCooldown = cooldowns[pair];

                if ((pairCooldown != 0 && pairCooldown != 0xFF) || (static_cast<uint32_t>(i) <= 5 && (bit & available) && pair <= 5 && ((1 << pair) & available) && !((1 << pair) & used) && (i & 1))) {
                    progress = progress - (1.0f - pairCooldown * 0.0039215689f);
                }
            }

            CGRuneInfo::m_runeLastUsed[i] = OsGetAsyncTimeMs() - static_cast<int32_t>(duration * progress * 1000.0f);
            CGRuneInfo::ComputeRuneRechargeTime(i, progress);
            FrameScript_SignalEvent(SCRIPT_EVENT_RUNE_POWER_UPDATE, "%d", i + 1);
        }
    }

    CGSpellBook::UpdateUsable();
}

// PROVEN: 0b9a8596 IDENTICAL/UNPROVEN
// OFFSET: 0x0093C920/0x1406A8170
void CGRuneInfo::ResyncRunes(CDataStore* msg) {
    msg->Get(CGRuneInfo::m_numRunes);

    for (int32_t i = 0; i < static_cast<int32_t>(CGRuneInfo::m_numRunes); i++) {
        uint8_t type;
        msg->Get(type);
        CGRuneInfo::m_runes[i] = type;
        CGRuneInfo::m_baseRunes[i] = type;
        uint8_t cooldown;
        msg->Get(cooldown);
        float progress = cooldown * 0.0039215689f;

        if (progress > 1.0f) {
            progress = 1.0f;
        }

        CGRuneInfo::m_runeLastUsed[i] = OsGetAsyncTimeMs() - static_cast<int32_t>(10000.0f * progress);
        CGRuneInfo::ComputeRuneRechargeTime(i, progress);
        FrameScript_SignalEvent(SCRIPT_EVENT_RUNE_POWER_UPDATE, "%d", i + 1);
    }
}

// PROVEN: d6eced7d IDENTICAL/DIFFERENT
// OFFSET: 0x0093CA00/0x1406A82B0
void CGRuneInfo::AddRunePower(CDataStore* msg) {
    uint32_t runes;
    msg->Get(runes);

    for (int32_t i = 0; i < static_cast<int32_t>(CGRuneInfo::m_numRunes); i++) {
        if (((1 << i) & runes) && !((1 << i) & CGRuneInfo::m_predictedRunes)) {
            CGRuneInfo::m_runeLastUsed[i] = OsGetAsyncTimeMs() - 10000;
            CGRuneInfo::ComputeRuneRechargeTime(i, 1.0f);
            FrameScript_SignalEvent(SCRIPT_EVENT_RUNE_POWER_UPDATE, "%d%b", i + 1, 1);

            int32_t pair = (i & 1) ? i - 1 : i + 1;

            if (static_cast<int32_t>(OsGetAsyncTimeMs() - CGRuneInfo::GetRuneRechargeTime(pair)) < 0) {
                if (CGRuneInfo::GetRuneRechargeTime(pair) - OsGetAsyncTimeMs() > 10000) {
                    CGRuneInfo::m_runeLastUsed[pair] = OsGetAsyncTimeMs();
                    CGRuneInfo::ComputeRuneRechargeTime(pair, 0.0f);
                    FrameScript_SignalEvent(SCRIPT_EVENT_RUNE_POWER_UPDATE, "%d", pair + 1);
                }
            }
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_UPDATE_COOLDOWN, NULL);
}

// PROVEN: 59e48eda IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGRuneInfo::LeaveWorld() {
}
