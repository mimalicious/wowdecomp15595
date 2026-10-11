#include "ClassTrainerFrame.h"
#include <cstdlib>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F4D5B0
char CGClassTrainer::m_greetingText[2048];

// STATIC: 0x00F4DDB0
WOWGUID CGClassTrainer::m_trainer;

// STATIC: 0x00F4DDB8
int32_t CGClassTrainer::m_trainerID;

// STATIC: 0x00F4DDBC
int32_t CGClassTrainer::m_trainerType;

// STATIC: 0x00F4DDC0
int32_t CGClassTrainer::m_currentSelection;

// STATIC: 0x00F4DDC4
uint32_t CGClassTrainer::m_numServices;

// STATIC: 0x00F4DDC8
uint32_t CGClassTrainer::m_filteredServices;

// STATIC: 0x00F4DDCC
TSGrowableArray<TrainerServiceInfo*> CGClassTrainer::m_services;

// STATIC: 0x00D15A28
int32_t CGClassTrainer::m_stepIndex = -1;

// PROVEN: 9ae5338c IDENTICAL/NONE
// OFFSET: 0x009B9030/0x140743E70
void TrainerItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_TRAINER_DESCRIPTION_UPDATE, NULL);
    }
}

// PROVEN: 842b99d2 IDENTICAL/NONE
// OFFSET: 0x009B9050/0x140743E90
void CGClassTrainer::EnterWorld() {
    CGClassTrainer::m_trainer = WOWGUID();
    CGClassTrainer::m_trainerID = 0;
    CGClassTrainer::m_currentSelection = 0;
    CGClassTrainer::m_numServices = 0;
    CGClassTrainer::m_greetingText[0] = '\0';
}

// OFFSET: 0x009B9120/0x140743F50
int32_t GetServiceTypeFromString(const char* type) {
    if (!type) {
        return 6;
    }

    if (!_stricmp(type, "available")) {
        return 1;
    }

    if (!_stricmp(type, "unavailable")) {
        return 2;
    }

    return _stricmp(type, "used") != 0 ? 6 : 0;
}

// OFFSET: 0x009B9290/0x140744170
void CGClassTrainer::SetTrainer(const WOWGUID& trainer, int32_t trainerID, int32_t type) {
    CGClassTrainer::m_trainerID = trainerID;

    if (trainer == CGClassTrainer::m_trainer) {
        return;
    }

    if (!trainer) {
        FrameScript_SignalEvent(SCRIPT_EVENT_TRAINER_CLOSED, NULL);
        CGGameUI::ClearInteractTarget(CGClassTrainer::m_trainer, 0);
        CGClassTrainer::m_trainer = WOWGUID();
        CGClassTrainer::m_numServices = 0;
        return;
    }

    if (trainer != ClntObjMgrGetActivePlayer()) {
        CGGameUI::SetInteractTarget(trainer);
    }

    CGClassTrainer::m_trainer = trainer;
    CGClassTrainer::m_trainerType = type;
    CGGossipInfo::ClearGossip();
}

// PROVEN: 42a706b4 IDENTICAL/NONE
// OFFSET: 0x009B9330
int32_t CGClassTrainer::GetSelectionIndex() {
    if (!CGClassTrainer::m_currentSelection) {
        return -1;
    }

    uint32_t i;

    for (i = 0; i < CGClassTrainer::m_numServices; i++) {
        if (CGClassTrainer::m_services.m_data[i]->m_spellID == CGClassTrainer::m_currentSelection) {
            break;
        }
    }

    if (i == CGClassTrainer::m_numServices) {
        return -1;
    }

    return i;
}

// OFFSET: 0x009B9370
static int32_t QSortServices_General(const void* a, const void* b) {
    const TrainerServiceInfo* serviceA = *static_cast<TrainerServiceInfo* const*>(a);
    const TrainerServiceInfo* serviceB = *static_cast<TrainerServiceInfo* const*>(b);

    if (!serviceA->m_visible || !serviceB->m_visible) {
        if (!serviceA->m_visible && !serviceB->m_visible) {
            return 0;
        }

        return 2 * (serviceB->m_visible != 0) - 1;
    }

    if (serviceB->m_state != serviceA->m_state) {
        return serviceB->m_state < serviceA->m_state ? 1 : -1;
    }

    if (serviceA->m_spellID < 0 || serviceB->m_spellID < 0) {
        return 2 * (serviceA->m_spellID >= 0) - 1;
    }

    SpellRec* spellA = g_spellDB.GetRecord(serviceA->m_spellID);
    SpellRec* spellB = g_spellDB.GetRecord(serviceB->m_spellID);

    if (!spellA || !spellB) {
        return 0;
    }

    if (serviceB->m_levelReq != serviceA->m_levelReq) {
        return serviceB->m_levelReq < serviceA->m_levelReq ? 1 : -1;
    }

    int32_t result = SStrSortUTF8I(spellA->m_name, spellB->m_name, STORM_MAX_STR);

    if (!result) {
        result = SStrSortUTF8I(spellA->m_nameSubtext, spellB->m_nameSubtext, STORM_MAX_STR);
    }

    return result;
}

