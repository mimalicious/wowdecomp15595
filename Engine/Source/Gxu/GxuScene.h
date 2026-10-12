#ifndef ENGINE_SOURCE_GXU_GXUSCENE_H
#define ENGINE_SOURCE_GXU_GXUSCENE_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class GxuSceneObject {
    public:
    virtual ~GxuSceneObject();
    virtual void Release() = 0;
    virtual int32_t IsAlive();
    virtual void Update(float elapsed);
    virtual void Render();

    GxuSceneObject();
};

class GxuScene {
    public:
    void Render();
    /* 0004 */ TSGrowableArray<GxuSceneObject*> m_objects;
    /* 0014 */ uint32_t m_lastUpdateTime;

    virtual ~GxuScene();
    void AddObject(GxuSceneObject* object);
    void Clear();
    void Update(uint32_t time);
};

extern GxuScene g_scene;

#endif
