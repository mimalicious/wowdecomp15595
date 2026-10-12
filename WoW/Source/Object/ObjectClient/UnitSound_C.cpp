#include "UnitSound_C.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/UnitCombat_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00CA812C/0x140B47F58
static const uint32_t s_unitSoundChance[14] = { 70, 100, 60, 100, 100, 40, 100, 100, 100, 100, 100, 100, 100, 0 };

// STATIC: 0x00CA8188/0x140B47FC0
static float s_attachmentHeights[53] = { 1.0f, 1.0f, 1.0f, 1.5f, 1.5f, 1.8f, 1.8f, 0.5f, 0.5f, 1.0f, 1.0f, 2.0f, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 2.5f, 0.0f, 2.0f, 1.5f, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 3.0f, 1.5f, 1.5f, 1.0f, 1.0f, 1.5f, 1.0f, 1.0f, 2.5f, 1.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f };

// STATIC: 0x00BB6B9C/0x14099DFA0
const uint32_t CGUnit_C::s_impactTypes[4] = { 0, 8, 7, 9 };

// STATIC: 0x00DD86E0/0x140CBEEC0
static uint32_t s_lastStandSoundTime;

// STATIC: 0x00DD86E4/0x140CBEED0
static TSFixedArray<DEATTHUDSOUNDINFO> s_deathThudSounds[5];

// INLINED
inline DEATTHUDSOUNDINFO::DEATTHUDSOUNDINFO() : m_soundEntryId(0), m_soundEntryIdWater(0) {
}

// PROVEN: 7b24c443 IDENTICAL/DIFFERENT
// OFFSET: 0x0060BF90/0x140295690
int32_t GetSoundID(const CreatureSoundDataRec* soundData, uint32_t soundType) {
    if (!soundData) {
        return 0;
    }

    switch (soundType) {
        case 0:
            return soundData->m_soundExertionID;
        case 1:
            return soundData->m_soundExertionCriticalID;
        case 2:
        case 13:
            return soundData->m_soundInjuryID;
        case 3:
            return soundData->m_soundInjuryCriticalID;
        case 4:
            return soundData->m_soundStunID;
        case 5:
            return soundData->m_soundStandID;
        case 7:
            return soundData->m_soundWingFlapID;
        case 8:
            return soundData->m_soundAlertID;
        case 9:
            return soundData->m_soundInjuryCrushingBlowID;
        case 10:
            return soundData->m_soundWingGlideID;
        case 11:
            return soundData->m_soundJumpStartID;
        case 12:
            return soundData->m_soundJumpEndID;
        default:
            return 0;
    }
}

// PROVEN: 3183c718 IDENTICAL/DIFFERENT
// OFFSET: 0x0060C0E0/0x1402957C0
void CGUnit_C::KillCreatureLoopSound() {
    SE3::StopSound(this->m_loopSound, 500, __FILE__, __LINE__, "CGUnit_C::KillCreatureLoopSound");
}

// PROVEN: 05564641 IDENTICAL/IDENTICAL
// OFFSET: 0x0060C110/0x140295800
void CGUnit_C::RefreshSoundData() {
    this->m_soundData = NULL;

    if (this->m_displayInfo) {
        this->m_soundData = this->GetSoundDataForID(this->m_displayInfo->m_soundID);

        if (!this->m_soundData && this->m_modelData) {
            this->m_soundData = this->GetSoundDataForID(this->m_modelData->m_soundID);
        }
    }
}

// PROVEN: 357f082d IDENTICAL/DIFFERENT
// OFFSET: 0x0060C160/0x140283F20
bool CGUnit_C::GetWeaponSwingType(bool offhand, int32_t* swingType) const {
    if (!this->GetVirtualItemDisplayID(offhand == false)) {
        *swingType = 0;
        return true;
    }

    const CGUnitVirtualItem* item = this->GetVirtualItem(offhand == false, false);

    if (!item || item->m_classID != 2) {
        return false;
    }

    const ItemSubClassRec* subClass = SDBItemSubclassGetSubClassRec(2, item->m_subclassID);

    if (subClass) {
        *swingType = subClass->m_weaponSwingSize;
    }

    return subClass != NULL;
}

