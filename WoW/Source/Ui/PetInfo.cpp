#include "PetInfo.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/ClassTrainerFrame.h"
#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SpellActivationOverlay.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Ui/StableInfo.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Tempest/C44Matrix.h"

// STATIC: 0x00BFC5D8
static const char s_petModeTokens[4][32] = { "PASSIVE", "DEFENSIVE", "AGGRESSIVE", "ASSIST" };

// STATIC: 0x00BFC658
static const char s_petOrderTokens[5][32] = { "WAIT", "FOLLOW", "ATTACK", "DISMISS", "MOVE_TO" };

// STATIC: 0x00F43B58
uint32_t CGPetInfo::m_petMode;

// STATIC: 0x00F43B5C
uint32_t CGPetInfo::m_expirationTime;

// STATIC: 0x00F43B60
WOWGUID CGPetInfo::m_pet;

// STATIC: 0x00F43B68
uint32_t CGPetInfo::m_petBarState;

// STATIC: 0x00F43B6C
int32_t CGPetInfo::m_attackingTarget;

// STATIC: 0x00F43B70
int32_t CGPetInfo::m_possessSpell;

// STATIC: 0x00F43B74
uint8_t CGPetInfo::m_comboPoints;

// STATIC: 0x00F43B75
bool CGPetInfo::m_petOrderMoveTo;

// STATIC: 0x00F43B78
WOWGUID CGPetInfo::m_comboTarget;

// STATIC: 0x00F43B80
TSGrowableArray<WOWGUID> s_pets;

// STATIC: 0x00F43B90
PetAction CGPetInfo::m_actions[10];

// STATIC: 0x00F43BB8
static TSGrowableArray<PetAction> s_petSpellActions;

// OFFSET: 0x0094FED0
void CGPetInfo::SetPetMode(uint32_t mode) {
    CGPetInfo::m_petMode = mode | (CGPetInfo::m_petMode & 0xFFFFFF00);
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, NULL);
}

// OFFSET: 0x0094FF00
void CGPetInfo::SetPetOrders(uint32_t orders) {
    CGPetInfo::m_petMode = (orders << 8) | (CGPetInfo::m_petMode & 0x80000FF);
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, NULL);
}

// OFFSET: 0x0094FF30/0x1406C0260
const char* CGPetInfo::GetModeToken(uint32_t mode) {
    return s_petModeTokens[mode];
}

// OFFSET: 0x0094FF40/0x1406C0280
const char* CGPetInfo::GetOrdersToken(uint32_t order) {
    return s_petOrderTokens[order];
}

// OFFSET: 0x0094FF50/0x1406C02A0
void CGPetInfo::ShowGrid() {
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_SHOWGRID, NULL);
}

// OFFSET: 0x0094FF60
void CGPetInfo::HideGrid() {
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_HIDEGRID, NULL);
}

// OFFSET: 0x0094FF70
void CGPetInfo::UpdateCooldowns() {
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE_COOLDOWN, NULL);
}

// OFFSET: 0x0094FF80
void CGPetInfo::SetIsAttackingTarget(int32_t attacking) {
    if (attacking) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PET_ATTACK_START, NULL);
        CGPetInfo::m_attackingTarget = attacking;
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_PET_ATTACK_STOP, NULL);
        CGPetInfo::m_attackingTarget = 0;
    }
}

// OFFSET: 0x0094FFC0
void CGPetInfo::SetPossessedSpell(int32_t spellId) {
    int32_t previous = CGPetInfo::m_possessSpell;
    CGPetInfo::m_possessSpell = spellId;

    if (spellId) {
        CGPetInfo::m_petBarState = 1;
    }

    if (previous != spellId) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BONUS_ACTIONBAR, NULL);
    }
}

// OFFSET: 0x00950000
void CGPetInfo::SetPetOrderMoveTo() {
    Spell_C_StopTargeting();
    CursorSetResetMode(2);
    CursorSetMode(28);
    CGPetInfo::m_petOrderMoveTo = 1;
}

// OFFSET: 0x00950020/0x1406C0370
void CGPetInfo::ClearPetOrderMoveTo() {
    CursorSetResetMode(1);
    CursorSetMode(1);
    CGPetInfo::m_petOrderMoveTo = 0;
}

// OFFSET: 0x00950040
int32_t CGPetInfo::OnPetBroken(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::DisplayError(ERR_PET_BROKEN);
    return 1;
}

// OFFSET: 0x00950060
int32_t CGPetInfo::OnPetRenameable(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_RENAMEABLE, NULL);
    return 1;
}

// PROVEN: 97502d5f IDENTICAL/NONE
// OFFSET: 0x00950160
static int32_t PetTempActionBarCallback(uint32_t index, const void* data) {
    return index < 10 ? index | 0x10000000 : 0;
}

// PROVEN: 9c7fd35f IDENTICAL/NONE
// OFFSET: 0x00950220/0x1406C07C0
void PetAction::SetAutocast(bool enable) {
    if (enable) {
        this->m_data |= 0x40000000;
    } else {
        this->m_data &= ~0x40000000;
    }
}

// OFFSET: 0x00950270/0x1406C0620
void CGPetInfo::LeaveWorld() {
    int32_t possessSpell = CGPetInfo::m_possessSpell;
    CGPetInfo::m_possessSpell = 0;

    if (possessSpell) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BONUS_ACTIONBAR, NULL);
    }
}

