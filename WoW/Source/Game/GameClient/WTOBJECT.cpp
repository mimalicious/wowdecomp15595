#include "WTOBJECT.h"
#include <new>
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxVertexPC.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Event/FrameTime.h"
#include <typeinfo>

// STATIC: 0x00CA10BC/0x140B3D9A8
static TSExplicitList<WTOBJECT, offsetof(WTOBJECT, m_link)> s_finishedTrails;

// STATIC: 0x00DC7098/0x140CACC48
static uint32_t s_count;

// STATIC: 0x00DC709C/0x140CACC50
static CGxShader* s_vertexShader;

// STATIC: 0x00DC70A0/0x140CACC58
static CGxShader* s_pixelShader;

// STATIC: 0x00DC70A4/0x140CACC60
static CDataAllocator s_freeSwings(sizeof(SWING), 8);

// STATIC: 0x00DC70B8/0x140CACC80
static CDataAllocator s_freeTrails(sizeof(WTOBJECT), 128);

// OFFSET: 0x004DFC20/0x14011FC30
void SWING::FillVertices(CImVector color, uint8_t fade, CGxVertexPC* vertices) {
    for (int32_t i = this->m_tail; i < this->m_head; i++, vertices++) {
        vertices->p = this->m_points[i & 0x7F];

        if (!(i & 0x1)) {
            color.a -= fade;
        }

        vertices->c = color;
    }
}

// OFFSET: 0x004DFC70/0x14011FCA0
void WeaponTrailsInitialize() {
    if (!s_vertexShader) {
        s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "WeaponTrails");
        s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "WeaponTrails");
    }
}

// OFFSET: 0x004DFCB0/0x14011FCF0
void WeaponTrailsShutdown() {
    s_freeTrails.Clear(typeid(WTOBJECT).raw_name(), SLOG_OBJECT, 1);
    s_freeSwings.Clear(typeid(SWING).raw_name(), SLOG_OBJECT, 1);

    if (s_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(s_vertexShader);
        s_vertexShader = NULL;
    }

    if (s_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(s_pixelShader);
        s_pixelShader = NULL;
    }
}

// OFFSET: 0x004DFD30
static SWING* AllocSwing(uint32_t zero) {
    SWING* swing = static_cast<SWING*>(s_freeSwings.GetData(zero, typeid(SWING).raw_name(), SLOG_OBJECT));

    if (swing) {
        new (swing) SWING();
    }

    return swing;
}

// OFFSET: 0x004DFD90/0x14011FD90
WTOBJECT::~WTOBJECT() {
    if (this->m_model) {
        this->m_model->m_drawCallback = NULL;
        this->m_model->Release();
    }

    if (this->m_swing) {
        s_freeSwings.PutData(this->m_swing, NULL, 0);
    }
}

// OFFSET: 0x004DFE00/0x14011FE30
void WeaponTrailsClearFinished() {
    for (WTOBJECT* trail = s_finishedTrails.Head(), * next; trail; trail = next) {
        next = s_finishedTrails.Next(trail);
        trail->m_model->m_drawCallback = NULL;
    }

    s_finishedTrails.UnlinkAll();
}