// OFFSET: 0x009B9460
static int32_t QSortServices_Tradeskill(const void* a, const void* b) {
    const TrainerServiceInfo* serviceA = *static_cast<TrainerServiceInfo* const*>(a);
    const TrainerServiceInfo* serviceB = *static_cast<TrainerServiceInfo* const*>(b);

    if (!serviceA->m_visible || !serviceB->m_visible) {
        if (!serviceA->m_visible && !serviceB->m_visible) {
            return 0;
        }

        return 2 * (serviceB->m_visible != 0) - 1;
    }

    if (serviceA->m_skillLine != serviceB->m_skillLine) {
        return 2 * (serviceA->m_skillLine > serviceB->m_skillLine) - 1;
    }

    if (serviceB->m_state != serviceA->m_state) {
        return serviceB->m_state < serviceA->m_state ? 1 : -1;
    }

    if (serviceA->m_spellID < 0 || serviceB->m_spellID < 0) {
        return 2 * (serviceA->m_spellID >= 0) - 1;
    }

    SpellRec* spellA = g_spellDB.GetRecord(serviceA->m_spellID);
    SpellRec* spellB = g_spellDB.GetRecord(serviceB->m_spellID);

    if (!spellA || !spellB) {
        return 0;
    }

    if (serviceB->m_skillRankReq != serviceA->m_skillRankReq) {
        return serviceB->m_skillRankReq < serviceA->m_skillRankReq ? 1 : -1;
    }

    return SStrSortUTF8I(spellA->m_name, spellB->m_name, STORM_MAX_STR);
}

// OFFSET: 0x009B9560
TrainerServiceInfo* CGClassTrainer::GetService(uint32_t index) {
    if (index < CGClassTrainer::m_numServices) {
        return CGClassTrainer::m_services.m_data[index];
    }

    return NULL;
}

// PROVEN: b695dbba IDENTICAL/NONE
// OFFSET: 0x009B9880/0x1407448B0
void CGClassTrainer::LeaveWorld() {
    CGClassTrainer::SetTrainer(WOWGUID(), 0, 0);
}

// OFFSET: 0x009B98A0/0x140744900
int32_t GetServiceSkillLine(int32_t spellId) {
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spellId, count);
    int32_t abilitySpell = spellId;

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (effect && effect->m_effect == 36 && effect->m_implicitTargetA != 5) {
            abilitySpell = effects->m_data[i]->m_effectTriggerSpell;
            break;
        }
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    return player->GetAbilitySkillLine(abilitySpell);
}

// OFFSET: 0x009B9930/0x140744990
void CGClassTrainer::FilterAndSortServices() {
    CGClassTrainer::m_filteredServices = CGClassTrainer::m_numServices;
    int32_t filter = g_serviceTypeFilterCVar->m_intValue;

    for (uint32_t i = 0; i < CGClassTrainer::m_numServices; i++) {
        TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[i];

        if (service->m_spellID < 0 || ((1 << service->m_state) & filter)) {
            service->m_visible = 1;
        } else {
            service->m_visible = 0;
            CGClassTrainer::m_filteredServices--;
        }
    }

    if (CGClassTrainer::m_trainerType == 2) {
        qsort(CGClassTrainer::m_services.m_data, CGClassTrainer::m_numServices, sizeof(TrainerServiceInfo*), QSortServices_Tradeskill);
    } else {
        qsort(CGClassTrainer::m_services.m_data, CGClassTrainer::m_numServices, sizeof(TrainerServiceInfo*), QSortServices_General);
    }

    int32_t step = -1;
    CGClassTrainer::m_stepIndex = -1;

    if (CGClassTrainer::m_numServices > 7) {
        for (uint32_t i = 0; i < CGClassTrainer::m_numServices; i++) {
            TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[i];

            if (service->m_skillLine != 2) {
                continue;
            }

            if (step != -1) {
                TrainerServiceInfo* current = CGClassTrainer::m_services.m_data[step];

                if (current->m_state == service->m_state) {
                    if (current->m_skillRankReq < service->m_skillRankReq) {
                        if (service->m_state == 2) {
                            continue;
                        }
                    } else if (service->m_state != 2) {
                        continue;
                    }
                } else if (service->m_state != 1 && (service->m_state != 2 || current->m_state)) {
                    continue;
                }
            }

            step = i;
            CGClassTrainer::m_stepIndex = i;
        }
    }

    uint32_t i;

    for (i = 0; i < CGClassTrainer::m_numServices; i++) {
        TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[i];

        if (service && service->m_spellID > 0 && service->m_state == 1 && service->m_visible) {
            if (i >= CGClassTrainer::m_numServices || (CGClassTrainer::m_currentSelection = service->m_spellID) < 0) {
                CGClassTrainer::m_currentSelection = 0;
            }

            break;
        }
    }

    if (i == CGClassTrainer::m_numServices) {
        if (CGClassTrainer::m_numServices <= 1 || (CGClassTrainer::m_currentSelection = CGClassTrainer::m_services.m_data[1]->m_spellID) < 0) {
            CGClassTrainer::m_currentSelection = 0;
        }
    }
}

