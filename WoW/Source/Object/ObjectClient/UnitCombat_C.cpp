#include "UnitCombat_C.h"
#include "WoW/Source/UIUtil/CameraManager.h"
#include "Unit_C.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/Object/ObjectClient/UnitDisplayTransition_C.h"
#include <cmath>
#include <new>
#include <typeinfo>
#include <cstring>
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AnimationDataRec.h"
#include "WoW/Source/DB/Rec/AreaGroupRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/CreatureMovementInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureSoundDataRec.h"
#include "WoW/Source/DB/Rec/EmotesRec.h"
#include "WoW/Source/DB/Rec/FactionRec.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/DB/Rec/UnitBloodLevelsRec.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/Game/GameClient/CDrawBowString.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Magic/MagicClient/SpellVisuals.h"
#include "WoW/Source/Net/NetClient/NetClient.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/CMoveSpline.h"
#include "WoW/Source/Object/CreatureStats.h"
#include "WoW/Source/Object/Object.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Missile_C.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/ObjectClient/ObjectEffect.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/UnitMissileTrajectory_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehicleCamera_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/InstanceEncounter_C.h"
#include "WoW/Source/Ui/SpellActivationOverlay.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Ui/ChatBubbleFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/Ui/NamePlateFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WorldClient/AreaList.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Frame/CLayoutFrame.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "ThirdParty/zlib/zlib.h"
#include "WoW/Source/DB/DBClient/GameObjectStats_C.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoExtraRec.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"
#include "WoW/Source/DB/Rec/EnvironmentalDamageRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/DB/Rec/SkillRaceClassInfoRec.h"
#include "WoW/Source/DB/Rec/SpellDispelTypeRec.h"
#include "WoW/Source/DB/Rec/SpellMissileMotionRec.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"
#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"
#include "WoW/Source/DB/Rec/TerrainTypeRec.h"
#include "WoW/Source/DB/Rec/UnitBloodRec.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Game/GameClient/PetNameCache.h"
#include "WoW/Source/Game/GameClient/ProjectTex.h"
#include "WoW/Source/Game/GameClient/tables.h"
#include "WoW/Source/Game/GameClient/WTOBJECT.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/Game/ViolenceLevels.h"
#include "WoW/Source/Glue/ComponentData.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Magic/Spell.h"
#include "WoW/Source/Object/CMovementStatus.h"
#include "WoW/Source/Object/MovementShared.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/UnitSound_C.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/ObjectAlloc/ObjectAlloc.h"
#include "WoW/Source/Ui/BarberShop.h"
#include "WoW/Source/Ui/CGBankInfo.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/ClassTrainerFrame.h"
#include "WoW/Source/Ui/DuelInfo.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/GuildBankFrame.h"
#include "WoW/Source/Ui/MailInfo.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/QuestFrame.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Ui/RuneInfo.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/StableInfo.h"
#include "WoW/Source/Ui/TaxiMapFrame.h"
#include "WoW/Source/Ui/TotemInfo.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/UIUtil/SmartScreenRect.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00DD8674
static uint32_t s_nextVictimImpactTime;

// PROVEN: 99326035 IDENTICAL/NONE
// OFFSET: 0x005FF6D0
void CGUnit_C::ShowPlayerPVPHonorGained(WOWGUID victim, int32_t rank, int32_t honor) {
    CGChat::LogHonorGain(this, victim, rank, honor);
}

// PROVEN: d784e696 IDENTICAL/NONE
// OFFSET: 0x005FF6F0/0x1402837F0
const CGUnitVirtualItem* CGUnit_C::GetParryingItem(bool offhand) const {
    const CGUnitVirtualItem* item = this->GetVirtualItem(0, false);

    if (offhand || (item && item->m_classID != 2)) {
        item = this->GetVirtualItem(1, false);

        if (item && item->m_classID != 2 && item->m_classID != 4) {
            return NULL;
        }
    }

    return item;
}

// PROVEN: bc3775bf IDENTICAL/NONE
// OFFSET: 0x005FF740/0x140283850
int32_t CGUnit_C::GetUnitSize() const {
    int32_t size = this->m_displayInfo->m_sizeClass;

    if (size == -1) {
        return this->m_modelData->m_sizeClass;
    }

    return size;
}

// PROVEN: 8eae0b5a IDENTICAL/NONE
// OFFSET: 0x005FF780
void TargetAttacker(const WOWGUID& attacker) {
    if (!CGGameUI::m_lockedTarget.m_guid) {
        CGGameUI::Target(attacker);
    }
}

