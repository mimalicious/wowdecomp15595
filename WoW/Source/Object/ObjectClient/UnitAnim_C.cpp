#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/JamClientAutoCode/JamClientMove.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/CMoveSpline.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Ui/GameUI.h"

// STATIC: 0x00BB6368/0x14099D6F8
static const uint32_t s_movementAnimKitIDs[8] = { 0, 41, 41, 42, 44, 43, 45, 301 };

// STATIC: 0x00BB6388/0x14099D718
static const int32_t s_movementAnimKitScalesSpeed[8] = { 0, 1, 0, 1, 1, 0, 1, 1 };

// STATIC: 0x00BB63A8/0x14099D738
static const uint32_t s_deathAnimKitIDs[8] = { 61, 62, 63, 64, 65, 66, 67, 68 };

// STATIC: 0x00BB63C8/0x14099D758
static const int32_t s_deathAnimKitIsCorpse[8] = { 0, 0, 1, 1, 0, 0, 0, 0 };

// STATIC: 0x00BB63E8/0x14099D778
static const uint32_t s_combatReadyAnimKitIDs[3] = { 86, 141, 142 };

// STATIC: 0x00BB63F4/0x14099D788
static const uint32_t s_combatActionAnimKitIDs[5] = { 0, 81, 81, 82, 1194 };

// STATIC: 0x00BB6408/0x14099D7A0
static const uint32_t s_precastAnimKitIDs[4] = { 84, 85, 121, 122 };

// STATIC: 0x00BB6418/0x14099D7B0
static const uint32_t s_standStateAnimKitIDs[6] = { 181, 182, 183, 184, 185, 186 };

// STATIC: 0x00BB6450/0x14099D7F0
static const uint32_t s_emoteAnimKitIDs[8] = { 1207, 1206, 205, 206, 1360, 1359, 1357, 1358 };

// STATIC: 0x00CA7B00
static UnitAnimationSet s_animSetGround = { 0, 11, 12, { { 4, 13, 4, 4 }, { 5, 13, 5, 5 }, { 143, 13, 143, 143 } }, 37, 38, 39, 187, 40, -1, 0 };

// STATIC: 0x00CA7B58
static UnitAnimationSet s_animSetGroundAlt = { 0, 11, 12, { { 4, 13, 4, 4 }, { 5, 13, 5, 5 }, { 143, 13, 143, 143 } }, 37, -1, 39, 187, -1, -1, 0 };

// STATIC: 0x00CA7BB0
static UnitAnimationSet s_animSetFlying = { 532, -1, -1, { { 544, 546, 536, 538 }, { 540, 546, 536, 538 }, { 542, 546, 536, 538 } }, -1, -1, -1, -1, 40, -1, 0 };

// STATIC: 0x00CA7C08
static UnitAnimationSet s_animSetSwimming = { 548, -1, -1, { { 560, 562, 552, 554 }, { 556, 562, 552, 554 }, { 558, 562, 552, 554 } }, 564, -1, 572, 574, 40, -1, 0 };

// STATIC: 0x00CA7C60
static UnitAnimationSet s_animSetMountedGround = { 0, 11, 12, { { 4, 13, 4, 4 }, { 5, 13, 5, 5 }, { 143, 13, 143, 143 } }, 37, 38, 39, 187, 40, 94, 0 };

// STATIC: 0x00CA7CB8
static UnitAnimationSet s_animSetMountedGroundAlt = { 0, 11, 12, { { 4, 13, 4, 4 }, { 5, 13, 5, 5 }, { 143, 13, 143, 143 } }, 564, -1, 572, 574, 40, 94, 0 };

// STATIC: 0x00CA7D10
static UnitAnimationSet s_animSetMountedFlying = { 532, -1, -1, { { 544, 546, 536, 538 }, { 540, 546, 536, 538 }, { 542, 546, 536, 538 } }, -1, -1, -1, -1, -1, -1, 0 };

// STATIC: 0x00CA7D68
static UnitAnimationSet s_animSetMountedSwimming = { 548, -1, -1, { { 560, 562, 552, 554 }, { 556, 562, 552, 554 }, { 558, 562, 552, 554 } }, 564, -1, 572, 574, 40, -1, 0 };

// STATIC: 0x00CA7DC0
static UnitAnimationSet s_animSetMountSelfGround = { 618, 630, 632, { { 620, 634, 620, 620 }, { 622, 634, 622, 622 }, { 624, 634, 624, 624 } }, 640, 638, 642, 644, 648, 636, 0 };

// STATIC: 0x00CA7E18
static UnitAnimationSet s_animSetMountSelfGroundAlt = { 618, 630, 632, { { 620, 634, 620, 620 }, { 622, 634, 622, 622 }, { 624, 634, 624, 624 } }, 564, -1, 572, 574, -1, 636, 0 };

// STATIC: 0x00CA7E70
static UnitAnimationSet s_animSetMountSelfFlying = { 532, -1, -1, { { 544, 546, 536, 538 }, { 540, 546, 536, 538 }, { 542, 546, 536, 538 } }, -1, -1, -1, -1, -1, -1, 0 };

// STATIC: 0x00CA7EC8
static UnitAnimationSet s_animSetMountSelfSwimming = { 548, -1, -1, { { 560, 562, 552, 554 }, { 556, 562, 552, 554 }, { 558, 562, 552, 554 } }, 564, -1, 572, 574, -1, -1, 0 };

// STATIC: 0x00CA7AD0
static const UnitAnimationSet* s_animSets[12] = {
    &s_animSetGround, &s_animSetGroundAlt, &s_animSetFlying, &s_animSetSwimming,
    &s_animSetMountedGround, &s_animSetMountedGroundAlt, &s_animSetMountedFlying, &s_animSetMountedSwimming,
    &s_animSetMountSelfGround, &s_animSetMountSelfGroundAlt, &s_animSetMountSelfFlying, &s_animSetMountSelfSwimming
};

// STATIC: 0x00CA7F20/0x140B47CF0
static UnitAnimationSet s_animSetStealth = { 120, 11, 12, { { 119, 13, 119, 119 }, { 223, 13, 223, 223 }, { 223, 13, 223, 223 } }, 37, 38, 39, 187, 40, -1, 0 };

// STATIC: 0x00CA7F78/0x140B47D50
static UnitAnimationSet s_animSetFlyingSpline = { 135, -1, -1, { { 135, 135, 135, 135 }, { 135, 135, 135, 135 }, { 135, 135, 135, 135 } }, -1, -1, -1, -1, -1, -1, 0 };

// STATIC: 0x00CA7FD0/0x140B47DB0
static UnitAnimationSet s_animSetHover = { 193, -1, -1, { { 4, 13, 4, 4 }, { 5, 13, 5, 5 }, { 143, 13, 143, 143 } }, 37, 38, 39, 187, 40, -1, 0 };

// PROVEN: fe8be2b9 IDENTICAL/IDENTICAL
// OFFSET: 0x005FAE70/0x1402800B0
void CGUnit_C::ShutdownAnims() {
    this->m_animKitManager.SetExpressionsBlocked(1, 0);

    if (this->m_mountAnimKitManager) {
        this->m_mountAnimKitManager->SetExpressionsBlocked(1, 0);
    }
}

// PROVEN: bac0d519 IDENTICAL/NONE
// OFFSET: 0x005FAEA0
void CGUnit_C::SetMovementAnimKitID(uint16_t id) {
    this->m_movementAnimKitID = id;
}

// PROVEN: cc4795cf IDENTICAL/IDENTICAL
// OFFSET: 0x005FAEC0/0x140280100
uint32_t CGUnit_C::IsHovering() const {
    return this->m_animationState & 0x40000;
}

// PROVEN: 57edf83d IDENTICAL/IDENTICAL
// OFFSET: 0x005FAED0/0x140280110
void CGUnit_C::MovementOneShotAnimKitStopCallback(CAnimKitInst* inst, uint64_t param) {
    CGUnit_C* unit = reinterpret_cast<CGUnit_C*>(static_cast<uintptr_t>(param));
    inst->Release();
    unit->m_movementAnimKit = NULL;
    unit->UpdateMovementAnimKit(0);
}

// PROVEN: 98d07b91 IDENTICAL/NONE
// OFFSET: 0x005FAF00
void CGUnit_C::SetMeleeAnimKitID(uint16_t id) {
    this->m_meleeAnimKitID = id;
}

