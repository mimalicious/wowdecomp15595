#include <cstddef>
#include <cstdint>
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxLight.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

// STATIC: 0x00C98A80/0x000B300C0
static const char* s_shaderNames[GxVertexBufferFormats_Last] = { "Default_P", "Default_PN", "Default_PNC", "Default_PNT", "Default_PNCT", NULL, NULL, "Default_PNCT2", "Default_PC", "Default_PCT", NULL, NULL, NULL, "Default_PT", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

// STATIC: 0x00D380C8/0x000BF1B40
static CGxShader* s_pixelShaders[GxVertexBufferFormats_Last];

// STATIC: 0x00D38120/0x000BF1BF0
static CGxShader* s_vertexShaders[GxVertexBufferFormats_Last];

// STATIC: 0x00D38178/0x140BF1CA0
static uint32_t s_initCount;

static C4Vector s_fogParams;

// STATIC: 0x00D3818C/0x140BF1CB8
static C4Vector s_fogColor;

// OFFSET: 0x00430960/0x140038CD0
void DefaultShaders::GetFogColor(CImVector& color) {
    color.r = static_cast<uint8_t>(s_fogColor.x * 255.0f);
    color.g = static_cast<uint8_t>(s_fogColor.y * 255.0f);
    color.b = static_cast<uint8_t>(s_fogColor.z * 255.0f);
    color.a = 0xFF;
}

// OFFSET: 0x00430B10/0x140038E20
void DefaultShaders::Initialize() {
    if (s_initCount++) {
        return;
    }

    for (uint32_t i = 0; i < GxVertexBufferFormats_Last; i++) {
        const char* name = s_shaderNames[i];
        s_vertexShaders[i] = NULL;
        s_pixelShaders[i] = NULL;

        if (name) {
            s_vertexShaders[i] = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, name);
            s_pixelShaders[i] = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, s_shaderNames[i]);
        }
    }
}

// OFFSET: 0x00430B80/0x140038EE0
void DefaultShaders::Destroy() {
    if (--s_initCount) {
        return;
    }

    for (uint32_t i = 0; i < GxVertexBufferFormats_Last; i++) {
        if (s_vertexShaders[i]) {
            g_theGxDevicePtr->ShaderRelease(s_vertexShaders[i]);
            s_vertexShaders[i] = NULL;
        }

        if (s_pixelShaders[i]) {
            g_theGxDevicePtr->ShaderRelease(s_pixelShaders[i]);
            s_pixelShaders[i] = NULL;
        }
    }
}

// OFFSET: 0x00430C70/0x140039040
static int32_t SelectVertexPermute(EGxVertexBufferFormat format, uint32_t textureCount) {
    if (!s_vertexShaders[format]) {
        return -1;
    }

    CGxAppRenderState* states = g_theGxDevicePtr->m_appRenderStates.m_data;

    if (states[GxRs_ColorMaterial].m_value.m_data.i[0]) {
        return -1;
    }

    int32_t lighting = states[GxRs_Lighting].m_value.m_data.i[0];

    if (lighting) {
        if (!g_theGxDevicePtr->GetLightEnabled(0)) {
            return -1;
        }

        for (uint32_t i = 1; i < 4; i++) {
            if (g_theGxDevicePtr->GetLightEnabled(i)) {
                return -1;
            }
        }

        CGxLight light;
        g_theGxDevicePtr->Light(0, light);

        if (light.m_flags & 0x2) {
            return -1;
        }
    }

    int32_t textureShader = 0;

    for (uint32_t i = 0; i < 4; i++) {
        if (!GxVertexHasAttrib(format, static_cast<EGxVertexAttrib>(GxVA_TexCoord0 + i))) {
            break;
        }

        states = g_theGxDevicePtr->m_appRenderStates.m_data;

        if (states[GxRs_TexGen0 + i].m_value.m_data.i[0]) {
            return -1;
        }

        int32_t shader = states[GxRs_TextureShader0 + i].m_value.m_data.i[0];

        if (shader == 2) {
            return -1;
        }

        if (i) {
            if (textureShader != shader) {
                return -1;
            }
        } else {
            textureShader = states[GxRs_TextureShader0].m_value.m_data.i[0];
        }

        if (states[GxRs_TextureCoord0 + i].m_value.m_data.i[0] != static_cast<int32_t>(i) || states[GxRs_ColorOp0 + i].m_value.m_data.i[0] || states[GxRs_AlphaOp0 + i].m_value.m_data.i[0]) {
            return -1;
        }
    }

    return lighting + 2 * (textureCount ? textureCount - 1 : 0);
}

