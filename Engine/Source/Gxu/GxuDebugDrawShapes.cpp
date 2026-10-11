#include "Engine/Source/Gxu/GxuDebugDrawShapes.h"
#include <cmath>
#include <new>
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gxu/GxuBufferCache.h"
#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00CA87A0
static GxuNullShapeFactory s_nullShapeFactory;

// STATIC: 0x00CA87A4
static GxuConeShapeFactory s_coneShapeFactory;

// STATIC: 0x00CA87B0
static GxuBoxShapeFactory s_boxShapeFactory;

// STATIC: 0x00CA87BC
static GxuTriangleShapeFactory s_triangleShapeFactory;

// STATIC: 0x00CA87C8
static GxuRotationRingShapeFactory s_rotationRingShapeFactory;

// STATIC: 0x00CA87D4
static GxuDirectionalDiamondShapeFactory s_directionalDiamondShapeFactory;

// STATIC: 0x00CA87E0
static GxuDebugShapeRenderStateBlock s_renderStateBlock(0);

// STATIC: 0x00CA87EC
static GxuDebugShapeRenderStateBlock s_renderStateBlockFrontCull(1);

// STATIC: 0x00CA87F8
static GxuDebugShapeRenderStateBlockNoDepth s_renderStateBlockNoDepth;

// STATIC: 0x00CA87FC
static GxuDebugShapeRenderStateBlock s_renderStateBlockWireframe(3);

// STATIC: 0x00DD9EB0
static GxuDebugShapeObject* s_shapeObjectFreeList;

// STATIC: 0x00DD9EB4
static GxuDebugObjectBlock<GxuDebugShapeObject>* s_shapeObjectBlocks;

// STATIC: 0x00DD9EB8
static bool s_shapeDrawerInitialized;

// STATIC: 0x00DD9EBC
static GxuBufferCache s_bufferCache;

// STATIC: 0x00DD9ED0
static GxuSphereShapeFactory s_sphereShapeFactory;

// STATIC: 0x00DD9F10
static GxuDirectionalSphereShapeFactory s_directionalSphereShapeFactory;

// INLINED
GxuConeShapeFactory::GxuConeShapeFactory() : m_vertexBuffer(0), m_indexBuffer(0) {
}

// INLINED
GxuBoxShapeFactory::GxuBoxShapeFactory() : m_vertexBuffer(0), m_indexBuffer(0) {
}

// INLINED
GxuTriangleShapeFactory::GxuTriangleShapeFactory() : m_vertexBuffer(0), m_indexBuffer(0) {
}

// INLINED
GxuRotationRingShapeFactory::GxuRotationRingShapeFactory() : m_vertexBuffer(0), m_indexBuffer(0) {
}

// INLINED
GxuDirectionalDiamondShapeFactory::GxuDirectionalDiamondShapeFactory() : m_vertexBuffer(0), m_indexBuffer(0) {
}

// INLINED
GxuDebugShapeRenderStateBlock::GxuDebugShapeRenderStateBlock(int32_t mode) : m_mode(mode), m_polygonFill(0) {
}

// INLINED
GxuDebugShapeObject::GxuDebugShapeObject(const C44Matrix& matrix, const CImVector& color, int32_t mode, IGxuDebugShapeFactory* factory, float time) : m_next(NULL), m_drawer(matrix, color, mode, factory), m_time(time) {
}

// INLINED
GxuDebugShapeDrawer::GxuDebugShapeDrawer() {
    this->m_color.b = 0;
    this->m_color.g = 0;
    this->m_color.r = 0;
    this->m_color.a = 0;
    this->m_mode = 4;
    this->m_scale = 0.0f;
    this->m_radius = 0.0f;
    this->m_vertexBuffer = 0xFFFF;
    this->m_indexBuffer = 0xFFFF;
    this->m_factory = &s_nullShapeFactory;
}

// OFFSET: [0x00847950]
void IGxuDebugShapeFactory::SetLOD(float distance) {
}

// OFFSET: [0x00842700/0x1407FDD80]
uint32_t IGxuDebugShapeFactory::GetIdxOffset() {
    return 0;
}

// OFFSET: [0x00842700/0x1407FDD80]
uint32_t GxuNullShapeFactory::GetNumVertices() {
    return 0;
}

// OFFSET: [0x00842700/0x1407FDD80]
uint32_t GxuNullShapeFactory::GetNumIndices() {
    return 0;
}

// OFFSET: [0x0076CA20/0x140586E10]
float GxuNullShapeFactory::GetRadius() {
    return 0.0f;
}

