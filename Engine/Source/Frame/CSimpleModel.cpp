#include <cmath>
#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/Services/CameraInt.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: %0x00BB87EC
const char* CSimpleModel::s_objectTypeName = "Model";

// STATIC: 0x00DD3A44/0x140CB9788
int32_t CSimpleModel::s_objectType;

// STATIC: 0x00DD3A48
CDataAllocator CSimpleModel::s_allocator(sizeof(CSimpleModel), 3);

// PROVEN: f38dfe15 IDENTICAL/NONE
// OFFSET: 0x0051F050
static void AnimFinishedCallback(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param) {
    if (!interrupted) {
        CSimpleModel* simpleModel = reinterpret_cast<CSimpleModel*>(static_cast<uintptr_t>(param));

        if (simpleModel->HasScript(simpleModel->m_onAnimFinished)) {
            simpleModel->RunScript(simpleModel->m_onAnimFinished, 0, NULL);
        }
    }
}

// INLINED
void CSimpleModel::operator delete(void* ptr) {
    CSimpleModel::s_allocator.PutData(ptr, NULL, 0);
}

// PROVEN: d0b5eef9 IDENTICAL/UNPROVEN
// OFFSET: 0x0051EC90/0x14016BDA0
int32_t CSimpleModel::IsDrawable() {
    return this->m_model && this->m_model->IsDrawableRecursive();
}

// PROVEN: 1eb733db IDENTICAL/IDENTICAL
// OFFSET: 0x0051EFD0/0x14016C1C0
bool CSimpleModel::IsA(int32_t type) {
    return type == CSimpleModel::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: f621db32 IDENTICAL/DIFFERENT
// OFFSET: 0x0051F010/0x140528A70
bool CSimpleModel::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleModel::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// PROVEN: 3029ebd5 IDENTICAL/DIFFERENT
// OFFSET: 0x0051ECB0/0x140528780
const char* CSimpleModel::GetObjectTypeName() {
    return CSimpleModel::s_objectTypeName;
}

// PROVEN: 504a298c IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CSimpleModel::SetGlow(float glow) {
}

// PROVEN: ae2b9613 IDENTICAL/DIFFERENT
// OFFSET: 0x009FCE20/0x14090FD50
int32_t CSimpleModel::AdvanceTime() {
    return 1;
}

// OFFSET: [%0x0052EBA0, %0x0052EED0, %0x0052F170]
CSimpleModel::~CSimpleModel() {
    if (this->m_camera) {
        CameraRelease(this->m_camera);
    }

    if (this->m_model) {
        this->m_model->Release();
    }

    if (this->m_scene) {
        this->m_scene->Release();
    }
}

// PROVEN: 7069b78b IDENTICAL/IDENTICAL
// OFFSET: 0x0051ECC0/0x14016BDE0
int32_t CSimpleModel::GetScriptMetaTable() {
    return CSimpleModel::s_metatable;
}

// PROVEN: 8e977b6b IDENTICAL/UNPROVEN
// OFFSET: 0x0051ECD0/0x14016BDF0
FrameScript_Object::ScriptIx* CSimpleModel::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    FrameScript_Object::ScriptIx* script = CSimpleFrame::GetScriptByName(name, data);

    if (script) {
        return script;
    }

    if (!SStrCmp(name, "onupdatemodel", STORM_MAX_STR)) {
        return &this->m_onUpdateModel;
    }

    if (!SStrCmp(name, "onanimfinished", STORM_MAX_STR)) {
        return &this->m_onAnimFinished;
    }

    return NULL;
}

// PROVEN: 884591cb IDENTICAL/IDENTICAL
// OFFSET: 0x0051ED70/0x14016BE70
void CSimpleModel::GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse) {
    FrameScript_Object::GetScriptTime(L, this->m_onUpdateModel, time, count);
    FrameScript_Object::GetScriptTime(L, this->m_onAnimFinished, time, count);
    CSimpleFrame::GetScriptTime(L, time, count, recurse);
}