// OFFSET: 0x005FF8A0
void CGUnit_C::PlayAttackerRound(const ATTACKROUNDINFO& info) {
    if (this->m_unit->m_unitFieldHealth <= 0 || (info.m_hitInfo & 0x40000)) {
        return;
    }

    bool skipAnimation = info.m_meleeSpellId != 0;
    uint32_t anim = 16;

    if (info.m_hitInfo & 0x4) {
        const CGUnitVirtualItem* offhand = this->GetVirtualItem(1, false);

        if (offhand && offhand->m_classID == 2) {
            anim = (offhand->m_subclassID == 15) + 87;
        } else {
            anim = 117;
        }
    } else {
        const CGUnitVirtualItem* mainhand = this->GetVirtualItem(0, false);
        const CGUnitVirtualItem* offhand = this->GetVirtualItem(1, false);

        if (mainhand) {
            if (mainhand->m_classID == 2) {
                switch (mainhand->m_subclassID) {
                    case 0:
                    case 4:
                    case 7:
                    case 14:
                        anim = 17;
                        break;

                    case 1:
                    case 5:
                    case 8:
                        anim = 18 - (offhand != NULL);
                        break;

                    case 6:
                    case 10:
                    case 20:
                        anim = 19;
                        break;

                    case 13:
                        anim = !offhand || offhand->m_subclassID == 13 ? 508 : 17;
                        break;

                    case 15:
                        anim = 85;
                        break;

                    default:
                        break;
                }
            }
        } else if (offhand && offhand->m_classID == 2) {
            anim = 508;
        }
    }

    if (!skipAnimation) {
        this->PlayCombatActionAnimKit(1, anim, 0, 0);
    }

    if (info.m_victimState) {
        this->PlayUnitSound((info.m_hitInfo >> 9) & 0x1, 0, 0);
    }
}