// OFFSET: [0x00599E10/0x1402082B0]
float GxuConeShapeFactory::GetRadius() {
    return 1.0f;
}

// OFFSET: [0x00599E10/0x1402082B0]
float GxuSphereShapeFactory::GetRadius() {
    return 1.0f;
}

// OFFSET: [0x00599E10/0x1402082B0]
float GxuDirectionalDiamondShapeFactory::GetRadius() {
    return 1.0f;
}

// OFFSET: [0x00B41A50/0x14090FD50]
uint32_t GxuSphereShapeFactory::GetNumVertices() {
    return this->m_numVertices;
}

// OFFSET: [0x00B2CAC0]
uint32_t GxuRotationRingShapeFactory::GetNumVertices() {
    return 64;
}

// OFFSET: [0x007B0AA0]
uint32_t GxuDirectionalDiamondShapeFactory::GetNumIndices() {
    return 24;
}

// OFFSET: 0x00619410
uint32_t GxuConeShapeFactory::GetNumVertices() {
    return 14;
}

// OFFSET: 0x00619420/0x1402A6A40
uint32_t GxuConeShapeFactory::GetNumIndices() {
    return 72;
}

// OFFSET: 0x00619430
GxuSphereShapeFactory::GxuSphereShapeFactory() {
    this->m_lod = 0;
    this->m_vertexBuffer = 0xFFFF;
    this->m_indexBuffer = 0xFFFF;
    this->m_totalIndices = 0;
    this->m_trianglesPerRow = 64;
    this->m_rows = 33;
    this->m_numVertices = 1056;
    this->m_quadRows = 32;
    this->m_segments = 32;
    this->m_idxOffset[0] = 0;
    this->m_numIndices[0] = this->m_quadRows * this->m_trianglesPerRow * 3;
    this->m_totalIndices += this->m_numIndices[0];
    this->m_numIndices[1] = 3 * (this->m_trianglesPerRow >> 1) * (this->m_quadRows >> 1);
    this->m_idxOffset[1] = this->m_numIndices[0] + this->m_idxOffset[0];
    this->m_totalIndices += this->m_numIndices[1];
    this->m_idxOffset[2] = this->m_numIndices[1] + this->m_idxOffset[1];
    this->m_numIndices[2] = 3 * (this->m_trianglesPerRow >> 2) * (this->m_quadRows >> 2);
    this->m_totalIndices += this->m_numIndices[2];
}

// OFFSET: 0x006194D0/0x1402A6A70
void GxuSphereShapeFactory::SetLOD(float distance) {
    float lod = 1.0f;

    if (!(distance / (GxuGetLODDistModifier() * 100.0f) >= 1.0f)) {
        lod = distance / (GxuGetLODDistModifier() * 100.0f);
    }

    this->m_lod = static_cast<uint32_t>(static_cast<int64_t>(lod + lod));
}

// OFFSET: 0x00619540/0x1402A6A50
uint32_t GxuSphereShapeFactory::GetIdxOffset() {
    return this->m_idxOffset[this->m_lod];
}

// OFFSET: 0x00619550
GxuDirectionalSphereShapeFactory::GxuDirectionalSphereShapeFactory() {
}

// OFFSET: 0x006195F0/0x1402A6A60
uint32_t GxuSphereShapeFactory::GetNumIndices() {
    return this->m_numIndices[this->m_lod];
}

// OFFSET: 0x00619600
uint32_t GxuBoxShapeFactory::GetNumVertices() {
    return 8;
}

// OFFSET: 0x00619610
uint32_t GxuBoxShapeFactory::GetNumIndices() {
    return 36;
}

// OFFSET: [0x00619620]
float GxuBoxShapeFactory::GetRadius() {
    return 0.5f;
}

// OFFSET: [0x00619620]
float GxuTriangleShapeFactory::GetRadius() {
    return 0.5f;
}

// OFFSET: [0x00619630/0x1402A6B50]
uint32_t GxuTriangleShapeFactory::GetNumVertices() {
    return 6;
}

// OFFSET: [0x00619630/0x1402A6B50]
uint32_t GxuTriangleShapeFactory::GetNumIndices() {
    return 6;
}

// OFFSET: [0x00619630/0x1402A6B50]
uint32_t GxuDirectionalDiamondShapeFactory::GetNumVertices() {
    return 6;
}

// OFFSET: 0x00619640
uint32_t GxuRotationRingShapeFactory::GetNumIndices() {
    return 192;
}

// OFFSET: [0x00619650]
float GxuDirectionalSphereShapeFactory::GetRadius() {
    return 2.0f;
}