// PROVEN: a2860510 IDENTICAL/IDENTICAL
// OFFSET: 0x0051EDC0/0x14016BEF0
void CSimpleModel::ModelLoaded(CM2Model* model, void* arg) {
    static_cast<CSimpleModel*>(arg)->OnModelLoaded(model);
}

// OFFSET: %0x0052ED50
void CSimpleModel::SetCamera(CCamera* camera) {
    CCamera* newCamera = camera;

    if (camera) {
        newCamera = reinterpret_cast<CCamera*>(HandleDuplicate(reinterpret_cast<HOBJECT>(camera)));
    }

    if (this->m_camera) {
        CameraRelease(this->m_camera);
    }

    this->m_camera = newCamera;
    this->m_pendingCameraIndex = 0xFFFFFFFF;
    this->m_pendingCameraId = 0xFFFFFFFF;
}

// PROVEN: 62e96598 IDENTICAL/DIFFERENT
// OFFSET: 0x0051EE30/0x14016BF80
void CSimpleModel::SetCameraByID(uint32_t id) {
    if (!this->m_model || !this->m_model->IsLoaded()) {
        this->m_pendingCameraId = id;
        return;
    }

    if (id < 2) {
        this->SetCamera(this->m_model->GetCameraById(id));
        return;
    }

    if (this->m_camera) {
        CameraRelease(this->m_camera);
    }

    this->m_camera = NULL;
    this->m_pendingCameraIndex = 0xFFFFFFFF;
    this->m_pendingCameraId = 0xFFFFFFFF;
}

// PROVEN: 0533821c IDENTICAL/UNPROVEN
// OFFSET: 0x0051EEB0/0x14016C020
void CSimpleModel::SetLight(const CM2Light& light) {
    this->m_light = light;
}

// PROVEN: cdd2e0de IDENTICAL/DIFFERENT
// OFFSET: 0x0051EED0/0x14016C040
void CSimpleModel::SetSequence(uint32_t sequence) {
    if (this->m_model) {
        this->m_model->SetBoneSequence(0xFFFFFFFF, sequence, 0xFFFFFFFF, 0, 1.0f, 0, 1, 0, 0, 0);
    }
}

// PROVEN: 2d4c40d2 IDENTICAL/DIFFERENT
// OFFSET: 0x0051EF00/0x14016C0A0
int32_t CSimpleModel::SetSequenceTime(uint32_t sequence, int32_t time) {
    if (this->m_model) {
        this->m_model->SetBoneSequence(0xFFFFFFFF, sequence, 0xFFFFFFFF, time, 1.0f, 0, 1, 0, 0, 0);
    }

    return 1;
}

// PROVEN: f2dad3f3 IDENTICAL/IDENTICAL
// OFFSET: 0x0051EF40/0x14016C100
CM2Scene* CSimpleModel::GetScene() {
    if (!this->m_scene) {
        this->m_scene = M2CreateScene();
    }

    return this->m_scene;
}

// OFFSET: %0x0052F000
CSimpleModel::CSimpleModel(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_scene = NULL;
    this->m_model = NULL;
    this->m_camera = NULL;
    this->m_pendingCameraIndex = 0;
    this->m_pendingCameraId = 0;
    this->m_facing = 0.0f;
    this->m_flags = 0;
    this->m_modelScale = 1.0f;
    this->m_fogNear = 0.0f;
    this->m_fogFar = 1.0f;
    this->m_bounds.b = C3Vector();
    this->m_bounds.t = C3Vector();
    this->m_light.SetVisible(0);
    this->m_light.SetLightType(M2LIGHT_POINT);
    this->m_light.m_ambColor = C3Vector(1.0f, 1.0f, 1.0f);
    this->m_light.m_dirColor = C3Vector(1.0f, 1.0f, 1.0f);
    this->m_fogColor.b = 0xFF;
    this->m_fogColor.g = 0xFF;
    this->m_fogColor.r = 0xFF;
    this->m_fogColor.a = 0xFF;
}