// OFFSET: 0x009502A0
void CGPetInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_PET_SPELLS_MESSAGE);
    ClientServices::ClearMessageHandler(SMSG_PET_LEARNED_SPELLS);
    ClientServices::ClearMessageHandler(SMSG_PET_UNLEARNED_SPELLS);
    ClientServices::ClearMessageHandler(SMSG_PET_MODE);
    ClientServices::ClearMessageHandler(SMSG_PET_ACTION_FEEDBACK);
    ClientServices::ClearMessageHandler(SMSG_PET_BROKEN);
    ClientServices::ClearMessageHandler(SMSG_PET_RENAMEABLE);
    ClientServices::ClearMessageHandler(SMSG_PET_UPDATE_COMBO_POINTS);
    ClientServices::ClearMessageHandler(SMSG_PET_GUIDS);
}

// OFFSET: 0x00950350/0x1406C0710
WOWGUID CGPetInfo::GetPet(uint32_t index) {
    if (!(index >= s_pets.m_count)) {
        return s_pets.m_data[index];
    }

    return WOWGUID();
}

// OFFSET: 0x00950380/0x1406C0730
WOWGUID CGPetInfo::GetTarget() {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    return pet ? pet->m_unit->m_unitFieldTarget : WOWGUID();
}

// OFFSET: 0x009503D0
PetAction* CGPetInfo::GetPetSpellAction(int32_t spellId) {
    for (uint32_t i = s_petSpellActions.m_count; i; ) {
        i--;

        if ((s_petSpellActions.m_data[i].m_data & 0xFFFFFF) == static_cast<uint32_t>(spellId)) {
            return &s_petSpellActions.m_data[i];
        }
    }

    return NULL;
}

// OFFSET: 0x00950410
void CGPetInfo::UpdatePetSpellAction(const PetAction* action) {
    uint32_t value = action->m_data;

    if ((value & 0x3F000000) != 0x1000000) {
        return;
    }

    for (uint32_t i = s_petSpellActions.m_count; i; ) {
        i--;

        if ((s_petSpellActions.m_data[i].m_data & 0x3FFFFFFF) == (value & 0x3FFFFFFF)) {
            s_petSpellActions.m_data[i].m_data = value;
            return;
        }
    }
}

// OFFSET: 0x00950460
void CGPetInfo::SetComboPoints(WOWGUID target, uint8_t points) {
    CGPetInfo::m_comboTarget = target;
    CGPetInfo::m_comboPoints = points;
    CGSpellBook::UpdateUsable();
    CGSpellActivationOverlay::UpdateActiveMoverOverlays();
    WOWGUID pet = CGPetInfo::GetPet(0);
    Script_SendUnitSignal(pet, 145);
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE_USABLE, NULL);
}

// OFFSET: 0x009504D0/0x1406C0820
uint8_t CGPetInfo::GetComboPoints(WOWGUID target, bool anyTarget) {
    if (!target) {
        target = CGGameUI::m_lockedTarget;
    }

    if (anyTarget) {
        if (!ClntObjMgrObjectPtr(CGPetInfo::m_comboTarget, TYPEMASK_UNIT, __FILE__, __LINE__)) {
            return 0;
        }
    } else if (target != CGPetInfo::m_comboTarget) {
        return 0;
    }

    return CGPetInfo::m_comboPoints;
}

// OFFSET: 0x00950530
PetAction* CGPetInfo::FindSpellByID(int32_t spellId) {
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t action = CGPetInfo::m_actions[i].m_data;
        uint32_t type = (action >> 24) & 0x3F;

        if ((type == 1 || (type >= 8 && type <= 0x11)) && static_cast<int32_t>(action & 0xFFFFFF) == spellId) {
            return &CGPetInfo::m_actions[i];
        }
    }

    return NULL;
}

// OFFSET: 0x00950580
int32_t CGPetInfo::OnPetSpellChanges(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char name[512];
    int32_t spellId;

    if (msgId != SMSG_PET_LEARNED_SPELLS) {
        if (msgId != SMSG_PET_UNLEARNED_SPELLS) {
            return 0;
        }

        msg->Get(spellId);
        SpellRec* spell = g_spellDB.GetRecord(spellId);

        if (!spell) {
            return 1;
        }

        if (*spell->m_nameSubtext) {
            SStrPrintf(name, sizeof(name), "%s (%s)", static_cast<const char*>(spell->m_name), static_cast<const char*>(spell->m_nameSubtext));
        } else {
            SStrCopy(name, spell->m_name, sizeof(name));
        }

        CGGameUI::DisplayError(ERR_PET_SPELL_UNLEARNED_S, name);
        return 1;
    }

    msg->Get(spellId);
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (spell) {
        if (*spell->m_nameSubtext) {
            SStrPrintf(name, sizeof(name), "%s (%s)", static_cast<const char*>(spell->m_name), static_cast<const char*>(spell->m_nameSubtext));
        } else {
            SStrCopy(name, spell->m_name, sizeof(name));
        }

        CGGameUI::DisplayError(static_cast<UIErrors>(ERR_PET_LEARN_SPELL_S + ((spell->m_attributes & 0x10) != 0)), name);
    }

    return 1;
}