// PROVEN: a6e59928 IDENTICAL/IDENTICAL
// OFFSET: 0x005FAF20/0x140280150
void CGUnit_C::CastAnimKitStopCallback(CAnimKitInst* inst, uint64_t param) {
    CGUnit_C* unit = reinterpret_cast<CGUnit_C*>(static_cast<uintptr_t>(param));

    if (inst == unit->m_castAnimKit) {
        if (unit->m_combatReadyAnimKit && unit->m_combatReadyAnimKit->GetID() == 142) {
            unit->m_combatReadyAnimKit->Restart();
        }

        unit->m_castAnimKit = NULL;
    }

    inst->Release();
}

// PROVEN: c1b599e4 IDENTICAL/IDENTICAL
// OFFSET: 0x005FAF70/0x1402801B0
void CGUnit_C::SheatheAnimKitStopCallback(CAnimKitInst* inst, uint64_t param) {
    CGUnit_C* unit = reinterpret_cast<CGUnit_C*>(static_cast<uintptr_t>(param));

    if (inst->GetID() == 161 && !(unit->m_animationState & 0x400) && unit->UnsheatheLeftWeapon()) {
        return;
    }

    if (inst->GetID() == 162 && !(unit->m_animationState & 0x200)) {
        unit->UnsheatheRightWeapon();
    }
}

// PROVEN: e7406c92 IDENTICAL/IDENTICAL
// OFFSET: 0x005FAFD0/0x140280210
void CGUnit_C::PlaySpawnAnimKit() {
    if (!(this->m_animationState & 0x8000)) {
        this->m_animKitManager.PlayAndForget(201, 0xFFFFFFFF, NULL);
    }
}

// PROVEN: 9f573d9a IDENTICAL/IDENTICAL
// OFFSET: 0x005FB000/0x140280240
void CGUnit_C::LootAnimKitStopCallback(CAnimKitInst* inst, uint64_t param) {
    reinterpret_cast<CGUnit_C*>(static_cast<uintptr_t>(param))->UpdateLootAnimKit(0);
}

// PROVEN: 7f8c8187 IDENTICAL/IDENTICAL
// OFFSET: 0x005FB010/0x140280250
void CGUnit_C::PlayMountSpecialAnimKit(uint32_t anim) {
    CAnimKitManager* manager;

    if (this->IsMountSelf(0)) {
        if (anim == 0xFFFFFFFF || anim == ANIM_MOUNT_SPECIAL) {
            anim = 636;
        }

        manager = &this->m_animKitManager;
    } else {
        manager = this->m_mountAnimKitManager;

        if (!manager) {
            return;
        }

        if (anim == 0xFFFFFFFF) {
            anim = ANIM_MOUNT_SPECIAL;
        }
    }

    if (manager && anim != 0xFFFFFFFF) {
        manager->PlayAndForget(1360, anim, NULL);
    }
}

// PROVEN: 913b40a4 IDENTICAL/IDENTICAL
// OFFSET: 0x005FB070/0x1402802C0
void CGUnit_C::ResetAIAnimKit() {
    this->m_aiAnimKitID = 0;

    if (this->m_aiAnimKit) {
        this->m_aiAnimKit->SetStopCallback(NULL, 0);
        this->m_aiAnimKit->Stop(0, NULL);
        this->m_aiAnimKit->Release();
        this->m_aiAnimKit = NULL;
    }
}

// OFFSET: 0x005FB0C0/0x140280320
void CGUnit_C::ProcessPlayFlagsForAnimKit(uint8_t playFlags, uint32_t* anim, uint8_t* userFlags) {
    if (playFlags & 0x2) {
        *userFlags = 8;
    } else {
        if (!(playFlags & 0x8) && (*anim == ANIM_SPECIAL_1H || *anim == ANIM_SPECIAL_2H)) {
            const CGUnitVirtualItem* mainHand = this->GetVirtualItem(0, false);
            const CGUnitVirtualItem* offHand = this->GetVirtualItem(1, false);
            *anim = ANIM_SPECIAL_UNARMED;

            if (mainHand) {
                if (mainHand->m_classID == 2 && (mainHand->m_subclassID != 13 || (offHand && offHand->m_subclassID != 13))) {
                    *anim = CGUnit_C::IsTwoHanded(mainHand) && !offHand ? ANIM_SPECIAL_2H : ANIM_SPECIAL_1H;
                } else {
                    *anim = 516;
                }
            } else if (offHand && offHand->m_classID == 2) {
                *anim = 516;
            } else {
                *anim = ANIM_SPECIAL_UNARMED;
            }
        }

        *userFlags = (playFlags & 0x4) + (playFlags & 0x4);
    }

    if (playFlags & 0x1) {
        *userFlags |= 1;
    }

    if (playFlags & 0x20) {
        this->m_animationState |= 0x4000;
    }
}

// PROVEN: a77c8428 IDENTICAL/IDENTICAL
// OFFSET: 0x005FB1B0/0x14026EE10
void CGUnit_C::PointerClearAnimKitStopCallback(CAnimKitInst* inst, uint64_t param) {
    inst->Release();
    *reinterpret_cast<CAnimKitInst**>(static_cast<uintptr_t>(param)) = NULL;
}

// PROVEN: 58345c30 IDENTICAL/IDENTICAL
// OFFSET: 0x005FB1D0/0x140280450
void CGUnit_C::UpdateAnimKitIDsFromClientInitData(const uint16_t* ids) {
    this->m_aiAnimKitID = ids[0];
    this->m_movementAnimKitID = ids[1];
    this->m_meleeAnimKitID = ids[2];
}

// PROVEN: d51272a8 IDENTICAL/NONE
// OFFSET: 0x005FB200
void CGUnit_C::SetAnimKitSplitBody(const CreatureModelDataRec* modelData, CAnimKitManager* manager, int32_t checkTier) {
    int32_t blocked = 0;
    int32_t blockAlt = 0;

    if (modelData) {
        uint32_t flags = modelData->m_flags;

        if (flags & 0x1000) {
            blocked = 1;
        }

        if (flags & 0x2000) {
            blockAlt = 1;
        }

        if (checkTier && (this->m_animTier == 3 || (this->m_movementData.m_flags & 0x1100000))) {
            if (flags & 0x4000) {
                blocked = 1;
            }

            if (flags & 0x8000) {
                blockAlt = 1;
            }
        }
    }

    if (manager) {
        manager->SetBoneDataIDBlocked(4, blocked, blockAlt);
    }
}

// PROVEN: 6adbddda IDENTICAL/IDENTICAL
// OFFSET: 0x005FB270/0x140280480
void CGUnit_C::DeathAnimKitStopCallback(CAnimKitInst* inst, uint64_t param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    if (unit->m_animationState & 0x2000) {
        if (unit->m_movementData.m_flags & 0x6008FF) {
            unit->PlayDeathAnimKit(3);
        } else {
            unit->PlayDeathAnimKit(4);
        }
    } else {
        unit->m_deathAnimKit = NULL;
        unit->m_animationState &= ~0x2000;
    }
}

// OFFSET: 0x005FB2E0/0x140280500
void CGUnit_C::PlayCombatReadyAnimKit(uint32_t type, uint32_t anim, uint32_t secondaryAnim) {
    uint32_t kitID = s_combatReadyAnimKitIDs[type];
    int32_t split = type == 2;
    CAnimKitInst** slot = &this->m_combatReadyAnimKit;

    if (*slot && (*slot)->GetID() == kitID) {
        if (split) {
            (*slot)->SetAnim(0, secondaryAnim, 255, 0, 0, 0);
            (*slot)->SetAnim(1, anim, 255, 0, 0, 0);
        } else {
            (*slot)->SetAnim(0, anim, 255, 0, 0, 0);
        }

        return;
    }

    if (*slot) {
        (*slot)->SetStopCallback(NULL, 0);
        (*slot)->Stop(1, NULL);
        (*slot)->Release();
    }

    *slot = this->m_animKitManager.PlayAndMaintain(kitID, split ? secondaryAnim : anim, NULL);

    if (*slot) {
        if (split) {
            (*slot)->SetAnim(1, anim, 255, 0, 0, 0);
        }

        (*slot)->SetStopCallback(&CGUnit_C::PointerClearAnimKitStopCallback, reinterpret_cast<uint64_t>(slot));
    }
}