// OFFSET: %0x0052F1A0
void CSimpleModel::LoadXML(linkObject* node, CStatus* status) {
    this->CSimpleFrame::LoadXML(node, status);

    if (!(node->m_flags & 0x100)) {
        return;
    }

    Model* model = static_cast<Model*>(node);

    if (model->m_modelFlags & 0x1) {
        this->SetModel(model->m_file.m_string);

        if (!this->m_model) {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "Frame %s: Bad model file: %s", name ? name : "<unnamed>", model->m_file.m_string);
        }
    }

    if (model->m_modelFlags & 0x2) {
        if (model->m_scale > 0.0f) {
            this->m_modelScale = model->m_scale;
        } else {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "Frame %s: Invalid model scale: %f", name ? name : "<unnamed>", model->m_scale);
        }
    }

    if (model->m_modelFlags & 0x4) {
        this->m_fogNear = model->m_fogNear < 0.0f ? 0.0f : model->m_fogNear;
    }

    if (model->m_modelFlags & 0x8) {
        this->m_fogFar = model->m_fogFar >= 0.0f ? model->m_fogFar : 0.0f;
    }

    linkObject* child = node->m_child;

    while (child && child->m_element != XML_FOGCOLOR) {
        child = child->m_next;
    }

    if (child) {
        CImVector color(0, 0, 0, 0);
        LoadXML_Color(child, color, status);
        this->m_fogColor = color;
        this->m_flags |= 0x1;
    }
}

// PROVEN: dfe86bf3 IDENTICAL/DIFFERENT
// OFFSET: 0x0051F3A0/0x14016C620
void CSimpleModel::SetModel(const char* fileName) {
    CM2Model* model = NULL;

    if (fileName && *fileName) {
        model = this->GetScene()->CreateModel(fileName, 1, 2);
    }

    this->SetModel(model);

    if (model) {
        model->Release();
    }
}

// PROVEN: c4181f50 IDENTICAL/DIFFERENT
// OFFSET: 0x0051F400/0x14016C6A0
void CSimpleModel::SetCameraByIndex(uint32_t index) {
    if (!this->m_model || !this->m_model->IsLoaded()) {
        this->m_pendingCameraIndex = index;
        return;
    }

    CM2Model* model = this->m_model;

    if (!(model->m_fFlags & 0x1)) {
        model->WaitForLoad(NULL);
    }

    if (index < model->m_shared->m_data->m_cameras.m_count) {
        this->SetCamera(static_cast<CCamera*>(this->m_model->GetCameraByIndex(index)));
        return;
    }

    if (this->m_camera) {
        CameraRelease(this->m_camera);
    }

    this->m_camera = NULL;
    this->m_pendingCameraIndex = 0xFFFFFFFF;
    this->m_pendingCameraId = 0xFFFFFFFF;
}

// OFFSET: %0x0052F410
float CSimpleModel::GetWidth() {
    float width = this->CLayoutFrame::GetWidth();

    if (width == 0.0f && this->m_model && this->m_model->IsLoaded()) {
        return this->m_bounds.t.x - this->m_bounds.b.x;
    }

    return width;
}

// OFFSET: %0x0052F470
float CSimpleModel::GetHeight() {
    float height = this->CLayoutFrame::GetHeight();

    if (height == 0.0f && this->m_model && this->m_model->IsLoaded()) {
        return this->m_bounds.t.y - this->m_bounds.b.y;
    }

    return height;
}

// PROVEN: f4cdf815 IDENTICAL/NONE
// OFFSET: 0x0051F570
void CSimpleModel::UpdateAlpha() {
    this->CSimpleFrame::UpdateAlpha();

    CM2Model* model = this->m_model;

    if (model) {
        model->m_fadeAlpha = static_cast<uint8_t>(this->m_alpha * this->m_parentAlpha / 255) * 0.0039215689f;
    }
}

