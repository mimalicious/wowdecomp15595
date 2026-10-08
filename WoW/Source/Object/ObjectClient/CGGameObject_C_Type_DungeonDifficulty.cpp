#include "CGGameObject_C_Type_DungeonDifficulty.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Ui/GameUI.h"

// OFFSET: 0x0059AE40
CGGameObject_C_Type_DungeonDifficulty::CGGameObject_C_Type_DungeonDifficulty(CGGameObject_C* owner) : CGGameObject_C_TypeBase(owner) {
    this->m_sound = 0;
    this->m_disabled = 1;
}

// PROVEN: 0e1ccec7 IDENTICAL/NONE
// OFFSET: 0x0059C6D0/0x14020B140
float CGGameObject_C_Type_DungeonDifficulty::GetBaseAlpha() const {
    if (this->m_owner->m_gameObject->m_dynamicFlags & 2) {
        return 1.0f;
    }

    return 0.5f;
}

// OFFSET: 0x0059AE80
CGGameObject_C_Type_DungeonDifficulty::~CGGameObject_C_Type_DungeonDifficulty() {
    SE3::StopSound(this->m_sound, -1, __FILE__, __LINE__, "CGGameObject_C_Type_DungeonDifficulty::CloseLoopingSound");
}

// PROVEN: 678a7c58 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGGameObject_C_Type_DungeonDifficulty::CanUse() const {
    return false;
}

// OFFSET: 0x00599670
void CGGameObject_C_Type_DungeonDifficulty::ShouldRender(uint32_t flags, int32_t& render, int32_t& a3) {
    a3 = this->m_disabled;
}

// OFFSET: 0x0059C6F0/0x14020B170
void CGGameObject_C_Type_DungeonDifficulty::HandleAnimEvent(uint32_t eventId, uint32_t data, const C3Vector& position, uint32_t time) {
    int32_t render = 0;
    int32_t a3 = 0;
    this->ShouldRender(0, render, a3);

    if (!a3) {
        GameObject_C_ProcessAnimEvent(eventId, this->m_owner->m_gameObject->m_displayID, data, position, &this->m_sound);
    }
}

// OFFSET: 0x00599660
void CGGameObject_C_Type_DungeonDifficulty::PostInitWithStats() {
    this->UpdateDisabled();
}

// OFFSET: 0x005A1140
int32_t CGGameObject_C_Type_DungeonDifficulty::GetMapID() const {
    return this->m_owner->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 87));
}

// OFFSET: 0x005A1180
int32_t CGGameObject_C_Type_DungeonDifficulty::GetDifficulty() const {
    return this->m_owner->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 88));
}

// OFFSET: 0x005A11C0
void CGGameObject_C_Type_DungeonDifficulty::UpdateDisabled() {
    this->m_disabled = 1;

    MapRec* map = NULL;

    if (this->m_owner->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 87))) {
        map = g_mapDB.GetRecord(this->m_owner->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 87)));
    }

    if (!map || map->m_instanceType != 2) {
        if (CGGameUI::GetCurrentDungeonDifficulty() == this->m_owner->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 88))) {
            this->m_disabled = 0;
        }
    } else if (CGGameUI::GetCurrentRaidDifficulty() == this->m_owner->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 88))) {
        this->m_disabled = 0;
    } else if ((map->m_instanceType == 1 && (CGGameUI::GetCurrentRaidDifficulty(), CGGameUI::GetCurrentRaidDifficulty() <= 3) && GetMapRaidDuration(map->m_id, CGGameUI::GetCurrentRaidDifficulty())) || (map->m_instanceType == 2 && CGGameUI::GetCurrentRaidDifficulty() == CGGameUI::GetCurrentRaidDifficulty() && CGGameUI::GetCurrentRaidDifficulty() > 1)) {
        uint32_t difficulty;

        if (map->m_instanceType == 2 && CGGameUI::GetCurrentRaidDifficulty() == CGGameUI::GetCurrentRaidDifficulty() && CGGameUI::GetCurrentRaidDifficulty() > 1) {
            difficulty = CGGameUI::GetCurrentRaidDifficulty() - 2;
        } else {
            difficulty = CGGameUI::GetCurrentRaidDifficulty();
        }

        if (difficulty == static_cast<uint32_t>(this->GetDifficulty()) && !GetMapDifficultyRec(this->GetMapID(), CGGameUI::GetCurrentRaidDifficulty(), NULL)) {
            this->m_disabled = 0;
        }
    }

    if (this->m_disabled && SE3::IsPlaying(this->m_sound)) {
        SE3::StopSound(this->m_sound, -1, __FILE__, __LINE__, "CGGameObject_C_Type_DungeonDifficulty::UpdateDisabled");
    }
}