// OFFSET: 0x00430D90/0x1400391B0
static int32_t SelectPixelPermute(EGxVertexBufferFormat format, uint32_t textureCount) {
    if (!s_pixelShaders[format]) {
        return -1;
    }

    CGxAppRenderState* states = g_theGxDevicePtr->m_appRenderStates.m_data;

    for (uint32_t i = 0; i < textureCount; i++) {
        if (!states[GxRs_Texture0 + i].m_value.m_data.p) {
            break;
        }

        if (states[GxRs_ColorOp0 + i].m_value.m_data.i[0] || states[GxRs_AlphaOp0 + i].m_value.m_data.i[0]) {
            return -1;
        }
    }

    if (textureCount) {
        textureCount--;
    }

    states = g_theGxDevicePtr->m_appRenderStates.m_data;
    int32_t alphaTest = states[GxRs_BlendingMode].m_value.m_data.i[0] == 1 || states[GxRs_AlphaRef].m_value.m_data.i[0] > 0;

    return alphaTest + 2 * textureCount;
}

// OFFSET: 0x00430E10/0x140039230
static void SetVertexConstants(int32_t permute, EGxVertexBufferFormat format) {
    CGxDevice* device = g_theGxDevicePtr;

    C44Matrix world;
    world = device->m_xforms[GxXform_World].m_mtx[device->m_xforms[GxXform_World].m_level];

    C44Matrix view;
    view = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];

    C44Matrix projection;
    projection = device->m_projNative;

    if (device->DeviceApi() == GxApi_OpenGl) {
        projection.c0 = projection.c0 * -1.0f;
        projection.c1 = projection.c1 * -1.0f;
        projection.c2 = projection.c2 * -1.0f;
        projection.c3 = projection.c3 * -1.0f;
    }

    C44Matrix worldView = world * view;
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&worldView), 4);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 4, reinterpret_cast<const float*>(&projection), 4);

    device = g_theGxDevicePtr;

    C44Matrix texMatrix0;
    C44Matrix texMatrix1;

    CGxAppRenderState* states = device->m_appRenderStates.m_data;
    int32_t textureShader1 = states[GxRs_TextureShader0 + 1].m_value.m_data.i[0];

    if (states[GxRs_TextureShader0].m_value.m_data.i[0]) {
        texMatrix0 = device->m_xforms[GxXform_Tex0].m_mtx[device->m_xforms[GxXform_Tex0].m_level];
    }

    if (textureShader1) {
        texMatrix1 = device->m_xforms[GxXform_Tex1].m_mtx[device->m_xforms[GxXform_Tex1].m_level];
    }

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 8, reinterpret_cast<const float*>(&texMatrix0), 4);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 12, reinterpret_cast<const float*>(&texMatrix1), 4);

    CImVector emissiveColor;
    *reinterpret_cast<uint32_t*>(&emissiveColor) = g_theGxDevicePtr->m_appRenderStates.m_data[GxRs_MatEmissive].m_value.m_data.u[0];
    C4Vector emissive(emissiveColor);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 16, &emissive.x, 1);

    CImVector diffuseColor;
    *reinterpret_cast<uint32_t*>(&diffuseColor) = g_theGxDevicePtr->m_appRenderStates.m_data[GxRs_MatDiffuse].m_value.m_data.u[0];
    C4Vector diffuse(diffuseColor);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 17, &diffuse.x, 1);

    C4Vector fogParams = s_fogParams;

    if (s_fogColor.w == 0.0f) {
        fogParams.x = 0.0f;
        fogParams.y = 1.0f;
        fogParams.z = 1.0f;
        fogParams.w = 0.0f;
    }

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 19, &fogParams.x, 1);

    if (g_theGxDevicePtr->m_appRenderStates.m_data[GxRs_Lighting].m_value.m_data.i[0] && g_theGxDevicePtr->GetLightEnabled(0)) {
        CGxLight light;
        g_theGxDevicePtr->Light(0, light);

        C4Vector direction;
        direction.x = -light.m_dir.x;
        direction.y = -light.m_dir.y;
        direction.z = -light.m_dir.z;
        direction.w = 0.0f;
        direction = direction * view;

        C4Vector ambient;
        ambient.x = light.m_ambientColor.x;
        ambient.y = light.m_ambientColor.y;
        ambient.z = light.m_ambientColor.z;
        ambient.w = 0.0f;

        C4Vector dirColor;
        dirColor.x = light.m_dirColor.x;
        dirColor.y = light.m_dirColor.y;
        dirColor.z = light.m_dirColor.z;
        dirColor.w = 0.0f;

        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 20, &direction.x, 1);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 21, &ambient.x, 1);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 22, &dirColor.x, 1);
    }

    GxRsSet(GxRs_VertexShader, s_vertexShaders[format], permute);
}