// OFFSET: 0x005FB3E0/0x140280680
void CGUnit_C::PlayWoundAnimKit(int32_t crit) {
    if (this->AppearsDead(1) || this->DoNotPlayWoundAnim()) {
        return;
    }

    for (CEffect* effect = this->m_effectList; effect; effect = effect->m_next) {
        if (effect->m_flags & 0x4) {
            return;
        }
    }

    uint32_t anim;
    uint32_t kitID;

    if (crit) {
        anim = 10;
    } else if (this->m_combat.m_attacking) {
        anim = 9;
    } else {
        anim = 8;

        if (!this->m_mountModel && !(this->m_movement->m_flags & 0x170080F) && !(this->m_animationState & 0x40000) && !this->m_standStateAnimKit) {
            kitID = 83;
            goto play;
        }
    }

    kitID = 101;

    if (this->m_combatReadyAnimKit && this->m_animKitManager.GetExpressingAnimKitInst(0xFFFFFFFF, 1) == this->m_combatReadyAnimKit) {
        kitID = 83;
    }

play:
    CAnimKitInst** slot = &this->m_woundAnimKit;

    if (*slot && (*slot)->GetID() == kitID) {
        (*slot)->SetAnim(0, anim, 255, 0, 0, 0);
        (*slot)->Restart();
        return;
    }

    if (*slot) {
        (*slot)->SetStopCallback(NULL, 0);
        (*slot)->Stop(1, NULL);
    }

    *slot = this->m_animKitManager.PlayAndMaintain(kitID, anim, NULL);

    if (*slot) {
        (*slot)->SetStopCallback(&CGUnit_C::PointerClearAnimKitStopCallback, reinterpret_cast<uint64_t>(slot));
    }
}

// OFFSET: 0x005FB540/0x140280840
void CGUnit_C::PlayPrecastAnimKit(uint32_t type, int32_t channelFlag, int32_t spellID, uint8_t playFlags, uint32_t anim, uint32_t secondaryAnim) {
    int32_t split = type == 1 || type == 3;
    uint32_t kitID = s_precastAnimKitIDs[type];
    uint8_t userFlags;
    this->ProcessPlayFlagsForAnimKit(playFlags, &anim, &userFlags);
    this->ProcessPlayFlagsForAnimKit(playFlags, &secondaryAnim, &userFlags);
    CAnimKitInst** slot = &this->m_precastAnimKit;

    if (*slot && (*slot)->GetID() == kitID) {
        if (userFlags) {
            (*slot)->SetUserFlags(0, userFlags);

            if (split) {
                (*slot)->SetUserFlags(1, userFlags);
            }
        }

        if (split) {
            (*slot)->SetAnim(0, secondaryAnim, 255, 0, 0, 0);
            (*slot)->SetAnim(1, anim, 255, 0, 0, 0);
        } else {
            (*slot)->SetAnim(0, anim, 255, 0, 0, 0);
        }
    } else {
        if (*slot) {
            (*slot)->SetStopCallback(NULL, 0);
            (*slot)->Stop(1, NULL);
            (*slot)->Release();
            *slot = NULL;
        }

        if (split) {
            *slot = this->m_animKitManager.PlayAndMaintain(kitID, secondaryAnim, NULL);

            if (!*slot) {
                return;
            }

            (*slot)->SetAnim(1, anim, 255, 0, 0, 0);
        } else {
            *slot = this->m_animKitManager.PlayAndMaintain(kitID, anim, NULL);

            if (!*slot) {
                return;
            }
        }

        if (userFlags) {
            (*slot)->SetUserFlags(0, userFlags);

            if (split) {
                (*slot)->SetUserFlags(1, userFlags);
            }
        }

        (*slot)->SetStopCallback(&CGUnit_C::PointerClearAnimKitStopCallback, reinterpret_cast<uint64_t>(slot));
    }

    if (*slot && channelFlag) {
        (*slot)->SetUserFlags(type == 1, 4);
    }
}

// PROVEN: 8620b61c IDENTICAL/DIFFERENT
// OFFSET: 0x005FB700/0x140280A90
void CGUnit_C::PlayCastAnimKit(uint32_t anim, uint8_t playFlags, int32_t fullBody, int32_t spellID) {
    uint32_t kitID = fullBody || !this->IsSplitbodyAnim(anim) ? 125 : 81;
    uint8_t userFlags;
    this->ProcessPlayFlagsForAnimKit(playFlags, &anim, &userFlags);

    if (this->m_castAnimKit) {
        this->m_castAnimKit->SetStopCallback(NULL, 0);
        this->m_castAnimKit->Stop(1, NULL);
        this->m_castAnimKit->Release();
        this->m_castAnimKit = NULL;
    }

    this->m_castAnimKit = this->m_animKitManager.PlayAndMaintain(kitID, anim, NULL);

    if (this->m_castAnimKit) {
        if (userFlags) {
            this->m_castAnimKit->SetUserFlags(0, userFlags);
        }

        this->m_castAnimKit->SetStopCallback(&CGUnit_C::CastAnimKitStopCallback, reinterpret_cast<uint64_t>(this));
    }
}

// PROVEN: 59028d73 IDENTICAL/DIFFERENT
// OFFSET: 0x005FB7C0/0x140280B80
void CGUnit_C::PlayOneShotSpellAnimKit(uint32_t anim, uint8_t playFlags, int32_t fullBody, int32_t spellID) {
    uint32_t kitID = fullBody || !this->IsSplitbodyAnim(anim) ? 125 : 81;
    uint8_t userFlags;
    this->ProcessPlayFlagsForAnimKit(playFlags, &anim, &userFlags);
    CAnimKitInst* inst = this->m_animKitManager.PlayAndMaintain(kitID, anim, NULL);

    if (inst) {
        if (userFlags) {
            inst->SetUserFlags(0, userFlags);
        }

        inst->Release();
    }
}

// OFFSET: 0x005FB830/0x140280C10
void CGUnit_C::UpdateEffectAnimKit() {
    CEffect* effect;

    for (effect = this->m_effectList; effect; effect = effect->m_next) {
        uint32_t flags = effect->m_flags;

        if (!(flags & 0x800000) && effect->m_kitRec && effect->m_kitRec->m_animID > 0 && (flags & 0x1000)) {
            break;
        }
    }

    if (!effect) {
        if (this->m_effectAnimKit) {
            this->m_effectAnimKit->Stop(1, NULL);
        }

        return;
    }

    uint8_t userFlags = static_cast<uint8_t>(effect->m_flags >> 21) & 8;
    uint32_t channelSpell = this->m_unit->m_unitChannelSpell;

    if (channelSpell && effect->m_spellId == channelSpell && (effect->m_kitRec->m_flags & 0x40)) {
        userFlags |= 4;
    }

    uint32_t kitID = this->IsSplitbodyAnim(effect->m_kitRec->m_animID) ? 84 : 121;
    CAnimKitInst** slot = &this->m_effectAnimKit;

    if (*slot && (*slot)->GetID() != kitID) {
        (*slot)->Stop(1, NULL);
    }

    if (*slot) {
        (*slot)->SetAnim(0, effect->m_kitRec->m_animID, 255, 0, 0, 0);
        (*slot)->SetUserFlags(0, userFlags);
        return;
    }

    *slot = this->m_animKitManager.PlayAndMaintain(kitID, effect->m_kitRec->m_animID, NULL);

    if (*slot) {
        (*slot)->SetStopCallback(&CGUnit_C::PointerClearAnimKitStopCallback, reinterpret_cast<uint64_t>(slot));
        (*slot)->SetUserFlags(0, userFlags);
    }
}

// PROVEN: c514e382 IDENTICAL/IDENTICAL
// OFFSET: 0x005FB980/0x140280DA0
void CGUnit_C::PlaySheatheAnimKit(uint32_t anim, int32_t leftHand) {
    this->m_animationState |= leftHand ? 0x20000 : 0x10000;
    CAnimKitInst* inst = this->m_animKitManager.PlayAndMaintain(leftHand ? 161 : 162, anim, NULL);

    if (inst) {
        inst->SetStopCallback(&CGUnit_C::SheatheAnimKitStopCallback, reinterpret_cast<uint64_t>(this));
        inst->Release();
    }
}

// OFFSET: 0x005FB9F0/0x140280E10
void CGUnit_C::PlayStandStateAnimKit(uint32_t type, uint32_t anim, uint32_t secondaryAnim) {
    int32_t split = type == 2 || type == 4;
    CAnimKitInst** slot = &this->m_standStateAnimKit;
    uint32_t currentID = *slot ? (*slot)->GetID() : 0;
    uint32_t kitID = s_standStateAnimKitIDs[type];

    if (currentID == kitID) {
        if (*slot) {
            if (split) {
                (*slot)->SetAnim(0, secondaryAnim, 255, 0, 0, 0);
                (*slot)->SetAnim(1, anim, 255, 0, 0, 0);
                (*slot)->Restart();
            } else {
                (*slot)->SetAnim(0, anim, 255, 0, 0, 0);
            }
        }

        return;
    }

    if (*slot) {
        (*slot)->SetStopCallback(NULL, 0);
        (*slot)->Stop(1, NULL);
        (*slot)->Release();
        *slot = NULL;
    }

    *slot = this->m_animKitManager.PlayAndMaintain(kitID, split ? secondaryAnim : anim, NULL);

    if (*slot) {
        if (split) {
            (*slot)->SetAnim(1, anim, 255, 0, 0, 0);
        }

        (*slot)->SetStopCallback(&CGUnit_C::PointerClearAnimKitStopCallback, reinterpret_cast<uint64_t>(slot));
    }
}