// OFFSET: 0x00950740
int32_t CGPetInfo::OnPetMode(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID pet;
    uint32_t mode;
    msg->Get(pet.m_guid);
    msg->Get(mode);

    if (pet == CGPetInfo::GetPet(0)) {
        CGPetInfo::m_petMode = mode;
        FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, NULL);
    }

    return 1;
}

// OFFSET: 0x009507B0
int32_t CGPetInfo::OnPetActionFeedback(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t feedback;
    msg->Get(feedback);

    switch (feedback) {
        case 1: {
            CGGameUI::DisplayError(ERR_PET_SPELL_DEAD);
            return 1;
        }

        case 2: {
            int32_t spellId = 0;
            msg->Get(spellId);
            UIErrors error = ERR_NO_ATTACK_TARGET;
            SpellRec* spell = g_spellDB.GetRecord(spellId);

            if (spell && SpellFaction(spell) == 1) {
                error = ERR_GENERIC_NO_VALID_TARGETS;
            }

            CGGameUI::DisplayError(error);
            Spell_C_PetSpellFailedUpdateCooldowns(spellId);
            return 1;
        }

        case 3: {
            CGGameUI::DisplayError(ERR_INVALID_ATTACK_TARGET);
            int32_t spellId = 0;
            msg->Get(spellId);
            Spell_C_PetSpellFailedUpdateCooldowns(spellId);
            return 1;
        }

        case 4: {
            CGGameUI::DisplayError(ERR_PET_SPELL_NOPATH);
            return 1;
        }

        default: {
            return 1;
        }
    }
}

// OFFSET: 0x009508C0
int32_t CGPetInfo::OnPetUpdateComboPoints(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID pet;
    SmartGUID target;
    uint8_t points;
    *msg >> pet;
    *msg >> target;
    msg->Get(points);

    if (pet == CGPetInfo::GetPet(0)) {
        CGPetInfo::SetComboPoints(target, points);
    }

    return 1;
}

// OFFSET: 0x00950940/0x1406C0DA0
const char* CGPetInfo::GetPetActionTexture(const PetAction& action) {
    uint32_t data = action.m_data;
    char token[64];

    switch ((data >> 24) & 0x3F) {
        case 1:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17: {
            SpellRec* spell = g_spellDB.GetRecord(data & 0xFFFFFF);

            if (!spell) {
                return NULL;
            }

            SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID);

            if (!icon) {
                return NULL;
            }

            return icon->m_textureFilename;
        }

        case 6: {
            SStrPrintf(token, sizeof(token), "PET_%s_TEXTURE", CGPetInfo::GetModeToken(data & 0xFFFFFF));
            return FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);
        }

        case 7: {
            SStrPrintf(token, sizeof(token), "PET_%s_TEXTURE", CGPetInfo::GetOrdersToken(data & 0xFFFFFF));
            return FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);
        }

        default: {
            return NULL;
        }
    }
}

// OFFSET: 0x00950A60
bool CGPetInfo::PetActionIsActive(const PetAction& action) {
    uint32_t data = action.m_data;
    uint32_t id = data & 0xFFFFFF;

    switch ((data >> 24) & 0x3F) {
        case 6: {
            return CGPetInfo::GetPetMode() == id;
        }

        case 7: {
            if (id == 2) {
                return CGPetInfo::m_attackingTarget != 0;
            }

            return (CGPetInfo::m_petMode >> 8) == id;
        }

        default: {
            return false;
        }
    }
}

// OFFSET: 0x00951110/0x1406C1610
bool CGPetInfo::PetActionsUsable(bool ignoreControl, bool ignoreControl2) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->m_unit->m_unitFieldCharmedby) {
        return false;
    }

    if (player->GetFarSightGuid() && player->GetFarSightGuid() != CGPetInfo::GetPet(0).m_guid) {
        return false;
    }

    for (uint32_t i = 0; i < s_pets.m_count; i++) {
        CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_pets.m_data[i], TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!pet) {
            continue;
        }

        const WOWGUID& owner = pet->m_unit->m_unitFieldCharmedby ? pet->m_unit->m_unitFieldCharmedby : pet->m_unit->m_unitFieldSummonedby;

        if (owner == ClntObjMgrGetActivePlayer() && (ignoreControl || ignoreControl2 || pet->IsControllable())) {
            if (!(CGPetInfo::m_petMode & 0x8000000)) {
                return true;
            }

            if (player->GetTransportGUID() == pet->m_obj->m_guid.m_guid) {
                return true;
            }
        }
    }

    return false;
}