// OFFSET: 0x005FF9F0
void CGUnit_C::ShowBloodSpurt(const WOWGUID& attacker, int32_t a3) {
    UnitBloodRec* blood = this->GetBloodRecord();

    if (!blood) {
        return;
    }

    int32_t attachType = 15;
    CGUnit_C* attackerUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(attacker, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (attackerUnit) {
        C3Vector position = this->GetPosition();
        C3Vector attackerPosition = attackerUnit->GetPosition();
        float dx = attackerPosition.x - position.x;
        float dy = attackerPosition.y - position.y;
        float facing = this->GetDisplayFacing();
        float cosFacing = cosf(facing);
        float sinFacing = sinf(facing);
        attachType = 16;

        if (dy * sinFacing + dx * cosFacing >= 0.0f) {
            attachType = 15;
        }
    }

    int32_t effectId = attachType == 15 ? blood->m_combatBloodSpurtFront[a3 != 0] : blood->m_combatBloodSpurtBack[a3 != 0];
    SpellVisualEffectNameRec* effectName = g_spellVisualEffectNameDB.GetRecord(effectId);

    if (!effectName) {
        return;
    }

    CEffect* effect = CEffect::Create();
    WOWGUID source = this->m_obj->m_guid;
    effect->AddEffect(attachType, 0, NULL, effectName, attacker, attacker, source, 0x20, CGObject_C::EffectOnceHandler, NULL, 0, NULL, -1, 0, 0);
    effect->Release();
}

// OFFSET: 0x005FFB60/0x140283D60
const CGUnitVirtualItem* CGUnit_C::GetAttackingWeapon(int32_t hand, bool requireLoadedModel) const {
    if (!this->m_sheatheState) {
        return NULL;
    }

    const CGUnitVirtualItem* item = this->GetVirtualItem(hand != 0, false);

    if (!item || item->m_classID != 2) {
        return NULL;
    }

    if (!requireLoadedModel) {
        return item;
    }

    CM2Model* model = this->m_worldModel;

    if (!model || !model->IsLoaded()) {
        return NULL;
    }

    CM2Model* attached = model->m_attachList;

    if (!attached) {
        return NULL;
    }

    while (attached->m_attachmentId != (hand != 0) + 1) {
        attached = attached->m_attachNext;

        if (!attached) {
            return NULL;
        }
    }

    if (attached->IsLoaded()) {
        return item;
    }

    return NULL;
}

// OFFSET: 0x005FFBF0
void UnitCombatClientShutdown() {
    ClientServices::ClearMessageHandler(SMSG_ATTACK_START);
    ClientServices::ClearMessageHandler(SMSG_ATTACK_STOP);
    ClientServices::ClearMessageHandler(SMSG_ATTACKER_STATE_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_ATTACKSWING_NOTINRANGE);
    ClientServices::ClearMessageHandler(SMSG_ATTACKSWING_BADFACING);
    ClientServices::ClearMessageHandler(SMSG_ATTACKSWING_DEADTARGET);
    ClientServices::ClearMessageHandler(SMSG_ATTACKSWING_CANT_ATTACK);
    ClientServices::ClearMessageHandler(SMSG_ENVIRONMENTAL_DAMAGE_LOG);
    ClientServices::ClearMessageHandler(SMSG_CLEAR_TARGET);
    ClientServices::ClearMessageHandler(SMSG_COMBAT_EVENT_FAILED);
    UnitCombatLogShutdown();
}

// OFFSET: 0x005FFCB0
void CGUnit_C::ShakeCameraOnHit() {
    if (this->m_unit->m_unitFieldPetnumber) {
        return;
    }

    int32_t shakeId = this->m_modelData->m_deathThudShakeSize;

    if (shakeId) {
        C3Vector position = this->GetPosition();
        StaticSingleton<CCameraManager>::Instance()->AddShakeById(shakeId, position);
    }
}

// OFFSET: 0x005FFD30
void ATTACKROUNDINFO::Unpack(CDataStore& msg) {
    msg.Get(this->m_hitInfo);
    msg >> *reinterpret_cast<SmartGUID*>(&this->m_attacker);
    msg >> *reinterpret_cast<SmartGUID*>(&this->m_target);
    msg.Get(reinterpret_cast<uint32_t&>(this->m_damage));
    msg.Get(reinterpret_cast<uint32_t&>(this->m_overkill));
    uint8_t count;
    msg.Get(count);

    for (uint8_t i = 0; i < count; i++) {
        msg.Get(this->m_schoolMask[i]);
        msg.Get(this->m_subDamage[i]);
        msg.Get(this->m_subDamageInt[i]);
    }

    if (this->m_hitInfo & 0x60) {
        for (uint8_t i = 0; i < count; i++) {
            msg.Get(this->m_absorbed[i]);
        }
    }

    if (this->m_hitInfo & 0x180) {
        for (uint8_t i = 0; i < count; i++) {
            msg.Get(this->m_resisted[i]);
        }
    }

    uint8_t victimState;
    msg.Get(victimState);
    this->m_victimState = victimState;
    msg.Get(this->unk_005C);
    msg.Get(this->m_meleeSpellId);

    if (this->m_hitInfo & 0x2000) {
        msg.Get(this->m_blocked);
    }

    if (this->m_hitInfo & 0x800000) {
        msg.Get(this->m_rageGained);
    }

    if (this->m_hitInfo & 0x1) {
        msg.Get(this->unk_006C);

        for (uint32_t i = 0; i < 8; i++) {
            msg.Get(this->unk_0070[i]);
        }

        for (uint32_t i = 0; i < 2; i++) {
            msg.Get(this->unk_0028[i]);
            msg.Get(this->unk_0030[i]);
        }

        msg.Get(this->unk_0090[0]);
    }

    if (ClntObjMgrObjectPtr(this->m_target, TYPEMASK_UNIT, __FILE__, __LINE__) && !this->m_damage && this->m_blocked) {
        this->m_victimState = 5;
    }
}

// OFFSET: 0x005FFF60
void CGUnit_C::PlayCombatSpellMissEffects(uint32_t reason, const WOWGUID& source) {
    if (this->IsMountSelf(0) || (this->m_unit->m_unitFieldFlags2 & 0x1) || this->GetClientStandState() == 7) {
        return;
    }

    CGUnit_C* attacker = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(source, TYPEMASK_UNIT, __FILE__, __LINE__));
    const CGUnitVirtualItem* weapon = NULL;

    if (attacker && attacker->m_sheatheState) {
        const CGUnitVirtualItem* item = attacker->GetVirtualItem(0, false);
        weapon = item && item->m_classID == 2 ? item : NULL;
    }

    WOWGUID mover = CGUnit_C::s_activeMover;
    bool canAnimate = !this->m_creatureCacheEntry || !(this->m_creatureCacheEntry->m_typeFlags[0] & 0x200000);

    switch (reason) {
        case 3:
            if (canAnimate) {
                this->PlayCombatActionAnimKit(3, 30, 0, 0);
            }

            break;

        case 4: {
            if (canAnimate) {
                this->PlayParryAnim();
            }

            C3Vector position = this->GetPosition();
            SI3::PlayParrySound(weapon, this->GetParryingItem(false), 0, position, mover == source);
            break;
        }

        case 5: {
            if (canAnimate) {
                this->PlayCombatActionAnimKit(2, 24, 0, 0);
            }

            C3Vector position = this->GetPosition();
            SI3::PlayParrySound(NULL, this->GetParryingItem(true), 0, position, mover == source);
            break;
        }

        default:
            break;
    }
}

// OFFSET: 0x00600100/0x140284440
void CGUnit_C::StopAttack(bool send) {
    if (send) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_ATTACK_STOP));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    this->m_stopAttackSent = 1;
}

