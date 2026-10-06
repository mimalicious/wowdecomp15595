#ifndef ENGINE_SOURCE_FRAME_CSIMPLEMODEL_H
#define ENGINE_SOURCE_FRAME_CSIMPLEMODEL_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"

class CCamera;
class CM2Lighting;
class CM2Model;
class CM2Scene;
class CRenderBatch;
class CStatus;
class linkObject;

class CSimpleModel : public CSimpleFrame {
    public:
    static CDataAllocator s_allocator;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void LightingCallback(CM2Model* model, CM2Lighting* lighting, void* arg);
    static void ModelLoaded(CM2Model* model, void* arg);
    static void RegisterScriptMethods(lua_State* L);
    static void RenderModel(void* arg);
    static int32_t SetLightHelper(lua_State* L, int32_t index, CM2Light* light);

    /* 01F0 */ CM2Scene* m_scene;
    /* 01F4 */ CM2Model* m_model;
    /* 01F8 */ CCamera* m_camera;
    /* 01FC */ uint32_t m_pendingCameraIndex;
    /* 0200 */ uint32_t m_pendingCameraId;
    /* 0204 */ CM2Light m_light;
    /* 0270 */ C3Vector m_position;
    /* 027C */ float m_facing;
    /* 0280 */ float m_modelScale;
    /* 0284 */ uint32_t m_flags;
    /* 0288 */ CImVector m_fogColor;
    /* 028C */ float m_fogNear;
    /* 0290 */ float m_fogFar;
    /* 0294 */ CAaBox m_bounds;
    /* 02AC */ FrameScript_Object::ScriptIx m_onUpdateModel;
    /* 02AD */ FrameScript_Object::ScriptIx m_onAnimFinished;

    virtual ~CSimpleModel();
    virtual int32_t GetScriptMetaTable();
    virtual FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data);
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse);
    virtual void OnLayerUpdate(float elapsedSec);
    virtual void OnFrameRender(CRenderBatch* batch, uint32_t layer);
    virtual void UpdateAlpha();
    virtual void SetModel(const char* fileName);
    virtual void SetModel(CM2Model* model);
    virtual int32_t IsDrawable();
    virtual void SetGlow(float glow);
    virtual void UpdateModel();
    virtual void OnModelLoaded(CM2Model* model);
    virtual void LoadXML(linkObject* node, CStatus* status);
    virtual float GetWidth();
    virtual float GetHeight();

    CSimpleModel(CSimpleFrame* parent);
    int32_t AdvanceTime();
    CM2Scene* GetScene();
    void ReplaceTexture(uint32_t type, const char* fileName);
    void SetCamera(CCamera* camera);
    void SetPosition(const C3Vector& position);
    void SetCameraByID(uint32_t id);
    void SetCameraByIndex(uint32_t index);
    void SetLight(const CM2Light& light);
    void SetSequence(uint32_t sequence);
    int32_t SetSequenceTime(uint32_t sequence, int32_t time);
};

#include "Engine/Source/Frame/CSimpleModel.inl"

#endif