// OFFSET: 0x0060C1E0/0x1402958F0
uint32_t CGUnit_C::GetImpactType() const {
    if (this->m_soundData && static_cast<uint32_t>(this->m_soundData->m_creatureImpactType) < 4) {
        return s_impactTypes[this->m_soundData->m_creatureImpactType];
    }

    return 0;
}

// PROVEN: 68ddd779 IDENTICAL/IDENTICAL
// OFFSET: 0x0060C200/0x140295920
void CGUnit_C::SetFootstepSoundsSuppressed(int32_t suppressed) {
    if (suppressed) {
        this->unk_09FC |= 0x40;
    } else {
        this->unk_09FC &= ~0x40;
    }
}

// PROVEN: 63699949 IDENTICAL/IDENTICAL
// OFFSET: 0x0060C2D0/0x140295A30
void CGUnit_C::PlayFidgetSound(uint32_t index) {
    CreatureSoundDataRec* soundData = this->GetSoundData(0);

    if (!soundData || index >= 5) {
        return;
    }

    int32_t soundKitId = soundData->m_soundFidget[index];

    if (!soundKitId) {
        return;
    }

    C3Vector position = this->GetPosition();
    position.z = position.z + 2.0f;
    SE3SoundKitProperties properties;
    int32_t listenerAtCharacter = SI3::sm_pCVar_Sound_ListenerAtCharacter && SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue;
    int32_t local;

    if (CGUnit_C::s_activeMover == this->m_obj->m_guid || this->IsActivePlayer()) {
        local = 1;
        properties.m_priority = 110;
    } else {
        local = 0;
    }

    properties.m_conditionArg = this;
    properties.m_fpCondition = &CGUnit_C::IsOwnedByActiveMover;
    SE3::PlaySoundKit(soundKitId, local && listenerAtCharacter ? NULL : &position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::PlayFidgetSound");
}

// PROVEN: fa759911 IDENTICAL/IDENTICAL
// OFFSET: 0x0060C3F0/0x140295BA0
void CGUnit_C::PlayFoleySound() {
    if (SI3::sm_pCVar_Sound_EnableArmorFoleySoundForOthers && SI3::sm_pCVar_Sound_EnableArmorFoleySoundForOthers->m_intValue) {
        SI3::PlayFoleySound(this->m_modelData->m_foleyMaterialID, this->GetPosition(), 0, this);
    }
}

// OFFSET: 0x0060C430/0x140295BF0
void CGUnit_C::SetImpactKitEffect(bool offhand, const ATTACKROUNDINFO* info, const C3Vector* position) {
    CGUnit_C* attacker = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(info->m_attacker, TYPEMASK_UNIT, __FILE__, __LINE__));
    ClntObjMgrObjectPtr(info->m_target, TYPEMASK_UNIT, __FILE__, __LINE__);
    const CGUnitVirtualItem* weapon = attacker ? attacker->GetAttackingWeapon((info->m_hitInfo >> 2) & 1, false) : NULL;
    const CGUnitVirtualItem* parry = this->GetParryingItem(offhand);

    if (parry) {
        int32_t self = s_activeMover == info->m_attacker || s_activeMover == info->m_target;
        SI3::PlayParrySound(weapon, parry, info->m_hitInfo & 0x200, *position, self);
    }
}

// OFFSET: 0x0060C4F0/0x140295CF0
void CGUnit_C::PlayStandSound() {
    if (this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        this->PlayUnitSound(5, 0, 0);
    }
}

