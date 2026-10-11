#ifndef ENGINE_SOURCE_GXU_GXUDEBUGDRAW_H
#define ENGINE_SOURCE_GXU_GXUDEBUGDRAW_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/GxuScene.h"
#include "Engine/Source/Storm/H/stpl.h"

class CGxString;
class CGxStringBatch;
class CImVector;

class GxuDebugText : public GxuDrawListItem {
    public:
    /* 0004 */ float m_x;
    /* 0008 */ float m_y;
    /* 000C */ const char* m_text;
    /* 0010 */ CGxStringBatch* m_batch;
    /* 0014 */ CGxString* m_string;

    virtual ~GxuDebugText();
    virtual void Draw(const C3Vector& viewPos);

    GxuDebugText();
    GxuDebugText(float x, float y, const char* text, const CImVector& color);
};

class GxuDebugTextObject : public GxuSceneObject {
    public:
    /* 0004 */ GxuDebugTextObject* m_next;
    /* 0008 */ GxuDebugText m_text;
    /* 0020 */ float m_time;

    virtual ~GxuDebugTextObject();
    virtual void Release();
    virtual bool IsAlive();
    virtual void Update(float elapsed);
    virtual void Render();

    GxuDebugTextObject();
    GxuDebugTextObject(float x, float y, const char* text, const CImVector& color, float time);
};

class GxuDebugText3DObject : public GxuSceneObject {
    public:
    /* 0004 */ GxuDebugText3DObject* m_next;
    /* 0008 */ uint8_t unk_0008[0x24];
};

class GxuDebugTextureObject : public GxuSceneObject {
    public:
    /* 0004 */ GxuDebugTextureObject* m_next;
    /* 0008 */ uint8_t unk_0008[0x44];
};

template <class T>
struct GxuDebugObjectBlock {
    /* 0000 */ TSGrowableArray<T> m_objects;
    /* 0010 */ GxuDebugObjectBlock<T>* m_next;
};

float GxuGetLODDistModifier();
int32_t GxuGetLODMode();
void GxuDebugDrawText(float x, float y, const char* text, const CImVector& color, float time);
void GxuShutdownDebugDraw();
void GxuShutdownDebugText3DDrawer();
void GxuShutdownDebugTextureDrawer();
void GxuShutdownDebugTextDrawer();

#endif