// OFFSET: 0x005FBB20/0x140280FB0
void CGUnit_C::UpdateLootAnimKit(int32_t immediate) {
    uint32_t currentID = this->m_lootAnimKit ? this->m_lootAnimKit->GetID() : 0;
    uint32_t kitID;

    if (this->IsLooting() && this->ShouldKneelForLoot()) {
        kitID = currentID == 203 || currentID == 202 || immediate ? 202 : 203;

        if (currentID == kitID) {
            return;
        }
    } else if (currentID == 202) {
        kitID = 204;
    } else {
        kitID = 0;

        if (currentID == kitID) {
            return;
        }
    }

    if (this->m_lootAnimKit) {
        this->m_lootAnimKit->SetStopCallback(NULL, 0);
        this->m_lootAnimKit->Stop(1, NULL);
        this->m_lootAnimKit->Release();
    }

    if (kitID) {
        this->m_lootAnimKit = this->m_animKitManager.PlayAndMaintain(kitID, 0xFFFFFFFF, NULL);

        if (this->m_lootAnimKit) {
            this->m_lootAnimKit->SetStopCallback(&CGUnit_C::LootAnimKitStopCallback, reinterpret_cast<uint64_t>(this));
        }
    } else {
        this->m_lootAnimKit = NULL;
    }
}

// PROVEN: abab3222 IDENTICAL/IDENTICAL
// OFFSET: 0x005FBC00/0x1402810C0
void CGUnit_C::PlayLootStartAnimKit() {
    if (this->m_lootAnimKit) {
        this->m_lootAnimKit->SetStopCallback(NULL, 0);
        this->m_lootAnimKit->Stop(1, NULL);
        this->m_lootAnimKit->Release();
    }

    this->m_lootAnimKit = this->m_animKitManager.PlayAndMaintain(203, 0xFFFFFFFF, NULL);

    if (this->m_lootAnimKit) {
        this->m_lootAnimKit->SetStopCallback(&CGUnit_C::LootAnimKitStopCallback, reinterpret_cast<uint64_t>(this));
    }
}

// PROVEN: 044f108f IDENTICAL/IDENTICAL
// OFFSET: 0x005FBC70/0x140281140
void CGUnit_C::DestroyEmoteSpellEffects() {
    for (CEffect* effect = this->m_effectList; effect; ) {
        CEffect* next = effect->m_next;
        uint32_t flags = effect->m_flags;

        if ((flags & 0x80000000) && !(flags & 0x1000)) {
            effect->MoveToDestroyList();
        }

        effect = next;
    }
}

// PROVEN: 525f0ef5 IDENTICAL/IDENTICAL
// OFFSET: 0x005FBCB0/0x140281190
void CGUnit_C::EmoteAnimKitStopCallback(CAnimKitInst* inst, uint64_t param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    if (unit->m_emoteAnimKit) {
        unit->m_emoteAnimKit->Release();
        unit->m_emoteAnimKit = NULL;
    }

    if (unit->m_animationState & 0x100) {
        unit->SetSheatheState(unit->m_previousSheatheState, 1, 0);
        unit->m_animationState &= ~0x100;
    }

    unit->UpdateEmoteAnimKit();
}

// OFFSET: 0x005FBD30/0x140281210
void CGUnit_C::PlayTierTransitionAnimKit(uint32_t anim) {
    CAnimKitManager* manager = this->m_mountAnimKitManager;

    if (!manager) {
        manager = &this->m_animKitManager;
    }

    CAnimKitInst* inst = manager->PlayAndMaintain(anim != 460 ? 1349 : 241, 0xFFFFFFFF, NULL);

    if (this->GetObjectModel()->IsLoaded()) {
        anim = manager->FilterAnim(anim, 1);
    }

    inst->SetAnim(0, anim, 255, 0, 0, 0);
    inst->Release();
}

// OFFSET: 0x005FBDC0/0x1402812D0
void CGUnit_C::UpdateHandItemAnim(CM2Model* model, uint32_t sheathedAnim) {
    if (!this->m_characterComponent) {
        return;
    }

    uint32_t attachIDs[6];
    uint32_t count = this->m_characterComponent->GetHandItemAttachIDs(attachIDs, 6);

    for (uint32_t i = 0; i < count; i++) {
        uint32_t attachID = attachIDs[i];
        CM2Model* attached = this->m_worldModel->m_attachList;

        while (attached && attached->m_attachmentId != attachID) {
            attached = attached->m_attachNext;
        }

        if (attached == model) {
            this->m_animKitManager.UpdateHandItemAnim(attachID, attached, reinterpret_cast<void*>(&CGUnit_C::HandItemAnimCallback), this->m_obj->m_guid.m_guid, sheathedAnim);
            return;
        }
    }
}

// OFFSET: 0x005FBE50/0x1402813A0
void CGUnit_C::UpdateAnimKitSplitBody() {
    CAnimKitManager* mountManager = this->m_mountAnimKitManager;

    if (!mountManager) {
        this->SetAnimKitSplitBody(this->m_modelData, &this->m_animKitManager, 1);
        return;
    }

    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_unit->m_unitFieldMountdisplayid);

    if (displayInfo) {
        this->SetAnimKitSplitBody(g_creatureModelDataDB.GetRecord(displayInfo->m_modelID), mountManager, 1);
    }

    CAnimKitManager* manager = &this->m_animKitManager;
    const CreatureModelDataRec* modelData = this->m_modelData;
    int32_t blocked = 0;
    int32_t blockAlt = 0;

    if (modelData) {
        uint32_t flags = modelData->m_flags;

        if (flags & 0x1000) {
            blocked = 1;
        }

        if (flags & 0x2000) {
            blockAlt = 1;
        }
    }

    if (manager) {
        manager->SetBoneDataIDBlocked(4, blocked, blockAlt);
    }
}

// OFFSET: 0x005FBF20/0x140281520
const UnitAnimationSet* CGUnit_C::SelectAnimationSet() {
    uint32_t flags = this->m_movementData.m_flags;
    uint32_t swimming = flags & 0x100000;
    uint32_t alternate = flags & 0x800000;
    uint32_t swimOrFly = flags & 0x1100000;
    int32_t mounted = this->m_mountDisplayId > 0 && !(this->m_clientFlags & 0x10000000) ? 1 : 0;
    int32_t mountSelf = this->IsMountSelf(0);
    uint32_t hover = this->m_animationState & 0x40000;
    uint32_t stealth = (this->m_unit->m_unitFieldBytes1[2] >> 1) & 1;
    CMoveSpline* spline = this->m_movement->m_spline;
    int32_t flyingSpline = spline && !(spline->m_flags & 0x80) && (spline->m_flags & 0x200) ? 1 : 0;

    if (stealth && !swimOrFly) {
        return &s_animSetStealth;
    }

    if (flyingSpline) {
        return &s_animSetFlyingSpline;
    }

    if (hover && !swimOrFly) {
        return &s_animSetHover;
    }

    uint32_t row = 0;

    if (mounted) {
        row = (mountSelf != 0) + 1;
    }

    if (swimOrFly) {
        return s_animSets[row * 4 + (swimming ? 2 : 3)];
    }

    return s_animSets[row * 4 + (alternate != 0)];
}