// OFFSET: 0x0060C520/0x140295D30
void CGUnit_C::PlayDeathThud() {
    if (this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_type & 0x10)) {
        return;
    }

    C3Vector position = this->GetPosition();
    uint32_t liquidType;
    float liquidLevel;
    int32_t liquidFlags;
    int32_t inLiquid = World::QueryObjectLiquid(this->m_worldObject, liquidType, liquidLevel, liquidFlags);

    if (inLiquid && liquidLevel - position.z > 2.0f) {
        return;
    }

    uint32_t size = this->GetUnitSize();

    if (size >= 5) {
        return;
    }

    TerrainTypeRec* terrainType = g_terrainTypeDB.GetRecord(this->m_terrainTypeId);

    if (!terrainType || static_cast<uint32_t>(terrainType->m_soundID) >= s_deathThudSounds[size].m_count) {
        return;
    }

    DEATTHUDSOUNDINFO* info = &s_deathThudSounds[size].m_data[terrainType->m_soundID];
    int32_t soundKitId = inLiquid ? info->m_soundEntryIdWater : info->m_soundEntryId;

    if (!soundKitId) {
        int32_t defaults[5] = { 907, 912, 917, 922, 927 };
        soundKitId = defaults[size];
        SysMsgPrintf(STATUS_WARNING, 2, "INVALID DEATH THUD SOUND: DEFAULTING TO ID %d", soundKitId);
    }

    SE3SoundKitProperties properties;
    SE3::PlaySoundKit(soundKitId, &position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::PlayDeathThud");
}

// OFFSET: 0x0060C670/0x140295F00
void CGUnit_C::PlaySplashSound(const C3Vector* position) {
    int32_t listenerAtCharacter = SI3::sm_pCVar_Sound_ListenerAtCharacter && SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue;
    int32_t local;
    int32_t atCharacter;

    if (CGUnit_C::s_activeMover == this->m_obj->m_guid) {
        local = 1;
        atCharacter = listenerAtCharacter ? 1 : 0;
    } else {
        local = 0;
        atCharacter = 0;
    }

    SE3SoundKitProperties properties;

    if (local) {
        properties.m_priority = 110;
    }

    properties.m_bus = static_cast<SE3BUS>(11);
    SE3::PlaySoundKit(this->m_splashSoundKitId, atCharacter ? NULL : position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::PlaySplashSound");
}

// OFFSET: 0x0060C740/0x140296020
void CGUnit_C::CheckLoopSound() {
    CreatureSoundDataRec* soundData = this->GetSoundData(0);
    CGUnitData* unit = this->m_unit;

    if (unit->m_unitFieldHealth <= 0 || (unit->m_unitFieldFlags2 & 0x1) || !soundData || !soundData->m_loopSoundID || SE3::IsPlaying(this->m_loopSound)) {
        SE3::StopSound(this->m_loopSound, 500, __FILE__, __LINE__, "CGUnit_C::KillCreatureLoopSound");
        return;
    }

    C3Vector position = this->GetPosition();

    if (SE3::IsPlaying(this->m_loopSound) || SE3::IsTooCloseToExistingSoundKit(soundData->m_loopSoundID, position)) {
        return;
    }

    SE3SoundKitProperties properties;
    properties.m_usage = 1;
    properties.m_maxAudibleBehavior = 1;
    properties.m_bit0 = 1;
    properties.m_checkRange = 0;
    properties.m_priority = 128;
    SE3::PlaySoundKit(soundData->m_loopSoundID, &position, &properties, &this->m_loopSound, __FILE__, __LINE__, "CGUnit_C::CheckLoopSound");
}

// OFFSET: 0x0060C870/0x1402961A0
int32_t CGUnit_C::GetNewNPCSoundCount(int32_t count, int32_t* soundKitId, int32_t* soundIndex) const {
    if (!this->m_displayInfo) {
        return -1;
    }

    NPCSoundsRec* sounds = g_npcSoundsDB.GetRecord(this->m_displayInfo->m_npcsoundID);

    if (!sounds || !sounds->m_soundID[0]) {
        return -1;
    }

    if (count < 5) {
        *soundKitId = sounds->m_soundID[0];
        *soundIndex = -1;
        return count + 1;
    }

    if (count - 5 < SE3::GetNumSoundsInSoundKit(sounds->m_soundID[2])) {
        *soundIndex = count - 5;
        *soundKitId = sounds->m_soundID[2];
        return count + 1;
    }

    *soundKitId = sounds->m_soundID[0];
    *soundIndex = -1;
    return 0;
}

// OFFSET: 0x0060C930/0x140296280
void CGUnit_C::PlayAISound(int32_t soundKitId, WOWGUID source) {
    if (soundKitId <= 0) {
        return;
    }

    int32_t listenerAtCharacter = SI3::sm_pCVar_Sound_ListenerAtCharacter && SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue;
    int32_t local = (CGUnit_C::s_activeMover == this->m_obj->m_guid || this->IsActivePlayer()) && listenerAtCharacter;
    uint32_t* handle = NULL;
    CGObject_C* object = ClntObjMgrObjectPtr(source, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
        handle = &static_cast<CGUnit_C*>(object)->m_npcInteractSound;
    }

    if (local) {
        SE3SoundKitProperties properties;
        properties.m_priority = 110;
        SE3::PlaySoundKit(soundKitId, NULL, &properties, handle, __FILE__, __LINE__, "CGUnit_C::PlayAISound");
    } else {
        C3Vector position = this->GetPosition();
        SE3SoundKitProperties properties;
        properties.m_highPriorityDownload = 1;
        SE3::PlaySoundKit(soundKitId, &position, &properties, handle, __FILE__, __LINE__, "CGUnit_C::PlayAISound");
    }
}

// PROVEN: bbf6e147 IDENTICAL/UNPROVEN
// OFFSET: 0x0060CAA0/0x140296440
void CGUnit_C::PlayEmoteStateSound(const C3Vector* position) {
    EmotesRec* emote = g_emotesDB.GetRecord(this->m_unit->m_unitNpcEmotestate);

    if (emote && emote->m_emoteSpecProc == 2) {
        SE3SoundKitProperties properties;
        properties.m_conditionArg = this;
        properties.m_fpCondition = &CGUnit_C::IsOwnedByActiveMover;
        SE3::PlaySoundKit(emote->m_eventSoundID, position, &properties, NULL, __FILE__, __LINE__, "CGUnit_C::PlayEmoteStateSound");
    }
}

// OFFSET: 0x0060CB50/0x140296540
int32_t CGUnit_C::GetUnitSoundKitIndex(uint32_t soundType) const {
    switch (soundType) {
        case 0:
        case 1:
            return this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[0] & 0x20) ? 18 : 12;

        case 2:
        case 3:
        case 9:
            return this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[0] & 0x20) ? 19 : 14;

        default:
            return 0;
    }
}

