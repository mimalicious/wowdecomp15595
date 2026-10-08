#include "CGGameObject_C_Type_DungeonDifficulty.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WorldClient/World.h"

// OFFSET: #0x00712820
CGGameObject_C_Type_DungeonDifficulty::CGGameObject_C_Type_DungeonDifficulty(CGGameObject_C* owner) : CGGameObject_C_TypeBase(owner) {
}

// PROVEN: 0e1ccec7 IDENTICAL/NONE
// OFFSET: 0x0059C6D0/0x14020B140
float CGGameObject_C_Type_DungeonDifficulty::GetBaseAlpha() const {
    if (this->m_owner->m_gameObject->m_dynamicFlags & 2) {
        return 1.0f;
    }

    return 0.5f;
}

// OFFSET: #0x00712870
CGGameObject_C_Type_DungeonDifficulty::~CGGameObject_C_Type_DungeonDifficulty() {
    SI3::StopSound(&this->m_sound, 0, -1.0f, 1);
}

// PROVEN: 678a7c58 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGGameObject_C_Type_DungeonDifficulty::CanUse() const {
    return false;
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

// OFFSET: #0x007109D0
int32_t CGGameObject_C_Type_DungeonDifficulty::GetMapID() const {
    return this->m_owner->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 87));
}

// OFFSET: #0x00710A10
int32_t CGGameObject_C_Type_DungeonDifficulty::GetDifficulty() const {
    return this->m_owner->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 88));
}

// OFFSET: #0x00710A50
void CGGameObject_C_Type_DungeonDifficulty::UpdateDisplay(uint32_t time) {
    int32_t hidden = 1;
    MapRec* map = NULL;

    if (this->m_owner->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 87))) {
        map = g_mapDB.GetRecord(this->m_owner->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 87)));
    }

    if (!map || map->m_instanceType != 2) {
        int32_t difficulty = this->m_owner->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 88));

        if (CGGameUI::GetCurrentDungeonDifficulty() == difficulty) {
            hidden = 0;
        }
    } else if (map->m_flags & 0x100) {
        int32_t difficulty = this->GetDifficulty();

        if (CGGameUI::GetEffectiveRaidDifficulty() == difficulty) {
            hidden = 0;
        }
    } else {
        int32_t difficulty = this->GetDifficulty();

        if (CGGameUI::GetCurrentRaidDifficulty() == difficulty) {
            hidden = 0;
        } else if (CGGameUI::GetCurrentRaidDifficulty() >= 2) {
            difficulty = this->GetDifficulty();

            if (CGGameUI::GetCurrentRaidDifficulty() - 2 == difficulty && !GetMapDifficultyEntryByMapIdAndDifficulty(this->GetMapID(), CGGameUI::GetCurrentRaidDifficulty(), NULL)) {
                hidden = 0;
            }
        }
    }

    World::ObjectSetHidden(this->m_owner->m_worldObject, hidden);

    if (hidden && SI3::IsPlaying(&this->m_sound)) {
        SI3::StopSound(&this->m_sound, 0, -1.0f, 1);
    }
}
