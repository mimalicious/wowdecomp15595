#include "CSimpleModelFFX.h"
#include <cstdint>
#include <new>
#include <typeinfo>
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/SoundInterface/SI3.h"

// STATIC: 0x00EBC488/0x140DAD460
CDataAllocator CSimpleModelFFX::s_simpleModelFFXHeap(sizeof(CSimpleModelFFX), 5);

// PROVEN: 7c3b0154 IDENTICAL/IDENTICAL
// OFFSET: 0x00811030/0x140528800
static void AnimEventCallback(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position) {
    if (eventId == 0x4F534424) {
        SE3SoundKitProperties properties;
        SE3::PlaySoundKit(static_cast<int32_t>(data), position, &properties, NULL, __FILE__, __LINE__, "Simple Model Anim Event: $DSO");
    }
}

// PROVEN: 8ea00fdd IDENTICAL/IDENTICAL
// OFFSET: 0x008110A0/0x140528890
void CSimpleModelFFX::SetModel(CM2Model* model) {
    this->CSimpleModel::SetModel(model);
    this->m_model->SetEventCallback(reinterpret_cast<void*>(AnimEventCallback), reinterpret_cast<uintptr_t>(this));
}

// PROVEN: 87cf2636 IDENTICAL/DIFFERENT
// OFFSET: 0x00810FC0/0x140528780
void CSimpleModelFFX::LoadXML(linkObject* node, CStatus* status) {
    this->CSimpleModel::LoadXML(node, status);

    if (!(node->m_flags & 0x20000)) {
        return;
    }

    ModelFFX* model = static_cast<ModelFFX*>(node);

    if (model->m_ffxFlags & 0x1) {
        this->SetGlow(model->m_glow);
    }
}

// PROVEN: 5fbdd967 IDENTICAL/IDENTICAL
// OFFSET: 0x00811010/0x1405287D0
void CSimpleModelFFX::OnLayerShow() {
    FFX::SetEffect(1, CGlueMgr::m_glowEffect);
    this->CSimpleFrame::OnLayerShow();
}

// INLINED
CM2LightArray::CM2LightArray() {
    this->m_lightCount = 0;
    this->m_hasCustomLight = 0;
    this->unk_01B5 = 0;
    this->m_modelFrame = NULL;
    this->unk_01BC = 0;
}

// OFFSET: 0x008110D0/0x1405288C0
CSimpleModelFFX::CSimpleModelFFX(CSimpleFrame* parent) : CSimpleModel(parent) {
    this->m_glow = 0.3f;
    this->m_lightArray1[0].m_modelFrame = this;
    this->m_lightArray1[1].m_modelFrame = this;
    this->m_lightArray2[0].m_modelFrame = this;
    this->m_lightArray2[1].m_modelFrame = this;
    this->m_lightArray3[0].m_modelFrame = this;
    this->m_lightArray3[1].m_modelFrame = this;
}

// PROVEN: 42e8c4ae IDENTICAL/IDENTICAL
// OFFSET: 0x00811210/0x140528A50
void CSimpleModelFFX::SetGlow(float glow) {
    this->m_glow = glow;
}

// PROVEN: 180241f1 IDENTICAL/IDENTICAL
// OFFSET: 0x00811230/0x140528A60
int32_t CSimpleModelFFX::GetScriptMetaTable() {
    return CSimpleModelFFX::s_metatable;
}

// OFFSET: 0x00811250/0x140528A80
void CSimpleModelFFX::Render(void* arg) {
    CSimpleModelFFX* model = static_cast<CSimpleModelFFX*>(arg);

    CImVector clearColor(0x00, 0x00, 0x00, 0xFF);
    GxSceneClear(31, clearColor);

    int32_t params[3];
    params[0] = 0;
    params[1] = static_cast<uint8_t>(model->m_glow * 255.0f);
    params[2] = 0;
    CGlueMgr::m_glowEffect->SetParameters(3, params);

    FFX::Start();

    clearColor.b = 0x00;
    clearColor.g = 0x00;
    clearColor.r = 0x00;
    clearColor.a = 0xFF;
    GxSceneClear(31, clearColor);

    if (model->m_model) {
        CSimpleModel::RenderModel(model);
    }

    FFX::DoPass(0);
    FFX::DoPass(1);
    FFX::Finish();
}

// OFFSET: [0x00811320/0x140528B50, 0x008113C0/0x140528C30]
CSimpleModelFFX::~CSimpleModelFFX() {
}

// INLINED
void CSimpleModelFFX::operator delete(void* ptr) {
    CSimpleModelFFX::s_simpleModelFFXHeap.PutData(ptr, NULL, 0);
}

// PROVEN: 6def4ccc IDENTICAL/IDENTICAL
// OFFSET: 0x008113F0/0x140528C80
CSimpleFrame* CSimpleModelFFX::Create(CSimpleFrame* parent) {
    void* data = CSimpleModelFFX::s_simpleModelFFXHeap.GetData(0, typeid(CSimpleModelFFX).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleModelFFX(parent) : NULL;
}

// PROVEN: 80f379d5 IDENTICAL/UNPROVEN
// OFFSET: 0x00811430/0x140528CD0
void CSimpleModelFFX::OnFrameRender(CRenderBatch* batch, uint32_t layer) {
    this->CSimpleFrame::OnFrameRender(batch, layer);

    if (layer == 2) {
        batch->QueueCallback(CSimpleModelFFX::Render, this);
    }
}