// OFFSET: 0x005FC040/0x140281660
void CGUnit_C::PlayMovementAnimKit(uint32_t type, uint32_t anim, uint32_t secondaryAnim, int32_t randomStart) {
    this->UpdateAnimKitSplitBody();
    uint32_t kitID;

    if (type == 1 && this->m_movementAnimKitID) {
        kitID = this->m_movementAnimKitID;
        anim = 0xFFFFFFFF;
    } else {
        kitID = s_movementAnimKitIDs[type];
    }

    CAnimKitInst** slot = &this->m_movementAnimKit;
    uint32_t currentID = *slot ? (*slot)->GetID() : 0;

    if (currentID == kitID) {
        if (*slot) {
            if (type == 3) {
                (*slot)->SetAnim(0, secondaryAnim, 255, 0, 0, 0);
                (*slot)->SetAnim(1, anim, 255, 0, 0, 0);
            } else {
                (*slot)->SetAnim(0, anim, 255, 0, 0, 0);
            }
        }
    } else {
        if (*slot) {
            (*slot)->SetStopCallback(NULL, 0);
            (*slot)->Stop(1, NULL);
            (*slot)->Release();
            *slot = NULL;
        }

        if (!kitID) {
            return;
        }

        AnimKitExtraPlayParams params;
        params.boneSetID = -1;
        params.startTime = 0;
        params.startFraction = 0.0f;
        params.oneShot = 0;
        params.blendTimeMode = 0;
        params.blendTime = 0;

        if (randomStart) {
            params.startFraction = CRandom::ufloat_(g_rndSeed);
        }

        int32_t split = type == 3;
        CAnimKitManager* manager = this->m_mountAnimKitManager;

        if (!manager) {
            manager = &this->m_animKitManager;
        }

        *slot = manager->PlayAndMaintain(kitID, split ? secondaryAnim : anim, &params);

        if (!*slot) {
            return;
        }

        if (split) {
            (*slot)->SetAnim(1, anim, 255, 0, 0, 0);
        }

        if (type == 6) {
            (*slot)->SetStopCallback(&CGUnit_C::MovementOneShotAnimKitStopCallback, reinterpret_cast<uint64_t>(this));
        } else {
            (*slot)->SetStopCallback(&CGUnit_C::PointerClearAnimKitStopCallback, reinterpret_cast<uint64_t>(slot));
        }
    }

    if (*slot && s_movementAnimKitScalesSpeed[type] && (this->m_movementData.m_flags & 0x60000F)) {
        float speed = this->m_movementData.GetBaseSpeed(0);
        uint32_t count = (*slot)->GetSegmentCount();

        for (uint32_t i = 0; i < count; i++) {
            (*slot)->SetSpeed(i, 3, speed);
        }
    }
}

// OFFSET: 0x005FC240/0x1402818F0
void CGUnit_C::PlayJumpEndAnimKit() {
    CAnimKitInst** slot = &this->m_jumpEndAnimKit;

    if (*slot) {
        return;
    }

    *slot = this->m_animKitManager.PlayAndMaintain(45, this->SelectAnimationSet()->m_jumpEnd, NULL);

    if (*slot) {
        (*slot)->SetStopCallback(&CGUnit_C::PointerClearAnimKitStopCallback, reinterpret_cast<uint64_t>(slot));
    }
}

// PROVEN: 98e48c60 IDENTICAL/DIFFERENT
// OFFSET: 0x005FC290/0x140281960
void CGUnit_C::PlayDeathAnimKit(uint32_t type) {
    if (this->m_deathAnimKit) {
        this->m_deathAnimKit->SetStopCallback(NULL, 0);
        this->m_deathAnimKit->Stop(1, NULL);
        this->m_deathAnimKit->Release();
        this->m_deathAnimKit = NULL;
    }

    this->m_deathAnimKit = this->m_animKitManager.PlayAndMaintain(s_deathAnimKitIDs[type], 0xFFFFFFFF, NULL);

    if (this->m_deathAnimKit) {
        this->m_deathAnimKit->SetStopCallback(&CGUnit_C::DeathAnimKitStopCallback, this->m_obj->m_guid.m_guid);

        if (s_deathAnimKitIsCorpse[type]) {
            this->m_animationState |= 0x2000;
        } else {
            this->m_animationState &= ~0x2000;
        }
    }
}

// OFFSET: 0x005FC340/0x140281A40
void CGUnit_C::UpdateCombatReadyAnimKit(int32_t a2) {
    if (this->IsInMelee()) {
        this->PlayCombatReadyAnimKit(0, this->GetCombatReadyAnim(), 0xFFFFFFFF);
        return;
    }

    if (this->m_sheatheState == 2 && (this->m_animationState & 0x8)) {
        uint32_t secondaryAnim = 25;
        uint32_t anim = 25;
        const CGUnitVirtualItem* ranged = this->GetVirtualItem(2, false);

        if (ranged && ranged->m_classID == 2) {
            switch (ranged->m_subclassID) {
                case 2:
                    secondaryAnim = 105;
                    anim = 109;
                    break;

                case 3:
                case 18:
                    secondaryAnim = 106;
                    anim = 110;
                    break;

                case 16:
                case 19:
                    secondaryAnim = 112;
                    anim = 111;
                    break;
            }
        }

        if (a2) {
            this->PlayCombatReadyAnimKit(1, anim, 0xFFFFFFFF);
        } else {
            this->PlayCombatReadyAnimKit(2, anim, secondaryAnim);
        }

        return;
    }

    if (this->m_combatReadyAnimKit) {
        this->m_combatReadyAnimKit->Stop(1, NULL);
    }
}

// OFFSET: 0x005FC450/0x140281B70
void CGUnit_C::PlayCombatActionAnimKit(uint32_t type, uint32_t anim, uint32_t playFlags, int32_t ignoreMeleeKit) {
    if (this->m_combatActionAnimKit) {
        this->m_pendingCombatActionAnim = anim;
        this->m_pendingCombatActionType = type;
        this->m_pendingCombatActionFlags = playFlags;
        uint32_t count = this->m_combatActionAnimKit->GetSegmentCount();

        for (uint32_t i = 0; i < count; i++) {
            this->m_combatActionAnimKit->SetSpeed(i, 2, 2.0f);
        }

        return;
    }

    uint8_t userFlags;
    this->ProcessPlayFlagsForAnimKit(playFlags, &anim, &userFlags);
    uint32_t kitID;

    if (type == 1 && !ignoreMeleeKit && this->m_meleeAnimKitID) {
        kitID = this->m_meleeAnimKitID;
        anim = 0xFFFFFFFF;
    } else {
        kitID = s_combatActionAnimKitIDs[type];
    }

    AnimKitExtraPlayParams params;
    params.boneSetID = -1;
    params.startTime = 0;
    params.startFraction = 0.0f;
    params.oneShot = 1;
    params.blendTimeMode = 0;
    params.blendTime = 0;
    this->m_combatActionAnimKit = this->m_animKitManager.PlayAndMaintain(kitID, anim, &params);

    if (this->m_combatActionAnimKit) {
        if (userFlags) {
            uint32_t count = this->m_combatActionAnimKit->GetSegmentCount();

            for (uint32_t i = 0; i < count; i++) {
                this->m_combatActionAnimKit->SetUserFlags(i, userFlags);
            }
        }

        this->m_combatActionAnimKit->SetStopCallback(&CGUnit_C::CombatActionAnimKitStopCallback, reinterpret_cast<uint64_t>(this));
    }
}

// PROVEN: 63d977c9 IDENTICAL/IDENTICAL
// OFFSET: 0x005FC580/0x140281CE0
void CGUnit_C::CombatActionAnimKitStopCallback(CAnimKitInst* inst, uint64_t param) {
    CGUnit_C* unit = reinterpret_cast<CGUnit_C*>(static_cast<uintptr_t>(param));
    inst->Release();
    unit->m_combatActionAnimKit = NULL;

    if (unit->m_pendingCombatActionAnim != -1 && unit->m_pendingCombatActionType) {
        unit->PlayCombatActionAnimKit(unit->m_pendingCombatActionType, unit->m_pendingCombatActionAnim, unit->m_pendingCombatActionFlags, 0);
        unit->m_pendingCombatActionAnim = -1;
    }
}

// OFFSET: 0x005FC5D0/0x140281D40
void CGUnit_C::UpdatePrecastAnimKit(int32_t a2, WOWGUID target) {
    const SpellRec* spell = g_spellDB.GetRecord(this->m_castSpellId);
    const SpellVisualRec* visual;
    SpellVisualKitRec* kit;

    if (spell && (visual = GetSpellVisual(spell, this)) && CanPlaySpellVisualKit(this->m_obj->m_guid.m_guid, target.m_guid, (kit = g_spellVisualKitDB.GetRecord(visual->m_precastKit)), visual)) {
        uint8_t playFlags = CGUnit_C::KitFlagsToPlayFlags(spell, visual, kit);
        int32_t hasCast = kit->m_startAnimID > 0;
        int32_t hasLoop = kit->m_animID > 0;

        if (kit->m_startAnimID > 0 || kit->m_animID > 0 || kit->m_animKitID > 0) {
            this->m_pendingCombatActionAnim = -1;

            if (this->m_combatActionAnimKit) {
                this->m_combatActionAnimKit->Stop(1, NULL);
            }

            if (hasCast || hasLoop) {
                int32_t channelFlag = kit->m_flags & 0x40;

                if (this->IsVehicle() && this->m_vehicle->HasHighPriorityCastingAnim()) {
                    if (!a2 && hasCast && hasLoop) {
                        this->PlayPrecastAnimKit(3, channelFlag, spell->m_id, playFlags, kit->m_animID, kit->m_startAnimID);
                    } else {
                        this->PlayPrecastAnimKit(2, channelFlag, spell->m_id, playFlags, (a2 || !hasCast) && hasLoop ? kit->m_animID : kit->m_startAnimID, 0xFFFFFFFF);
                    }
                } else if (!a2 && hasCast && hasLoop) {
                    this->PlayPrecastAnimKit(1, channelFlag, spell->m_id, playFlags, kit->m_animID, kit->m_startAnimID);
                } else {
                    this->PlayPrecastAnimKit(0, channelFlag, spell->m_id, playFlags, (a2 || !hasCast) && hasLoop ? kit->m_animID : kit->m_startAnimID, 0xFFFFFFFF);
                }

                return;
            }

            return;
        }
    }

    if (this->m_precastAnimKit) {
        this->m_precastAnimKit->SetStopCallback(NULL, 0);
        this->m_precastAnimKit->Stop(1, NULL);
        this->m_precastAnimKit->Release();
        this->m_precastAnimKit = NULL;
    }
}

