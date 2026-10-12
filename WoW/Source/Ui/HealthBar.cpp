#include "HealthBar.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

CDataAllocator CGSimpleHealthBar::s_simpleHealthBarHeap(sizeof(CGSimpleHealthBar), 5);

// INLINED
void CGSimpleHealthBar::operator delete(void* ptr) {
    CGSimpleHealthBar::s_simpleHealthBarHeap.PutData(ptr, NULL, 0);
}

// OFFSET: 0x00A3B840/0x1407E88D0
static int32_t SimpleHealthUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        CGSimpleHealthBar* bar = static_cast<CGSimpleHealthBar*>(param);
        bar->SetMinMaxValues(0.0f, static_cast<float>(static_cast<int32_t>(unit->m_unit->m_unitFieldMaxhealth)));
        bar->SetValue(static_cast<float>(unit->m_unit->m_unitFieldHealth));
    }

    return 1;
}

// OFFSET: 0x00A3B7C0/0x1407E8840
CGSimpleHealthBar::CGSimpleHealthBar(CSimpleFrame* parent) : CSimpleStatusBar(parent) {
    this->m_guid.m_guid = 0;
    this->m_autoColor = 1;
}

// OFFSET: 0x00A3BA80/0x1407E8BC0
CGSimpleHealthBar::~CGSimpleHealthBar() {
    if (ClntObjMgrGetCurrent()) {
        this->RemoveMirrorHandlers();
    }

    this->m_guid.m_guid = 0;
}

// OFFSET: 0x00A3B800/0x1407E8890
void CGSimpleHealthBar::SetStatusBarColor(const CImVector& color) {
    if (this->m_barTexture) {
        this->m_barTexture->SetVertexColor(color);
    }

    this->m_autoColor = 0;
}

// OFFSET: 0x00A3B8B0/0x1407E8950
void CGSimpleHealthBar::SetValue(float value) {
    CSimpleStatusBar::SetValue(value);

    if (!this->m_autoColor) {
        return;
    }

    float percent = this->GetAnimValue();

    CImVector color(0, percent <= 0.2f ? 0 : 0xFF, percent <= 0.2f ? 0xFF : 0, 0xFF);
    this->SetStatusBarColor(color);
    this->m_autoColor = 1;
}

// OFFSET: 0x00A3B940/0x1407E89D0
void CGSimpleHealthBar::InstallMirrorHandlers() {
    if (!this->m_guid) {
        return;
    }

    ClntObjMgrSetObjMirrorHandler(this->m_guid, ID_UNIT, offsetof(CGUnitData, m_unitFieldHealth), sizeof(uint32_t), SimpleHealthUpdateHandler, this, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(this->m_guid, ID_UNIT, offsetof(CGUnitData, m_unitFieldMaxhealth), sizeof(uint32_t), SimpleHealthUpdateHandler, this, static_cast<HANDLER_PRIORITY>(0), 0);
}

// OFFSET: 0x00A3B9A0
void CGSimpleHealthBar::RemoveMirrorHandlers() {
    if (!this->m_guid) {
        return;
    }

    ClntObjMgrUnsetObjMirrorHandler(this->m_guid, ID_UNIT, offsetof(CGUnitData, m_unitFieldHealth), SimpleHealthUpdateHandler, this);
    ClntObjMgrUnsetObjMirrorHandler(this->m_guid, ID_UNIT, offsetof(CGUnitData, m_unitFieldMaxhealth), SimpleHealthUpdateHandler, this);
}

// OFFSET: 0x00A3B9F0/0x1407E8A60
void CGSimpleHealthBar::SetUnit(CGUnit_C* unit) {
    if (ClntObjMgrGetCurrent()) {
        this->RemoveMirrorHandlers();
    }

    if (!unit) {
        this->m_guid.m_guid = 0;
        return;
    }

    this->m_guid = unit->m_obj->m_guid;
    this->InstallMirrorHandlers();
    this->SetMinMaxValues(0.0f, static_cast<float>(static_cast<int32_t>(unit->m_unit->m_unitFieldMaxhealth)));
    this->SetValue(static_cast<float>(unit->m_unit->m_unitFieldHealth));
}
