#include "PassFogSeed.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Tempest/CImVector.h"

// STATIC: 0x00D27BC0
int32_t PassFogSeed::s_deviceReset = 1;

// PROVEN: ccf39bf9 IDENTICAL/NONE
// OFFSET: 0x00B0B740
void __fastcall PassFogSeed::DeviceRestoredCallback() {
    PassFogSeed::s_deviceReset = 1;
}

// OFFSET: 0x00B0B7B0
PassFogSeed::PassFogSeed(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_row = 0;
    this->m_color = CImVector(0, 0, 0, 0);
    PassFogSeed::s_deviceReset = 1;
    this->m_shader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXFogSeed");
    this->m_permute = FFX::s_useRECT != 0;
    g_theGxDevicePtr->AddDeviceRestoredCallback(PassFogSeed::DeviceRestoredCallback);
}

// OFFSET: 0x00B0CA20
PassFogSeed::~PassFogSeed() {
    if (this->m_shader) {
        g_theGxDevicePtr->ShaderRelease(this->m_shader);
        this->m_shader = NULL;
    }

    g_theGxDevicePtr->RemoveDeviceRestoredCallback(PassFogSeed::DeviceRestoredCallback);
}

// PROVEN: 4f1f4334 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool PassFogSeed::Valid() {
    return true;
}

// OFFSET: 0x00B0CA80
void PassFogSeed::Execute() {
    struct Vertex {
        C3Vector p;
        CImVector c;
        C2Vector tc;
    };

    FFX::SetupGx(this->m_output, 0);

    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(Vertex), 4);
    Vertex* vertex = reinterpret_cast<Vertex*>(GxBufLock(vertexBuf));

    float top = this->m_output->m_texelSize.y * 2.0f - 1.0f;
    vertex[0].p = C3Vector(-1.0f, -1.0f, 0.0f);
    vertex[1].p = C3Vector(-1.0f, top, 0.0f);
    vertex[2].p = C3Vector(1.0f, top, 0.0f);
    vertex[3].p = C3Vector(1.0f, -1.0f, 0.0f);

    FFX::Tex* input = this->m_inputs[0];
    float invTexWidth = 1.0f / input->m_texSize.x;
    float invTexHeight = 1.0f / input->m_texSize.y;
    float height = static_cast<float>(input->m_size.y);
    float left = invTexWidth * 0.5f;
    float right = invTexWidth * (input->m_size.x + 0.5f);
    float bottom = invTexHeight * 0.5f;
    float row = bottom + (invTexHeight * (height + 0.5f) - bottom) / height * this->m_row;
    vertex[0].tc = C2Vector(left, row);
    vertex[1].tc = C2Vector(left, row);
    vertex[2].tc = C2Vector(right, row);
    vertex[3].tc = C2Vector(right, row);

    CImVector color;

    if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
        color.b = this->m_color.r;
        color.g = this->m_color.g;
        color.r = this->m_color.b;
        color.a = this->m_color.a;
    } else {
        color = this->m_color;
    }

    vertex[0].c = color;
    vertex[1].c = color;
    vertex[2].c = color;
    vertex[3].c = color;

    GxBufUnlock(vertexBuf, 0);
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_PCT);

    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 6);
    uint16_t* index = reinterpret_cast<uint16_t*>(GxBufLock(indexBuf));
    index[0] = 0;
    index[1] = 1;
    index[3] = 1;
    index[2] = 3;
    index[5] = 3;
    index[4] = 2;
    GxBufUnlock(indexBuf, 0);
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);

    if (PassFogSeed::s_deviceReset) {
        GxSceneClear(15, CImVector(0, 0, 0, 0));
        PassFogSeed::s_deviceReset = 0;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture));

    for (int32_t state = GxRs_Texture1; state <= GxRs_Texture15; state++) {
        GxRsSet(static_cast<EGxRenderState>(state), 0);
    }

    if (FFX::s_passThrough_PCT) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &FFX::s_passThrough_PCT->m_permutes[0]);
    } else {
        GxRsSet(GxRs_VertexShader, 0);
    }

    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_shader->m_permutes[this->m_permute]);
    GxRsSet(GxRs_BlendingMode, this->m_blend);
    GxUpdateAlphaRef(GxRs_AlphaRef);

    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = 6;
    batch.m_minIndex = 0;
    batch.m_maxIndex = 3;
    g_theGxDevicePtr->Draw(batch);

    FFX::SetupReset();

    if (this->m_row < static_cast<uint32_t>(this->m_inputs[0]->m_texSize.y)) {
        this->m_row++;
    } else {
        this->m_row = 0;
    }
}
