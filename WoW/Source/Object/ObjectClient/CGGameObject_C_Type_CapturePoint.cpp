#include "CGGameObject_C_Type_CapturePoint.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Object/GameObjectStats.h"
#include "WoW/Source/Ui/GameUI.h"

// OFFSET: 0x005A2B40/0x140212EC0
CGGameObject_C_Type_CapturePoint::CGGameObject_C_Type_CapturePoint(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
}

// OFFSET: 0x005A2BA0
bool CGGameObject_C_Type_CapturePoint::CanHighlight() const {
    uint32_t highlightIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 18);
    GameObjectStats* highlightStats = this->m_owner->m_stats;
    uint32_t highlight = (highlightStats && highlightIndex < 32) ? highlightStats->m_propValue[highlightIndex] : 0;

    return highlight != 0;
}

// PROVEN: a6e33d75 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGGameObject_C_Type_CapturePoint::CanUse() const {
    return false;
}

// OFFSET: 0x005A10B0
void CGGameObject_C_Type_CapturePoint::PostInitWithStats() {
    uint32_t a3Index = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 24);
    GameObjectStats* a3Stats = this->m_owner->m_stats;
    uint32_t a3 = (a3Stats && a3Index < 32) ? a3Stats->m_propValue[a3Index] : 0;
    uint32_t a2Index = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 67);
    GameObjectStats* a2Stats = this->m_owner->m_stats;
    uint32_t a2 = (a2Stats && a2Index < 32) ? a2Stats->m_propValue[a2Index] : 0;

    g_capturePointUIManager.AddCapturePoint(this->m_owner, a2, a3);
}

// OFFSET: 0x005A10B0
void CGGameObject_C_Type_CapturePoint::Reenable() {
    uint32_t a3Index = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 24);
    GameObjectStats* a3Stats = this->m_owner->m_stats;
    uint32_t a3 = (a3Stats && a3Index < 32) ? a3Stats->m_propValue[a3Index] : 0;
    uint32_t a2Index = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 67);
    GameObjectStats* a2Stats = this->m_owner->m_stats;
    uint32_t a2 = (a2Stats && a2Index < 32) ? a2Stats->m_propValue[a2Index] : 0;

    g_capturePointUIManager.AddCapturePoint(this->m_owner, a2, a3);
}

// OFFSET: 0x0059AE30
void CGGameObject_C_Type_CapturePoint::Disable() {
    g_capturePointUIManager.RemoveCapturePoint(this->m_owner);
}