// OFFSET: 0x0060CC00/0x1402965D0
C3Vector CGUnit_C::GetAttachmentPosition(uint32_t attachId, const C3Vector* offset) {
    CM2Model* model = this->m_worldModel;

    if (model && model->IsLoaded() && model->HasAttachment(attachId)) {
        if (!offset || (offset->x == 0.0f && offset->y == 0.0f && offset->z == 0.0f)) {
            return model->GetAttachmentPosition(attachId);
        }

        C44Matrix transform = model->GetAttachmentWorldTransform(attachId);
        return *offset * transform;
    }

    C3Vector local(0.0f, 0.0f, 0.0f);

    if (offset) {
        local = *offset;
    }

    if (attachId <= 52) {
        local.z = s_attachmentHeights[attachId] + local.z;
    }

    if (!offset || (offset->x == 0.0f && offset->y == 0.0f && offset->z == 0.0f)) {
        C3Vector base = this->m_vehiclePassenger ? this->GetModelPosition() : this->GetPosition();
        return C3Vector(base.x + local.x, base.y + local.y, base.z + local.z);
    }

    C44Matrix matrix = this->m_vehiclePassenger ? this->GetModelMatrix() : this->GetMatrix();
    return local * matrix;
}

// OFFSET: 0x0060CEE0/0x140296850
void CGUnit_C::PlayEmoteSound(int32_t soundKitId, bool checkCVars, int32_t fileIndex, int32_t interactHandle) {
    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && this->m_unit->m_unitFieldHealth <= 0) {
        return;
    }

    if (checkCVars) {
        if (!SI3::sm_pCVar_Sound_EnableEmoteSounds || !SI3::sm_pCVar_Sound_EnableEmoteSounds->m_intValue) {
            return;
        }

        if (SI3::sm_pCVar_Sound_EnablePetSounds && !SI3::sm_pCVar_Sound_EnablePetSounds->m_intValue && this->m_unit->m_unitFieldPetnumber) {
            return;
        }
    }

    int32_t listenerAtCharacter = SI3::sm_pCVar_Sound_ListenerAtCharacter && SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue;
    int32_t local;
    int32_t atCharacter;

    if (CGUnit_C::s_activeMover == this->m_obj->m_guid || this->IsActivePlayer()) {
        local = 1;
        atCharacter = listenerAtCharacter ? 1 : 0;
    } else {
        local = 0;
        atCharacter = 0;
    }

    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && static_cast<CGPlayer_C*>(this)->m_dance) {
        return;
    }

    C3Vector position;
    CM2Model* model = this->m_worldModel;

    if (model && model->IsLoaded() && model->HasAttachment(17)) {
        position = model->GetAttachmentPosition(17);
    } else {
        C3Vector base = this->m_vehiclePassenger ? this->GetModelPosition() : this->GetPosition();
        position = C3Vector(base.x, base.y, base.z + s_attachmentHeights[17]);
    }

    SE3SoundKitProperties properties;
    properties.m_highPriorityDownload = 1;
    properties.m_nIndexOverride = fileIndex;
    properties.m_uniqueness = 2;
    properties.m_conditionArg = this;
    properties.m_fpCondition = &CGUnit_C::IsOwnedByActiveMover;

    if (local) {
        properties.m_priority = 110;
    }

    if (soundKitId != 6576 && this->m_unit->m_unitFieldBytes0[1] == 6) {
        properties.m_attachCustomDSPChain = 1;
        properties.m_fpCustomDSPChainCallback = SI3::DSP_AttachCustomDspChain;
        SI3::DK_GetPlayerFilterName(this, properties.m_sCustomDSPChainName, sizeof(properties.m_sCustomDSPChainName));
    }

    SE3::PlaySoundKit(soundKitId, atCharacter ? NULL : &position, &properties, interactHandle ? &this->m_npcInteractSound : &this->m_npcSound, __FILE__, __LINE__, "CGUnit_C::PlayEmoteSound");
}