// OFFSET: 0x00431580/0x140039D80
static void SetPixelConstants(int32_t permute, EGxVertexBufferFormat format) {
    if (g_theGxDevicePtr->m_caps.unk_02FC) {
        GxRsSet(GxRs_Fog, static_cast<int32_t>(s_fogColor.w != 0.0f));
    }

    CImVector fogColor;
    *reinterpret_cast<uint32_t*>(&fogColor) = g_theGxDevicePtr->m_appRenderStates.m_data[GxRs_FogColor].m_value.m_data.u[0];
    C4Vector fog(fogColor);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &fog.x, 1);

    float alphaRef[4];
    alphaRef[0] = g_theGxDevicePtr->m_appRenderStates.m_data[GxRs_AlphaRef].m_value.m_data.i[0] * 0.0039215689f;
    alphaRef[1] = 0.0f;
    alphaRef[2] = 0.0f;
    alphaRef[3] = 0.0f;
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 1, alphaRef, 1);

    GxRsSet(GxRs_PixelShader, s_pixelShaders[format], permute);
}

// OFFSET: 0x00431690/0x140039F00
void DefaultShaders::SetFogParams(float start, float end, float density, const CImVector& color) {
    s_fogColor.x = color.r * 0.0039215689f;
    s_fogColor.y = color.g * 0.0039215689f;
    s_fogColor.z = color.b * 0.0039215689f;

    if (CShaderEffect::s_enableShaders) {
        float scale = 1.0f / (end - start);
        s_fogParams.x = -(CShaderEffect::s_fogMul * scale);
        s_fogParams.y = scale * end;
        s_fogParams.z = density;
        s_fogParams.w = 0.0f;

        if (!g_theGxDevicePtr->m_caps.unk_02F8) {
            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &s_fogColor.x, 1);
            return;
        }
    } else {
        GxRsSet(GxRs_FogStart, start);
        GxRsSet(GxRs_FogEnd, end);
    }

    GxRsSet(GxRs_FogColor, *reinterpret_cast<const int32_t*>(&color));
}

// OFFSET: 0x00431840/0x14003A0E0
void DefaultShaders::SetFogEnabled(int32_t enabled) {
    s_fogColor.w = enabled && g_theGxDevicePtr->MasterEnable(GxMasterEnable_Fog) ? 1.0f : 0.0f;

    if (!CShaderEffect::s_enableShaders || g_theGxDevicePtr->m_caps.unk_02FC) {
        GxRsSet(GxRs_Fog, enabled);
    }
}

// OFFSET: 0x004318B0
static uint32_t CountTextures(uint32_t textureCount) {
    if (textureCount != 0xFFFFFFFF) {
        return textureCount;
    }

    uint32_t count = 0;

    do {
        if (!g_theGxDevicePtr->m_appRenderStates.m_data[GxRs_Texture0 + count].m_value.m_data.p) {
            break;
        }

        count++;
    } while (count < 3);

    return count;
}

// OFFSET: 0x004318E0/0x14003A180
int32_t DefaultShaders::SetDefaultShaders(EGxVertexBufferFormat format, uint32_t textureCount) {
    if (!CShaderEffect::s_enableShaders) {
        return 0;
    }

    if (format == GxVertexBufferFormats_Last) {
        format = g_theGxDevicePtr->PrimVertexFormat();
    }

    uint32_t count = CountTextures(textureCount);

    if (count > 2) {
        return 0;
    }

    int32_t vertexPermute = SelectVertexPermute(format, count);
    int32_t pixelPermute = SelectPixelPermute(format, count);

    if (vertexPermute < 0 || pixelPermute < 0) {
        return 0;
    }

    SetVertexConstants(vertexPermute, format);
    SetPixelConstants(pixelPermute, format);

    return 1;
}

// OFFSET: 0x00431960/0x14003A240
int32_t DefaultShaders::SetPixelShader(EGxVertexBufferFormat format, uint32_t textureCount) {
    if (!CShaderEffect::s_enableShaders) {
        return 0;
    }

    uint32_t count = CountTextures(textureCount);

    if (count > 2) {
        return 0;
    }

    int32_t pixelPermute = SelectPixelPermute(format, count);

    if (pixelPermute < 0) {
        return 0;
    }

    SetPixelConstants(pixelPermute, format);

    return 1;
}
