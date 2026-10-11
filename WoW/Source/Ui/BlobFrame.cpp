#include "BlobFrame.h"
#include <cfloat>
#include <cmath>
#include <malloc.h>
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Spline_CatmullRom.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/DungeonMapRec.h"
#include "WoW/Source/DB/Rec/WorldMapAreaRec.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/WorldMap.h"

// STATIC: 0x00D175D0
const char* CGBlobFrame::s_objectTypeName = "CGBlobFrame";

// STATIC: 0x00D175D4
int32_t CGBlobFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00F76C28
int32_t CGBlobFrame::s_objectType;

// STATIC: 0x00F76C2C
CDataAllocator CGBlobFrame::s_blobHeap(sizeof(CGBlobFrame), 5);

// STATIC: 0x00D1765C
static int32_t s_highlightIndex = -1;

// INLINED
void CGBlobFrame::operator delete(void* ptr) {
    CGBlobFrame::s_blobHeap.PutData(ptr, NULL, 0);
}

// OFFSET: 0x008C63D0
CSimpleFrame* CGBlobFrame::Create(CSimpleFrame* parent) {
    void* data = CGBlobFrame::s_blobHeap.GetData(0, typeid(CGBlobFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGBlobFrame(parent) : NULL;
}

// OFFSET: 0x009FFCB0
void CGBlobFrame::RenderCallback(void* param) {
    if (param) {
        static_cast<CGBlobFrame*>(param)->Render();
    }
}

// OFFSET: 0x009FFD60
void CGBlobFrame::OnFrameRender(CRenderBatch* batch, uint32_t layer) {
    CSimpleFrame::OnFrameRender(batch, layer);

    if (layer == 3) {
        batch->QueueCallback(CGBlobFrame::RenderCallback, this);
    }
}

// OFFSET: 0x009FFD90
void CGBlobFrame::MergeBlobs(uint32_t index) {
    BlobRenderGroup* group = &this->m_groups[index];
    int32_t mask = group->m_info->m_objectiveMask;

    if (mask == -1) {
        return;
    }

    for (uint32_t i = 0; i < group->m_renderData.Count(); i++) {
        BlobRenderData* first = &group->m_renderData.m_data[i];

        if (!first->m_valid || !((1 << first->m_blob->m_objectiveIndex) & mask)) {
            continue;
        }

        for (uint32_t j = i + 1; j < group->m_renderData.Count(); j++) {
            BlobRenderData* second = &group->m_renderData.m_data[j];

            if (!second->m_valid || !((1 << second->m_blob->m_objectiveIndex) & mask)) {
                continue;
            }

            CRect overlap = first->m_bounds & second->m_bounds;

            if (!(overlap.maxY > overlap.minY) || !(overlap.maxX > overlap.minX)) {
                continue;
            }

            BlobRenderData* smaller;
            BlobRenderData* larger;

            if ((second->m_bounds.maxX - second->m_bounds.minX) * (second->m_bounds.maxY - second->m_bounds.minY) <= (first->m_bounds.maxX - first->m_bounds.minX) * (first->m_bounds.maxY - first->m_bounds.minY)) {
                smaller = second;
                larger = first;
            } else {
                smaller = first;
                larger = second;
            }

            uint32_t inside = 0;

            for (uint32_t k = 0; k < smaller->m_fillVerts.Count() - 1; k++) {
                if (NTempest::Intersect(smaller->m_fillVerts.m_data[k], larger->m_fillVerts.m_data, larger->m_fillVerts.Count() - 1, C3Vector::AXIS_Z)) {
                    inside++;
                }
            }

            if (static_cast<float>(inside) / static_cast<float>(smaller->m_fillVerts.Count() - 1) > this->m_mergeThreshold) {
                smaller->m_valid = 0;
            }
        }
    }
}

// OFFSET: 0x009FFFF0
bool IntersectLines(const C3Vector& a0, const C3Vector& a1, const C3Vector& b0, const C3Vector& b1, C3Vector* result) {
    if (fequalz(a0.x, a1.x, 0.001f) || fequalz(a0.y, a1.y, 0.001f) || fequalz(a0.x, b0.x, 0.001f) || fequalz(a0.y, b0.y, 0.001f) || fequalz(a0.x, b1.x, 0.001f) || fequalz(a0.y, b1.y, 0.001f)) {
        return false;
    }

    if (fequalz(a1.x, b0.x, 0.001f) || fequalz(a1.y, b0.y, 0.001f) || fequalz(a1.x, b1.x, 0.001f) || fequalz(a1.y, b1.y, 0.001f) || fequalz(b0.x, b1.x, 0.001f) || fequalz(b0.y, b1.y, 0.001f)) {
        return false;
    }

    float ax = a0.x - a1.x;
    float bx = b0.x - b1.x;
    float denom = (b0.y - b1.y) * ax - (a0.y - a1.y) * bx;

    if (fequal(denom, 0.0f)) {
        return false;
    }

    float ca = a0.x * a1.y - a1.x * a0.y;
    float cb = b0.x * b1.y - b1.x * b0.y;
    result->x = (bx * ca - ax * cb) * (1.0f / denom);
    result->y = 1.0f / denom * ((b0.y - b1.y) * ca - (a0.y - a1.y) * cb);
    result->z = 0.0f;
    return true;
}

// PROVEN: 766b947d IDENTICAL/NONE
// OFFSET: 0x00A007D0/0x14079EE00
CGBlobFrame::BlobRenderData::~BlobRenderData() {
}

// OFFSET: 0x00A00890
void CGBlobFrame::BuildBlobOutline(BLOB* blob, TSGrowableArray<BLOBPOI>* points, bool smoothing, uint32_t numSplinePoints) {
    if (!smoothing) {
        points->SetCount(blob->m_points.Count());

        for (uint32_t i = 0; i < blob->m_points.Count(); i++) {
            points->m_data[i] = blob->m_points.m_data[i];
        }

        return;
    }

    C3Spline_CatmullRom spline;
    uint32_t count = blob->m_points.Count() + 3;
    TSStackArray<C3Vector> controls(static_cast<C3Vector*>(alloca(sizeof(C3Vector) * count)), count, 1);
    controls.m_data[0] = C3Vector(static_cast<float>(blob->m_points.m_data[blob->m_points.Count() - 1].m_x), static_cast<float>(blob->m_points.m_data[blob->m_points.Count() - 1].m_y), 0.0f);

    for (uint32_t i = 0; i < blob->m_points.Count(); i++) {
        controls.m_data[i + 1] = C3Vector(static_cast<float>(blob->m_points.m_data[i].m_x), static_cast<float>(blob->m_points.m_data[i].m_y), 0.0f);
    }

    controls.m_data[blob->m_points.Count() + 1] = C3Vector(static_cast<float>(blob->m_points.m_data[0].m_x), static_cast<float>(blob->m_points.m_data[0].m_y), 0.0f);
    controls.m_data[blob->m_points.Count() + 2] = C3Vector(static_cast<float>(blob->m_points.m_data[1].m_x), static_cast<float>(blob->m_points.m_data[1].m_y), 0.0f);
    spline.SetPoints(controls.m_data, blob->m_points.Count() + 3);

    float t = 0.01f;
    float step = 1.0f / static_cast<float>(numSplinePoints);

    for (uint32_t i = 0; i < numSplinePoints; i++) {
        C3Vector position(0.0f, 0.0f, 0.0f);
        spline.Pos(t, position, C3Spline::EVAL_ARCLENGTH);
        int32_t y = static_cast<int32_t>(position.y);
        int32_t x = static_cast<int32_t>(position.x);
        BLOBPOI* point = points->New();

        if (point) {
            point->m_x = x;
            point->m_y = y;
        }

        t = step + t;
    }
}

// OFFSET: 0x00A00B70
void CGBlobFrame::BuildBlobRenderData(BlobRenderData* data, BLOB* blob, bool filter, int32_t mask) {
    if (!blob) {
        if (data) {
            data->m_blob = NULL;
            data->m_valid = 0;
        }

        return;
    }

    data->m_blob = blob;
    WorldMapAreaRec* area;

    if (blob->m_points.Count() < 3 || !(area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetCurrentAreaID()))) {
        data->m_valid = 0;
        return;
    }

    DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecord(CGWorldMap::GetCurrentDungeonMap());

    if (dungeonMap && (dungeonMap->m_mapID != blob->m_mapId || dungeonMap->m_floorIndex != blob->m_floor)) {
        data->m_valid = 0;
        return;
    }

    TSGrowableArray<BLOBPOI> points;
    CGBlobFrame::BuildBlobOutline(blob, &points, this->m_smoothing, this->m_numSplinePoints);
    uint32_t count = points.Count();
    data->m_fillVerts.SetCount(count + 1);
    data->m_fillTexCoords.SetCount(count + 1);
    data->m_fillIndices.SetCount(3 * count);

    data->m_objectives[0] = 0;
    data->m_objectives[1] = 0;
    data->m_objectives[2] = 0;
    data->m_objectives[3] = 0;
    data->m_objectives[0] = data->m_blob->m_objectiveIndex;
    data->m_numObjectives = 1;
    bool failed = false;
    C3Vector center(0.0f, 0.0f, 0.0f);

    for (uint32_t i = 0; i < count; i++) {
        C3Vector position(static_cast<float>(points.m_data[i].m_x), static_cast<float>(points.m_data[i].m_y), 0.0f);
        float x;
        float y;
        bool outland = false;

        if (!CGWorldMap::GetWorldPosition(position, area->m_mapID, &x, &y, &outland, false, CGWorldMap::GetCurrentDungeonMap()) || outland || fabs(fabs(x * y)) < 0.001f) {
            failed = true;
            break;
        }

        C3Vector& vertex = data->m_fillVerts.m_data[i];
        vertex.x = (this->m_rect.maxX - this->m_rect.minX) * x + this->m_rect.minX;
        vertex.y = (this->m_rect.maxY - this->m_rect.minY) * (1.0f - y) + this->m_rect.minY;
        vertex.z = 1.0f;
        data->m_fillTexCoords.m_data[i] = C2Vector(1.0f, 1.0f);
        center.x += vertex.x;
        center.y += vertex.y;
        center.z += vertex.z;
        data->m_fillIndices.m_data[3 * i] = static_cast<uint16_t>(i);
        data->m_fillIndices.m_data[3 * i + 1] = static_cast<uint16_t>((i + 1) % count);
        data->m_fillIndices.m_data[3 * i + 2] = static_cast<uint16_t>(count);
    }

    data->m_valid = !failed;

    if (!failed && filter && mask != -1 && blob->m_objectiveIndex != -1 && !((1 << blob->m_objectiveIndex) & mask)) {
        data->m_valid = 0;
    }

    if (failed) {
        data->m_fillVerts.Clear();
        data->m_fillTexCoords.Clear();
        data->m_fillIndices.Clear();
        data->m_borderVerts.Clear();
        data->m_borderTexCoords.Clear();
        data->m_borderIndices.Clear();
        return;
    }

    data->m_bounds.minX = FLT_MAX;
    data->m_bounds.maxX = FLT_MIN;
    data->m_bounds.minY = FLT_MAX;
    data->m_bounds.maxY = FLT_MIN;

    for (uint32_t i = 0; i < data->m_fillVerts.Count() - 1; i++) {
        const C3Vector& vertex = data->m_fillVerts.m_data[i];
        data->m_bounds.minX = vertex.x <= data->m_bounds.minX ? vertex.x : data->m_bounds.minX;
        data->m_bounds.maxX = vertex.x >= data->m_bounds.maxX ? vertex.x : data->m_bounds.maxX;
        data->m_bounds.minY = vertex.y <= data->m_bounds.minY ? vertex.y : data->m_bounds.minY;
        data->m_bounds.maxY = vertex.y >= data->m_bounds.maxY ? vertex.y : data->m_bounds.maxY;
    }

    float scale = 1.0f / count;
    center.x *= scale;
    center.y *= scale;
    center.z *= scale;
    data->m_fillVerts.m_data[count] = center;
    data->m_fillTexCoords.m_data[count] = C2Vector(0.0f, 0.0f);

    data->m_borderVerts.SetCount(2 * count);
    data->m_borderTexCoords.SetCount(2 * count);
    data->m_borderIndices.SetCount(2 * count + 2);

    TSGrowableArray<C3Vector> edges[2];
    edges[0].SetCount(count);
    edges[1].SetCount(count);

    for (uint32_t i = 0; i < count; i++) {
        const C3Vector& current = data->m_fillVerts.m_data[i];
        data->m_borderVerts.m_data[2 * i] = current;
        data->m_borderTexCoords.m_data[2 * i] = C2Vector(0.0f, 0.0f);
        data->m_borderTexCoords.m_data[2 * i + 1] = C2Vector(1.0f, 1.0f);
        data->m_borderIndices.m_data[2 * i] = static_cast<uint16_t>(2 * i);
        data->m_borderIndices.m_data[2 * i + 1] = static_cast<uint16_t>((2 * i + 1) % data->m_borderVerts.Count());

        uint32_t next = (i + 1) % count;
        const C3Vector& following = data->m_fillVerts.m_data[next];
        float nx = -(following.y - current.y);
        float ny = following.x - current.x;
        float nz = 0.0f;
        float length = ny * ny + nx * nx;

        if (length > 0.00000023841858f) {
            float inverse = 1.0f / sqrt(length);
            nx *= inverse;
            ny *= inverse;
            nz = inverse * 0.0f;
        }

        float border = this->m_borderScalar * 0.0099999998f;
        nx *= border;
        ny *= border;
        nz *= border;

        if ((center.y - current.y) * ny + (center.z - current.z) * nz + (center.x - current.x) * nx > 0.0f) {
            nx *= -1.0f;
            ny *= -1.0f;
            nz *= -1.0f;
        }

        edges[0].m_data[i] = C3Vector(current.x + nx, current.y + ny, current.z + nz);
        edges[1].m_data[i] = C3Vector(following.x + nx, following.y + ny, following.z + nz);
    }

    data->m_borderIndices.m_data[2 * count] = 0;
    data->m_borderIndices.m_data[2 * count + 1] = 1;

    for (uint32_t i = 0; i < count; i++) {
        uint32_t next = (i + 1) % count;
        C3Vector hit;
        hit.x = 0.0f;
        hit.y = 0.0f;

        if (IntersectLines(edges[0].m_data[i], edges[1].m_data[i], edges[0].m_data[next], edges[1].m_data[next], &hit)) {
            data->m_borderVerts.m_data[2 * i + 1] = C3Vector(hit.x, hit.y, 1.0f);
        } else {
            data->m_borderVerts.m_data[2 * i + 1] = edges[0].m_data[i];
        }
    }
}

// OFFSET: 0x00A015B0/0x1407A0020
void CGBlobFrame::SetFillTexture(const char* fileName) {
    if (this->m_fillTexture) {
        TextureRelease(this->m_fillTexture);
        this->m_fillTexture = NULL;
    }

    CStatus status;
    this->m_fillTexture = TextureCreate(fileName, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), &status, 0, NULL, 2);
}