// OFFSET: 0x00951270/0x1406C17C0
int32_t CGPetInfo::PetActionSlotUsable(const PetAction* action) {
    if (!action) {
        return 1;
    }

    uint32_t type = (action->m_data >> 24) & 0x3F;

    if (type != 1 && (type <= 7 || type > 0x11)) {
        return CGPetInfo::PetActionsUsable(false, false) ? 1 : 0;
    }

    if (CGPetInfo::PetActionsUsable(true, false)) {
        SpellRec* spell = g_spellDB.GetRecord(action->m_data & 0xFFFFFF);

        if (!spell) {
            return 0;
        }

        uint32_t count = s_pets.m_count;
        uint32_t stunned = 0;
        uint32_t feared = 0;
        uint32_t confused = 0;
        uint32_t unusable = 0;

        if (!count) {
            return 1;
        }

        for (uint32_t i = 0; i < count; i++) {
            CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(i), TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!pet) {
                return 0;
            }

            if ((pet->m_unit->m_unitFieldFlags & 0x40000) && !(spell->m_attributesExE & 0x8) && !Spell_C_CancelsStun(pet, spell, NULL)) {
                stunned++;
            } else if ((pet->m_unit->m_unitFieldFlags & 0x800000) && !(spell->m_attributesExE & 0x20000) && !Spell_C_CancelsFear(pet, spell, NULL)) {
                feared++;
            } else if ((pet->m_unit->m_unitFieldFlags & 0x400000) && !(spell->m_attributesExE & 0x40000) && !Spell_C_CancelsConfuse(pet, spell, NULL)) {
                confused++;
            }

            WOWGUID target = pet->m_unit->m_unitFieldTarget;

            if (!Spell_C_IsUsableAction(pet, spell, target, NULL, true, true, false)) {
                unusable++;
            }
        }

        return (!stunned || stunned != count) && (!feared || feared != count) && (!confused || confused != count) && (!unusable || unusable != count);
    }

    return 0;
}

// OFFSET: 0x00951460
int32_t CGPetInfo::IsAttackAction(uint32_t index) {
    PetAction* action = CGPetInfo::GetPet(0) && index < 10 ? &CGPetInfo::m_actions[index] : NULL;

    if (!action || !action->m_data || (action->m_data & 0x3F000000) != 0x07000000 || (action->m_data & 0xFFFFFF) != 2) {
        return 0;
    }

    return 1;
}

// OFFSET: 0x009514C0/0x1406C19F0
void CGPetInfo::SendPetAction(const PetAction& action, const WOWGUID& target, int32_t useGiven) {
    if (!CGGameUI::CanPerformAction(2)) {
        return;
    }

    WOWGUID actionTarget;

    if (useGiven || target) {
        actionTarget = target;
    } else {
        actionTarget = CGGameUI::m_lockedTarget;
    }

    switch ((action.m_data >> 24) & 0x3F) {
        case 1:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17: {
            if (!CGPetInfo::PetActionsUsable(true, false)) {
                return;
            }

            SpellRec* spell = g_spellDB.GetRecord(action.m_data & 0xFFFFFF);

            if (!spell) {
                return;
            }

            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (!player) {
                return;
            }

            if (Spell_C_GetSpellCooldown(spell->m_id, 1, NULL, NULL, NULL, NULL)) {
                Spell_C_PetSpellFailed(NULL, spell, 69, -1, -1);
                return;
            }

            CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

            if (Spell_C_IsActiveSpell(spell, pet)) {
                if (!Spell_C_WasSpellRecentlyUsed(spell->m_id, 1)) {
                    Spell_C_CancelAura(spell, pet, -1);
                }

                return;
            }

            uint32_t stunned = 0;
            uint32_t feared = 0;
            uint32_t confused = 0;

            if (s_pets.m_count) {
                for (uint32_t i = 0; i < s_pets.m_count; i++) {
                    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(i), TYPEMASK_UNIT, __FILE__, __LINE__));

                    if (!unit) {
                        return;
                    }

                    if ((unit->m_unit->m_unitFieldFlags & 0x40000) && !(spell->m_attributesExE & 0x8) && !Spell_C_CancelsStun(unit, spell, NULL)) {
                        stunned++;
                    } else if ((unit->m_unit->m_unitFieldFlags & 0x800000) && !(spell->m_attributesExE & 0x20000) && !Spell_C_CancelsFear(unit, spell, NULL)) {
                        feared++;
                    } else if ((unit->m_unit->m_unitFieldFlags & 0x400000) && !(spell->m_attributesExE & 0x40000) && !Spell_C_CancelsConfuse(unit, spell, NULL)) {
                        confused++;
                    }
                }

                if ((stunned && stunned == s_pets.m_count) || (feared && feared == s_pets.m_count) || (confused && confused == s_pets.m_count)) {
                    return;
                }
            }

            if (!(spell->m_attributesExD & 0x20) && player->GetPossessedUnit() != pet) {
                Spell_C_StartSpellCooldown(spell, pet, 0, 0);
                break;
            }

            Spell_C_CastSpell(pet, spell->m_id, NULL, actionTarget, NULL, false, false, 0, 0);
            return;
        }

        case 6: {
            if (!CGPetInfo::PetActionsUsable(false, false)) {
                return;
            }

            CGPetInfo::SetPetMode(action.m_data & 0xFFFFFF);
            break;
        }

        case 7: {
            if (!CGPetInfo::PetActionsUsable(false, false)) {
                return;
            }

            switch (action.m_data & 0xFFFFFF) {
                case 0:
                case 1: {
                    CGPetInfo::SetPetOrders(action.m_data & 0xFFFFFF);
                    CGPetInfo::ClearPetOrderMoveTo();
                    break;
                }

                case 2: {
                    int32_t attacking = 0;

                    for (uint32_t i = 0; i < s_pets.m_count; i++) {
                        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(i), TYPEMASK_UNIT, __FILE__, __LINE__));

                        if (unit && unit->VerifyAttackIconPressed(&actionTarget, useGiven, 1, false, true)) {
                            attacking = 1;
                        }
                    }

                    CGPetInfo::ClearPetOrderMoveTo();

                    if (!attacking) {
                        return;
                    }

                    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, NULL);
                    break;
                }

                case 4: {
                    CGPetInfo::SetPetOrderMoveTo();
                    return;
                }

                default: {
                    break;
                }
            }

            break;
        }

        default: {
            if (!CGPetInfo::PetActionsUsable(false, false)) {
                return;
            }

            break;
        }
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PET_ACTION));
    msg.Put(CGPetInfo::GetPet(0).m_guid);
    msg.Put(action.m_data);
    msg.Put(actionTarget.m_guid);
    msg.Put(0.0f);
    msg.Put(0.0f);
    msg.Put(0.0f);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00951950