// PROVEN: aab13ccb IDENTICAL/IDENTICAL
// OFFSET: 0x005FC800/0x140281FC0
void CGUnit_C::EmoteSpellEffectStopCallback(CAnimKitInst* inst, uint64_t param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->DestroyEmoteSpellEffects();
    }
}

// PROVEN: 8120a699 IDENTICAL/IDENTICAL
// OFFSET: 0x005FC830/0x140282030
void CGUnit_C::StopEmoteSpellEffects() {
    for (CEffect* effect = this->m_effectList; effect; ) {
        CEffect* next = effect->m_next;
        uint32_t flags = effect->m_flags;

        if ((flags & 0x80000000) && (flags & 0x1000)) {
            CAnimKitInst* stopKit = NULL;
            effect->StopAnimKit(0, &stopKit);

            if (stopKit) {
                stopKit->SetStopCallback(&CGUnit_C::EmoteSpellEffectStopCallback, this->m_obj->m_guid.m_guid);
            }

            this->OnSpellEffectClear(effect);
        }

        effect = next;
    }
}

// OFFSET: 0x005FC8A0/0x1402820E0
void CGUnit_C::UpdateAIAnimKit(int32_t randomStart) {
    CAnimKitInst** slot = &this->m_aiAnimKit;

    if (*slot) {
        if ((*slot)->GetID() == this->m_aiAnimKitID) {
            return;
        }

        (*slot)->SetStopCallback(NULL, 0);
        (*slot)->Stop(1, NULL);
        (*slot)->Release();
        *slot = NULL;
    }

    if (!this->m_aiAnimKitID) {
        return;
    }

    AnimKitExtraPlayParams params;
    params.boneSetID = -1;
    params.startTime = 0;
    params.startFraction = 0.0f;
    params.oneShot = 0;
    params.blendTimeMode = 0;
    params.blendTime = 0;

    if (randomStart) {
        params.startFraction = CRandom::ufloat_(g_rndSeed);
    }

    *slot = this->m_animKitManager.PlayAndMaintain(this->m_aiAnimKitID, 0xFFFFFFFF, &params);

    if (*slot) {
        (*slot)->SetStopCallback(&CGUnit_C::PointerClearAnimKitStopCallback, reinterpret_cast<uint64_t>(slot));
    }
}

// PROVEN: 83636f7f IDENTICAL/IDENTICAL
// OFFSET: 0x005FC990/0x140282210
void CGUnit_C::HandItemAnimCallback(CM2Model* model, uint32_t boneId, uint32_t sheathedAnim, uint32_t a4, uint32_t a5, uint64_t param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->UpdateHandItemAnim(model, sheathedAnim);
    }
}

// PROVEN: 6822d352 IDENTICAL/IDENTICAL
// OFFSET: 0x005FC9D0/0x140282260
void CGUnit_C::UpdateAnimKitsBeforeMountModelChange(int32_t mounting) {
    if (!mounting || !this->m_mountAnimKitManager) {
        return;
    }

    if (this->m_vehiclePassenger) {
        this->m_vehiclePassenger->FreeAnimKit();
    }

    if (this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        this->m_vehicle->FreeAnimKits();
    }

    CAnimKitManager* manager = this->m_mountAnimKitManager;
    this->m_movementAnimKit = NULL;
    CAnimKitManager::Free(manager);
    this->m_mountAnimKitManager = NULL;
}

// OFFSET: 0x005FCA40/0x1402822D0
void CGUnit_C::UpdateAnimKitMgrTier() {
    this->m_animKitManager.SetTier(this->m_animTier);

    if (this->m_mountAnimKitManager) {
        this->m_mountAnimKitManager->SetTier(this->m_animTier);
    }

    this->UpdateAnimKitSplitBody();

    if (this->m_vehiclePassenger && this->m_vehiclePassenger->MirrorsVehicleAnimTier()) {
        this->m_vehiclePassenger->UpdateVehicleAnimMirroring();
    }
}

// OFFSET: 0x005FCAA0/0x140282340
void CGUnit_C::UpdateMovementAnimKit(int32_t force) {
    CMovement_C* movement = &this->m_movementData;
    uint32_t flags = this->m_movementData.m_flags;

    if ((flags & 0x800) && ((flags & 0x1000) || movement->m_fallVelocity != 0.0f)) {
        return;
    }

    CMoveSpline* spline = movement->m_spline;

    if ((movement->m_flags2 & 0x40) && spline && !(spline->m_flags & 0x80) && (spline->m_flags & 0x2000000)) {
        return;
    }

    float walkSpeed = this->m_movementData.m_walkSpeed * 2.0f;
    float speed = movement->GetBaseSpeed(0);
    uint32_t stealth = (this->m_unit->m_unitFieldBytes1[2] >> 1) & 1;
    uint32_t moveDir = this->m_movementData.m_flags & 0xF;
    uint32_t hover = this->m_animationState & 0x40000;
    int32_t splineCustom = (movement->m_flags2 & 0x80) && spline && !(spline->m_flags & 0x80) && (spline->m_flags & 0x1000000) ? 1 : 0;
    int32_t splineFall = spline && !(spline->m_flags & 0x80) && (spline->m_flags & 0x40) ? 1 : 0;
    const UnitAnimationSet* set = this->SelectAnimationSet();
    uint32_t type;
    uint32_t anim;

    if (splineCustom) {
        type = 5;
        anim = 0;
    } else {
        if (splineFall) {
            anim = set->m_fall;
            type = 1;
        } else if (moveDir) {
            type = 1;
            uint32_t tier;

            if (speed >= 11.0f) {
                tier = 2;
            } else {
                tier = walkSpeed >= speed ? 0 : 1;
            }

            uint32_t moveFlags = this->m_movementData.m_flags;

            if (moveFlags & 0x2) {
                anim = set->m_move[tier].m_backward;
            } else if (this->m_movement->m_flags & 0xC) {
                anim = (moveFlags & 0x4) ? set->m_move[tier].m_left : set->m_move[tier].m_right;
            } else {
                anim = set->m_move[tier].m_forward;
            }
        } else {
            uint32_t moveFlags = this->m_movementData.m_flags;
            uint32_t swimOrFly = moveFlags & 0x1100000;
            int32_t turnLeft = (moveFlags & 0x10) || (this->m_animationState & 0x20) ? 1 : 0;
            int32_t turnRight = (moveFlags & 0x20) || (this->m_animationState & 0x40) ? 1 : 0;
            int32_t fullBody = (this->m_animationState & 0x2) || (this->GetModelData() && !(this->GetModelData()->m_flags & 0x1000)) ? 1 : 0;

            if (swimOrFly) {
                anim = set->m_stand;
                type = fullBody ? 7 : 5;
            } else if (hover) {
                type = !stealth && fullBody ? 7 : 5;
                anim = set->m_stand;
            } else if (!turnLeft && !turnRight) {
                type = 5;
                anim = set->m_stand;
            } else {
                type = 4;
                anim = turnLeft ? set->m_shuffleLeft : set->m_shuffleRight;
            }
        }

        if (anim == 0xFFFFFFFF) {
            type = 0;
        }
    }

    this->PlayMovementAnimKit(type, anim, 0xFFFFFFFF, force);
}

// OFFSET: 0x005FCD20/0x140282630
void CGUnit_C::UpdateDeathAnimKit() {
    if (this->AppearsDead(0)) {
        if (this->m_worldModel->IsLoaded() && !this->m_deathAnimKit) {
            if (this->HasDeathSequence()) {
                this->PlayDeathAnimKit((this->m_movementData.m_flags & 0x6008FF) ? 3 : 5);
            } else {
                this->PlayDeathAnimKit((this->m_movement->m_flags & 0x100000) ? 6 : 0);
            }
        }

        return;
    }

    if (this->m_deathAnimKit) {
        this->m_deathAnimKit->SetStopCallback(NULL, 0);
        this->m_deathAnimKit->Stop(1, NULL);
        this->m_deathAnimKit->Release();
        this->m_deathAnimKit = NULL;
    }

    this->m_animationState &= ~0x2000;
}

