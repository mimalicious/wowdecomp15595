#include "PlayerSound_C.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MaterialRec.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Source/Object/ObjectClient/UnitSound_C.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/SoundInterface/SI3.h"

// STATIC: 0x00CA80D0/0x140B47EF0
static const uint32_t s_playerSoundChance[14] = { 35, 100, 30, 100, 100, 40, 100, 100, 100, 100, 100, 100, 100, 0 };

// OFFSET: 0x0060AF00/0x1402941D0
void CGPlayer_C::PlayUnitSound(uint32_t soundType, int32_t force, int32_t ignoreOverride) {
    if (!force && static_cast<uint32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * 101) >> 32) > s_playerSoundChance[soundType]) {
        return;
    }

    if (!CheckUnitSoundTimer(soundType)) {
        return;
    }

    int32_t soundKitId = GetSoundID(this->GetSoundData(ignoreOverride), soundType);

    if (!soundKitId) {
        return;
    }

    int32_t bus;

    if ((soundType == 2 || soundType == 3 || soundType == 9) && this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        bus = 15;
    } else {
        bus = this->GetUnitSoundKitIndex(soundType);
    }

    C3Vector position = this->GetPosition();
    position.z += 2.0f;
    int32_t listenerAtCharacter = SI3::sm_pCVar_Sound_ListenerAtCharacter && SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue;

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        SE3SoundKitProperties properties;
        properties.m_bus = static_cast<SE3BUS>(bus);
        properties.m_priority = 110;

        if (soundType == 7) {
            properties.m_maxAudibleBehavior = 1;
        } else if (soundType != 5 && soundType != 6 && soundType != 10 && soundType != 11 && soundType != 12 && soundType != 13 && this->m_unit->m_unitFieldBytes0[1] == 6) {
            properties.m_attachCustomDSPChain = 1;
            properties.m_fpCustomDSPChainCallback = SI3::DSP_AttachCustomDspChain;
            SI3::DK_GetPlayerFilterName(this, properties.m_sCustomDSPChainName, sizeof(properties.m_sCustomDSPChainName));
        }

        SE3::PlaySoundKit(soundKitId, listenerAtCharacter ? NULL : &position, &properties, NULL, __FILE__, 129, "CGPlayer_C::PlayUnitSound", 0, NULL);
    } else {
        SE3SoundKitProperties properties;
        properties.m_bus = static_cast<SE3BUS>(bus);

        if (soundType != 5 && soundType != 6 && soundType != 7 && soundType != 10 && soundType != 11 && soundType != 12 && soundType != 13 && this->m_unit->m_unitFieldBytes0[1] == 6) {
            properties.m_attachCustomDSPChain = 1;
            properties.m_fpCustomDSPChainCallback = SI3::DSP_AttachCustomDspChain;
            SI3::DK_GetPlayerFilterName(this, properties.m_sCustomDSPChainName, sizeof(properties.m_sCustomDSPChainName));
        }

        SE3::PlaySoundKit(soundKitId, &position, &properties, NULL, __FILE__, 155, "CGPlayer_C::PlayUnitSound", 0, NULL);
    }
}

// OFFSET: 0x0060B190/0x1402944C0
void CGPlayer_C::PlayFoleySound() {
    if ((this->m_obj->m_guid == ClntObjMgrGetActivePlayer() && SI3::sm_pCVar_Sound_EnableArmorFoleySoundForSelf && SI3::sm_pCVar_Sound_EnableArmorFoleySoundForSelf->m_intValue)
        || (this->m_obj->m_guid != ClntObjMgrGetActivePlayer() && SI3::sm_pCVar_Sound_EnableArmorFoleySoundForOthers && SI3::sm_pCVar_Sound_EnableArmorFoleySoundForOthers->m_intValue)) {
        VisibleItem_C* chest = const_cast<VisibleItem_C*>(this->GetVisibleItem(4));

        if (chest) {
            int32_t self = this->m_obj->m_guid == ClntObjMgrGetActivePlayer();
            SI3::PlayFoleySound(chest->GetMaterial(), this->GetPosition(), self, this);
        }
    }
}

// PROVEN: 0c1e808a IDENTICAL/UNPROVEN
// OFFSET: 0x0060B240/0x140294590
uint32_t CGPlayer_C::GetImpactType() const {
    VisibleItem_C* chest = const_cast<VisibleItem_C*>(this->GetVisibleItem(4));

    if (!chest || chest->GetClassID() != 4) {
        return 0;
    }

    MaterialRec* rec = g_materialDB.GetRecord(chest->GetMaterial());

    if (!rec) {
        return 0;
    }

    int32_t flags = rec->m_flags;

    if (flags & 0x2) {
        return 2;
    }

    return static_cast<uint32_t>(static_cast<int8_t>(flags) & 0x4) >> 2;
}

// OFFSET: 0x0060B2A0/0x140294610
void CGPlayer_C::HandleSpellEventSound() {
    if (!this->m_castSpellId) {
        return;
    }

    const SpellRec* spell = g_spellDB.GetRecord(this->m_castSpellId);

    if (!spell) {
        SysMsgPrintf(STATUS_ERROR, 2, "NOSPELLIDFOUND|%d", this->m_castSpellId);
        return;
    }

    SpellVisualRec visualRec;
    const SpellVisualRec* visual = this->GetAppropriateSpellVisual(spell, visualRec, 0, 0);

    if (!visual) {
        SysMsgPrintf(STATUS_ERROR, 2, "SPELLVISUALIDNOTFOUND|%d", spell->m_spellVisualID[0]);
        return;
    }

    if (visual->m_animEventSoundID) {
        C3Vector position = this->GetPosition();
        position.z += 1.0f;
        SE3SoundKitProperties properties;
        properties.m_conditionArg = this;
        properties.m_fpCondition = &CGUnit_C::IsOwnedByActiveMover;
        SE3::PlaySoundKit(visual->m_animEventSoundID, &position, &properties, NULL, __FILE__, __LINE__, "CGPlayer_C::HandleSpellEventSound", 0, NULL);
    }
}