// PROVEN: ba26896b IDENTICAL/UNPROVEN
// OFFSET: 0x0060D130/0x140296B60
void CGUnit_C::PlayTextEmoteSound(uint32_t emote, uint32_t emoteNum) {
    CVar* cvar = CVar::Lookup("Sound_EnableEmoteSounds");

    if (!cvar || !cvar->m_intValue) {
        return;
    }

    if (!this->IsActivePlayer()) {
        WOWGUID guid = this->m_obj->m_guid;

        if (!CGGameUI::IsInParty(guid)) {
            return;
        }
    }

    int32_t soundKitId = SI3::GetTextEmoteSoundID(emote, this->GetDisplayRace(), this->GetVoiceSex());

    if (this->IsShapeShifted()) {
        SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(this->GetShapeshiftForm());

        if (form && (form->m_flags & 0x1000)) {
            return;
        }
    }

    this->PlayEmoteSound(soundKitId, false, emoteNum, 1);
}

// PROVEN: 4f014aa2 IDENTICAL/IDENTICAL
// OFFSET: 0x0060D200/0x140296C60
void CGUnit_C::HandlePlayStandSound(uint32_t eventId, int32_t a3) {
    if (SI3::sm_pCVar_Sound_EnablePetSounds && !SI3::sm_pCVar_Sound_EnablePetSounds->m_intValue && this->m_unit->m_unitFieldPetnumber) {
        return;
    }

    switch (eventId) {
        case 0x31444624:
            this->PlayFidgetSound(0);
            break;
        case 0x32444624:
            this->PlayFidgetSound(1);
            break;
        case 0x33444624:
            this->PlayFidgetSound(2);
            break;
        case 0x34444624:
            this->PlayFidgetSound(3);
            break;
        case 0x35444624:
            this->PlayFidgetSound(4);
            break;
        case 0x36444624:
        case 0x37444624:
        case 0x38444624:
        case 0x39444624:
            this->GetSoundData(0);
            break;
        case 0x58444624:
            this->PlayStandSound();
            break;
    }
}