// OFFSET: 0x00600180
void CGUnit_C::PlayVictimWorldFeedback(const ATTACKROUNDINFO& info) {
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

    if (info.m_meleeSpellId && info.m_victimState != 1) {
        return;
    }

    uint32_t victimState = info.m_victimState;

    if (victimState == 8) {
        WOWGUID source = info.m_attacker;
        this->AddWorldText(source, 9, NULL, false);
        CGGameUI::ShowCombatFeedbackAttackRoundInfo(&info);
        return;
    }

    if (victimState == 3) {
        WOWGUID source = info.m_attacker;
        this->AddWorldText(source, 4, NULL, false);
        CGGameUI::ShowCombatFeedbackAttackRoundInfo(&info);
        return;
    }

    if (victimState == 6) {
        WOWGUID source = info.m_attacker;
        this->AddWorldText(source, 6, NULL, false);
        CGGameUI::ShowCombatFeedbackAttackRoundInfo(&info);
        return;
    }

    if (victimState == 2) {
        if (info.m_attacker.m_guid) {
            CGUnit_C* attacker = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(info.m_attacker, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (attacker) {
                bool oneHand = !CGUnit_C::IsTwoHanded(attacker->GetVirtualItem(0, false));
                int32_t self = activePlayer == info.m_attacker || activePlayer == info.m_target;
                C3Vector position = attacker->GetPosition();
                SI3::PlayMissedSound(oneHand, position, self);
            }
        }

        WOWGUID source = info.m_attacker;
        this->AddWorldText(source, 3, NULL, false);
        CGGameUI::ShowCombatFeedbackAttackRoundInfo(&info);
        return;
    }

    if (victimState == 5) {
        WOWGUID source = info.m_attacker;
        this->AddWorldText(source, 5, NULL, false);
        CGGameUI::ShowCombatFeedbackAttackRoundInfo(&info);
        return;
    }

    if (victimState == 7) {
        WOWGUID source = info.m_attacker;
        this->AddWorldText(source, 7, NULL, false);
        CGGameUI::ShowCombatFeedbackAttackRoundInfo(&info);
        return;
    }

    if (info.m_damage || (info.m_hitInfo & 0x1000000)) {
        CGUnit_C* victim = NULL;
        CGUnit_C* attacker = NULL;

        if (info.m_attacker.m_guid) {
            attacker = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(info.m_attacker, TYPEMASK_UNIT, __FILE__, __LINE__));
        }

        if (info.m_target.m_guid) {
            victim = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(info.m_target, TYPEMASK_UNIT, __FILE__, __LINE__));
        }

        if (attacker && victim) {
            int32_t self = activePlayer == info.m_attacker || activePlayer == info.m_target;
            C3Vector position = victim->GetPosition();
            SI3::PlayHitSound(attacker->GetAttackingWeapon((info.m_hitInfo >> 2) & 1, true), victim->GetImpactType(), info.m_hitInfo & 0x200, position, self);
        }

        WOWGUID source = info.m_attacker;
        this->AddWorldDamageText(source, info.m_damage, 0, (info.m_hitInfo & 0x200) != 0, false, 0);
        CGGameUI::ShowCombatFeedbackAttackRoundInfo(&info);
        return;
    }

    if (activePlayer != info.m_target && !(info.m_hitInfo & 0x4000)) {
        int32_t reason;

        if (info.m_hitInfo & 0x20) {
            reason = 10;
        } else {
            reason = (info.m_hitInfo & 0x80) ? 2 : 1;
        }

        WOWGUID source = info.m_attacker;
        this->AddWorldText(source, reason, NULL, false);
    }

    if (info.m_attacker.m_guid) {
        CGUnit_C* attacker = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(info.m_attacker, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (attacker) {
            bool oneHand = !CGUnit_C::IsTwoHanded(attacker->GetVirtualItem(0, false));
            int32_t self = activePlayer == info.m_attacker || activePlayer == info.m_target;
            C3Vector position = attacker->GetPosition();
            SI3::PlayMissedSound(oneHand, position, self);
            CGGameUI::ShowCombatFeedbackAttackRoundInfo(&info);
            return;
        }
    }

    CGGameUI::ShowCombatFeedbackAttackRoundInfo(&info);
}

