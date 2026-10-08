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
#include "WoW/Source/SoundInterface/SI3.h"

// STATIC: #0x00B6B48C
CDataAllocator CSimpleModelFFX::s_simpleModelFFXHeap(sizeof(CSimpleModelFFX), 5);

// OFFSET: #0x004E5EA0
static void AnimEventCallback(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time, uint64_t param) {
    if (eventId == 0x4F534424) {
        SI3::PlaySoundKit(static_cast<int32_t>(data), NULL, NULL, NULL, 0, NULL, 1, 0);
    }
}

// OFFSET: #0x004E5ED0
void CSimpleModelFFX::SetModel(CM2Model* model) {
    this->CSimpleModel::SetModel(model);
    this->m_model->SetEventCallback(reinterpret_cast<void*>(AnimEventCallback), reinterpret_cast<uintptr_t>(this));
}

// OFFSET: #0x004E5F00
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

// OFFSET: #0x004E5F50
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

// OFFSET: #0x004E5F70
CSimpleModelFFX::CSimpleModelFFX(CSimpleFrame* parent) : CSimpleModel(parent) {
    this->m_glow = 0.3f;
    this->m_lightArray1[0].m_modelFrame = this;
    this->m_lightArray1[1].m_modelFrame = this;
    this->m_lightArray2[0].m_modelFrame = this;
    this->m_lightArray2[1].m_modelFrame = this;
    this->m_lightArray3[0].m_modelFrame = this;
    this->m_lightArray3[1].m_modelFrame = this;
}

// PROVEN: 42e8c4ae IDENTICAL/NONE
// OFFSET: 0x00811210
void CSimpleModelFFX::SetGlow(float glow) {
    this->m_glow = glow;
}

// PROVEN: 180241f1 IDENTICAL/NONE
// OFFSET: 0x00811230/0x140528A60
int32_t CSimpleModelFFX::GetScriptMetaTable() {
    return CSimpleModelFFX::s_metatable;
}

// OFFSET: #0x004E6190
void CSimpleModelFFX::Render(void* arg) {
    CSimpleModelFFX* model = static_cast<CSimpleModelFFX*>(arg);

    CImVector clearColor(0x00, 0x00, 0x00, 0xFF);
    GxSceneClear(3, clearColor);

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
    GxSceneClear(3, clearColor);

    if (model->m_model) {
        CSimpleModel::RenderModel(model);
    }

    FFX::DoPass(0);
    FFX::DoPass(1);
    FFX::Finish();
}

// OFFSET: [#0x004E6230, #0x004E62D0]
CSimpleModelFFX::~CSimpleModelFFX() {
}

// INLINED
void CSimpleModelFFX::operator delete(void* ptr) {
    CSimpleModelFFX::s_simpleModelFFXHeap.PutData(ptr, NULL, 0);
}

// OFFSET: #0x004E6300
CSimpleFrame* CSimpleModelFFX::Create(CSimpleFrame* parent) {
    void* data = CSimpleModelFFX::s_simpleModelFFXHeap.GetData(0, typeid(CSimpleModelFFX).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleModelFFX(parent) : NULL;
}

// OFFSET: #0x004E6330
void CSimpleModelFFX::OnFrameRender(CRenderBatch* batch, uint32_t layer) {
    this->CSimpleFrame::OnFrameRender(batch, layer);

    if (layer == 2) {
        batch->QueueCallback(CSimpleModelFFX::Render, this);
    }
}