// OFFSET: 0x009B9A90
void CGClassTrainer::BuyService(uint32_t index) {
    if (index >= CGClassTrainer::m_numServices) {
        return;
    }

    TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[index];

    if (!service || service->m_state != 1) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TRAINER_BUY_SPELL));
    msg.Put(CGClassTrainer::m_trainer.m_guid);
    msg.Put(static_cast<uint32_t>(CGClassTrainer::m_trainerID));
    msg.Put(static_cast<uint32_t>(service->m_spellID));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// PROVEN: 2f1cb4bc IDENTICAL/NONE
// OFFSET: 0x009B9B60
void CGClassTrainer::BuyAllServices() {
    for (uint32_t i = 0; i < CGClassTrainer::m_filteredServices; i++) {
        CGClassTrainer::BuyService(i);
    }
}

// PROVEN: eab3fdf1 IDENTICAL/NONE
// OFFSET: 0x009B9B90
void CGClassTrainer::SetServiceTypeFilter(int32_t filter) {
    g_serviceTypeFilterCVar->Set(filter, true, false, false, true);
    CGClassTrainer::FilterAndSortServices();
    FrameScript_SignalEvent(SCRIPT_EVENT_TRAINER_UPDATE, NULL);
}

// OFFSET: 0x009BA450
void CGClassTrainer::ShutdownGame() {
    for (uint32_t i = 0; i < CGClassTrainer::m_services.m_count; i++) {
        delete CGClassTrainer::m_services.m_data[i];
    }

    CGClassTrainer::m_services.m_count = 0;
}

// OFFSET: 0x009BA4A0
void CGClassTrainer::AddServices(uint32_t count, const int32_t* spellIds, const int32_t* moneyCosts, const uint8_t* levelReqs, const int32_t* skillReqs, const uint32_t* skillRankReqs, const int32_t* const* abilityReqs, const int32_t* talentTabReqs, const uint8_t* states, const char* greeting) {
    for (uint32_t i = CGClassTrainer::m_services.Count(); i < count; i++) {
        TrainerServiceInfo* info = static_cast<TrainerServiceInfo*>(SMemAlloc(sizeof(TrainerServiceInfo), __FILE__, __LINE__, 0));
        *CGClassTrainer::m_services.New() = info;
    }

    uint32_t skipped = 0;

    for (uint32_t i = 0; i + skipped < count; i++) {
        uint32_t source = i + skipped;
        CGClassTrainer::m_services.m_data[i]->m_spellID = spellIds[source];
        CGClassTrainer::m_services.m_data[i]->m_moneyCost = moneyCosts[source];
        CGClassTrainer::m_services.m_data[i]->m_levelReq = levelReqs[source];
        CGClassTrainer::m_services.m_data[i]->m_skillReq = skillReqs[source];
        CGClassTrainer::m_services.m_data[i]->m_skillRankReq = skillRankReqs[source];
        CGClassTrainer::m_services.m_data[i]->m_abilityReq[0] = abilityReqs[0][source];
        CGClassTrainer::m_services.m_data[i]->m_abilityReq[1] = abilityReqs[1][source];
        CGClassTrainer::m_services.m_data[i]->m_abilityReq[2] = abilityReqs[2][source];
        CGClassTrainer::m_services.m_data[i]->m_talentTabReq = talentTabReqs[source];
        CGClassTrainer::m_services.m_data[i]->m_state = states[source];
        int32_t skillLine;

        if (CGClassTrainer::m_trainerType == 2) {
            skillLine = 1;

            if (SpellRec* spell = g_spellDB.GetRecord(CGClassTrainer::m_services.m_data[i]->m_spellID)) {
                uint32_t effectCount;
                const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

                for (uint32_t j = 0; j < effectCount; j++) {
                    if (effects->m_data[j] && effects->m_data[j]->m_effect == 44) {
                        skillLine = 2;
                        break;
                    }
                }
            }
        } else {
            TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[i];

            if (service->m_talentTabReq && TalentInfoGetPrimaryTalentTree(TalentInfoGetActiveTalentGroup(0, 0), 0, 0) != service->m_talentTabReq) {
                CGClassTrainer::m_services.m_data[i]->m_state = 2;
            }

            skillLine = GetServiceSkillLine(CGClassTrainer::m_services.m_data[i]->m_spellID);
        }

        CGClassTrainer::m_services.m_data[i]->m_skillLine = skillLine;

        if (!skillLine) {
            i--;
            skipped++;
        }
    }

    CGClassTrainer::m_numServices = count - skipped;
    CGClassTrainer::FilterAndSortServices();

    if (greeting) {
        QuestParserParseText(greeting, CGClassTrainer::m_greetingText, sizeof(CGClassTrainer::m_greetingText), ClntObjMgrGetActivePlayer(), NULL, 0);
    }
}