// OFFSET: 0x00600570/0x140284950
void CGUnit_C::PlayVictimRound(const ATTACKROUNDINFO& info) {
    if (this->m_unit->m_unitDynamicFlags & 0x1) {
        return;
    }

    this->PlayVictimWorldFeedback(info);

    if (info.m_hitInfo & 0x2) {
        this->PlayWoundAnimKit(info.m_hitInfo & 0x200);

        if ((info.m_damage || (info.m_hitInfo & 0x1000000)) && (info.m_victimState == 1 || info.m_victimState == 4)) {
            WOWGUID attacker = info.m_attacker;
            this->ShowBloodSpurt(attacker, info.m_hitInfo & 0x8000);
        }
    }

    if (this->DoNotPlayWoundAnim()) {
        uint32_t now = FrameTime::GetCurTimeMs();

        if (now <= s_nextVictimImpactTime) {
            return;
        }

        s_nextVictimImpactTime = static_cast<uint32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * 2000) >> 32) + now + 3000;
    }

    if (info.m_victimState == 3) {
        C3Vector position = this->GetPosition();
        this->SetImpactKitEffect(false, &info, &position);
    } else if (info.m_blocked) {
        C3Vector position = this->GetPosition();
        this->SetImpactKitEffect(true, &info, &position);
    } else if (info.m_victimState == 8) {
        C3Vector position = this->GetPosition();
        position.z += 2.0f;
        SE3SoundKitProperties properties;
        bool local = CGUnit_C::s_activeMover == this->m_obj->m_guid || this->IsActivePlayer();

        if (local) {
            properties.m_priority = 110;
        }

        SE3::PlaySoundKit("(DONOTRENAME)ShieldWoodImpact", local ? NULL : &position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::PlayVictimRound");
    }

    if ((info.m_hitInfo & 0xA0) || info.m_victimState == 7) {
        C3Vector position = this->GetPosition();
        position.z += 2.0f;
        SE3SoundKitProperties properties;
        bool local = CGUnit_C::s_activeMover == this->m_obj->m_guid || this->IsActivePlayer();

        if (local) {
            properties.m_priority = 110;
        }

        SE3::PlaySoundKit("(DONOTRENAME)AbsorbGetHit", local ? NULL : &position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::PlayVictimRound");
    } else if (info.m_hitInfo & 0x2) {
        if (info.m_hitInfo & 0x20000) {
            this->PlayUnitSound(9, 0, 0);
        } else if (info.m_hitInfo & 0x200) {
            this->PlayUnitSound(3, 0, 0);
        } else if (!(info.m_hitInfo & 0x10)) {
            this->PlayUnitSound(2, 0, 0);
        }
    }
}

// OFFSET: 0x00600870/0x14023F970
void CGUnit_C::PlayRangedItemPull() {
    CM2Model* ranged = this->m_rangedWeaponModel;

    if (!ranged || !ranged->IsLoaded() || !ranged->HasSequence(160)) {
        return;
    }

    if (!this->m_worldModel || !this->m_worldModel->IsLoaded()) {
        return;
    }

    M2BoneSequenceInfo info;
    info.m_sequenceId = 0xFFFFFFFF;

    if (this->m_torsoKeyBone == 4) {
        this->m_worldModel->GetBoneSequenceInfo(4, info);

        if (info.m_sequenceId == 0xFFFFFFFF) {
            this->m_worldModel->GetBoneSequenceInfo(0xFFFFFFFF, info);
        }
    } else {
        this->m_worldModel->GetBoneSequenceInfo(0xFFFFFFFF, info);
    }

    if (info.m_finished) {
        return;
    }

    int32_t elapsed = static_cast<int32_t>(static_cast<int64_t>(static_cast<double>(info.m_time) / info.m_speed));
    int32_t remaining = info.m_endTime - elapsed - info.m_startTime;

    if (remaining <= 0) {
        return;
    }

    M2SequenceInfo sequence;
    sequence.Clear();
    this->m_rangedWeaponModel->GetSequenceInfo(160, 0, sequence);
    this->m_rangedWeaponModel->SetBoneSequence(0xFFFFFFFF, 160, 0, 0, static_cast<float>(static_cast<int32_t>(sequence.m_duration)) / remaining, 1, 1, 0, 0, 0);
}

// OFFSET: 0x006009B0
void CGUnit_C::ClearAttackRound() {
    if (!this->m_lastAttackRound.m_attacker.m_guid || !this->m_lastAttackRound.m_target.m_guid) {
        return;
    }

    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_lastAttackRound.m_target, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (target) {
        target->PlayVictimWorldFeedback(this->m_lastAttackRound);
    }

    memset(&this->m_lastAttackRound, 0, sizeof(ATTACKROUNDINFO));
}

// OFFSET: 0x00600A10/0x140284F50
void CGUnit_C::HandleEnvironmentDamage(uint32_t type, int32_t damage, int32_t absorbed, int32_t resisted) {
    if (damage || absorbed || resisted) {
        if (!(this->m_obj->m_type & TYPEMASK_PLAYER) || (!(static_cast<CGPlayer_C*>(this)->m_player->m_playerFlags & 0x4000) && this->m_unit->m_unitFieldHealth > 0)) {
            this->PlayUnitSound(13, 1, type == 1);
        }

        WOWGUID guid = this->m_obj->m_guid;
        UnitCombatLog(guid, type, damage, absorbed, resisted);

        if (this->IsActivePlayer()) {
            uint32_t flags = absorbed ? 0x20 : 0;

            if (resisted) {
                flags |= 0x80;
            }

            WOWGUID target = this->m_obj->m_guid;
            CGGameUI::ShowCombatFeedback(target, damage, 1 << s_environmentalDamageSchools[type], flags);
        }
    }

    SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(g_environmentalDamageVisualKits[type]);

    if (kit) {
        PlaySpellVisuaKitData data(NULL, kit, 0);
        this->PlaySpellVisualKit(data);
    }
}

// OFFSET: 0x00600B40/0x140285150
void CGUnit_C::OnDeathAnimate() {
    if (!this->DoNotLogDeath()) {
        UnitCombatLogUnitDead(this);
    }

    this->PlayDeathAnim(0);

    if (!(this->m_obj->m_type & TYPEMASK_PLAYER) || !(static_cast<CGPlayer_C*>(this)->m_player->m_playerFlags & 0x4000)) {
        this->PlayUnitEventSound(4);
    }

    this->KillCreatureLoopSound();

    if (this->IsActivePlayer()) {
        CGGameUI::UpdateActivePlayer();
    }
}

// OFFSET: 0x00600BA0/0x1402851D0
void CGUnit_C::HandleCombatAnimEvent(uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time) {
    CGUnit_C* attacker = NULL;
    CGUnit_C* target = NULL;

    if (this->m_lastAttackRound.m_attacker) {
        attacker = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_lastAttackRound.m_attacker, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    if (this->m_lastAttackRound.m_target) {
        target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_lastAttackRound.m_target, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    switch (eventId) {
        case 0x30484124:
        case 0x31484124:
        case 0x32484124:
        case 0x33484124:
            if (this->m_lastAttackRound.m_damage && attacker && attacker->m_soundData) {
                SE3SoundKitProperties properties;
                properties.m_bus = static_cast<SE3BUS>(17);
                properties.m_conditionArg = this;
                properties.m_fpCondition = &CGUnit_C::IsOwnedByActiveMover;

                switch (eventId) {
                    case 0x30484124:
                        SE3::PlaySoundKit(attacker->m_soundData->m_customAttack[0], position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::HandleCombatAnimEvent", 0, NULL);
                        break;

                    case 0x31484124:
                        SE3::PlaySoundKit(attacker->m_soundData->m_customAttack[1], position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::HandleCombatAnimEvent", 0, NULL);
                        break;

                    case 0x32484124:
                        SE3::PlaySoundKit(attacker->m_soundData->m_customAttack[2], position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::HandleCombatAnimEvent", 0, NULL);
                        break;

                    case 0x33484124:
                        SE3::PlaySoundKit(attacker->m_soundData->m_customAttack[3], position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::HandleCombatAnimEvent", 0, NULL);
                        break;
                }

                this->m_lastAttackRound.m_hitInfo |= 0x200000;
            }

        case 0x48414324:
            if (attacker) {
                uint32_t victimState = attacker->m_lastAttackRound.m_victimState;

                if ((!victimState || victimState == 2 || victimState == 6) && attacker->m_combatActionAnimKit) {
                    attacker->m_combatActionAnimKit->SetSpeed(0, 2, 0.5f);
                }
            } else if (!target) {
                if (this->m_obj->m_type & TYPEMASK_PLAYER) {
                    static_cast<CGPlayer_C*>(this)->HandleSpellEventSound();
                } else {
                    CreatureSoundDataRec* soundData = this->GetSoundData(0);

                    if (soundData) {
                        SE3SoundKitProperties properties;
                        properties.m_conditionArg = this;
                        properties.m_fpCondition = &CGUnit_C::IsOwnedByActiveMover;
                        SE3::PlaySoundKit(soundData->m_soundExertionID, position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::HandleCombatAnimEvent", 0, NULL);
                    }
                }
            }

            if (target) {
                target->PlayVictimRound(this->m_lastAttackRound);
            }

            this->m_lastAttackRound.m_attacker = WOWGUID();
            this->m_lastAttackRound.m_target = WOWGUID();
            return;

        case 0x53534324: {
            if (!attacker) {
                return;
            }

            uint32_t victimState = this->m_lastAttackRound.m_victimState;

            if (!victimState || victimState == 2 || victimState == 6) {
                return;
            }

            int32_t swingType = 0;

            if (this->GetWeaponSwingType(!(this->m_lastAttackRound.m_hitInfo & 4), &swingType) && this->m_lastAttackRound.m_victimState) {
                int32_t self = CGUnit_C::s_activeMover == this->m_lastAttackRound.m_attacker || CGUnit_C::s_activeMover == this->m_lastAttackRound.m_target;
                SI3::PlayWeaponSwooshSound(swingType, this->m_lastAttackRound.m_hitInfo & 0x200, *position, this->m_lastAttackRound.m_hitInfo & 0x10, self);
            }

            return;
        }

        case 0x50574224: {
            if (this->IsActivePlayer()) {
                const SpellRec* spell = g_spellDB.GetRecord(this->m_castSpellId);

                if (!spell || (!(spell->m_attributes & 0x2) && !(spell->m_attributesExJ & 0x4))) {
                    spell = g_spellDB.GetRecord(Spell_C_GetAutoRepeatingSpell());

                    if (!spell || (!(spell->m_attributes & 0x2) && !(spell->m_attributesExJ & 0x4))) {
                        return;
                    }
                }
            }

            this->PlayRangedItemPull();
            const CGUnitVirtualItem* item = this->GetVirtualItem(2, false);

            if (!item || item->m_classID != 2 || (item->m_subclassID != 3 && item->m_subclassID != 18)) {
                ItemDisplayInfoRec displayInfo;
                SpellVisualRec* visual;

                if (g_itemDisplayInfoDB.GetRecord(this->GetVirtualItemDisplayID(2), &displayInfo) && displayInfo.m_spellVisualID && (visual = g_spellVisualDB.GetRecord(displayInfo.m_spellVisualID)) && visual->m_missileModel) {
                    this->UpdateAmmoModel(visual->m_missileModel);

                    if (this->m_ammoModel && !this->m_ammoModel->m_attachParent) {
                        this->m_ammoModel->AttachToParent(this->m_worldModel, 35, NULL, 0);
                    }
                } else if (this->m_ammoModel && this->m_ammoModel->m_attachParent) {
                    this->m_ammoModel->DetachFromParent();
                }
            }

            this->m_animationState |= 0x80;
            return;
        }

        case 0x48544424:
            this->PlayDeathThud();
            this->ShakeCameraOnHit();
            this->ShowLootEffect();
            return;

        case 0x50504324: {
            if (!target || (target->m_unit->m_unitFieldFlags2 & 1) || target->GetClientStandState() == 7) {
                return;
            }

            uint32_t victimState = this->m_lastAttackRound.m_victimState;
            bool canAnimate = !target->m_creatureCacheEntry || !(target->m_creatureCacheEntry->m_typeFlags[0] & 0x200000);

            if (victimState == 3) {
                if (canAnimate && !target->IsMountSelf(0)) {
                    target->PlayParryAnim();
                }

                this->m_lastAttackRound.m_hitInfo &= ~2u;
            } else if (victimState == 2 || victimState == 8) {
                if (canAnimate && !target->IsMountSelf(0)) {
                    target->PlayCombatActionAnimKit(3, 30, 0, 0);
                }

                this->m_lastAttackRound.m_hitInfo &= ~2u;
            } else if (this->m_lastAttackRound.m_blocked) {
                if (canAnimate && !target->IsMountSelf(0)) {
                    target->PlayCombatActionAnimKit(2, 24, 0, 0);
                }

                this->m_lastAttackRound.m_hitInfo &= ~2u;
            }

            return;
        }

        default:
            return;
    }
}

// OFFSET: 0x006011A0
void CGUnit_C::OnAttackStop(uint64_t target, int32_t faceTarget) {
    this->m_combat.m_attacking = WOWGUID::Make(0);
    this->m_combat.m_attackSent = 0;
    this->m_stopAttackSent = 0;
    this->UpdateCombatReadyAnimKit(0);
    this->ClearAttackRound();

    if (!target || !faceTarget) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(target), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        this->m_clientFlags |= 0x1;
        C3Vector targetPosition = unit->GetPosition();
        C3Vector position = this->GetPosition();
        this->m_scriptedFacing = CalculateFacingTo(&position, &targetPosition);
    }
}

// OFFSET: 0x00601240
int32_t Packet_Group_29(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    switch (msgId) {
        case SMSG_ATTACK_START: {
            WOWGUID attacker;
            WOWGUID victim;
            msg->Get(attacker.m_guid);
            msg->Get(victim.m_guid);
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(attacker, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!unit) {
                return 0;
            }

            unit->m_combat.m_attackSent = 0;
            unit->SetAttacking(victim);

            if (!unit->IsActivePlayer()) {
                return 1;
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_ENTER_COMBAT, NULL);
            CGGameUI::UpdateCombatMode();
            return 1;
        }

        case SMSG_ATTACK_STOP:
        case SMSG_COMBAT_EVENT_FAILED: {
            SmartGUID attacker;
            SmartGUID victim;
            *msg >> attacker;
            *msg >> victim;
            uint32_t faceTarget;
            msg->Get(faceTarget);
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(attacker, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!unit) {
                return 1;
            }

            unit->OnAttackStop(victim.m_guid, faceTarget);

            if (!unit->IsActivePlayer()) {
                return 1;
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_LEAVE_COMBAT, NULL);
            CGGameUI::UpdateCombatMode();
            return 1;
        }

        case SMSG_ATTACKSWING_NOTINRANGE:
            SetAttackSwingError(1);
            return 0;

        case SMSG_ATTACKSWING_BADFACING:
            SetAttackSwingError(2);
            return 0;

        case SMSG_ATTACKSWING_DEADTARGET:
        case SMSG_ATTACKSWING_CANT_ATTACK: {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player) {
                player->SetCombatMode();
            }

            return 0;
        }

        case SMSG_ATTACKER_STATE_UPDATE: {
            ATTACKROUNDINFO info;
            info.Unpack(*msg);
            CGUnit_C* attacker = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(info.m_attacker, TYPEMASK_UNIT, __FILE__, __LINE__));
            CGUnit_C* victim = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(info.m_target, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (attacker) {
                if (attacker->m_sheatheState != 1) {
                    attacker->SetSheatheState(1, 1, 0);
                }

                attacker->m_combat.m_attackSent = 0;
                attacker->ClearAttackRound();
                attacker->m_lastAttackRound = info;

                if (attacker->IsActivePlayer() && victim) {
                    SetAttackSwingError(0);
                }

                attacker->PlayAttackerRound(info);

                if (attacker->m_lastAttackRound.m_target == ClntObjMgrGetActivePlayer()) {
                    WOWGUID guid = attacker->m_obj->m_guid;
                    TargetAttacker(guid);
                }

                if (attacker->IsAutoTracking() && !attacker->IsActiveMoverTracking(4)) {
                    attacker->ClearTrackingTarget(true, true);
                }

                if (!(attacker->m_unit->m_unitFieldFlags2 & 0x2000000)) {
                    UnitCombatLog(info);
                    return 0;
                }
            } else {
                if (!victim) {
                    return 0;
                }

                victim->PlayVictimRound(info);
            }

            if (victim && (victim->m_unit->m_unitFieldFlags & 0x8)) {
                UnitCombatLog(info);
            }

            return 0;
        }

        case SMSG_ENVIRONMENTAL_DAMAGE_LOG: {
            uint64_t guid = 0;
            msg->Get(guid);
            uint8_t type = 0;
            msg->Get(type);
            uint32_t damage = 0;
            msg->Get(damage);
            uint32_t absorbed = 0;
            msg->Get(absorbed);
            uint32_t resisted = 0;
            msg->Get(resisted);
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(guid), TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                unit->HandleEnvironmentDamage(type, damage, absorbed, resisted);
            }

            return 1;
        }

        case SMSG_CLEAR_TARGET: {
            uint64_t guid;
            msg->Get(guid);
            CGGameUI::ClearTarget(WOWGUID::Make(guid), 1);
            return 0;
        }

        default:
            return 0;
    }
}

// OFFSET: 0x00601650/0x140285F90
void UnitCombatClientInitialize() {
    ClientServices::SetMessageHandler(SMSG_ATTACK_START, Packet_Group_29, NULL);
    ClientServices::SetMessageHandler(SMSG_ATTACK_STOP, Packet_Group_29, NULL);
    ClientServices::SetMessageHandler(SMSG_ATTACKER_STATE_UPDATE, Packet_Group_29, NULL);
    ClientServices::SetMessageHandler(SMSG_ATTACKSWING_NOTINRANGE, Packet_Group_29, NULL);
    ClientServices::SetMessageHandler(SMSG_ATTACKSWING_BADFACING, Packet_Group_29, NULL);
    ClientServices::SetMessageHandler(SMSG_ATTACKSWING_DEADTARGET, Packet_Group_29, NULL);
    ClientServices::SetMessageHandler(SMSG_ATTACKSWING_CANT_ATTACK, Packet_Group_29, NULL);
    ClientServices::SetMessageHandler(SMSG_ENVIRONMENTAL_DAMAGE_LOG, Packet_Group_29, NULL);
    ClientServices::SetMessageHandler(SMSG_CLEAR_TARGET, Packet_Group_29, NULL);
    ClientServices::SetMessageHandler(SMSG_COMBAT_EVENT_FAILED, Packet_Group_29, NULL);
    UnitCombatLogInitialize();
}