void CGPetInfo::PetFollow() {
    PetAction action;
    action.m_data = 0x7000001;
    CGPetInfo::SendPetAction(action, WOWGUID(), 0);
}

// PROVEN: f641700b IDENTICAL/NONE
// OFFSET: 0x00951980/0x1406C2020
void CGPetInfo::PetAttackTarget(const WOWGUID& target, int32_t useGiven) {
    PetAction action;
    action.m_data = 0x7000002;
    CGPetInfo::SendPetAction(action, target, useGiven);
}

// OFFSET: 0x009519B0/0x1406C2050
void CGPetInfo::PetClearTarget() {
    if (!CGPetInfo::m_attackingTarget) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PET_STOP_ATTACK));
    msg.Put(CGPetInfo::GetPet(0).m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGPetInfo::m_attackingTarget = 0;
}

// OFFSET: 0x00951A50/0x1406C2110
void CGPetInfo::PetDismiss() {
    if (!CGPetInfo::PetActionsUsable(false, true)) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (CGPetInfo::GetPet(0) == CGUnit_C::s_activeMover && player && CGPetInfo::GetPet(0).m_guid == player->GetTransportGUID()) {
        CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!mover) {
            return;
        }

        uint8_t seat = player->m_movementData.m_transportSeat;

        if (!mover->m_vehicle) {
            return;
        }

        VehicleSeatRec* seatRec = mover->m_vehicle->GetVehicleSeatRec(seat);

        if (!seatRec || !(seatRec->m_flags & 0x2000000)) {
            return;
        }

        if (player->m_vehiclePassenger) {
            player->m_vehiclePassenger->StopMovement();
        }

        mover->OnDismissControlledVehicleLocal(FrameTime::GetCurTimeMs());
        return;
    }

    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
    SummonPropertiesRec* properties;

    if (pet && (properties = pet->GetSummonProperties()) && (properties->m_flags & 0x20)) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PET_ACTION));
    msg.Put(CGPetInfo::GetPet(0).m_guid);
    msg.Put(static_cast<uint32_t>(0x07000003));
    msg.Put(static_cast<uint64_t>(0));
    msg.Put(0.0f);
    msg.Put(0.0f);
    msg.Put(0.0f);
    msg.Finalize();
    ClientServices::Send2(&msg);
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_DISMISS_START, "%d", 10000);
}

// OFFSET: 0x00951C90/0x1406C2380
void CGPetInfo::PetAbandon() {
    if (s_pets.m_count && s_pets.m_data[0]) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_PET_ABANDON));
        msg.Put(CGPetInfo::GetPet(0).m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (CGStableInfo::m_stableMaster) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(MSG_LIST_STABLED_PETS));
        msg.Put(CGStableInfo::m_stableMaster.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }
}

// OFFSET: 0x00951DA0/0x1406C24C0
void CGPetInfo::PetRename(const char* name, const char* declinedNames) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PET_RENAME));
    msg.Put(CGPetInfo::GetPet(0).m_guid);
    msg.PutString(name);

    if (declinedNames) {
        msg.Put(static_cast<uint8_t>(1));

        for (uint32_t i = 0; i < 5; i++) {
            msg.PutString(declinedNames);
            declinedNames += 96;
        }
    } else {
        msg.Put(static_cast<uint8_t>(0));
    }

    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00951E70/0x1406C25C0
int32_t CGPetInfo::IsPetBarUsed() {
    if (!s_pets.m_count || !s_pets.m_data[0]) {
        return 0;
    }

    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_pets.m_data[0], TYPEMASK_UNIT, __FILE__, __LINE__));
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    return player && pet && pet->IsPetBarUsed() && (!CGPetInfo::m_petBarState || CGPetInfo::m_possessSpell || pet->CanGainXP()) && (!(player->m_player->m_playerFieldBytes[0] & 0x20) || !pet->m_unit->m_unitFieldPetnumber);
}

// OFFSET: 0x00951F30
void CGPetInfo::UpdatePetTempBar() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t active = 0;

    if (player && player->m_obj->m_guid == ClntObjMgrGetActivePlayer() && player->m_player->m_playerFarsight && player->GetFarSightGuid() == CGPetInfo::GetPet(0).m_guid && CGPetInfo::IsPetBarUsed()) {
        active = 1;

        if (s_pets.m_count && s_pets.m_data[0] && player->GetTransportGUID() == s_pets.m_data[0].m_guid) {
            CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

            if (pet->m_vehicle) {
                VehicleSeatRec* seat = pet->m_vehicle->GetVehicleSeatRec(player->m_movementData.m_transportSeat);

                if (seat) {
                    active = seat->m_vehicleAbilityDisplay == 1;
                }
            }
        }
    }

    if (active) {
        CGActionBar::UpdateTemporaryActionBar(PetTempActionBarCallback, NULL);
        CGSpellActivationOverlay::AddActiveMoverSpells();
    } else {
        CGSpellActivationOverlay::ClearActiveMoverSpells();
    }

    CGActionBar::SetTemporaryActionBarFlags(1, active);
    CGActionBar::UpdateOverrideSpellBar();
    CGActionBar::UpdateBonusBar();
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, NULL);
}