// OFFSET: 0x004DFE40/0x14011FED0
bool SWING::BuildVertexBuffer(uint32_t time, const C3Vector& top, const C3Vector& bottom) {
    uint32_t elapsed = time - this->m_startTime;

    if (elapsed < this->m_duration) {
        int32_t index = this->m_head++;

        if (this->m_tail < this->m_head - 128) {
            this->m_tail = this->m_head - 128;
        }

        this->m_points[index & 0x7F] = bottom;
        index = this->m_head++;

        if (this->m_tail < this->m_head - 128) {
            this->m_tail = this->m_head - 128;
        }

        this->m_points[index & 0x7F] = top;
    }

    uint8_t alpha = this->m_color.a;
    int32_t delta = time - this->m_lastTime;
    this->m_lastTime = time;
    uint8_t fade = static_cast<uint8_t>(FloatToInt64(delta * 0.0033333334f * alpha));

    if (!fade) {
        fade = 1;
    }

    int32_t segments = alpha / fade;

    if (segments <= 1) {
        return false;
    }

    int32_t head = this->m_head;

    if (this->m_tail < head - 2 * segments) {
        this->m_tail = head - 2 * segments;
    }

    int32_t count = head - this->m_tail;

    if (!count) {
        return false;
    }

    if (count >= 3) {
        g_theGxDevicePtr->RsPush();
        GxRsSet(GxRs_Culling, 0);
        GxRsSet(GxRs_Fog, 0);
        GxRsSet(GxRs_Lighting, 0);
        GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
        GxUpdateAlphaRef(GxRs_AlphaRef);
        GxRsSet(GxRs_DepthWrite, 0);
        GxRsSet(GxRs_DepthTest, 1);
        GxRsSet(GxRs_Texture0, 0);
        GxRsSet(GxRs_Texture1, 0);

        if (s_vertexShader) {
            g_theGxDevicePtr->RsSet(GxRs_VertexShader, &s_vertexShader->m_permutes[0]);
            g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_pixelShader->m_permutes[0]);

            C44Matrix view;
            GxXformView(view);
            C44Matrix world;
            GxXform(GxXform_World, world);
            C44Matrix proj;
            GxXformProjNative(proj);
            C44Matrix transform = world * view * proj;
            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&transform), 4);
        }

        CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPC), count);
        CGxVertexPC* vertices = reinterpret_cast<CGxVertexPC*>(g_theGxDevicePtr->BufLock(buf));
        this->FillVertices(GxSwizzleColor(this->m_color), fade, vertices);

        if (static_cast<int32_t>(elapsed) > static_cast<int32_t>(this->m_duration >> 1)) {
            this->m_color.a -= fade;
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
        GxPrimVertexPtr(buf, GxVBF_PC);

        CGxBatch batch;
        batch.m_primType = GxPrim_TriangleStrip;
        batch.m_start = 0;
        batch.m_count = count;
        batch.m_minIndex = 0;
        batch.m_maxIndex = count - 1;
        g_theGxDevicePtr->DrawNonIndexed(batch);
        g_theGxDevicePtr->RsPop();
    }

    return true;
}

// OFFSET: 0x004E02C0/0x140120530
WTOBJECT* WeaponTrailCreate(CM2Model* model) {
    s_count++;
    WTOBJECT* trail = static_cast<WTOBJECT*>(s_freeTrails.GetData(0, typeid(WTOBJECT).raw_name(), SLOG_OBJECT));

    if (trail) {
        new (trail) WTOBJECT();
        trail->m_swing = NULL;
        trail->m_model = NULL;
    }

    trail->m_model = model;
    model->m_refCount++;
    return trail;
}

// OFFSET: 0x004E0310/0x140120590
static void WeaponTrailCallback(CM2Model* model, const CM2Lighting* lighting, void* arg) {
    if (!model->HasEvent('BTW$') || !model->HasEvent('TTW$')) {
        return;
    }

    uint32_t time = FrameTime::GetCurTimeMs();
    C3Vector bottom = model->GetEventPosition('BTW$');
    C3Vector top = model->GetEventPosition('TTW$');
    WTOBJECT* trail = static_cast<WTOBJECT*>(arg);

    if (!trail->m_swing->BuildVertexBuffer(time, top, bottom)) {
        s_freeSwings.PutData(trail->m_swing, NULL, 0);
        trail->m_swing = NULL;
        s_finishedTrails.LinkToTail(trail);
    }
}

// OFFSET: 0x004E03A0/0x1401206C0
void WTOBJECT::SetDrawTrail(const uint32_t* color, uint32_t duration) {
    if (this->m_swing) {
        this->m_swing->m_tail = 0;
        this->m_swing->m_head = 0;
    } else {
        this->m_swing = AllocSwing(0);
        CM2Model* model = this->m_model;
        model->m_drawCallback = WeaponTrailCallback;
        model->m_drawArg = this;
        model->m_drawFlags = (model->m_drawFlags & 0xFFF8) | 2;
    }

    *reinterpret_cast<uint32_t*>(&this->m_swing->m_color) = *color;
    this->m_swing->m_duration = duration;
    this->m_swing->m_startTime = FrameTime::GetCurTimeMs();
    this->m_swing->m_lastTime = this->m_swing->m_startTime;
}

// OFFSET: 0x004E0430/0x1401207B0
void WeaponTrailClose(WTOBJECT* trail) {
    s_count--;
    s_freeTrails.Delete(trail);
}

// OFFSET: 0x004E0460/0x1401207E0
void WeaponTrailSetDrawTrail(WTOBJECT* trail, const uint32_t* a2, uint32_t a3) {
    if (a3 && trail) {
        trail->SetDrawTrail(a2, a3);
    }
}