// OFFSET: 0x0060D2C0/0x140296D40
void CGUnit_C::HandleFootfallAnimEvent(const C3Vector& position) {
    if ((this->m_unit->m_unitFieldBytes1[2] & 0x2) || (this->m_movementData.m_flags & 0x10000000)) {
        return;
    }

    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(this)->m_player->m_playerFlags & 0x10)) {
        return;
    }

    CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

    if (passenger && passenger->m_state && passenger->m_seatRec && !(passenger->m_seatRec->m_flagsC & 0x1)) {
        return;
    }

    this->PlayFoleySound();
    CreatureSoundDataRec* soundData = this->GetSoundData(0);

    if (soundData && soundData->m_soundFootstepID && SI3::sm_pCVar_Sound_FootstepSounds && SI3::sm_pCVar_Sound_FootstepSounds->m_intValue) {
        int32_t large = this->GetUnitSize() >= 3;
        int32_t splashing = this->IsSplashing(position);
        SI3::PlayFootstepSound(soundData->m_soundFootstepID, &position, this->m_terrainTypeId, splashing, large, this);
    }
}

// PROVEN: d55e9337 IDENTICAL/IDENTICAL
// OFFSET: 0x0060C0B0/0x140295780
int32_t CheckUnitSoundTimer(uint32_t soundType) {
    if (soundType == 5) {
        uint32_t now = FrameTime::GetCurTimeMs();

        if (static_cast<int32_t>(now - s_lastStandSoundTime - 10000) < 0) {
            return 0;
        }

        s_lastStandSoundTime = now;
    }

    return 1;
}

// OFFSET: 0x0060D3A0/0x140296E60
void CGUnit_C::PlayUnitSound(uint32_t soundType, int32_t force, int32_t ignoreOverride) {
    if (!force && static_cast<uint32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * 101) >> 32) > s_unitSoundChance[soundType]) {
        return;
    }

    if (!CheckUnitSoundTimer(soundType)) {
        return;
    }

    int32_t soundKitId = GetSoundID(this->GetSoundData(ignoreOverride), soundType);

    if (!soundKitId) {
        return;
    }

    C3Vector position = this->GetPosition();
    SE3SoundKitProperties properties;
    const C3Vector* soundPosition;
    int32_t line;

    if (CGUnit_C::s_activeMover == this->m_obj->m_guid || this->IsActivePlayer()) {
        int32_t listenerAtCharacter = SI3::sm_pCVar_Sound_ListenerAtCharacter && SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue;
        properties.m_bus = static_cast<SE3BUS>(this->GetUnitSoundKitIndex(soundType));
        properties.m_priority = 110;
        soundPosition = listenerAtCharacter ? NULL : &position;
        line = 353;
    } else {
        if (soundType == 7) {
            properties.m_maxAudibleBehavior = 1;
            properties.m_priority = 128;
        }

        properties.m_bus = static_cast<SE3BUS>(this->GetUnitSoundKitIndex(soundType));
        soundPosition = &position;
        line = 371;
    }

    properties.m_conditionArg = this;
    properties.m_fpCondition = &CGUnit_C::IsOwnedByActiveMover;
    SE3::PlaySoundKit(soundKitId, soundPosition, &properties, NULL, __FILE__, line, "CGUnit_C::PlayUnitSound");
}