// OFFSET: 0x009520A0
int32_t CGPetInfo::HandleMoveToTerrainRay(const CTerrainClickEvent& evt) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    C3Vector point = evt.m_point;

    if (evt.m_guid) {
        CGObject_C* object = ClntObjMgrObjectPtr(evt.m_guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object) {
            point = point * object->GetMatrix();
        }
    }

    C3Vector position = player->GetPosition();
    float distSquared = (position.x - point.x) * (position.x - point.x) + (position.z - point.z) * (position.z - point.z) + (position.y - point.y) * (position.y - point.y);

    if (distSquared <= 3600.0f) {
        return 0;
    }

    if (distSquared <= 57600.0f) {
        return 1;
    }

    return 2;
}

// OFFSET: 0x00952220
bool CGPetInfo::HandleTerrainClick(const CTerrainClickEvent& evt) {
    if (CGPetInfo::m_petOrderMoveTo) {
        WOWGUID guid = evt.m_guid;
        C3Vector point = evt.m_point;
        CGPetInfo::ClearPetOrderMoveTo();

        if (!CGPetInfo::HandleMoveToTerrainRay(evt)) {
            CGPetInfo::SetPetOrders(4);
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_PET_ACTION));
            msg.Put(CGPetInfo::GetPet(0).m_guid);
            msg.Put(static_cast<uint32_t>(0x07000004));
            msg.Put(guid.m_guid);
            msg.Put(point.x);
            msg.Put(point.y);
            msg.Put(point.z);
            msg.Finalize();
            ClientServices::Send2(&msg);
        }

        return true;
    }

    return false;
}

// OFFSET: 0x00952F70
void CGPetInfo::SetPetCount(uint32_t count) {
    s_pets.SetCount(count);
}

// OFFSET: 0x00952FD0
int32_t CGPetInfo::SetAction(uint32_t index, PetAction& action, int32_t send) {
    uint32_t data = action.m_data;
    uint32_t type = (data >> 24) & 0x3F;

    if (type > 0x11) {
        return 0;
    }

    if (type) {
        if (type == 6) {
            if ((data & 0xFFFFFF) >= 4) {
                return 0;
            }
        } else if (type == 7 && (data & 0xFFFFFF) > 4) {
            return 0;
        }
    } else if (data) {
        return 0;
    }

    if (!CGGameUI::CanPerformAction(14)) {
        return 0;
    }

    if (send && !CGPetInfo::PetActionsUsable(false, false)) {
        return 0;
    }

    if (action.m_data == CGPetInfo::m_actions[index].m_data) {
        return 0;
    }

    if ((action.m_data & 0x3F000000) == 0x1000000) {
        SpellRec* spell = g_spellDB.GetRecord(action.m_data & 0xFFFFFF);

        if (!spell || (spell->m_attributes & 0x40)) {
            return 0;
        }
    }

    CGPetInfo::m_actions[index] = action;

    if (send) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_PET_SET_ACTION));
        msg.Put(CGPetInfo::GetPet(0).m_guid);
        msg.Put(index);
        msg.Put(CGPetInfo::m_actions[index].m_data);
        msg.Finalize();
        ClientServices::Send2(&msg);
        FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, NULL);
    }

    return 1;
}

// OFFSET: 0x00953120/0x1406C3C70
void CGPetInfo::SetAutocast(uint32_t index, int32_t enable) {
    if (!CGGameUI::CanPerformAction(2) || !CGPetInfo::PetActionsUsable(false, false)) {
        return;
    }

    PetAction* action = &CGPetInfo::m_actions[index];

    if (action && (action->m_data & 0x80000000)) {
        action->SetAutocast(enable < 0 ? !(action->m_data & 0x40000000) : enable != 0);
        CGPetInfo::UpdatePetSpellAction(action);
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_PET_SET_ACTION));
        msg.Put(CGPetInfo::GetPet(0).m_guid);
        msg.Put(index);
        msg.Put(action->m_data);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, NULL);
}

// OFFSET: 0x00953220
void CGPetInfo::SetSpellAutocast(int32_t spellId, int32_t enable) {
    if (!CGGameUI::CanPerformAction(2) || !CGPetInfo::PetActionsUsable(false, false)) {
        return;
    }

    PetAction* action = CGPetInfo::GetPetSpellAction(spellId);

    if (!action || !(action->m_data & 0x80000000)) {
        return;
    }

    if (enable < 0) {
        enable = !(action->m_data & 0x40000000);
    }

    action->SetAutocast(enable != 0);

    for (uint32_t i = 0; i < 10; i++) {
        if (!((action->m_data ^ CGPetInfo::m_actions[i].m_data) & 0x3FFFFFFF)) {
            if (action->m_data & 0x40000000) {
                CGPetInfo::m_actions[i].m_data |= 0x40000000;
            } else {
                CGPetInfo::m_actions[i].m_data &= ~0x40000000;
            }
        }
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PET_SPELL_AUTOCAST));
    msg.Put(CGPetInfo::GetPet(0).m_guid);
    msg.Put(static_cast<uint32_t>(spellId));
    msg.Put(static_cast<uint8_t>((action->m_data & 0x40000000) != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, NULL);
}