// OFFSET: [0x00619650]
float GxuRotationRingShapeFactory::GetRadius() {
    return 2.0f;
}

// OFFSET: 0x00619660
GxuDrawListState* GxuDebugShapeDrawer::GetState() {
    switch (this->m_mode) {
        case 1:
            return &s_renderStateBlockFrontCull;

        case 2:
            return &s_renderStateBlockNoDepth;

        case 3:
            return &s_renderStateBlockWireframe;

        default:
            return &s_renderStateBlock;
    }
}

// OFFSET: 0x006196A0
uint32_t GxuDebugShapeDrawer::GetVtxStream() {
    return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(s_bufferCache.GetBuffer(this->m_vertexBuffer)));
}

// OFFSET: 0x006196B0
void GxuDebugShapeObject::Release() {
    this->m_next = s_shapeObjectFreeList;
    s_shapeObjectFreeList = this;
}

// OFFSET: 0x006196C0
bool GxuDebugShapeObject::IsAlive() {
    return this->m_time >= 0.0f;
}

// OFFSET: 0x006196E0
void GxuDebugShapeObject::Update(float elapsed) {
    this->m_time = this->m_time - elapsed;
}

// OFFSET: 0x006196F0
void GxuDebugShapeObject::Render() {
    if (this->m_time == 0.0f) {
        this->m_time = this->m_time - 0.0010000000474974513;
    }

    GxuAddToDrawList(static_cast<EGxuDrawListCategory>(this->m_drawer.m_color.a != 0xFF), &this->m_drawer);
}

// OFFSET: [0x00619740]
uint32_t GxuNullShapeFactory::CreateVertexBuffer() {
    return 0xFFFF;
}

// OFFSET: [0x00619740]
uint32_t GxuNullShapeFactory::CreateIndexBuffer() {
    return 0xFFFF;
}