// OFFSET: 0x0060D570/0x1402970B0
void CGUnit_C::PlayUnitEventSound(int32_t reaction) {
    if (reaction == 4 && this->m_creatureCacheEntry && (this->m_creatureCacheEntry->m_typeFlags[0] & 0x1000000)) {
        return;
    }

    if (!this->m_soundData) {
        return;
    }

    if (reaction != 4 && SE3::IsPlaying(this->m_npcInteractSound) && reaction <= static_cast<int32_t>(this->m_reactionSoundPriority)) {
        return;
    }

    this->m_reactionSoundPriority = reaction;
    int32_t soundKitId;
    int32_t petMuted = SI3::sm_pCVar_Sound_EnablePetSounds && !SI3::sm_pCVar_Sound_EnablePetSounds->m_intValue && this->m_unit->m_unitFieldPetnumber;

    switch (reaction) {
        case 0:
            if (petMuted) {
                return;
            }

            soundKitId = this->m_soundData->m_soundAggroID;
            break;

        case 1:
            if (petMuted) {
                return;
            }

            soundKitId = this->m_soundData->m_soundPetOrderID;
            break;

        case 2:
            if (petMuted) {
                return;
            }

            soundKitId = this->m_soundData->m_soundPetAttackID;
            break;

        case 4:
            soundKitId = this->m_soundData->m_soundDeathID;
            break;

        default:
            return;
    }

    C3Vector position;
    CM2Model* model = this->m_worldModel;

    if (model && model->IsLoaded() && model->HasAttachment(17)) {
        position = model->GetAttachmentPosition(17);
    } else {
        C3Vector base = this->m_vehiclePassenger ? this->GetModelPosition() : this->GetPosition();
        position = C3Vector(base.x, base.y, base.z + s_attachmentHeights[17]);
    }

    int32_t listenerAtCharacter = SI3::sm_pCVar_Sound_ListenerAtCharacter && SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue;
    int32_t local = CGUnit_C::s_activeMover == this->m_obj->m_guid;
    int32_t atCharacter = local && listenerAtCharacter;
    SE3SoundKitProperties properties;

    if (local) {
        properties.m_priority = 110;
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (player && player->m_unit->m_unitFieldBytes0[1] == 6) {
            properties.m_attachCustomDSPChain = 1;
            properties.m_fpCustomDSPChainCallback = SI3::DSP_AttachCustomDspChain;
            SI3::DK_GetPlayerFilterName(player, properties.m_sCustomDSPChainName, sizeof(properties.m_sCustomDSPChainName));
        }
    }

    SE3::PlaySoundKit(soundKitId, atCharacter ? NULL : &position, &properties, &this->m_npcInteractSound, __FILE__, __LINE__, "CGUnit_C::PlayUnitEventSound");
}

// PROVEN: 6f4a82f6 IDENTICAL/IDENTICAL
// OFFSET: 0x0060D820/0x140297400
void CGUnit_C::UpdateSoundPosition() {
    C3Vector position = this->GetPosition();
    SE3::Set3DPosition(this->m_npcInteractSound, position);
    SE3::Set3DPosition(this->m_npcSound, position);
    SE3::Set3DPosition(this->m_loopSound, position);
    SE3::Set3DPosition(this->m_displayTransitionSound, position);
    CVehicle_C* vehicle = this->m_vehicle;

    if (vehicle && vehicle->m_vehicleRec) {
        vehicle->UpdatePassengerSoundPositions();
    }
}

// OFFSET: 0x0060D940/0x140297520
static void UnitSoundLoadDeathThuds() {
    int32_t maxSoundId = g_terrainTypeSoundsDB.m_maxID;

    for (int32_t i = 0; i < 5; i++) {
        s_deathThudSounds[i].SetCount(maxSoundId + 1);
    }

    for (int32_t i = g_deathThudLookupsDB.m_numRecords; i; ) {
        i--;
        DeathThudLookupsRec* rec = g_deathThudLookupsDB.GetRecordByIndex(i);

        if (static_cast<uint32_t>(rec->m_sizeClass) < 5 && rec->m_terrainTypeSoundID <= maxSoundId) {
            s_deathThudSounds[rec->m_sizeClass].m_data[rec->m_terrainTypeSoundID].m_soundEntryId = rec->m_soundEntryID;
            s_deathThudSounds[rec->m_sizeClass].m_data[rec->m_terrainTypeSoundID].m_soundEntryIdWater = rec->m_soundEntryIdwater;
        }
    }
}

// OFFSET: 0x0060D9F0
void UnitSoundShutdown() {
    for (int32_t i = 0; i < 5; i++) {
        s_deathThudSounds[i].Clear();
    }
}

// PROVEN: 40a3f34d IDENTICAL/NONE
// OFFSET: 0x0060DA30
void UnitSoundDestroy() {
    UnitSoundShutdown();
}

// PROVEN: 7217bd7f IDENTICAL/IDENTICAL
// OFFSET: 0x0060DA40/0x140297670
void UnitSoundInitialize() {
    UnitSoundLoadDeathThuds();
    s_lastStandSoundTime = FrameTime::GetCurTimeMs();
}
