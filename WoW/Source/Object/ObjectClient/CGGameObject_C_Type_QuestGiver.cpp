#include "CGGameObject_C_Type_QuestGiver.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Object/GameObjectStats.h"
#include "Engine/Source/Model2/M2Model.h"

// OFFSET: 0x005A1E40/0x140211FB0
CGGameObject_C_Type_QuestGiver::CGGameObject_C_Type_QuestGiver(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
    this->m_interactDistance = 5.5555553f;
}

// OFFSET: 0x0059FD70/0x140207C90
int32_t CGGameObject_C_Type_QuestGiver::CanChangeCursor() const {
    int32_t cursor = this->m_owner->GetQuestGiverCursor(!this->CanUseNow(NULL, NULL, NULL));

    if (!cursor) {
        return CGGameObject_C_TypeBase::CanChangeCursor();
    }

    return cursor;
}

// OFFSET: 0x0059FEE0/0x14020F830
void CGGameObject_C_Type_QuestGiver::CloseInteraction() {
    uint32_t keepOpenIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 22);
    GameObjectStats* keepOpenStats = this->m_owner->m_stats;
    uint32_t keepOpen = (keepOpenStats && keepOpenIndex < 32) ? keepOpenStats->m_propValue[keepOpenIndex] : 0;

    if (keepOpen) {
        return;
    }

    if (this->m_animState == 2 || this->m_animState == 3) {
        this->UpdateAnimState(4);
    }
}

// OFFSET: 0x0059FDA0/0x14020F610
void CGGameObject_C_Type_QuestGiver::StartInteraction() {
    uint32_t index = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 22);
    GameObjectStats* stats = this->m_owner->m_stats;

    if (stats && index < 24 && stats->m_propValue[index]) {
        uint32_t animIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 22);
        GameObjectStats* animStats = this->m_owner->m_stats;
        this->m_owner->PlayCustomAnim(animStats && animIndex < 24 ? animStats->m_propValue[animIndex] - 1 : 0xFFFFFFFF);
        return;
    }

    CM2Model* model = this->m_owner->m_worldModel;

    if (!model || !model->IsLoaded()) {
        return;
    }

    uint32_t anim = CGGameObject_C_TypeAnimated::s_animSequenceIds[2];

    if (!model->HasSequence(anim)) {
        M2SequenceInfo info;
        info.Clear();
        model->GetSequenceInfo(anim, 0, info);
        anim = info.m_animId;
    }

    if (model->GetBoneSequenceId(0xFFFFFFFF) != anim) {
        this->UpdateAnimState(2);
    }
}
