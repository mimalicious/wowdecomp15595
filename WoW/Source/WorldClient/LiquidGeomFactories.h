#ifndef WOW_SOURCE_WORLDCLIENT_LIQUIDGEOMFACTORIES_H
#define WOW_SOURCE_WORLDCLIENT_LIQUIDGEOMFACTORIES_H

#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/LiquidSystem/Liquid.h"

class CChunkLiquid;
class CMapAreaLow;
class CMapChunkBuf;
class CMapObj;
class CMapObjGroup;

class CMeshGeomFactory : public Liquid::IGeomFactory {
    public:
    static CDataAllocator s_allocator;

    static CMeshGeomFactory* Create(CMapObj* mapObj, CMapObjGroup* mapObjGroup);
    static void Destroy();

    /* 0004 */ int32_t m_refCount;
    /* 0008 */ CMapObj* m_mapObj;
    /* 000C */ CMapObjGroup* m_mapObjGroup;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ CImVector m_diffuseColor;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ float m_texCoord1ValueX;

    CMeshGeomFactory();
    virtual ~CMeshGeomFactory();
    virtual void Release();
    virtual void GetBuffers(EGxVertexBufferFormat format, CGxBuf*& vertexBuf, CGxBuf*& indexBuf, CGxBatch& batch, int32_t a5);
    virtual void Purge();
    virtual float GetHeight();
    virtual bool SetTransform(const C44Matrix& transform);
    virtual bool GetWorldSpace();
    void SetDiffuseColor(const CImVector& color);
    void SetTexCoord1ValueX(float value);
};

class CChunkGeomFactory : public Liquid::IGeomFactory {
    public:
    static CDataAllocator s_allocator;

    static CChunkGeomFactory* Create(const TSGrowableArray<CChunkLiquid*>& liquids);
    static void Destroy();

    /* 0004 */ int32_t m_refCount;
    /* 0008 */ int32_t m_dirty;
    /* 000C */ TSGrowableArray<CChunkLiquid*> m_liquids;
    /* 001C */ CMapChunkBuf* m_buf;
    /* 0020 */ CGxBatch m_batch;
    /* 0030 */ EGxVertexBufferFormat m_format;
    /* 0034 */ C44Matrix m_transform;

    CChunkGeomFactory();
    virtual ~CChunkGeomFactory();
    virtual void Release();
    virtual void GetBuffers(EGxVertexBufferFormat format, CGxBuf*& vertexBuf, CGxBuf*& indexBuf, CGxBatch& batch, int32_t a5);
    virtual void Purge();
    virtual float GetHeight();
    virtual bool SetTransform(const C44Matrix& transform);
    virtual bool GetWorldSpace();
};

class CAreaGeomFactory : public Liquid::IGeomFactory {
    public:
    static CDataAllocator s_allocator;

    static CAreaGeomFactory* Create(const CMapAreaLow* area);
    static void Destroy();

    /* 0004 */ int32_t m_refCount;
    /* 0008 */ int32_t m_dirty;
    /* 000C */ const CMapAreaLow* m_area;
    /* 0010 */ CMapChunkBuf* m_buf;
    /* 0014 */ CGxBatch m_batch;
    /* 0024 */ EGxVertexBufferFormat m_format;
    /* 0028 */ C44Matrix m_transform;

    CAreaGeomFactory();
    virtual ~CAreaGeomFactory();
    virtual void Release();
    virtual void GetBuffers(EGxVertexBufferFormat format, CGxBuf*& vertexBuf, CGxBuf*& indexBuf, CGxBatch& batch, int32_t a5);
    virtual void Purge();
    virtual float GetHeight();
    virtual bool SetTransform(const C44Matrix& transform);
    virtual bool GetWorldSpace();
};

#endif