// OFFSET: 0x005FCDD0/0x140282710
void CGUnit_C::UpdateStandStateAnimKit(uint32_t oldStandState, int32_t instant) {
    uint32_t standState = this->GetClientStandState();

    if (oldStandState == standState) {
        return;
    }

    if (oldStandState == 7 && !this->AppearsDead(1)) {
        this->UpdateDeathAnimKit();
    }

    switch (standState) {
        case 0:
            switch (oldStandState) {
                case 1:
                    this->PlayStandStateAnimKit(0, 98, 0xFFFFFFFF);
                    return;

                case 3:
                    this->PlayStandStateAnimKit(0, 101, 0xFFFFFFFF);
                    return;

                case 8:
                    this->PlayStandStateAnimKit(0, 116, 0xFFFFFFFF);
                    return;

                case 9:
                    this->PlayStandStateAnimKit(5, 0xFFFFFFFF, 0xFFFFFFFF);
                    return;
            }

            break;

        case 1:
            if (instant) {
                this->PlayStandStateAnimKit(1, 97, 0xFFFFFFFF);
            } else {
                this->PlayStandStateAnimKit(2, 97, 96);
            }

            return;

        case 3:
            if (instant) {
                this->PlayStandStateAnimKit(1, 100, 0xFFFFFFFF);
            } else {
                this->PlayStandStateAnimKit(2, 100, 99);
            }

            return;

        case 4:
            this->PlayStandStateAnimKit(1, 102, 0xFFFFFFFF);
            return;

        case 5:
            this->PlayStandStateAnimKit(1, 103, 0xFFFFFFFF);
            return;

        case 6:
            this->PlayStandStateAnimKit(1, 104, 0xFFFFFFFF);
            return;

        case 7:
            this->DisengageFromActivePlayer();
            this->PlayDeathAnim(0);
            break;

        case 8:
            if (instant) {
                this->PlayStandStateAnimKit(1, 115, 0xFFFFFFFF);
            } else {
                this->PlayStandStateAnimKit(2, 115, 114);
            }

            return;

        case 9:
            if (instant) {
                this->PlayStandStateAnimKit(3, 0xFFFFFFFF, 0xFFFFFFFF);
            } else {
                this->PlayStandStateAnimKit(4, 0xFFFFFFFF, 0xFFFFFFFF);
            }

            return;
    }

    if (this->m_standStateAnimKit) {
        this->m_standStateAnimKit->Stop(1, NULL);
    }
}

// OFFSET: 0x005FCFE0
void CGUnit_C::StopEmoteAnimKit() {
    if (this->m_emoteAnimKit) {
        this->m_emoteAnimKit->SetStopCallback(NULL, 0);
        this->m_emoteAnimKit->Stop(1, NULL);
        this->m_emoteAnimKit->Release();
        this->m_emoteAnimKit = NULL;
    }

    this->StopEmoteSpellEffects();
}

// PROVEN: adf55beb IDENTICAL/IDENTICAL
// OFFSET: 0x005FD030/0x140282920
void CGUnit_C::EmoteSpellVisualKitStopCallback(CAnimKitInst* inst, uint64_t param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->StopEmoteAnimKit();
    }
}

// OFFSET: 0x005FD3E0
bool ClientSetPlayHoverAnimHandler(ClientSetPlayHoverAnim* msg) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(msg->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        uint32_t state = msg->m_playHoverAnim ? (unit->m_animationState | 0x40000) : (unit->m_animationState & ~0x40000);

        if (unit->m_animationState != state) {
            unit->m_animationState = state;
            unit->UpdateMovementAnimKit(0);
        }
    }

    return true;
}

// PROVEN: 124b4646 IDENTICAL/UNPROVEN
// OFFSET: 0x005FD060/0x1402829A0
void CGUnit_C::SetAIAnimKitID(uint16_t id) {
    if (this->m_aiAnimKitID != id) {
        this->m_aiAnimKitID = id;
        this->UpdateAIAnimKit(0);
    }
}

// PROVEN: adc92318 DIFFERENT/IDENTICAL
// OFFSET: 0x005FD090/0x1402829C0
void CGUnit_C::UpdateHandItemAnims() {
    if (!this->m_characterComponent) {
        return;
    }

    uint32_t attachIDs[6];
    uint32_t count = this->m_characterComponent->GetHandItemAttachIDs(attachIDs, 6);

    for (uint32_t i = 0; i < count; i++) {
        CM2Model* attached = this->m_worldModel->m_attachList;
        uint32_t attachID = attachIDs[i];

        while (attached && attached->m_attachmentId != attachID) {
            attached = attached->m_attachNext;
        }

        if (attached) {
            this->m_animKitManager.UpdateHandItemAnim(attachID, attached, reinterpret_cast<void*>(&CGUnit_C::HandItemAnimCallback), this->m_obj->m_guid.m_guid, 0xFFFFFFFF);
        }
    }
}

// OFFSET: 0x005FD110/0x140282A70
void CGUnit_C::UpdateCharacterAnimReplacementSets() {
    uint32_t maxCount = this->GetAuraCount();
    int32_t* setIDs = static_cast<int32_t*>(alloca(4 * maxCount + 4));
    int32_t count = 0;
    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->GetLocalDisplayID());

    if (displayInfo && displayInfo->m_animReplacementSetID > 0) {
        count = 1;
        setIDs[0] = displayInfo->m_animReplacementSetID;
    }

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        AuraData* aura = this->GetAura(i);
        uint32_t effectCount;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(aura->m_spellId, effectCount);

        for (uint32_t j = 0; j < effectCount; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect && effect->m_effectAura == 312 && (aura->m_flags & (1 << j))) {
                setIDs[count++] = effect->m_effectMiscValue;
            }
        }
    }

    this->m_animKitManager.UpdateAnimReplacementSets(count, setIDs);
}

// OFFSET: 0x005FD270/0x140282C10
void CGUnit_C::UpdateMountAnimReplacementSets() {
    if (!this->m_mountAnimKitManager) {
        return;
    }

    uint32_t maxCount = this->GetAuraCount();
    int32_t* setIDs = static_cast<int32_t*>(alloca(4 * maxCount + 4));
    int32_t count = 0;
    CreatureDisplayInfoRec* displayInfo = g_creatureDisplayInfoDB.GetRecord(this->m_mountDisplayId);

    if (displayInfo && displayInfo->m_animReplacementSetID > 0) {
        count = 1;
        setIDs[0] = displayInfo->m_animReplacementSetID;
    }

    for (uint32_t i = 0; i < this->GetAuraCount(); i++) {
        AuraData* aura = this->GetAura(i);
        uint32_t effectCount;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(aura->m_spellId, effectCount);

        for (uint32_t j = 0; j < effectCount; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect && effect->m_effectAura == 313 && (aura->m_flags & (1 << j))) {
                setIDs[count++] = effect->m_effectMiscValue;
            }
        }
    }

    this->m_mountAnimKitManager->UpdateAnimReplacementSets(count, setIDs);
}

// PROVEN: 429d76c6 DIFFERENT/IDENTICAL
// OFFSET: 0x005FD440/0x140282E10
void CGUnit_C::PlayEmoteAnimKit(uint32_t anim, int32_t oneShot, int32_t allowMounted) {
    int32_t mountSelf = this->IsMountSelf(0);

    if (mountSelf && !allowMounted) {
        allowMounted = 1;
    }

    int32_t split = this->IsSplitbodyAnim(anim);

    if (mountSelf && !split && !(this->m_movement->m_flags2 & 0x800)) {
        return;
    }

    if (this->m_emoteAnimKit) {
        this->m_emoteAnimKit->SetStopCallback(NULL, 0);
        this->m_emoteAnimKit->Stop(1, NULL);
        this->m_emoteAnimKit->Release();
        this->m_emoteAnimKit = NULL;
    }

    this->StopEmoteSpellEffects();
    this->m_emoteAnimKit = this->m_animKitManager.PlayAndMaintain(s_emoteAnimKitIDs[4 * (allowMounted != 0) + 2 * (split != 0) + (oneShot != 0)], anim, NULL);

    if (this->m_emoteAnimKit) {
        this->m_emoteAnimKit->SetStopCallback(&CGUnit_C::EmoteAnimKitStopCallback, this->m_obj->m_guid.m_guid);
    }
}