// OFFSET: 0x00619750/0x1402A6C60
uint32_t GxuConeShapeFactory::CreateVertexBuffer() {
    if (!s_bufferCache.Exists(this->m_vertexBuffer)) {
        this->m_vertexBuffer = s_bufferCache.CreateBuffer(this->m_vertexBuffer, GxPoolTarget_Vertex, sizeof(C3Vector), 14);
        CGxBuf* buf = s_bufferCache.AcquireBuffer(this->m_vertexBuffer);
        C3Vector* vertices = reinterpret_cast<C3Vector*>(g_theGxDevicePtr->BufLock(buf));
        vertices[0].x = 0.0f;
        vertices[0].y = 0.0f;
        vertices[0].z = 0.5f;
        vertices[1].x = 0.0f;
        vertices[1].y = 0.0f;
        vertices[1].z = -0.5f;
        vertices += 2;

        for (uint32_t i = 0; i < 12; i++) {
            double angle = static_cast<double>(i) / 12.0 * 6.283185307179586;
            float x = static_cast<float>(cos(angle));
            float y = static_cast<float>(sin(angle));
            vertices->x = x;
            vertices->y = y;
            vertices->z = -0.5f;
            vertices++;
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        s_bufferCache.ReleaseBuffer(this->m_vertexBuffer);
    }

    return this->m_vertexBuffer;
}

// OFFSET: 0x006198A0/0x1402A6E30
uint32_t GxuConeShapeFactory::CreateIndexBuffer() {
    if (!s_bufferCache.Exists(this->m_indexBuffer)) {
        this->m_indexBuffer = s_bufferCache.CreateBuffer(this->m_indexBuffer, GxPoolTarget_Index, sizeof(uint16_t), 72);
        CGxBuf* buf = s_bufferCache.AcquireBuffer(this->m_indexBuffer);
        uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(buf));

        for (uint16_t i = 0; i < 2; i++) {
            uint32_t second = ((i - 1) & 1) + 1;
            uint32_t first = i + 1;

            for (uint16_t j = 0; j < 11; j++) {
                indices[0] = i;
                indices[first] = j + 2;
                indices[second] = j + 3;
                indices += 3;
            }

            indices[0] = i;
            indices[first] = 13;
            indices[second] = 2;
            indices += 3;
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        s_bufferCache.ReleaseBuffer(this->m_indexBuffer);
    }

    return this->m_indexBuffer;
}

// OFFSET: 0x006199B0/0x1402A6F70
IGxuDebugShapeFactory::~IGxuDebugShapeFactory() {
}

// OFFSET: 0x006199E0/0x1402A6FB0
uint32_t GxuSphereShapeFactory::CreateVertexBuffer() {
    if (!s_bufferCache.Exists(this->m_vertexBuffer)) {
        this->m_vertexBuffer = s_bufferCache.CreateBuffer(this->m_vertexBuffer, GxPoolTarget_Vertex, sizeof(C3Vector), this->m_numVertices);
        CGxBuf* buf = s_bufferCache.AcquireBuffer(this->m_vertexBuffer);
        C3Vector* vertices = reinterpret_cast<C3Vector*>(g_theGxDevicePtr->BufLock(buf));

        for (uint32_t row = 0; row < this->m_rows; row++) {
            for (uint32_t segment = 0; segment < this->m_segments; segment++) {
                double theta = static_cast<double>(segment) / static_cast<double>(this->m_segments) * 6.283185307179586;
                double phi = static_cast<float>(row) / static_cast<double>(this->m_rows - 1) * 3.141592653589793;
                double ring = fabs(sin(phi));
                float x = static_cast<float>(cos(theta) * ring);
                float y = static_cast<float>(sin(theta) * ring);
                float z = static_cast<float>(cos(phi));
                vertices->x = x;
                vertices->y = y;
                vertices->z = z;
                vertices++;
            }
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        s_bufferCache.ReleaseBuffer(this->m_vertexBuffer);
    }

    return this->m_vertexBuffer;
}

// OFFSET: 0x00619B70/0x1402A7200
uint32_t GxuSphereShapeFactory::CreateIndexBuffer() {
    if (!s_bufferCache.Exists(this->m_indexBuffer)) {
        this->m_indexBuffer = s_bufferCache.CreateBuffer(this->m_indexBuffer, GxPoolTarget_Index, sizeof(uint16_t), this->m_totalIndices);
        CGxBuf* buf = s_bufferCache.AcquireBuffer(this->m_indexBuffer);
        uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(buf));

        for (uint32_t lod = 0; lod < 3; lod++) {
            uint32_t rows = this->m_quadRows >> lod;
            uint16_t step = static_cast<uint16_t>(1 << lod);

            for (uint32_t row = 0; row < rows; row++) {
                uint32_t columns = this->m_trianglesPerRow >> lod >> 1;
                uint16_t column = 0;

                for (uint32_t i = 0; i < columns; i++) {
                    uint16_t base = static_cast<uint16_t>(step * row * this->m_segments);
                    uint16_t next = column + step;
                    indices[0] = column + base;
                    indices[1] = base + column + step * static_cast<uint16_t>(this->m_segments);
                    indices[2] = base + static_cast<uint16_t>(next % this->m_segments);
                    indices[3] = base + static_cast<uint16_t>(next % this->m_segments);
                    indices[4] = base + column + step * static_cast<uint16_t>(this->m_segments);
                    indices[5] = base + step * static_cast<uint16_t>(this->m_segments) + static_cast<uint16_t>(next % this->m_segments);
                    indices += 6;
                    column = next;
                }
            }
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        s_bufferCache.ReleaseBuffer(this->m_indexBuffer);
    }

    return this->m_indexBuffer;
}

// OFFSET: 0x00619CF0
uint32_t GxuDirectionalSphereShapeFactory::CreateVertexBuffer() {
    if (!s_bufferCache.Exists(this->m_vertexBuffer)) {
        this->m_vertexBuffer = s_bufferCache.CreateBuffer(this->m_vertexBuffer, GxPoolTarget_Vertex, sizeof(C3Vector), this->m_numVertices);
        CGxBuf* buf = s_bufferCache.AcquireBuffer(this->m_vertexBuffer);
        C3Vector* vertices = reinterpret_cast<C3Vector*>(g_theGxDevicePtr->BufLock(buf));

        for (uint32_t row = 0; row < this->m_rows; row++) {
            for (uint32_t segment = 0; segment < this->m_segments; segment++) {
                double theta = static_cast<double>(segment) / static_cast<double>(this->m_segments) * 6.283185307179586;
                double phi = static_cast<float>(row) / static_cast<double>(this->m_rows - 1) * 3.141592653589793;
                double ring = fabs(sin(phi));
                float x = static_cast<float>(cos(theta) * ring);
                float y = static_cast<float>(sin(theta) * ring);
                float z = static_cast<float>(cos(phi));

                if (row <= 2) {
                    z = z + z;
                }

                vertices->x = x;
                vertices->y = y;
                vertices->z = z;
                vertices++;
            }
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        s_bufferCache.ReleaseBuffer(this->m_vertexBuffer);
    }

    return this->m_vertexBuffer;
}

// OFFSET: 0x00619EB0
uint32_t GxuBoxShapeFactory::CreateVertexBuffer() {
    if (!s_bufferCache.Exists(this->m_vertexBuffer)) {
        this->m_vertexBuffer = s_bufferCache.CreateBuffer(this->m_vertexBuffer, GxPoolTarget_Vertex, sizeof(C3Vector), 8);
        CGxBuf* buf = s_bufferCache.AcquireBuffer(this->m_vertexBuffer);
        C3Vector* vertices = reinterpret_cast<C3Vector*>(g_theGxDevicePtr->BufLock(buf));
        float bottom = static_cast<float>((0.0 * 2.0 - 1.0) * 0.5);

        for (uint32_t i = 0; i < 2; i++) {
            float y = static_cast<float>((static_cast<double>(i) * 2.0 - 1.0) * 0.5);

            for (uint32_t j = 0; j < 2; j++) {
                float t = static_cast<float>(j);
                float x = static_cast<float>(((i ? 1.0f - t : t) * 2.0 - 1.0) * 0.5);
                vertices[0].x = x;
                vertices[0].y = y;
                vertices[0].z = bottom;
                x = static_cast<float>(((i ? 1.0f - t : t) * 2.0 - 1.0) * 0.5);
                vertices[1].x = x;
                vertices[1].y = y;
                vertices[1].z = 0.5f;
                vertices += 2;
            }
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        s_bufferCache.ReleaseBuffer(this->m_vertexBuffer);
    }

    return this->m_vertexBuffer;
}

// OFFSET: 0x0061A040
uint32_t GxuBoxShapeFactory::CreateIndexBuffer() {
    if (!s_bufferCache.Exists(this->m_indexBuffer)) {
        this->m_indexBuffer = s_bufferCache.CreateBuffer(this->m_indexBuffer, GxPoolTarget_Index, sizeof(uint16_t), 36);
        CGxBuf* buf = s_bufferCache.AcquireBuffer(this->m_indexBuffer);
        uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(buf));

        for (uint32_t i = 0; i < 8; i++) {
            for (uint32_t j = 0; j < 3; j++) {
                uint32_t index;

                if (i & 1) {
                    index = i + j;
                } else {
                    index = i - j + 2;
                }

                *indices++ = static_cast<uint16_t>(index & 7);
            }
        }

        for (uint32_t i = 0; i < 2; i++) {
            for (uint32_t j = 0; j < 3; j++) {
                *indices++ = static_cast<uint16_t>((2 * (2 * i - j) + 4) & 7);
            }
        }

        for (uint32_t i = 0; i < 2; i++) {
            for (uint32_t j = 0; j < 3; j++) {
                *indices++ = static_cast<uint16_t>((2 * (j + 2 * i) + 1) & 7);
            }
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        s_bufferCache.ReleaseBuffer(this->m_indexBuffer);
    }

    return this->m_indexBuffer;
}

// OFFSET: 0x0061A170
uint32_t GxuTriangleShapeFactory::CreateVertexBuffer() {
    if (!s_bufferCache.Exists(this->m_vertexBuffer)) {
        this->m_vertexBuffer = s_bufferCache.CreateBuffer(this->m_vertexBuffer, GxPoolTarget_Vertex, sizeof(C3Vector), 6);
        CGxBuf* buf = s_bufferCache.AcquireBuffer(this->m_vertexBuffer);
        C3Vector* vertices = reinterpret_cast<C3Vector*>(g_theGxDevicePtr->BufLock(buf));
        vertices[0] = C3Vector(0.0f, 0.0f, 0.5f);
        vertices[1] = C3Vector(0.0f, 0.5f, -0.5f);
        vertices[2] = C3Vector(0.0f, -0.5f, -0.5f);
        vertices[3] = C3Vector(0.0f, 0.0f, 0.5f);
        vertices[4] = C3Vector(0.0f, -0.5f, -0.5f);
        vertices[5] = C3Vector(0.0f, 0.5f, -0.5f);
        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        s_bufferCache.ReleaseBuffer(this->m_vertexBuffer);
    }

    return this->m_vertexBuffer;
}

// OFFSET: 0x0061A2D0
uint32_t GxuTriangleShapeFactory::CreateIndexBuffer() {
    if (!s_bufferCache.Exists(this->m_indexBuffer)) {
        this->m_indexBuffer = s_bufferCache.CreateBuffer(this->m_indexBuffer, GxPoolTarget_Index, sizeof(uint16_t), 6);
        CGxBuf* buf = s_bufferCache.AcquireBuffer(this->m_indexBuffer);
        uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(buf));

        for (uint16_t i = 0; i < 6; i++) {
            indices[i] = i;
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        s_bufferCache.ReleaseBuffer(this->m_indexBuffer);
    }

    return this->m_indexBuffer;
}

// OFFSET: 0x0061A360
uint32_t GxuRotationRingShapeFactory::CreateVertexBuffer() {
    if (!s_bufferCache.Exists(this->m_vertexBuffer)) {
        this->m_vertexBuffer = s_bufferCache.CreateBuffer(this->m_vertexBuffer, GxPoolTarget_Vertex, sizeof(C3Vector), 64);
        CGxBuf* buf = s_bufferCache.AcquireBuffer(this->m_vertexBuffer);
        C3Vector* vertices = reinterpret_cast<C3Vector*>(g_theGxDevicePtr->BufLock(buf));

        for (uint32_t i = 0; i < 32; i++) {
            double angle = static_cast<double>(i) * 0.1963495463132858;
            float x = static_cast<float>(cos(angle));
            float y = static_cast<float>(sin(angle));
            vertices[0].x = x;
            vertices[0].y = y;
            vertices[0].z = 2.0f;
            vertices[1].x = x;
            vertices[1].y = y;
            vertices[1].z = -2.0f;
            vertices += 2;
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        s_bufferCache.ReleaseBuffer(this->m_vertexBuffer);
    }

    return this->m_vertexBuffer;
}

// OFFSET: 0x0061A490
uint32_t GxuRotationRingShapeFactory::CreateIndexBuffer() {
    if (!s_bufferCache.Exists(this->m_indexBuffer)) {
        this->m_indexBuffer = s_bufferCache.CreateBuffer(this->m_indexBuffer, GxPoolTarget_Index, sizeof(uint16_t), 384);
        CGxBuf* buf = s_bufferCache.AcquireBuffer(this->m_indexBuffer);
        uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(buf));

        for (uint32_t i = 0; i < 32; i++) {
            uint16_t base = static_cast<uint16_t>(2 * i);
            uint16_t a = base & 0x3F;
            uint16_t b = (base + 1) % 64;
            uint16_t d = (base + 3) % 64;
            uint16_t c = (base + 2) % 64;
            indices[0] = a;
            indices[1] = b;
            indices[2] = d;
            indices[3] = a;
            indices[4] = d;
            indices[5] = c;
            indices[6] = a;
            indices[7] = d;
            indices[8] = b;
            indices[9] = a;
            indices[10] = c;
            indices[11] = d;
            indices += 12;
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        s_bufferCache.ReleaseBuffer(this->m_indexBuffer);
    }

    return this->m_indexBuffer;
}

// OFFSET: 0x0061A5C0
uint32_t GxuDirectionalDiamondShapeFactory::CreateVertexBuffer() {
    if (!s_bufferCache.Exists(this->m_vertexBuffer)) {
        this->m_vertexBuffer = s_bufferCache.CreateBuffer(this->m_vertexBuffer, GxPoolTarget_Vertex, sizeof(C3Vector), 6);
        CGxBuf* buf = s_bufferCache.AcquireBuffer(this->m_vertexBuffer);
        C3Vector* vertices = reinterpret_cast<C3Vector*>(g_theGxDevicePtr->BufLock(buf));
        *vertices++ = C3Vector(0.0f, 0.0f, 1.0f);
        *vertices++ = C3Vector(0.0f, -0.25f, 0.0f);
        *vertices++ = C3Vector(0.0f, 0.25f, 0.0f);
        *vertices++ = C3Vector(0.0f, 0.0f, -0.25f);
        *vertices++ = C3Vector(0.5f, 0.0f, 0.0f);
        *vertices = C3Vector(-0.5f, 0.0f, 0.0f);
        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        s_bufferCache.ReleaseBuffer(this->m_vertexBuffer);
    }

    return this->m_vertexBuffer;
}

// OFFSET: 0x0061A740
uint32_t GxuDirectionalDiamondShapeFactory::CreateIndexBuffer() {
    if (!s_bufferCache.Exists(this->m_indexBuffer)) {
        this->m_indexBuffer = s_bufferCache.CreateBuffer(this->m_indexBuffer, GxPoolTarget_Index, sizeof(uint16_t), 24);
        CGxBuf* buf = s_bufferCache.AcquireBuffer(this->m_indexBuffer);
        uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(buf));
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 4;
        indices[3] = 0;
        indices[4] = 4;
        indices[5] = 2;
        indices[6] = 0;
        indices[7] = 2;
        indices[8] = 5;
        indices[9] = 0;
        indices[10] = 5;
        indices[11] = 1;
        indices[12] = 3;
        indices[13] = 2;
        indices[14] = 4;
        indices[15] = 3;
        indices[16] = 4;
        indices[17] = 1;
        indices[18] = 3;
        indices[19] = 1;
        indices[20] = 5;
        indices[21] = 3;
        indices[22] = 5;
        indices[23] = 2;
        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        s_bufferCache.ReleaseBuffer(this->m_indexBuffer);
    }

    return this->m_indexBuffer;
}

// OFFSET: 0x0061A8C0
void GxuDebugShapeRenderStateBlock::End() {
    g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_PolygonFill, this->m_polygonFill);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0061A920
void GxuDebugShapeRenderStateBlockNoDepth::End() {
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0061A930
GxuDebugShapeDrawer::~GxuDebugShapeDrawer() {
}

// OFFSET: 0x0061A960/0x1402A8380
GxuDebugShapeDrawer::GxuDebugShapeDrawer(const C44Matrix& matrix, const CImVector& color, int32_t mode, IGxuDebugShapeFactory* factory) {
    this->m_matrix = matrix;
    this->m_color = color;
    this->m_mode = mode;
    float scaleX = static_cast<float>(sqrt(matrix.a2 * matrix.a2 + matrix.a0 * matrix.a0 + matrix.a1 * matrix.a1));
    float scaleY = static_cast<float>(sqrt(matrix.b0 * matrix.b0 + matrix.b1 * matrix.b1 + matrix.b2 * matrix.b2));
    float scale = scaleX <= scaleY ? scaleY : scaleX;
    float scaleZ = static_cast<float>(sqrt(matrix.c1 * matrix.c1 + matrix.c0 * matrix.c0 + matrix.c2 * matrix.c2));

    if (scale <= scaleZ) {
        scale = scaleZ;
    }

    this->m_scale = scale;
    this->m_vertexBuffer = factory->CreateVertexBuffer();
    this->m_indexBuffer = factory->CreateIndexBuffer();
    this->m_factory = factory;
    this->m_radius = factory->GetRadius() * this->m_scale;
}

// OFFSET: 0x0061ABC0
float GxuDebugShapeDrawer::GetDepth(const C3Vector& viewPos) {
    float x = this->m_matrix.d0 - viewPos.x;
    float y = this->m_matrix.d1 - viewPos.y;
    float z = this->m_matrix.d2 - viewPos.z;
    return static_cast<float>(sqrt(x * x + y * y + z * z)) - this->m_radius;
}

// OFFSET: 0x0061AC20
GxuDebugShapeObject::GxuDebugShapeObject() : m_next(NULL), m_time(0.0f) {
}

// OFFSET: 0x0061ACD0
GxuDebugShapeObject::~GxuDebugShapeObject() {
}

// OFFSET: 0x0061AD60/0x1402A8740
void GxuDebugShapeDrawer::Draw(const C3Vector& viewPos) {
    C44Matrix matrix = this->m_matrix;

    if (GxuGetLODMode()) {
        matrix.d0 = matrix.d0 - viewPos.x;
        matrix.d1 = matrix.d1 - viewPos.y;
        matrix.d2 = matrix.d2 - viewPos.z;
    }

    g_theGxDevicePtr->XformPushAndSet(GxXform_World, matrix);
    GxRsSet(GxRs_MatDiffuse, this->m_color);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_BlendingMode, this->m_color.a == 0xFF ? 0 : 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);

    for (uint32_t i = 0; i < 3; i++) {
        GxRsSet(static_cast<EGxRenderState>(GxRs_Texture0 + i), static_cast<void*>(NULL));
    }

    this->m_factory->SetLOD(this->GetDepth(viewPos));
    g_theGxDevicePtr->PrimVertexPtr(s_bufferCache.AcquireBuffer(this->m_vertexBuffer), GxVBF_P);
    g_theGxDevicePtr->PrimIndexPtr(s_bufferCache.AcquireBuffer(this->m_indexBuffer));
    DefaultShaders::SetFogEnabled(0);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    CGxBatch batch;
    batch.m_start = this->m_factory->GetIdxOffset();
    batch.m_count = this->m_factory->GetNumIndices();
    uint32_t numVertices = this->m_factory->GetNumVertices();
    batch.m_minIndex = 0;
    batch.m_maxIndex = static_cast<uint16_t>(numVertices - 1);
    batch.m_primType = GxPrim_Triangles;
    g_theGxDevicePtr->Draw(batch);
    s_bufferCache.ReleaseBuffer(this->m_indexBuffer);
    s_bufferCache.ReleaseBuffer(this->m_vertexBuffer);
    GxXformPop(GxXform_World);
}

// OFFSET: 0x0061B120
void GxuDebugShapeRenderStateBlock::Begin() {
    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_DepthTest, 1);
    GxRsSet(GxRs_DepthFunc, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_PolygonOffset, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_ColorWrite, 15);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_PixelShader, static_cast<void*>(NULL));
    GxRsSet(GxRs_VertexShader, static_cast<void*>(NULL));

    switch (this->m_mode) {
        case 1:
            GxRsSet(GxRs_Culling, 2);
            break;

        case 3:
            GxRsSet(GxRs_Culling, 0);
            break;

        default:
            GxRsSet(GxRs_Culling, 1);
            break;
    }

    this->m_polygonFill = g_theGxDevicePtr->MasterEnable(GxMasterEnable_PolygonFill);
    g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_PolygonFill, this->m_mode != 3);
}

// OFFSET: 0x0061B3A0
void GxuDebugShapeRenderStateBlockNoDepth::Begin() {
    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthFunc, 0);
    GxRsSet(GxRs_DepthWrite, 1);
    GxRsSet(GxRs_PolygonOffset, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_ColorWrite, 15);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_PixelShader, static_cast<void*>(NULL));
    GxRsSet(GxRs_VertexShader, static_cast<void*>(NULL));
    GxRsSet(GxRs_Culling, 0);
}

