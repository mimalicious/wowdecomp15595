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

// OFFSET: #0x007634F0
uint32_t CGPlayer_C::GetImpactType() const {
    const CGBag_C* bag = this->GetBag();
    WOWGUID guid = static_cast<uint32_t>(bag->m_numSlots) > 4 ? bag->m_slots[4] : WOWGUID();
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));
    const CGUnitVirtualItem* chest = item ? &item->m_virtualItem : NULL;

    if (!chest || chest->m_classID != 4) {
        return 0;
    }

    MaterialRec* rec = g_materialDB.GetRecord(chest->m_material);

    if (!rec) {
        return 0;
    }

    if (rec->m_flags & 0x2) {
        return 2;
    }

    return (static_cast<int8_t>(rec->m_flags) & 0x4) >> 2;
}

// OFFSET: 0x0060B2A0
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