// OFFSET: 0x00953430/0x1406C3F90
void CGPetInfo::PlaceAction(PetAction& action, uint32_t index) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!pet || (pet->m_unit->m_unitFieldFlags & 0x1000000) || !pet->m_unit->m_unitFieldPetnumber) {
        return;
    }

    PetAction* current = GetPetAction(index);
    PetAction previous;
    previous.m_data = current ? current->m_data : 0;

    if (CGPetInfo::SetAction(index, action, 1)) {
        CGGameUI::SetCursorPetAction(&previous);
    }
}

// OFFSET: 0x009534D0/0x1406C4050
void CGPetInfo::FindPossessedSpell() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    int32_t onCharmVehicle = 0;

    if (player->GetTransportGUID()) {
        const WOWGUID& charm = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;

        if (player->GetTransportGUID() == charm.m_guid) {
            onCharmVehicle = 1;
        }
    }

    for (uint32_t i = 0; i < player->GetAuraCount(); i++) {
        AuraData* aura = i < player->GetAuraCount() ? &player->m_auras[i] : NULL;

        if (aura->m_flags & 0x80) {
            continue;
        }

        SpellRec* spell = g_spellDB.GetRecord(aura->m_spellId);

        if (!spell) {
            continue;
        }

        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

        for (uint32_t j = 0; j < count; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (!effect) {
                continue;
            }

            if (effect->m_effectAura == 2 || effect->m_effectAura == 6 || (effect->m_effectAura == 236 && onCharmVehicle)) {
                CGPetInfo::SetPossessedSpell(aura->m_spellId);
                return;
            }
        }

        for (uint32_t j = 0; j < count; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (!effect || effect->m_effect != 28) {
                continue;
            }

            SummonPropertiesRec* properties = g_summonPropertiesDB.GetRecord(effect->m_effectMiscValueB);

            if (properties && properties->m_control == 3) {
                CGPetInfo::SetPossessedSpell(aura->m_spellId);
                return;
            }
        }
    }
}

// OFFSET: 0x009536D0
int32_t CGPetInfo::OnPetGUIDs(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t count;
    msg->Get(count);
    CGPetInfo::SetPetCount(count);

    for (uint32_t i = 0; i < count; i++) {
        WOWGUID guid;
        msg->Get(guid.m_guid);

        if (i < s_pets.m_count) {
            s_pets.m_data[i] = guid;
        }
    }

    return 1;
}

// OFFSET: 0x00953740
void CGPetInfo::PickupAction(uint32_t index, int32_t send) {
    if (index < 10) {
        PetAction action;
        action.m_data = 0;
        CGPetInfo::SetAction(index, action, send);
    }
}

// OFFSET: 0x00953770
void CGPetInfo::PlaceCursorAction(PetAction action, uint32_t index) {
    CGPetInfo::PlaceAction(action, index);
}

// OFFSET: 0x00953790/0x1406C4350
void CGPetInfo::SetPet(WOWGUID pet, uint32_t expiration, int32_t family) {
    int32_t changed = CGPetInfo::m_pet != pet;
    CGPetInfo::m_pet = pet;

    if (!pet) {
        CGPetInfo::SetPetCount(0);
    } else {
        uint32_t index = 0;

        if (s_pets.m_count) {
            for (index = 0; index < s_pets.m_count; index++) {
                if (s_pets.m_data[index] == pet) {
                    break;
                }
            }

            if (index && index < s_pets.m_count) {
                WOWGUID first = s_pets.m_data[0];
                s_pets.m_data[0] = pet;

                if (index < s_pets.m_count) {
                    s_pets.m_data[index] = first;
                }
            }
        }

        if (index == s_pets.m_count) {
            CGPetInfo::SetPetCount(1);

            if (s_pets.m_count) {
                s_pets.m_data[0] = pet;
            }
        }
    }

    CGPetInfo::m_expirationTime = expiration ? expiration + FrameTime::GetCurTimeMs() : 0;

    if (changed) {
        CGPetInfo::m_petBarState = 0;
        CGPetInfo::m_attackingTarget = 0;

        if (s_pets.m_count && s_pets.m_data[0]) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_pets.m_data[0], TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                CGPetInfo::m_attackingTarget = unit->IsInCombat() != 0;

                if (unit->CanGainXP()) {
                    FrameScript_SignalEvent(SCRIPT_EVENT_PET_UI_CLOSE, NULL);
                }
            }

            CGTalentInfo::UpdatePetTabInfo(family);
        } else {
            CGPetInfo::SetComboPoints(WOWGUID(), 0);
            CGTalentInfo::UpdatePetTabInfo(0);
        }

        CGActionBar::UpdateSpellEffect(56);
        FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_PET_2, "%s", "player");
    }

    CGPetInfo::UpdatePetTempBar();
    WOWGUID current = CGPetInfo::GetPet(0);

    if (Spell_C_IsModal(current) && (changed || !s_pets.m_count || !s_pets.m_data[0])) {
        Spell_C_CancelSpellByCaster(pet);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && changed && player->m_unit->m_unitFieldHealth <= 0) {
        if (s_pets.m_count && s_pets.m_data[0]) {
            FrameScript_SignalEvent(SCRIPT_EVENT_RAISED_AS_GHOUL, NULL);
        } else {
            CGGameUI::UpdateActivePlayer();
        }
    }
}