// OFFSET: 0x005FD530/0x140282F40
void CGUnit_C::AnimKitMgrExpressionCallback(CAnimKitManager* manager) {
    this->UpdateObjectEffectAnimationStates();

    if (manager && manager == &this->m_animKitManager) {
        uint32_t boneDataID = 0xFFFFFFFF;
        uint32_t anim = this->m_animKitManager.GetExpressingHandItemAnim(boneDataID, 1, 1);
        this->UpdateHandItemAnimEffects(anim);
        this->UpdateRangedWeaponRotation(anim);
    }

    CVehicle_C* vehicle = this->m_vehicle;

    if (vehicle && vehicle->m_vehicleRec && vehicle->IsMirroringAnimsToPassengers()) {
        for (CPassenger* passenger = vehicle->m_passengers.Head(); passenger; passenger = vehicle->m_passengers.Next(passenger)) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims()) {
                unit->m_vehiclePassenger->UpdateVehicleAnimMirroring();
            }
        }
    }

    this->UpdateHandItemAnims();
}

// OFFSET: 0x005FD620/0x140283070
void CGUnit_C::UpdateEmoteAnimKit() {
    if (this->m_emoteAnimKit) {
        this->m_emoteAnimKit->SetStopCallback(NULL, 0);
        this->m_emoteAnimKit->Stop(1, NULL);
        this->m_emoteAnimKit->Release();
        this->m_emoteAnimKit = NULL;
    }

    this->StopEmoteSpellEffects();

    if (this->IsMountSelf(0)) {
        return;
    }

    uint32_t emoteState = this->m_unit->m_unitNpcEmotestate;

    if (!emoteState) {
        return;
    }

    EmotesRec* emote = g_emotesDB.GetRecord(emoteState);

    if (!emote) {
        return;
    }

    uint32_t emoteFlags = emote->m_emoteFlags;

    if ((emoteFlags & 0x10000) && this->m_mountModel) {
        return;
    }

    if (CGGameUI::m_interactTarget == this->m_obj->m_guid && (emoteFlags & 0x2000)) {
        return;
    }

    if ((emoteFlags & 0x80000) && !(this->m_worldModel->IsLoaded() && this->m_worldModel->HasSequence(emote->m_animID))) {
        if (emote->m_spellVisualKitID) {
            this->DestroyEmoteSpellEffects();
        }

        return;
    }

    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        int32_t blocked = 0;
        int32_t standBlocked = 0;
        int32_t inCombat = this->m_combat.m_attacking ? 1 : 0;

        if (!CGUnit::CanPlayEmote(emote->m_emoteFlags, 1, &blocked, inCombat, &standBlocked, (this->m_animationState & 0x10) != 0) || blocked || standBlocked) {
            return;
        }
    }

    if (emote->m_spellVisualKitID) {
        SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(emote->m_spellVisualKitID);

        if (kit && CanPlaySpellVisualKit(0, this->m_obj->m_guid.m_guid, kit, NULL)) {
            PlaySpellVisuaKitData data(NULL, kit, 2);
            data.m_flags = 0x80000000;
            data.m_stopCallback = &CGUnit_C::EmoteSpellVisualKitStopCallback;
            data.m_stopParam = this->m_obj->m_guid.m_guid;
            this->PlaySpellVisualKit(data);
        }
    } else {
        this->PlayEmoteAnimKit(emote->m_animID, emote->m_emoteSpecProc != 0, emote->m_emoteFlags & 0x20000);
    }
}

// PROVEN: 52e64244 IDENTICAL/IDENTICAL
// OFFSET: 0x005FD840/0x140283360
void CGUnit_C::AnimKitMgrExpressionCallback(CAnimKitManager* manager, uint64_t param) {
    CGUnit_C* unit = reinterpret_cast<CGUnit_C*>(static_cast<uintptr_t>(param));

    if (unit) {
        unit->AnimKitMgrExpressionCallback(manager);
    }
}

// OFFSET: 0x005FD860/0x140283380
void CGUnit_C::UpdateAnimKitsAfterMountModelChange(int32_t mounting) {
    if (this->m_mountModel) {
        if (!this->m_mountAnimKitManager) {
            if (this->m_movementAnimKit) {
                this->m_movementAnimKit->SetStopCallback(NULL, 0);
                this->m_movementAnimKit->Stop(1, NULL);
                this->m_movementAnimKit->Release();
                this->m_movementAnimKit = NULL;
            }

            this->m_mountAnimKitManager = CAnimKitManager::Alloc();
            this->m_mountAnimKitManager->SetGlueMode(1);
            this->m_mountAnimKitManager->SetExpressionCallback(&CGUnit_C::AnimKitMgrExpressionCallback, reinterpret_cast<uint64_t>(this));
        }

        this->m_mountAnimKitManager->SetModel(this->m_mountModel, 0);
        this->m_mountAnimKitManager->SetTier(this->m_animTier);
        this->UpdateAnimKitSplitBody();
        this->UpdateMountAnimReplacementSets();
        this->UpdateMovementAnimKit(0);

        if (this->m_mountedAnimKit) {
            this->m_mountedAnimKit->SetAnim(0, this->m_mountAnimBehaviorId, 255, 0, 0, 0);
        } else {
            this->m_mountedAnimKit = this->m_animKitManager.PlayAndMaintain(123, this->m_mountAnimBehaviorId, NULL);

            if (this->m_mountedAnimKit) {
                this->m_mountedAnimKit->SetStopCallback(&CGUnit_C::PointerClearAnimKitStopCallback, reinterpret_cast<uint64_t>(&this->m_mountedAnimKit));
            }
        }
    } else {
        if (mounting) {
            this->UpdateMovementAnimKit(0);
        }

        if (this->m_mountedAnimKit) {
            this->m_mountedAnimKit->Stop(1, NULL);
        }
    }

    if (this->m_vehiclePassenger && this->m_vehiclePassenger->IsMirroringVehicleAnims()) {
        this->m_vehiclePassenger->UpdateVehicleAnimMirroring();
    }
}

// OFFSET: 0x005FD9C0/0x140283520
void CGUnit_C::InitAnims() {
    this->m_animKitManager.SetExpressionCallback(&CGUnit_C::AnimKitMgrExpressionCallback, reinterpret_cast<uint64_t>(this));

    if (this->m_mountAnimKitManager) {
        this->m_mountAnimKitManager->SetExpressionCallback(&CGUnit_C::AnimKitMgrExpressionCallback, reinterpret_cast<uint64_t>(this));
    }

    this->m_animKitManager.ResetModel();

    if (this->m_mountAnimKitManager) {
        this->m_mountAnimKitManager->ResetModel();
    }

    int32_t mirroring = this->m_vehiclePassenger && this->m_vehiclePassenger->IsCurrentlyMirroringVehicleAnims();
    this->m_animKitManager.SetExpressionsBlocked(mirroring, 1);

    if (this->m_mountAnimKitManager) {
        this->m_mountAnimKitManager->SetExpressionsBlocked(mirroring, 1);
    }

    if (mirroring) {
        this->m_vehiclePassenger->UpdateVehicleAnimMirroring();
    }

    this->UpdateAnimKitMgrTier();
    this->UpdateCharacterAnimReplacementSets();
    this->UpdateMountAnimReplacementSets();
    this->UpdateMovementAnimKit(1);
    this->UpdateDeathAnimKit();
    this->UpdateCombatReadyAnimKit(1);
    this->UpdatePrecastAnimKit(1, WOWGUID());
    this->UpdateEffectAnimKit();

    switch (this->GetClientStandState()) {
        case 0:
            break;

        case 1:
            this->PlayStandStateAnimKit(1, 97, 0xFFFFFFFF);
            break;

        case 3:
            this->PlayStandStateAnimKit(1, 100, 0xFFFFFFFF);
            break;

        case 4:
            this->PlayStandStateAnimKit(1, 102, 0xFFFFFFFF);
            break;

        case 5:
            this->PlayStandStateAnimKit(1, 103, 0xFFFFFFFF);
            break;

        case 6:
            this->PlayStandStateAnimKit(1, 104, 0xFFFFFFFF);
            break;

        case 7:
            this->DisengageFromActivePlayer();
            this->PlayDeathAnim(0);

            if (this->m_standStateAnimKit) {
                this->m_standStateAnimKit->Stop(1, NULL);
            }

            break;

        case 8:
            this->PlayStandStateAnimKit(1, 115, 0xFFFFFFFF);
            break;

        case 9:
            this->PlayStandStateAnimKit(3, 0xFFFFFFFF, 0xFFFFFFFF);
            break;

        default:
            if (this->m_standStateAnimKit) {
                this->m_standStateAnimKit->Stop(1, NULL);
            }

            break;
    }

    this->UpdateLootAnimKit(1);
    this->UpdateEmoteAnimKit();
    this->UpdateAIAnimKit(1);
}