// OFFSET: %0x0052F520
void CSimpleModel::UpdateModel() {
    if (this->HasScript(this->m_onUpdateModel)) {
        this->RunScript(this->m_onUpdateModel, 0, NULL);
    }

    float height = NDCToDDCHeight(1.0f) * 1.6666666f;
    float layoutScale = this->m_layoutScale;

    C3Vector position;
    position.x = layoutScale * this->m_position.x;
    position.y = this->m_position.y * layoutScale;
    position.z = layoutScale * this->m_position.z;

    this->m_model->SetWorldTransform(position, this->m_facing, this->m_modelScale * this->m_layoutScale * height);
}

// OFFSET: 0x0051F690
void CSimpleModel::RenderModel(void* arg) {
    CSimpleModel* simpleModel = static_cast<CSimpleModel*>(arg);

    if (!simpleModel->m_model) {
        return;
    }

    CRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (!simpleModel->GetRect(&rect)) {
        return;
    }

    CGxDevice* device = g_theGxDevicePtr;
    C44Matrix projection;
    C44Matrix view;
    projection = device->m_projection;
    view = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];

    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);

    CCamera* camera = simpleModel->m_camera;
    C3Vector cameraPos;
    C3Vector cameraTarget;

    if (camera) {
        CameraGetCoord(camera, 7, &cameraPos);
        CameraGetCoord(camera, 8, &cameraTarget);
        CameraSetupWorldProjection(camera, rect, 0);
    } else {
        C2Vector offset;
        offset.x = rect.minX;
        offset.y = rect.minY;
        CameraSetupScreenProjection(rect, offset, 0.0f, 1);
    }

    DDCToNDC(rect.minX, rect.minY, &rect.minX, &rect.minY);
    DDCToNDC(rect.maxX, rect.maxY, &rect.maxX, &rect.maxY);

    rect.minX = rect.minX >= 1.0f ? 1.0f : (rect.minX <= 0.0f ? 0.0f : rect.minX);
    rect.maxX = rect.maxX >= 1.0f ? 1.0f : (rect.maxX <= 0.0f ? 0.0f : rect.maxX);
    rect.minY = rect.minY >= 1.0f ? 1.0f : (rect.minY <= 0.0f ? 0.0f : rect.minY);
    rect.maxY = rect.maxY >= 1.0f ? 1.0f : (rect.maxY <= 0.0f ? 0.0f : rect.maxY);

    rect.minX *= maxX;
    rect.maxX *= maxX;
    rect.minY *= maxY;
    rect.maxY *= maxY;

    if (!GxXformIsValidViewport(rect.minX, rect.maxX, rect.minY, rect.maxY, 0.0f, 1.0f)) {
        g_theGxDevicePtr->XformSetProjection(projection);
        g_theGxDevicePtr->XformSetView(view);
        GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
        return;
    }

    GxXformSetViewport(rect.minX, rect.maxX, rect.minY, rect.maxY, 0.0f, 1.0f);

    CImVector clearColor(0x00, 0x00, 0x00, 0xFF);
    GxSceneClear(0x10, clearColor);

    CShaderEffect::UpdateProjMatrix();
    simpleModel->UpdateModel();

    CM2Scene* scene = simpleModel->GetScene();

    if (simpleModel->m_pendingCameraIndex == 0xFFFFFFFF) {
        scene->Animate(cameraPos);
        scene->BeginDraw();
        scene->Draw(M2PASS_0);
        scene->Draw(M2PASS_1);
        scene->Draw(M2PASS_2);
        scene->EndDraw();
    }

    g_theGxDevicePtr->XformSetProjection(projection);
    g_theGxDevicePtr->XformSetView(view);
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
    CShaderEffect::UpdateProjMatrix();
    scene->GarbageCollectModels();
}