// OFFSET: 0x0061B590
void GxuShutdownDebugShapeDrawer() {
    if (!s_shapeDrawerInitialized) {
        return;
    }

    s_shapeObjectFreeList = NULL;

    while (s_shapeObjectBlocks) {
        GxuDebugObjectBlock<GxuDebugShapeObject>* block = s_shapeObjectBlocks;
        s_shapeObjectBlocks = block->m_next;
        block->m_objects.Clear();
        delete block;
    }

    s_bufferCache.ReleaseCache();
    s_shapeDrawerInitialized = false;
}

// OFFSET: 0x0061B6F0/0x1402A9390
static void GxuDebugDrawShape(const C44Matrix& matrix, const CImVector& color, int32_t mode, IGxuDebugShapeFactory* factory, float time) {
    if (!s_shapeDrawerInitialized) {
        s_shapeDrawerInitialized = true;
    }

    GxuDebugShapeObject* object = s_shapeObjectFreeList;

    if (!object) {
        GxuDebugObjectBlock<GxuDebugShapeObject>* block = new (__FILE__, __LINE__) GxuDebugObjectBlock<GxuDebugShapeObject>;
        block->m_objects.SetCount(16);

        for (uint32_t i = 0; i < 16; i++) {
            block->m_objects[i].m_next = object;
            object = &block->m_objects[i];
        }

        block->m_next = s_shapeObjectBlocks;
        s_shapeObjectBlocks = block;
    }

    s_shapeObjectFreeList = object->m_next;
    object->~GxuDebugShapeObject();
    new (object) GxuDebugShapeObject(matrix, color, mode, factory, time);
    g_scene.AddObject(object);
}

