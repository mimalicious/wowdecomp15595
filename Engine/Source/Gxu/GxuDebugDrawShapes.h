#ifndef ENGINE_SOURCE_GXU_GXUDEBUGDRAWSHAPES_H
#define ENGINE_SOURCE_GXU_GXUDEBUGDRAWSHAPES_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/GxuScene.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CImVector.h"

class IGxuDebugShapeFactory {
    public:
    virtual ~IGxuDebugShapeFactory();
    virtual void SetLOD(float distance);
    virtual uint32_t CreateVertexBuffer() = 0;
    virtual uint32_t CreateIndexBuffer() = 0;
    virtual uint32_t GetNumVertices() = 0;
    virtual uint32_t GetIdxOffset();
    virtual uint32_t GetNumIndices() = 0;
    virtual float GetRadius() = 0;
};

class GxuNullShapeFactory : public IGxuDebugShapeFactory {
    public:
    virtual uint32_t CreateVertexBuffer();
    virtual uint32_t CreateIndexBuffer();
    virtual uint32_t GetNumVertices();
    virtual uint32_t GetNumIndices();
    virtual float GetRadius();
};

class GxuConeShapeFactory : public IGxuDebugShapeFactory {
    public:
    /* 0004 */ uint32_t m_vertexBuffer;
    /* 0008 */ uint32_t m_indexBuffer;

    virtual uint32_t CreateVertexBuffer();
    virtual uint32_t CreateIndexBuffer();
    virtual uint32_t GetNumVertices();
    virtual uint32_t GetNumIndices();
    virtual float GetRadius();

    GxuConeShapeFactory();
};

class GxuSphereShapeFactory : public IGxuDebugShapeFactory {
    public:
    /* 0004 */ uint32_t m_lod;
    /* 0008 */ uint32_t m_vertexBuffer;
    /* 000C */ uint32_t m_indexBuffer;
    /* 0010 */ uint32_t m_segments;
    /* 0014 */ uint32_t m_rows;
    /* 0018 */ uint32_t m_numVertices;
    /* 001C */ uint32_t m_trianglesPerRow;
    /* 0020 */ uint32_t m_quadRows;
    /* 0024 */ uint32_t m_idxOffset[3];
    /* 0030 */ uint32_t m_numIndices[3];
    /* 003C */ uint32_t m_totalIndices;

    virtual void SetLOD(float distance);
    virtual uint32_t CreateVertexBuffer();
    virtual uint32_t CreateIndexBuffer();
    virtual uint32_t GetNumVertices();
    virtual uint32_t GetIdxOffset();
    virtual uint32_t GetNumIndices();
    virtual float GetRadius();

    GxuSphereShapeFactory();
};

class GxuDirectionalSphereShapeFactory : public GxuSphereShapeFactory {
    public:
    virtual uint32_t CreateVertexBuffer();
    virtual float GetRadius();

    GxuDirectionalSphereShapeFactory();
};

class GxuBoxShapeFactory : public IGxuDebugShapeFactory {
    public:
    /* 0004 */ uint32_t m_vertexBuffer;
    /* 0008 */ uint32_t m_indexBuffer;

    virtual uint32_t CreateVertexBuffer();
    virtual uint32_t CreateIndexBuffer();
    virtual uint32_t GetNumVertices();
    virtual uint32_t GetNumIndices();
    virtual float GetRadius();

    GxuBoxShapeFactory();
};

class GxuTriangleShapeFactory : public IGxuDebugShapeFactory {
    public:
    /* 0004 */ uint32_t m_vertexBuffer;
    /* 0008 */ uint32_t m_indexBuffer;

    virtual uint32_t CreateVertexBuffer();
    virtual uint32_t CreateIndexBuffer();
    virtual uint32_t GetNumVertices();
    virtual uint32_t GetNumIndices();
    virtual float GetRadius();

    GxuTriangleShapeFactory();
};

class GxuRotationRingShapeFactory : public IGxuDebugShapeFactory {
    public:
    /* 0004 */ uint32_t m_vertexBuffer;
    /* 0008 */ uint32_t m_indexBuffer;

    virtual uint32_t CreateVertexBuffer();
    virtual uint32_t CreateIndexBuffer();
    virtual uint32_t GetNumVertices();
    virtual uint32_t GetNumIndices();
    virtual float GetRadius();

    GxuRotationRingShapeFactory();
};

class GxuDirectionalDiamondShapeFactory : public IGxuDebugShapeFactory {
    public:
    /* 0004 */ uint32_t m_vertexBuffer;
    /* 0008 */ uint32_t m_indexBuffer;

    virtual uint32_t CreateVertexBuffer();
    virtual uint32_t CreateIndexBuffer();
    virtual uint32_t GetNumVertices();
    virtual uint32_t GetNumIndices();
    virtual float GetRadius();

    GxuDirectionalDiamondShapeFactory();
};

class GxuDebugShapeRenderStateBlock : public GxuDrawListState {
    public:
    /* 0004 */ int32_t m_mode;
    /* 0008 */ int32_t m_polygonFill;

    virtual void Begin();
    virtual void End();

    GxuDebugShapeRenderStateBlock(int32_t mode);
};

class GxuDebugShapeRenderStateBlockNoDepth : public GxuDrawListState {
    public:
    virtual void Begin();
    virtual void End();
};

class GxuDebugShapeDrawer : public GxuDrawListItem {
    public:
    /* 0004 */ C44Matrix m_matrix;
    /* 0044 */ CImVector m_color;
    /* 0048 */ int32_t m_mode;
    /* 004C */ float m_scale;
    /* 0050 */ float m_radius;
    /* 0054 */ uint32_t m_vertexBuffer;
    /* 0058 */ uint32_t m_indexBuffer;
    /* 005C */ IGxuDebugShapeFactory* m_factory;

    virtual ~GxuDebugShapeDrawer();
    virtual float GetDepth(const C3Vector& viewPos);
    virtual GxuDrawListState* GetState();
    virtual uintptr_t GetVtxStream();
    virtual void Draw(const C3Vector& viewPos);

    GxuDebugShapeDrawer();
    GxuDebugShapeDrawer(const C44Matrix& matrix, const CImVector& color, int32_t mode, IGxuDebugShapeFactory* factory);
};

class GxuDebugShapeObject : public GxuSceneObject {
    public:
    /* 0004 */ GxuDebugShapeObject* m_next;
    /* 0008 */ GxuDebugShapeDrawer m_drawer;
    /* 0068 */ float m_time;

    virtual ~GxuDebugShapeObject();
    virtual void Release();
    virtual bool IsAlive();
    virtual void Update(float elapsed);
    virtual void Render();

    GxuDebugShapeObject();
    GxuDebugShapeObject(const C44Matrix& matrix, const CImVector& color, int32_t mode, IGxuDebugShapeFactory* factory, float time);
};

void GxuShutdownDebugShapeDrawer();
void GxuDebugDrawCone(const C44Matrix& matrix, const CImVector& color, int32_t mode, float time);
void GxuDebugDrawSphere(const C44Matrix& matrix, const CImVector& color, int32_t mode, float time);
void GxuDebugDrawDirectionalSphere(const C44Matrix& matrix, const CImVector& color, int32_t mode, float time);
void GxuDebugDrawBox(const C44Matrix& matrix, const CImVector& color, int32_t mode, float time);
void GxuDebugDrawTriangle(const C44Matrix& matrix, const CImVector& color, int32_t mode, float time);
void GxuDebugDrawRotationRing(const C44Matrix& matrix, const CImVector& color, int32_t mode, float time);

#endif