// OFFSET: 0x00953A30
int32_t CGPetInfo::OnPetSpells(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    for (uint32_t i = 0; i < 10; i++) {
        CGPetInfo::m_actions[i].m_data = 0;
    }

    s_petSpellActions.SetCount(0);
    Spell_C_ClearPetCooldowns();
    uint32_t expiration = 0;
    uint16_t family = 0;
    uint64_t pet;
    msg->Get(pet);

    if (pet) {
        msg->Get(family);
        msg->Get(expiration);
        uint32_t mode;
        msg->Get(mode);
        CGPetInfo::m_petMode = mode;

        for (uint32_t i = 0; i < 10; i++) {
            uint32_t data;
            msg->Get(data);
            PetAction action;
            action.m_data = data;
            CGPetInfo::SetAction(i, action, 0);

            if ((pet & 0xF0F0000000000000ull) != 0xF050000000000000ull && ((data >> 24) & 0x3F) >= 8 && ((data >> 24) & 0x3F) <= 17) {
                CGSpellBook::AddSpellToNameCache(data & 0xFFFFFF, 1);
            }
        }

        uint8_t count;
        msg->Get(count);
        s_petSpellActions.SetCount(count);
        CGSpellBook::ClearPetSpells();

        for (uint32_t i = 0; i < count; i++) {
            uint32_t data;
            msg->Get(data);
            s_petSpellActions.m_data[i].m_data = data;
            uint32_t type = (data >> 24) & 0x3F;

            if (type == 1 || (type >= 8 && type <= 17)) {
                CGSpellBook::AddPetSpell(data & 0xFFFFFF);
            }
        }

        CGSpellBook::AddPetActions();
        msg->Get(count);

        for (uint32_t i = 0; i < count; i++) {
            int32_t spellId;
            msg->Get(spellId);
            uint16_t category;
            msg->Get(category);
            int32_t cooldown;
            msg->Get(cooldown);
            int32_t categoryCooldown;
            msg->Get(categoryCooldown);
            bool onHold = categoryCooldown < 0;
            categoryCooldown &= ~0x80000000;
            Spell_C_SetCooldownLeft(spellId, 0, category, cooldown, categoryCooldown, onHold, 1);
        }
    } else {
        CGSpellBook::ClearPetSpells();
    }

    CGPetInfo::SetPet(WOWGUID::Make(pet), expiration, family);
    CGSpellBook::UpdateSpells(0, 1, 1);
    CGSpellBook::UpdateUsable();
    CGSpellBook::UpdateCooldowns();
    CGClassTrainer::RefreshList();
    return 1;
}

// OFFSET: 0x00953FF0/0x1406C4E40
void CGPetInfo::InitializeGame() {
    CGPetInfo::m_pet = WOWGUID();
    CGPetInfo::m_expirationTime = 0;
    CGPetInfo::m_petBarState = 0;
    CGPetInfo::m_comboTarget = WOWGUID();
    CGPetInfo::m_comboPoints = 0;
    CGPetInfo::m_petOrderMoveTo = 0;
    CGPetInfo::SetPetCount(0);
    ClientServices::SetMessageHandler(SMSG_PET_SPELLS_MESSAGE, CGPetInfo::OnPetSpells, NULL);
    ClientServices::SetMessageHandler(SMSG_PET_LEARNED_SPELLS, CGPetInfo::OnPetSpellChanges, NULL);
    ClientServices::SetMessageHandler(SMSG_PET_UNLEARNED_SPELLS, CGPetInfo::OnPetSpellChanges, NULL);
    ClientServices::SetMessageHandler(SMSG_PET_MODE, CGPetInfo::OnPetMode, NULL);
    ClientServices::SetMessageHandler(SMSG_PET_ACTION_FEEDBACK, CGPetInfo::OnPetActionFeedback, NULL);
    ClientServices::SetMessageHandler(SMSG_PET_BROKEN, CGPetInfo::OnPetBroken, NULL);
    ClientServices::SetMessageHandler(SMSG_PET_RENAMEABLE, CGPetInfo::OnPetRenameable, NULL);
    ClientServices::SetMessageHandler(SMSG_PET_UPDATE_COMBO_POINTS, CGPetInfo::OnPetUpdateComboPoints, NULL);
    ClientServices::SetMessageHandler(SMSG_PET_GUIDS, CGPetInfo::OnPetGUIDs, NULL);
}

// OFFSET: 0x00954100/0x1406C4F70
void CGPetInfo::EnterWorld() {
    CGPetInfo::SetPet(WOWGUID(), 0, 0);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGPetInfo::m_petOrderMoveTo = 0;

    if (!player) {
        return;
    }

    const WOWGUID& pet = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;

    if (!pet) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PET_INFO));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGPetInfo::FindPossessedSpell();
}