// OFFSET: 0x0061B7E0
void GxuDebugDrawCone(const C44Matrix& matrix, const CImVector& color, int32_t mode, float time) {
    GxuDebugDrawShape(matrix, color, mode, &s_coneShapeFactory, time);
}

// OFFSET: 0x0061B810
void GxuDebugDrawSphere(const C44Matrix& matrix, const CImVector& color, int32_t mode, float time) {
    GxuDebugDrawShape(matrix, color, mode, &s_sphereShapeFactory, time);
}

// OFFSET: 0x0061B840
void GxuDebugDrawDirectionalSphere(const C44Matrix& matrix, const CImVector& color, int32_t mode, float time) {
    GxuDebugDrawShape(matrix, color, mode, &s_directionalSphereShapeFactory, time);
}

// OFFSET: 0x0061B870
void GxuDebugDrawBox(const C44Matrix& matrix, const CImVector& color, int32_t mode, float time) {
    GxuDebugDrawShape(matrix, color, mode, &s_boxShapeFactory, time);
}

// OFFSET: 0x0061B8A0
void GxuDebugDrawTriangle(const C44Matrix& matrix, const CImVector& color, int32_t mode, float time) {
    GxuDebugDrawShape(matrix, color, mode, &s_triangleShapeFactory, time);
}

// OFFSET: 0x0061B8D0
void GxuDebugDrawRotationRing(const C44Matrix& matrix, const CImVector& color, int32_t mode, float time) {
    GxuDebugDrawShape(matrix, color, mode, &s_rotationRingShapeFactory, time);
}