// OFFSET: %0x0052FA90
void CSimpleModel::LightingCallback(CM2Model* model, CM2Lighting* lighting, void* arg) {
    CSimpleModel* simpleModel = static_cast<CSimpleModel*>(arg);

    if (simpleModel->m_flags & 0x1) {
        lighting->SetFog(C3Vector(simpleModel->m_fogColor), simpleModel->m_fogNear, simpleModel->m_fogFar);
    }

    if (simpleModel->m_light.m_visible) {
        lighting->AddLight(&simpleModel->m_light);
    }
}

// OFFSET: %0x0052FB00
void CSimpleModel::SetModel(CM2Model* model) {
    if (model) {
        model->m_refCount++;
    }

    if (this->m_model) {
        this->m_model->SetSequenceCallback(NULL, 0);
        this->m_model->m_lightingCallback = NULL;
        this->m_model->m_lightingArg = NULL;
        this->m_model->SetLoadedCallback(NULL, NULL);
        this->m_model->Release();
        this->GetScene()->GarbageCollectModels();
    }

    this->m_model = model;

    if (model) {
        model->m_fadeAlpha = static_cast<uint8_t>(this->m_alpha * this->m_parentAlpha / 255) * 0.0039215689f;
        this->m_model->SetSequenceCallback(reinterpret_cast<void*>(&AnimFinishedCallback), reinterpret_cast<uintptr_t>(this));
        this->m_model->m_lightingCallback = &CSimpleModel::LightingCallback;
        this->m_model->m_lightingArg = this;
        this->m_model->SetLoadedCallback(&CSimpleModel::ModelLoaded, this);
    }

    this->NotifyDrawLayerChanged(2);
}

// OFFSET: %0x0052FC00
void CSimpleModel::OnModelLoaded(CM2Model* model) {
    model->SetAnimating(1);
    model->m_scene->Animate(C3Vector());

    if (this->m_pendingCameraIndex != 0xFFFFFFFF) {
        this->SetCameraByIndex(this->m_pendingCameraIndex);
    } else if (this->m_pendingCameraId != 0xFFFFFFFF) {
        this->SetCameraByID(this->m_pendingCameraId);
    }

    this->m_bounds = model->GetBoundingBox();
    this->Resize(0);
}

// PROVEN: cc13c90d IDENTICAL/NONE
// OFFSET: 0x0051FDD0
void CSimpleModel::OnFrameRender(CRenderBatch* batch, uint32_t layer) {
    this->CSimpleFrame::OnFrameRender(batch, layer);

    if (this->m_model && layer == 2) {
        batch->QueueCallback(&CSimpleModel::RenderModel, this);
    }
}

// OFFSET: %0x0052FCF0
void CSimpleModel::OnLayerUpdate(float elapsedSec) {
    this->CSimpleFrame::OnLayerUpdate(elapsedSec);

    if (this->m_model && this->m_visible && this->m_pendingCameraIndex == 0xFFFFFFFF) {
        this->m_model->SetAnimating(1);

        CM2Model* model = this->m_model;

        if (model->m_attachParent) {
            model->m_fFlags |= 0x40;
        } else {
            model->m_fFlags |= 0x8;
        }

        if (model->m_attachParent) {
            model->m_fFlags |= 0x10000;
        } else {
            model->m_fFlags |= 0x8000;
        }
    }

    if (!this->m_scene) {
        this->m_scene = M2CreateScene();
    }

    this->m_scene->AdvanceTime(FloatToInt64(elapsedSec * 1000.0f));
}

// OFFSET: %0x0052FDA0
void CSimpleModel::ReplaceTexture(uint32_t type, const char* fileName) {
    if (this->m_model) {
        CStatus status;
        CTexture* texture = TextureCreate(fileName, CGxTexFlags(GxTex_LinearMipNearest, 0, 0, 0, 0, 1), &status, 2, NULL, 2);
        SysMsgAdd(&status);
        this->m_model->ReplaceTexture(type, texture);
        TextureRelease(texture);
    }
}

// PROVEN: bb4dc5e6 IDENTICAL/NONE
// OFFSET: 0x00538510
void CSimpleModel::SetPosition(const C3Vector& position) {
    this->m_position = position;
}