// OFFSET: 0x009BA770/0x140745CD0
void CGClassTrainer::RefreshList() {
    if (!CGClassTrainer::m_trainer) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    for (uint32_t i = 0; i < CGClassTrainer::m_numServices; i++) {
        int32_t spellId = CGClassTrainer::m_services.m_data[i]->m_spellID;

        if (spellId == -1) {
            continue;
        }

        SpellRec* spell = g_spellDB.GetRecord(spellId);

        if (!spell) {
            continue;
        }

        uint32_t effectCount;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);
        CGClassTrainer::m_services.m_data[i]->m_state = 1;
        bool learnFound = false;
        int32_t total = 0;
        int32_t known = 0;

        for (uint32_t j = 0; j < effectCount; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (!effect || !effect->m_effect) {
                continue;
            }

            if (effect->m_effect == 44) {
                int32_t minPoints;
                int32_t maxPoints;
                Spell_C_GetMinMaxPoints(spell, j, &minPoints, &maxPoints, 0, 0, 0, 0);
                total++;
                int32_t index = player->GetSkillIndex(effects->m_data[j]->m_effectMiscValue);

                if (index >= 0 && player->GetSkillStep(index) >= maxPoints) {
                    known++;
                }

                continue;
            }

            int32_t learnSpell;

            if (effect->m_effect == 36 && effect->m_implicitTargetA != 5) {
                learnSpell = effect->m_effectTriggerSpell;
                learnFound = true;
            } else {
                if (learnFound) {
                    continue;
                }

                learnSpell = spell->m_id;
            }

            total++;

            if (player->IsSpellKnown(learnSpell) || player->IsSpellSuperceded(learnSpell)) {
                known++;
            }

            if (learnSpell == spell->m_id) {
                break;
            }
        }

        if (total > 0 && known == total) {
            CGClassTrainer::m_services.m_data[i]->m_state = 0;
        }

        if (CGClassTrainer::m_services.m_data[i]->m_state != 1) {
            continue;
        }

        if (CGClassTrainer::m_services.m_data[i]->m_skillReq) {
            int32_t index = player->GetSkillIndex(CGClassTrainer::m_services.m_data[i]->m_skillReq);

            if (index >= 0 && player->GetSkillValue(index) < CGClassTrainer::m_services.m_data[i]->m_skillRankReq) {
                CGClassTrainer::m_services.m_data[i]->m_state = 2;
            }
        }

        if (CGClassTrainer::m_services.m_data[i]->m_talentTabReq && TalentInfoGetPrimaryTalentTree(TalentInfoGetActiveTalentGroup(0, 0), 0, 0) != CGClassTrainer::m_services.m_data[i]->m_talentTabReq) {
            CGClassTrainer::m_services.m_data[i]->m_state = 2;
        }

        if (CGClassTrainer::m_services.m_data[i]->m_state != 1) {
            continue;
        }

        bool abilitiesKnown = true;

        for (uint32_t j = 0; j < 3; j++) {
            int32_t ability = CGClassTrainer::m_services.m_data[i]->m_abilityReq[j];

            if (ability && !player->IsSpellKnown(ability) && !player->IsSpellSuperceded(CGClassTrainer::m_services.m_data[i]->m_abilityReq[j])) {
                abilitiesKnown = false;
                break;
            }
        }

        if (CGSpellBook::IsUnlearnedSpell(CGClassTrainer::m_services.m_data[i]->m_spellID)) {
            CGClassTrainer::m_services.m_data[i]->m_state = 0;
        } else if (abilitiesKnown) {
            TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[i];

            if (service->m_levelReq > 1 && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) < service->m_levelReq) {
                service->m_state = 2;
            }
        } else {
            CGClassTrainer::m_services.m_data[i]->m_state = 2;
        }
    }

    CGClassTrainer::FilterAndSortServices();
    FrameScript_SignalEvent(SCRIPT_EVENT_TRAINER_UPDATE, NULL);
}