// OFFSET: 0x00A01630/0x1407A00D0
void CGBlobFrame::SetBorderTexture(const char* fileName) {
    if (this->m_borderTexture) {
        TextureRelease(this->m_borderTexture);
        this->m_borderTexture = NULL;
    }

    CStatus status;
    this->m_borderTexture = TextureCreate(fileName, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), &status, 0, NULL, 2);
}

// OFFSET: [0x00A01840, 0x00A01D40]
CGBlobFrame::~CGBlobFrame() {
    if (this->m_fillTexture) {
        TextureRelease(this->m_fillTexture);
        this->m_fillTexture = NULL;
    }

    if (this->m_borderTexture) {
        TextureRelease(this->m_borderTexture);
        this->m_borderTexture = NULL;
    }
}

// OFFSET: 0x00A01900
const char* CGBlobFrame::GetObjectTypeName() {
    return CGBlobFrame::s_objectTypeName;
}

// OFFSET: 0x00A01910
int32_t CGBlobFrame::GetScriptMetaTable() {
    return CGBlobFrame::s_metatable;
}

// OFFSET: 0x00A01930
bool CGBlobFrame::IsA(int32_t type) {
    return type == CGBlobFrame::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: 0x00A01970
bool CGBlobFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CGBlobFrame::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: 0x00A019B0
void CGBlobFrame::LoadXML(linkObject* node, CStatus* status) {
    CSimpleFrame::LoadXML(node, status);

    if (!(node->m_flags & 0x40000)) {
        return;
    }

    BlobFrame* blob = static_cast<BlobFrame*>(node);

    if (blob->m_blobFlags & 0x1) {
        this->SetFillTexture(blob->m_fillTexture.m_string);
    }

    if (blob->m_blobFlags & 0x2) {
        this->SetBorderTexture(blob->m_borderTexture.m_string);
    }
}

// OFFSET: 0x00A01A10
void CGBlobFrame::RemoveBlob(int32_t id) {
    if (!id) {
        return;
    }

    uint32_t index = 0;

    while (!this->m_groups[index].m_info || this->m_groups[index].m_info->m_questId != id) {
        if (++index >= 4) {
            return;
        }
    }

    BlobRenderGroup* group = &this->m_groups[index];
    group->m_info = NULL;

    for (uint32_t i = 0; i < group->m_renderData.Count(); i++) {
        BlobRenderData* data = &group->m_renderData.m_data[i];
        data->m_fillVerts.Clear();
        data->m_fillTexCoords.Clear();
        data->m_fillIndices.Clear();
        data->m_borderVerts.Clear();
        data->m_borderTexCoords.Clear();
        data->m_borderIndices.Clear();
    }

    group->m_renderData.Clear();
}

// OFFSET: 0x00A01C30
CGBlobFrame::CGBlobFrame(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->SetProtected();
    this->DisableEvent(SIMPLE_EVENT_MOUSE);
    this->m_fillAlpha = 0xFF;
    this->m_borderAlpha = 0xFF;
    this->m_borderScalar = 1.0f;
    this->m_fillTexture = NULL;
    this->m_borderTexture = NULL;
    this->m_mouseOverQuestId = 0;
    this->m_mouseOverCount = 0;
    this->m_smoothing = 1;
    this->m_merging = 1;
    this->m_mergeThreshold = 0.25f;
    this->m_numSplinePoints = 20;

    for (uint32_t i = 0; i < sizeof(this->m_groups) / sizeof(this->m_groups[0]); i++) {
        this->m_groups[i].m_info = NULL;
    }
}

// OFFSET: 0x00A01D70
void CGBlobFrame::DrawNone() {
    for (uint32_t i = 0; i < 4; i++) {
        if (this->m_groups[i].m_info) {
            this->RemoveBlob(this->m_groups[i].m_info->m_questId);
        }
    }
}

// OFFSET: 0x00A01E00
bool CGBlobFrame::BuildRenderData(int32_t index, bool filter) {
    if (index >= 0 && index < sizeof(this->m_groups) / sizeof(this->m_groups[0]) && this->m_groups[index].m_info) {
        if (fabs(this->m_rect.maxX - this->m_rect.minX) < 0.001f && fabs(this->m_rect.maxY - this->m_rect.minY) < 0.001f) {
            return false;
        }

        for (uint32_t i = 0; i < this->m_groups[index].m_info->m_blobs.Count(); i++) {
            BlobRenderData* data = this->m_groups[index].m_renderData.New();

            if (data) {
                this->BuildBlobRenderData(data, &this->m_groups[index].m_info->m_blobs.m_data[i], filter, this->m_groups[index].m_info->m_objectiveMask);
            }
        }

        if (this->m_merging) {
            this->MergeBlobs(index);
        }
    }

    return true;
}

// OFFSET: 0x00A01EE0
void CGBlobFrame::Render() {
    CTexture* fill = this->m_fillTexture;

    if (!fill || !this->m_borderTexture) {
        return;
    }

    WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetCurrentAreaID());

    if (CGWorldMap::GetCurrentAreaID() == -1 || !area) {
        return;
    }

    CGxTex* fillTexture = TextureGetGxTex(fill);

    if (!fillTexture) {
        return;
    }

    CGxTex* borderTexture = TextureGetGxTex(this->m_borderTexture);

    if (!borderTexture) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_BlendingMode, 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Multisample, 0);
    GxRsSet(GxRs_Texture0, fillTexture);
    GxRsSet(GxRs_MatDiffuse, CImVector(0xFF, 0xFF, 0xFF, this->m_fillAlpha));
    DefaultShaders::SetDefaultShaders(GxVBF_PT, 1);

    for (uint32_t i = 0; i < 4; i++) {
        BlobRenderGroup* group = &this->m_groups[i];

        if (!group->m_info) {
            continue;
        }

        if (group->m_dirty) {
            if (!this->BuildRenderData(i, false)) {
                continue;
            }

            group->m_dirty = 0;
        }

        for (uint32_t j = 0; j < group->m_renderData.Count(); j++) {
            BlobRenderData* data = &group->m_renderData.m_data[j];

            if (data->m_blob && data->m_valid && CGQuestLog::IsPOIVisibleOnWorldMap(data->m_blob->m_center, data->m_blob->m_mapId, area->m_mapID) && (s_highlightIndex == -1 || s_highlightIndex == static_cast<int32_t>(j))) {
                GxPrimLockVertexPtrs(data->m_fillVerts.Count(), data->m_fillVerts.m_data, sizeof(C3Vector), NULL, 0, NULL, 0, NULL, 0, data->m_fillTexCoords.m_data, sizeof(C2Vector), NULL, 0);
                GxDrawLockedElements(GxPrim_Triangles, data->m_fillIndices.Count(), data->m_fillIndices.m_data);
                GxPrimUnlockVertexPtrs();
            }
        }
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, borderTexture);
    GxRsSet(GxRs_MatDiffuse, CImVector(0xFF, 0xFF, 0xFF, this->m_borderAlpha));
    DefaultShaders::SetDefaultShaders(GxVBF_PT, 1);

    for (uint32_t i = 0; i < 4; i++) {
        BlobRenderGroup* group = &this->m_groups[i];

        if (!group->m_info) {
            continue;
        }

        for (uint32_t j = 0; j < group->m_renderData.Count(); j++) {
            BlobRenderData* data = &group->m_renderData.m_data[j];

            if (data->m_blob && data->m_valid && (s_highlightIndex == -1 || s_highlightIndex == static_cast<int32_t>(j))) {
                GxPrimLockVertexPtrs(data->m_borderVerts.Count(), data->m_borderVerts.m_data, sizeof(C3Vector), NULL, 0, NULL, 0, NULL, 0, data->m_borderTexCoords.m_data, sizeof(C2Vector), NULL, 0);
                GxDrawLockedElements(GxPrim_TriangleStrip, data->m_borderIndices.Count(), data->m_borderIndices.m_data);
                GxPrimUnlockVertexPtrs();
            }
        }
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x00A021F0
void CGBlobFrame::AddBlob(QuestPOIInfo* info, bool filter) {
    if (!info) {
        return;
    }

    int32_t slot = -1;

    for (uint32_t i = 0; i < 4; i++) {
        if (!this->m_groups[i].m_info) {
            if (slot == -1) {
                slot = i;
            }
        } else if (this->m_groups[i].m_info->m_questId == info->m_questId) {
            return;
        }
    }

    if (slot > -1) {
        this->m_groups[slot].m_info = info;
        this->m_groups[slot].m_dirty = !this->BuildRenderData(slot, filter);
    }
}

// OFFSET: [0x00841AE0]
void CGBlobFrame::DrawBlob(int32_t id, bool show) {
}
