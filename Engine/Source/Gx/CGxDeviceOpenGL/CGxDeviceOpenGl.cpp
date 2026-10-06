#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <malloc.h>
#include <new>
#include "Engine/Source/Gx/CGxDeviceOpenGL/CGxDeviceOpenGl.h"
#include "Engine/Source/Gx/CGxDeviceOpenGL/CGxOglTables.h"
#include "Engine/Source/Gx/CGxDevice/Win32/GxWindow.h"
#include "Engine/Source/Gx/CGxDeviceOpenGL/CGxVboBroker1.h"
#include "Engine/Source/Gx/CGxDeviceOpenGL/Types.h"
#include "Engine/Source/Gx/Blit.h"
#include "Engine/Source/Gx/Buffer.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00AFBF9C
static const GLenum s_glBufferTarget[GxPoolTargets_Last] = { GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB };

// STATIC: %0x00AFBFA4
static const CGxDeviceOpenGl::EDeviceState s_bufferState[GxPoolTargets_Last] = { CGxDeviceOpenGl::Ds_ArrayBuffer, CGxDeviceOpenGl::Ds_ElementArrayBuffer };

// STATIC: %0x00AFBFAC
static const GLenum s_glBufferUsage[3] = { GL_STATIC_DRAW_ARB, GL_DYNAMIC_DRAW_ARB, GL_STREAM_DRAW_ARB };

// STATIC: 0x00BD4DC8/0x0009C9D70
const GLenum s_glTexTarget[4] = { GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_2D };

// STATIC: %0x00AFC014
static const GLenum s_glQueryTarget[1] = { GL_SAMPLES_PASSED_ARB };

// STATIC: %0x00AFC020
static const GLenum s_glAttachment[GxBuffers_Last] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT, GL_DEPTH_ATTACHMENT_EXT };

// STATIC: %0x00AFD3C8
const GLint s_glInternalFormat[GxTexFormats_Last] = { 0, 0x8058, 0x8058, 0x8056, 0x8057, 0x8050, 0x8709, 0x83F1, 0x83F2, 0x83F3, 0, 0x881B, 0, 0, 0, 0, 0, 0x81A6, 0x81A5, 0x81A7, 0, 0, 0, 0 };

// STATIC: %0x00AFD488
const GLenum s_glFormat[GxTexFormats_Last] = { 0, 0x1908, 0x80E1, 0x80E1, 0x80E1, 0x1907, 0x86F5, 0, 0, 0, 0x1907, 0x1907, 0x1908, 0x1907, 0x1907, 0x1908, 0, 0x1902, 0x1902, 0x1902, 0, 0, 0, 0 };

// STATIC: %0x00AFD4E8
const GLenum s_glType[GxTexFormats_Last] = { 0, 0x8367, 0x8367, 0x8365, 0x8366, 0x8363, 0x1400, 0, 0, 0, 0, 0x140B, 0x140B, 0, 0, 0, 0, 0x1405, 0x1405, 0x1405, 0, 0, 0, 0 };

// STATIC: %0x00D92EE0
static bool s_dummyContextCreated;

// STATIC: 0x00EC66BC/0x140DB8824
static int32_t s_deviceChanging;

// STATIC: %0x00C0E904
GLint s_glWrapMode[3] = { GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT_ARB };

// STATIC: %0x00C0E7D8
static int32_t s_firstAttach = 1;

// STATIC: %0x00D92EA4
static EmergencyMem s_emergencyMem[GxPoolTargets_Last];

// OFFSET: %0x007B0190
CGxDevice* CreateCGxDeviceOpenGl() {
    void* mem = SMemAlloc(sizeof(CGxDeviceOpenGl), __FILE__, __LINE__, 0);

    if (!mem) {
        return NULL;
    }

    return new (mem) CGxDeviceOpenGl();
}

// OFFSET: %0x007AF690
CGxDeviceOpenGl::CGxDeviceOpenGl() {
    this->m_api = GxApi_OpenGl;
    this->m_hwnd = NULL;
    this->m_ownhwnd = 0;
    this->m_hdc = NULL;
    this->m_hglrc = NULL;
    this->m_pbuffer = NULL;
    this->m_pbufferDC = NULL;
    this->m_pbufferRC = NULL;
    this->m_cursor = NULL;
    this->m_fbo = 0;
    this->m_boundFbo = 0;
    this->m_viewDirty = 0;
    this->m_caps.m_colorFormat = GxCF_rgba;
    this->m_caps.unk_001C = 0;
    this->m_renderTargetMode = 0;
    this->m_ambientOnlyMode = 0;
    this->m_attribDirty = 0;
    this->m_curVertexFormat = GxVertexBufferFormats_Last;
    this->m_curVertexBuf = NULL;
    this->DsInit();
    this->unk_2938 = 0;
    this->m_materialDirty = 7;
    this->m_sphereMapMode = GL_SPHERE_MAP;

    for (uint32_t target = 0; target < GxPoolTargets_Last; target++) {
        for (uint32_t usage = 0; usage < 3; usage++) {
            this->m_useVbo[target][usage] = 1;
            this->m_useVboBroker[target][usage] = 0;
        }
    }

    this->m_boundTexture = 0xFFFFFFFF;
    this->DeviceCreatePools();
    this->DeviceCreateStreamBufs();
    this->m_glVendor = NULL;
    this->m_glRenderer = NULL;
    this->m_glVersion = NULL;
}

// OFFSET: %0x007B01C0
CGxDeviceOpenGl::~CGxDeviceOpenGl() {
    this->m_glVendor = NULL;
    this->m_glRenderer = NULL;
    this->m_glVersion = NULL;
}

// PROVEN: 3b4d5cbc IDENTICAL/DIFFERENT
// OFFSET: 0x0084E2C0/0x14057D070
void CGxDeviceOpenGl::DeviceOverride(EGxOverride override, uint32_t value) {
    CGxDevice::DeviceOverride(override, value);

    switch (override) {
        case 1:
            if (!value) {
                glARBVertexBufferObject = 0;
            }

            break;

        case 2:
            if (!value) {
                this->m_useVbo[0][2] = 0;
            }

            break;

        case 3:
            if (!value) {
                this->m_useVbo[1][2] = 0;
            }

            break;

        case 4:
            if (value) {
                this->m_useVboBroker[0][2] = 1;
            }

            break;

        case 5:
            if (value) {
                this->m_useVboBroker[1][2] = 1;
            }

            break;

        case 6:
            this->m_sphereMapMode = value != 0 ? GL_REFLECTION_MAP_ARB : GL_SPHERE_MAP;
            break;

        case 7:
            this->m_caps.m_texNonPow2 = value != 0;
            break;

        default:
            break;
    }
}

// OFFSET: %0x007B0210
void CGxDeviceOpenGl::LogCrashInfo(char* buffer, uint32_t size) {
    char line[260];

    if (this->m_glVendor) {
        SStrPrintf(line, sizeof(line), "Vendor: %s\r\n", this->m_glVendor);
        SStrPack(buffer, line, size);
    }

    if (this->m_glRenderer) {
        SStrPrintf(line, sizeof(line), "Renderer: %s\r\n", this->m_glRenderer);
        SStrPack(buffer, line, size);
    }

    if (this->m_glVersion) {
        SStrPrintf(line, sizeof(line), "Version: %s\r\n", this->m_glVersion);
        SStrPack(buffer, line, size);
    }
}

// OFFSET: %0x007AF870
void CGxDeviceOpenGl::ISetCaps(const CGxFormat& format) {
    this->m_caps.m_numTmus = glMaxTextureUnits >= 8 ? 8 : glMaxTextureUnits;
    this->m_caps.m_numVertexTmus = 0;
    this->m_caps.m_numShaderTmus = glMaxTextureUnits >= 16 ? 16 : glMaxTextureUnits;

    if (glARBFragmentProgram) {
        this->m_caps.m_numShaderTmus = glMaxTextureImageUnitsARB >= 16 ? 16 : glMaxTextureImageUnitsARB;
    }

    this->m_caps.m_pixelCenterOnEdge = 0;
    this->m_caps.m_texelCenterOnEdge = 1;

    GLint maxSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
    this->m_caps.m_texMaxSize[GxTex_2d] = maxSize < 256 ? 256 : maxSize;
    this->m_caps.m_texTarget[GxTex_2d] = 1;

    maxSize = 0;
    GLint cubeSize;

    if (!glARBTextureCubeMap || (glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, &maxSize), cubeSize = maxSize, maxSize < 256)) {
        cubeSize = 256;
    }

    this->m_caps.m_texMaxSize[GxTex_CubeMap] = cubeSize;

    GLint rectangleSize = 0;
    this->m_caps.m_texTarget[GxTex_CubeMap] = glARBTextureCubeMap;
    maxSize = 0;

    GLint rectangleCap;

    if (glARBTextureRectangle && (glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB, &maxSize), rectangleSize = maxSize, maxSize >= 256)) {
        rectangleCap = maxSize;
    } else {
        rectangleCap = 256;
    }

    this->m_caps.m_texMaxSize[GxTex_Rectangle] = rectangleCap;
    this->m_caps.m_texTarget[GxTex_Rectangle] = glARBTextureRectangle;
    this->m_caps.m_texNonPow2 = 1;

    if (glARBFragmentProgram || glNVTextureShader || glNVTextureShader2 || glNVTextureShader3) {
        this->m_caps.m_texNonPow2 = 0;
    }

    if (rectangleSize < 256) {
        rectangleSize = 256;
    }

    this->m_caps.m_texMaxSize[GxTex_NonPow2] = rectangleSize;
    this->m_caps.m_texTarget[GxTex_NonPow2] = glARBTextureNonPowerOfTwo;
    this->m_caps.m_texMaxSize[GxTex_Volume] = 256;
    this->m_caps.m_texTarget[GxTex_Volume] = 0;
    this->m_caps.m_texTarget[GxTex_5] = 0;
    this->m_caps.m_texMaxSize[GxTex_5] = this->m_caps.m_texMaxSize[GxTex_2d];
    this->m_caps.m_texTarget[GxTex_6] = 0;
    this->m_caps.m_texMaxSize[GxTex_6] = 0;

    for (uint32_t i = 0; i < GxTexFormats_Last; i++) {
        this->m_caps.m_vertexTexFmt[i] = 0;
        this->m_caps.m_rttFormat[i] = 0;
        this->m_caps.m_pcfFormat[i] = 0;
        this->m_caps.m_texFmt[i] = s_glType[i] != 0;
    }

    this->m_caps.m_texFmt[GxTex_Dxt1] = glExtTextureCompressionS3tc;
    this->m_caps.m_texFmt[GxTex_Dxt3] = glExtTextureCompressionS3tc;
    this->m_caps.m_texFmt[GxTex_Dxt5] = glExtTextureCompressionS3tc;
    this->m_caps.m_texFmt[GxTex_Uv88] = glNVTextureShader;

    GLint numStreams = this->m_caps.m_numTmus + 3;

    if (glARBVertexProgram) {
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB, &numStreams);
    }

    this->m_caps.m_numStreams = numStreams;

    for (uint32_t i = 0; i < GxShTargets_Last; i++) {
        this->m_caps.m_shaderTargets[i] = 0;
        this->m_caps.m_shaderConstants[i] = 0;
    }

    for (uint32_t i = 0; i < GxShProfiles_Last; i++) {
        this->m_caps.m_shaderProfiles[i] = 0;
    }

    this->m_caps.m_shaderProfiles[GxShProfile_VsFf] = 1;
    this->m_caps.m_shaderProfiles[GxShProfile_PsFf] = 1;

    if (glARBVertexProgram) {
        this->m_caps.m_shaderProfiles[GxShProfile_Arbvp1] = 1;
    }

    if (glNVRegisterCombiners) {
        this->m_caps.m_shaderProfiles[GxShProfile_Fp10] = 1;
    }

    if (glNVRegisterCombiners2) {
        if (glNVTextureShader && glNVTextureShader2) {
            this->m_caps.m_shaderProfiles[GxShProfile_Fp20] = 1;
        }

        if (glNVTextureShader3) {
            this->m_caps.m_shaderProfiles[GxShProfile_Fp25] = 1;
        }
    }

    if (glARBFragmentProgram) {
        this->m_caps.m_shaderProfiles[GxShProfile_Arbfp1] = 1;
    }

    this->ChooseShaderModel(format.m_shaderModel);

    switch (this->m_caps.m_shaderTargets[GxSh_Vertex]) {
        case GxShProfile_Arbvp1:
        case GxShProfile_Arbvp1Cg12: {
            GLint envParameters;
            GLint nativeParameters;
            glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB, &envParameters);
            glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB, &nativeParameters);
            this->m_caps.m_shaderConstants[GxSh_Vertex] = envParameters >= nativeParameters ? nativeParameters : envParameters;
            break;
        }

        case GxShProfile_Vp20:
            this->m_caps.m_shaderConstants[GxSh_Vertex] = 96;
            break;

        case GxShProfile_Vp30:
            this->m_caps.m_shaderConstants[GxSh_Vertex] = 256;
            break;

        default:
            break;
    }

    this->m_caps.m_texFilterTrilinear = 1;

    if (glExtTextureFilterAnisotropic) {
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, reinterpret_cast<GLint*>(&this->m_caps.m_maxTexAnisotropy));

        if (this->m_caps.m_maxTexAnisotropy > 1) {
            this->m_caps.m_texFilterAnisotropic = 1;
        }
    }

    this->m_caps.m_colorWrite = 1;

    GLint maxClipPlanes = 0;
    glGetIntegerv(GL_MAX_CLIP_PLANES, &maxClipPlanes);
    this->m_caps.unk_02BC = 0;
    this->m_caps.m_maxClipPlanes = maxClipPlanes != 0;

    if (this->m_caps.m_shaderTargets[GxSh_Vertex] == GxShProfile_Arbvp1 || this->m_caps.m_shaderTargets[GxSh_Vertex] == GxShProfile_Arbvp1Cg12) {
        this->m_caps.m_maxClipPlanes = 0;
    }

    this->m_caps.m_occlusionQuery = glARBOcclusionQuery;
    this->m_caps.m_hwCursor = 1;
    this->m_caps.m_pointParameters = glARBPointParameters;
    this->m_caps.m_pointScaleMax = 1.0f;

    if (glARBPointParameters) {
        GLfloat pointSizeMax;
        glGetFloatv(GL_POINT_SIZE_MAX_ARB, &pointSizeMax);
        this->m_caps.m_pointScaleMax = pointSizeMax;
    }

    this->m_caps.m_pointSprite = glARBPointSprite;

    if (!(this->m_caps.m_pointScaleMax > 1.0f)) {
        this->m_caps.m_pointParameters = 0;
        this->m_caps.m_pointSprite = 0;
    }

    this->m_caps.unk_02D8[1] = 0;
    this->m_caps.unk_02D8[0] = 0;
    this->m_caps.unk_02D8[2] = 0;
    this->m_caps.unk_02D8[3] = 0;
    this->m_caps.unk_02D8[4] = 0;
    this->m_caps.unk_02FC = this->m_caps.m_shaderTargets[GxSh_Pixel] != GxShProfile_Arbfp1;
    this->m_caps.m_stereoAvailable = 0;
    this->m_caps.unk_02F4 = 1;
    this->m_caps.unk_02F8 = 1;
    this->m_caps.unk_0300 = 0;
    this->m_caps.unk_0314 = 0;
    this->m_caps.unk_0304 = 0;

    uint16_t vendorID = 0;
    uint16_t deviceID = 0;
    uint32_t driverVersionHi;
    uint32_t driverVersionLo;

    if (!CGxDevice::AdapterID(vendorID, deviceID, driverVersionHi, driverVersionLo)) {
        CGxDevice::Log("Failed to determine vendor and device id");
    }

    this->m_caps.m_vendorId = vendorID;
    this->m_caps.m_deviceId = deviceID;
    this->Log(this->m_caps);
}

// OFFSET: %0x007AFDD0
void CGxDeviceOpenGl::DestroyPoolAPI(CGxPool* pool) {
    if (!pool) {
        return;
    }

    pool->Invalidate();

    if (!pool->m_apiSpecific) {
        return;
    }

    if (glARBVertexBufferObject) {
        if (this->m_useVboBroker[pool->m_target][pool->m_usage]) {
            EDeviceState state = s_bufferState[pool->m_target];

            if (this->m_deviceStates[state] == static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pool->m_apiSpecific))) {
                this->DsSet(state, 0, 0);
            }

            if (pool->m_apiSpecific) {
                delete static_cast<CGxVboBroker*>(pool->m_apiSpecific);
            }

            pool->m_apiSpecific = NULL;
            return;
        }

        if (this->m_useVbo[pool->m_target][pool->m_usage]) {
            EDeviceState state = s_bufferState[pool->m_target];

            if (this->m_deviceStates[state] == static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pool->m_apiSpecific))) {
                this->DsSet(state, 0, 0);
            }

            glDeleteBuffersARB(1, reinterpret_cast<GLuint*>(&pool->m_apiSpecific));
            pool->m_apiSpecific = NULL;
            return;
        }
    }

    SMemFree(pool->m_apiSpecific, __FILE__, __LINE__, 0);
    pool->m_apiSpecific = NULL;
}

// PROVEN: d1bae711 IDENTICAL/IDENTICAL
// OFFSET: 0x0084EA60/0x14057D830
void CGxDeviceOpenGl::PoolDestroy(CGxPool* pool) {
    this->DestroyPoolAPI(pool);
    CGxDevice::PoolDestroy(pool);
}

// PROVEN: 3ab52cc6 IDENTICAL/NONE
// OFFSET: 0x0084EA80
void CGxDeviceOpenGl::QueryCreate(CGxQuery*& query, EGxQueryType type) {
    CGxDevice::QueryCreate(query, type);

    CGxQuery* created = query;

    if (!created->m_apiSpecific && glARBOcclusionQuery) {
        GLuint id;
        glGenQueriesARB(1, &id);
        created->m_apiSpecific = reinterpret_cast<void*>(static_cast<uintptr_t>(id));
    }
}

// PROVEN: 58d0af76 IDENTICAL/IDENTICAL
// OFFSET: 0x0084EAC0/0x14057D8A0
void CGxDeviceOpenGl::QueryDestroy(CGxQuery*& query) {
    CGxQuery* destroyed = query;

    if (destroyed->m_apiSpecific) {
        GLuint id = static_cast<GLuint>(reinterpret_cast<uintptr_t>(destroyed->m_apiSpecific));
        glDeleteQueriesARB(1, &id);
        destroyed->m_apiSpecific = NULL;
    }

    CGxDevice::QueryDestroy(query);
}

// PROVEN: 29563168 IDENTICAL/NONE
// OFFSET: 0x0084EB00
int32_t CGxDeviceOpenGl::QueryBegin(CGxQuery* query) {
    if (!this->m_context) {
        return 0;
    }

    if (!query->m_apiSpecific) {
        if (glARBOcclusionQuery) {
            GLuint id;
            glGenQueriesARB(1, &id);
            query->m_apiSpecific = reinterpret_cast<void*>(static_cast<uintptr_t>(id));
        }

        if (!query->m_apiSpecific) {
            return 0;
        }
    }

    glBeginQueryARB(s_glQueryTarget[query->m_type], static_cast<GLuint>(reinterpret_cast<uintptr_t>(query->m_apiSpecific)));
    return glGetError() == GL_NO_ERROR;
}

// PROVEN: 933d8e32 IDENTICAL/NONE
// OFFSET: 0x0084EB70
int32_t CGxDeviceOpenGl::QueryEnd(CGxQuery* query) {
    if (!this->m_context) {
        return 0;
    }

    if (!query->m_apiSpecific) {
        return 0;
    }

    glEndQueryARB(s_glQueryTarget[query->m_type]);
    return glGetError() == GL_NO_ERROR;
}

// PROVEN: f906afd0 IDENTICAL/NONE
// OFFSET: 0x0084EBB0
int32_t CGxDeviceOpenGl::QueryGetParam(CGxQuery* query, EGxQueryParam param, uint32_t* data) {
    if (!this->m_context) {
        return 0;
    }

    if (!query->m_apiSpecific) {
        return 0;
    }

    GLenum error = GL_INVALID_OPERATION;

    switch (param) {
        case GxQueryParam_Unk0: {
            GLint bits;
            glGetQueryivARB(s_glQueryTarget[query->m_type], GL_QUERY_COUNTER_BITS_ARB, &bits);
            error = glGetError();

            if (!error) {
                *data = bits / 8;
            }

            break;
        }

        case GxQueryParam_Unk1: {
            GLuint available;
            glGetQueryObjectuivARB(static_cast<GLuint>(reinterpret_cast<uintptr_t>(query->m_apiSpecific)), GL_QUERY_RESULT_AVAILABLE_ARB, &available);
            error = glGetError();

            if (!error) {
                *data = available;
            }

            break;
        }

        default:
            break;
    }

    return error == GL_NO_ERROR;
}

// PROVEN: ae17b8a1 IDENTICAL/NONE
// OFFSET: 0x0084EC60
int32_t CGxDeviceOpenGl::QueryGetData(CGxQuery* query, uint32_t* data) {
    if (!this->m_context) {
        return 0;
    }

    if (!query->m_apiSpecific) {
        return 0;
    }

    glGetQueryObjectuivARB(static_cast<GLuint>(reinterpret_cast<uintptr_t>(query->m_apiSpecific)), GL_QUERY_RESULT_ARB, reinterpret_cast<GLuint*>(data));
    GLenum error = glGetError();

    if (!error && !query->m_type) {
        *data /= this->m_format.m_multisampleCount;
    }

    return error == GL_NO_ERROR;
}

// OFFSET: %0x007B0370
void CGxDeviceOpenGl::CreatePoolAPI(CGxPool* pool) {
    if (glARBVertexBufferObject) {
        if (this->m_useVboBroker[pool->m_target][pool->m_usage]) {
            uint32_t maxBits = 0;

            if (pool->m_target == GxPoolTarget_Vertex) {
                maxBits = 21;
            } else if (pool->m_target == GxPoolTarget_Index) {
                maxBits = 18;
            }

            void* mem = SMemAlloc(sizeof(CGxVboBroker1), __FILE__, __LINE__, 0);
            pool->m_apiSpecific = mem ? new (mem) CGxVboBroker1(s_glBufferTarget[pool->m_target], maxBits, this) : NULL;
            return;
        }

        if (this->m_useVbo[pool->m_target][pool->m_usage]) {
            if (this->m_context) {
                glGenBuffersARB(1, reinterpret_cast<GLuint*>(&pool->m_apiSpecific));

                if (pool->m_apiSpecific) {
                    this->DsSet(s_bufferState[pool->m_target], static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pool->m_apiSpecific)), 0);
                    glBufferDataARB(s_glBufferTarget[pool->m_target], pool->m_size, NULL, s_glBufferUsage[pool->m_usage]);
                }
            }

            return;
        }
    }

    pool->m_apiSpecific = SMemAlloc(pool->m_size, __FILE__, __LINE__, 0);
}

// PROVEN: 3d600ffa IDENTICAL/NONE
// OFFSET: 0x0084F010
void CGxDeviceOpenGl::PoolSizeSet(CGxPool* pool, uint32_t size) {
    if (glARBVertexBufferObject) {
        if (this->m_useVboBroker[pool->m_target][pool->m_usage]) {
            this->DestroyPoolAPI(pool);
            pool->m_size = size;
            this->CreatePoolAPI(pool);
            return;
        }

        if (this->m_useVbo[pool->m_target][pool->m_usage]) {
            this->DsSet(s_bufferState[pool->m_target], static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pool->m_apiSpecific)), 0);
            glBufferDataARB(s_glBufferTarget[pool->m_target], size, NULL, s_glBufferUsage[pool->m_usage]);
            pool->Invalidate();
            pool->m_size = size;
            return;
        }
    }

    if (pool->m_apiSpecific) {
        SMemFree(pool->m_apiSpecific, __FILE__, __LINE__, 0);
    }

    pool->m_apiSpecific = SMemAlloc(size, __FILE__, __LINE__, 0);
    pool->Invalidate();
    pool->m_size = size;
}

// OFFSET: %0x007B0540
char* CGxDeviceOpenGl::BufLock(CGxBuf* buf) {
    CGxDevice::BufLock(buf);

    if (buf == this->m_curVertexBuf) {
        this->m_curVertexBuf = NULL;
    }

    CGxPool* pool = buf->m_pool;

    if (!pool->m_apiSpecific) {
        this->CreatePoolAPI(pool);
    }

    char* mem = static_cast<char*>(pool->m_apiSpecific);

    if (!mem) {
        mem = static_cast<char*>(this->m_emergencyMem[pool->m_target].Lock(buf->m_size));
        pool->m_mem = mem;
        return mem;
    }

    if (glARBVertexBufferObject) {
        if (this->m_useVboBroker[pool->m_target][pool->m_usage]) {
            GLuint vbo;
            uint32_t offset;
            static_cast<CGxVboBroker*>(pool->m_apiSpecific)->SelectVbo(buf->m_size, vbo, offset);
            this->DsSet(s_bufferState[pool->m_target], vbo, 0);
            pool->m_mem = glMapBufferARB(s_glBufferTarget[pool->m_target], GL_WRITE_ONLY_ARB);
            buf->m_index = offset / buf->m_itemSize;
            return static_cast<char*>(pool->m_mem) + offset;
        }

        if (this->m_useVbo[pool->m_target][pool->m_usage]) {
            this->DsSet(s_bufferState[pool->m_target], static_cast<uint32_t>(reinterpret_cast<uintptr_t>(mem)), 0);

            if (pool->m_usage == GxPoolUsage_Stream) {
                glBufferDataARB(s_glBufferTarget[pool->m_target], buf->m_size, NULL, GL_STREAM_DRAW_ARB);
                mem = static_cast<char*>(s_emergencyMem[pool->m_target].Lock(buf->m_size));
                pool->m_mem = mem;
                buf->m_index = 0;
                return mem;
            }

            if (pool->m_usage <= GxPoolUsage_Dynamic) {
                mem = static_cast<char*>(s_emergencyMem[pool->m_target].Lock(buf->m_size));
                pool->m_mem = mem;
                return mem;
            }

            return NULL;
        }
    }

    if (pool->m_usage == GxPoolUsage_Stream) {
        pool->m_mem = mem;
        buf->m_index = 0;
        return mem;
    }

    if (pool->m_usage <= GxPoolUsage_Dynamic) {
        pool->m_mem = mem;
        return mem + buf->m_itemSize * buf->m_index;
    }

    return NULL;
}

// OFFSET: %0x007B0720
void CGxDeviceOpenGl::BufData(CGxBuf* buf, const void* data, uint32_t size, uint32_t offset) {
    CGxDevice::BufData(buf, data, size, offset);

    if (buf == this->m_curVertexBuf) {
        this->m_curVertexBuf = NULL;
    }

    CGxPool* pool = buf->m_pool;

    if (!pool->m_apiSpecific) {
        this->CreatePoolAPI(pool);
    }

    if (glARBVertexBufferObject) {
        if (this->m_useVboBroker[pool->m_target][pool->m_usage]) {
            GLuint vbo;
            uint32_t vboOffset;
            static_cast<CGxVboBroker*>(pool->m_apiSpecific)->SelectVbo(size, vbo, vboOffset);
            this->DsSet(s_bufferState[pool->m_target], vbo, 0);
            glBufferSubDataARB(s_glBufferTarget[pool->m_target], vboOffset, size, data);
            buf->m_index = vboOffset / buf->m_itemSize;
            return;
        }

        if (this->m_useVbo[pool->m_target][pool->m_usage]) {
            this->DsSet(s_bufferState[pool->m_target], static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pool->m_apiSpecific)), 0);

            if (pool->m_usage == GxPoolUsage_Stream) {
                glBufferDataARB(s_glBufferTarget[pool->m_target], size, data, GL_STREAM_DRAW_ARB);
                buf->m_index = 0;
            } else if (pool->m_usage <= GxPoolUsage_Dynamic) {
                if (size == static_cast<uint32_t>(pool->m_size)) {
                    glBufferDataARB(s_glBufferTarget[pool->m_target], size, data, s_glBufferUsage[pool->m_usage]);
                } else {
                    glBufferSubDataARB(s_glBufferTarget[pool->m_target], offset + buf->m_itemSize * buf->m_index, size, data);
                }
            }

            return;
        }
    }

    if (pool->m_usage == GxPoolUsage_Stream) {
        buf->m_index = 0;
        memcpy(static_cast<char*>(pool->m_apiSpecific) + offset, data, size);
    } else if (pool->m_usage <= GxPoolUsage_Dynamic) {
        memcpy(static_cast<char*>(pool->m_apiSpecific) + buf->m_itemSize * buf->m_index + offset, data, size);
    }
}

// OFFSET: %0x007B08F0
void CGxDeviceOpenGl::IQueryRelease() {
    for (CGxQuery* query = this->m_queryList.Head(); query; query = this->m_queryList.Next(query)) {
        if (query->m_apiSpecific) {
            GLuint id = static_cast<GLuint>(reinterpret_cast<uintptr_t>(query->m_apiSpecific));
            glDeleteQueriesARB(1, &id);
            query->m_apiSpecific = NULL;
        }
    }
}

// OFFSET: %0x007AED20
void CGxDeviceOpenGl::IReleaseShaders(int32_t freeShaders) {
    if (!freeShaders) {
        return;
    }

    for (uint32_t target = 0; target < GxShTargets_Last; target++) {
        CGxShader* next;

        for (CGxShader* shader = CGxDevice::s_shaderList[target].Head(); shader; shader = next) {
            next = CGxDevice::s_shaderList[target].Next(shader);

            for (uint32_t i = 0; i < shader->m_permutes.Count(); i++) {
                this->IShaderDestroy(&shader->m_permutes[i]);
            }
        }
    }
}

// OFFSET: %0x007B0950
void CGxDeviceOpenGl::IDeviceSetRenderTargetFBO(EGxBuffer buffer, CGxTex* texture, uint32_t plane) {
    CGxDevice::DeviceSetRenderTarget(buffer, texture, plane);

    GLenum attachment = s_glAttachment[buffer];

    if (texture) {
        this->m_textureTarget[buffer].m_apiSpecific = texture;

        if (!texture->m_apiSpecificData) {
            glGenTextures(1, reinterpret_cast<GLuint*>(&texture->m_apiSpecificData));
        }

        if (texture->m_needsCreation) {
            this->BindTexture(texture, 0xFFFFFFFF);

            GLenum target = s_glTexTarget[texture->m_target];
            glTexImage2D(target, 0, s_glInternalFormat[texture->m_format], texture->m_width, texture->m_height, 0, s_glFormat[texture->m_format], s_glType[texture->m_format], NULL);

            if (s_glFormat[texture->m_format] == GL_DEPTH_COMPONENT) {
                glTexParameteri(target, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
                glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
                glTexParameteri(target, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE);
            }

            texture->m_needsCreation = 0;
        }

        if (!this->m_fbo) {
            glGenFramebuffersEXT(1, &this->m_fbo);
        }

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->m_fbo);
        this->m_boundFbo = this->m_fbo;
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment, s_glTexTarget[texture->m_target], static_cast<GLuint>(reinterpret_cast<uintptr_t>(texture->m_apiSpecificData)), 0);

        CRect rect;
        rect.minY = 0.0f;
        rect.minX = 0.0f;
        rect.maxY = static_cast<float>(texture->m_height);
        rect.maxX = static_cast<float>(texture->m_width);
        this->DeviceSetCurWindow(rect);
    } else {
        if (this->m_boundFbo) {
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, s_glAttachment[buffer], GL_TEXTURE_2D, 0, 0);
        }

        this->DeviceSetCurWindow(this->DeviceDefWindow());
        this->m_textureTarget[buffer].m_apiSpecific = NULL;

        for (uint32_t i = GxBuffers_Color; !this->m_textureTarget[i].m_apiSpecific; i++) {
            if (i + 1 >= GxBuffers_Last) {
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
                this->m_boundFbo = 0;
                break;
            }
        }
    }

    GxXformSetViewport(this->m_viewport.m_x.m_l, this->m_viewport.m_x.m_h, this->m_viewport.m_y.m_l, this->m_viewport.m_y.m_h, this->m_viewport.m_z.m_l, this->m_viewport.m_z.m_h);
    g_theGxDevicePtr->XformSetProjection(this->m_projection);
}

// PROVEN: bc59d12d IDENTICAL/NONE
// OFFSET: 0x0084F740
void CGxDeviceOpenGl::DeviceSetRenderTarget(EGxBuffer buffer, CGxTex* texture, uint32_t plane) {
    if (texture == this->m_backBufferColorTex || texture == this->m_backBufferDepthTex) {
        texture = NULL;
    }

    switch (this->m_renderTargetMode) {
        case 0:
            this->IDeviceSetRenderTargetCopy(buffer, texture, plane);
            break;

        case 1:
            this->IDeviceSetRenderTargetFBO(buffer, texture, plane);
            break;
    }

    this->m_viewportDirty = 1;
    this->m_scissorRectDirty = 1;
}

// OFFSET: %0x007B0C00
int32_t CGxDeviceOpenGl::BufUnlock(CGxBuf* buf, uint32_t size) {
    CGxDevice::BufUnlock(buf, size);

    CGxPool* pool = buf->m_pool;
    int32_t result = 1;

    if (!pool->m_apiSpecific) {
        this->m_emergencyMem[pool->m_target].Unlock();
        buf->unk_001D = 0;
        this->m_intF5C = 1;
        pool->m_mem = NULL;
        return 0;
    }

    if (glARBVertexBufferObject) {
        if (this->m_useVboBroker[pool->m_target][pool->m_usage]) {
            static_cast<CGxVboBroker*>(pool->m_apiSpecific)->UnlockVbo(buf->m_itemSize * buf->m_index, size);
            result = glUnmapBufferARB(s_glBufferTarget[pool->m_target]);
            buf->unk_001D = result;
            pool->m_mem = NULL;
            return result;
        }

        if (this->m_useVbo[pool->m_target][pool->m_usage]) {
            EGxPoolTarget target = pool->m_target;

            if (this->m_emergencyMem[target].m_lock) {
                this->m_emergencyMem[target].Unlock();
                result = 0;
                this->m_intF5C = 1;
            } else if (s_emergencyMem[target].m_lock) {
                glBufferSubDataARB(s_glBufferTarget[target], buf->m_itemSize * buf->m_index, buf->m_size, s_emergencyMem[target].m_data.m_data);
                s_emergencyMem[pool->m_target].Unlock();
            }

            buf->unk_001D = result;
        }
    }

    pool->m_mem = NULL;
    return result;
}

// OFFSET: %0x007B0D50
void CGxDeviceOpenGl::DeviceReadPixels(CiRect& rect, TSGrowableArray<CImVector>& pixels) {
    this->ClampRectToWindow(rect);

    int32_t height = rect.maxY - rect.minY;
    int32_t width = rect.maxX - rect.minX;
    pixels.SetCount(width * height);

    const CRect& window = this->DeviceCurWindow();
    glReadPixels(rect.minX, static_cast<GLint>(window.maxY - rect.maxY), width, height, 0x80E1, GL_UNSIGNED_BYTE, pixels.Ptr());

    uint32_t pitch = sizeof(CImVector) * width;
    CImVector* row = static_cast<CImVector*>(alloca(pitch));

    for (int32_t i = 0; i < width; i++) {
        new (&row[i]) CImVector();
    }

    CImVector* top = pixels.Ptr();
    CImVector* bottom = top + width * (height - 1);

    for (int32_t i = height / 2; i; i--) {
        memcpy(row, top, pitch);
        memcpy(top, bottom, pitch);
        memcpy(bottom, row, pitch);
        bottom -= width;
        top += width;
    }
}

// OFFSET: %0x007B0E40
void CGxDeviceOpenGl::DeviceReadDepths(CiRect& rect, TSGrowableArray<float>& depths) {
    this->ClampRectToWindow(rect);

    int32_t width = rect.maxX - rect.minX;
    int32_t height = rect.maxY - rect.minY;
    depths.SetCount(width * height);

    const CRect& window = this->DeviceCurWindow();
    glReadPixels(rect.minX, static_cast<GLint>(window.maxY - rect.maxY), width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depths.Ptr());

    float* top = depths.Ptr();
    float* bottom = top + width * (height - 1);

    for (int32_t i = height / 2; i; i--) {
        for (int32_t j = 0; j < width; j++) {
            float value = top[j];
            top[j] = bottom[j];
            bottom[j] = value;
        }

        bottom -= width;
        top += width;
    }
}

// OFFSET: %0x007B0FC0
void CGxDeviceOpenGl::DeviceDestroyPoolAPI() {
    for (CGxPool* pool = CGxDevice::s_poolList.Head(), * next; !CGxDevice::s_poolList.IsEnd(pool); pool = next) {
        next = CGxDevice::s_poolList.RawNext(pool);
        this->DestroyPoolAPI(pool);
    }
}

// PROVEN: 46618c12 IDENTICAL/IDENTICAL
// OFFSET: 0x00850510/0x14057F940
void BindWglExtensions() {
    wglGetExtensionsStringARB = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(wglGetProcAddress("wglGetExtensionsStringARB"));

    if (HasWGLExtension("WGL_ARB_pixel_format")) {
        wglArbPixelFormat = 1;
        wglGetPixelFormatAttribivARB = reinterpret_cast<PFNWGLGETPIXELFORMATATTRIBIVARBPROC>(wglGetProcAddress("wglGetPixelFormatAttribivARB"));
        wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
    }

    if (HasWGLExtension("WGL_ARB_pbuffer")) {
        wglArbPbuffer = 1;
        wglCreatePbufferARB = reinterpret_cast<PFNWGLCREATEPBUFFERARBPROC>(wglGetProcAddress("wglCreatePbufferARB"));
        wglGetPbufferDCARB = reinterpret_cast<PFNWGLGETPBUFFERDCARBPROC>(wglGetProcAddress("wglGetPbufferDCARB"));
        wglReleasePbufferDCARB = reinterpret_cast<PFNWGLRELEASEPBUFFERDCARBPROC>(wglGetProcAddress("wglReleasePbufferDCARB"));
        wglDestroyPbufferARB = reinterpret_cast<PFNWGLDESTROYPBUFFERARBPROC>(wglGetProcAddress("wglDestroyPbufferARB"));
        wglQueryPbufferARB = reinterpret_cast<PFNWGLQUERYPBUFFERARBPROC>(wglGetProcAddress("wglQueryPbufferARB"));
    }

    if (HasWGLExtension("WGL_EXT_swap_control")) {
        wglExtSwapControl = 1;
        wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
    }
}

// OFFSET: %0x007B69D0
static void CreateDummyContext() {
    if (s_dummyContextCreated) {
        return;
    }

    s_dummyContextCreated = true;

    HINSTANCE instance = GetModuleHandleA(NULL);

    WNDCLASSEXA windowClass;
    memset(&windowClass, 0, sizeof(windowClass));
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = DefWindowProcA;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = "hwacc";

    ATOM atom = RegisterClassExA(&windowClass);

    if (!atom) {
        return;
    }

    HWND window = CreateWindowExA(0, reinterpret_cast<LPCSTR>(static_cast<uintptr_t>(atom)), "hwacc", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);

    if (window) {
        HDC dc = GetDC(window);

        if (dc) {
            int32_t count = DescribePixelFormat(dc, 0, 0, NULL);

            PIXELFORMATDESCRIPTOR pfd;
            memset(&pfd.dwFlags, 0, sizeof(pfd) - offsetof(PIXELFORMATDESCRIPTOR, dwFlags));
            pfd.nSize = sizeof(pfd);
            pfd.nVersion = 1;

            for (int32_t i = 1; i <= count; i++) {
                DescribePixelFormat(dc, i, sizeof(pfd), &pfd);

                if ((pfd.dwFlags & (PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL)) && !(pfd.dwFlags & (PFD_GENERIC_FORMAT | PFD_SUPPORT_GDI))) {
                    SetPixelFormat(dc, i, &pfd);

                    HGLRC context = wglCreateContext(dc);

                    if (context) {
                        wglMakeCurrent(dc, context);
                        BindGlExtensions();
                        BindWglExtensions();
                        wglMakeCurrent(NULL, NULL);
                    }

                    wglDeleteContext(context);
                    break;
                }
            }

            ReleaseDC(window, dc);
        }

        DestroyWindow(window);
    }

    UnregisterClassA(reinterpret_cast<LPCSTR>(static_cast<uintptr_t>(atom)), instance);
}

// PROVEN: 13e0859e IDENTICAL/NONE
// OFFSET: 0x00850780
void CGxDeviceOpenGl::DeviceDestroyPbuffer() {
    if (this->m_pbuffer) {
        wglReleasePbufferDCARB(this->m_pbuffer, this->m_pbufferDC);
        wglDestroyPbufferARB(this->m_pbuffer);
    }

    this->m_pbuffer = NULL;
    this->m_pbufferDC = NULL;
    this->m_pbufferRC = NULL;
}

// OFFSET: %0x007B6BC0
void CGxDeviceOpenGl::IDevSetFocus(int32_t focus, CGxFormat& format) {
    this->m_windowFocus = focus;

    if (format.m_window) {
        if (focus && format.m_maximize != 2) {
            ShowWindow(this->m_hwnd, SW_SHOW);
        }
    } else if (focus) {
        HMONITOR monitor = static_cast<HMONITOR>(CGxDevice::AdapterGetMonitorHandle(format.m_monitor));

        MONITORINFOEXA info;
        info.cbSize = sizeof(info);
        GetMonitorInfoA(monitor, &info);

        DEVMODEA mode;
        memset(&mode, 0, sizeof(mode));
        mode.dmSize = sizeof(mode);
        EnumDisplaySettingsA(info.szDevice, format.m_apiSpecificModeID, &mode);
        ChangeDisplaySettingsExA(info.szDevice, &mode, NULL, CDS_FULLSCREEN, NULL);

        GetMonitorInfoA(monitor, &info);
        int32_t width = info.rcMonitor.right - info.rcMonitor.left;
        int32_t height = info.rcMonitor.bottom - info.rcMonitor.top;
        format.m_pos.x = info.rcMonitor.left;
        format.m_pos.y = info.rcMonitor.top;
        format.m_size.x = width;
        format.m_size.y = height;
        SetWindowPos(this->m_hwnd, NULL, info.rcMonitor.left, info.rcMonitor.top, width, height, SWP_NOREDRAW | SWP_NOCOPYBITS | SWP_DEFERERASE);
        ShowWindow(this->m_hwnd, SW_MAXIMIZE);

        RECT windowRect;
        GetWindowRect(this->m_hwnd, &windowRect);
        ClipCursor(&windowRect);
    } else {
        ChangeDisplaySettingsExA(NULL, NULL, NULL, 0, NULL);
        ShowWindow(this->m_hwnd, SW_MINIMIZE);
    }
}

// OFFSET: %0x007B6D20
int32_t CGxDeviceOpenGl::SetFormatMode(CGxFormat& format) {
    if (format.m_window) {
        format.m_apiSpecificModeID = 0;
        return 1;
    }

    HMONITOR monitor = static_cast<HMONITOR>(CGxDevice::AdapterGetMonitorHandle(format.m_monitor));

    MONITORINFOEXA info;
    info.cbSize = sizeof(info);
    GetMonitorInfoA(monitor, &info);

    uint32_t bpp = format.m_colorFormat != CGxFormat::Fmt_Rgb565 ? 32 : 16;

    DEVMODEA mode;
    memset(&mode, 0, sizeof(mode));
    uint32_t modeNum = 0;
    mode.dmSize = sizeof(mode);

    if (!EnumDisplaySettingsA(info.szDevice, 0, &mode)) {
        return 0;
    }

    while (((mode.dmFields & DM_DISPLAYFIXEDOUTPUT) && mode.dmDisplayFixedOutput == DMDFO_CENTER) || mode.dmBitsPerPel != bpp || mode.dmDisplayFrequency != format.m_refreshRate / format.m_refreshRateDenom || mode.dmPelsWidth != static_cast<DWORD>(format.m_size.x) || mode.dmPelsHeight != static_cast<DWORD>(format.m_size.y)) {
        if (!EnumDisplaySettingsA(info.szDevice, ++modeNum, &mode)) {
            return 0;
        }
    }

    format.m_apiSpecificModeID = modeNum;
    return 1;
}

// PROVEN: 7c6b421f IDENTICAL/DIFFERENT
// OFFSET: 0x00850A30/0x14057FFD0
void CGxDeviceOpenGl::IDevRemoveGlContext() {
    this->DeviceDestroyPbuffer();

    if (this->m_hglrc) {
        this->ICursorDestroy();
        this->DeviceDestroyPoolAPI();
        this->ITexForceRecreation();
        this->IReleaseShaders(1);
        this->IQueryRelease();
        UnbindGlExtensions();
        WglDestroyContext(this->m_hglrc);
        this->m_hglrc = NULL;
    }

    if (this->m_hdc) {
        SetDeviceGammaRamp(this->m_hdc, &this->m_systemGammaRamp);
        ReleaseDC(this->m_hwnd, this->m_hdc);
        this->m_hdc = NULL;
    }

    this->m_context = 0;
}

// PROVEN: dbd8dcb5 IDENTICAL/UNPROVEN
// OFFSET: 0x00850B00/0x1405800B0
void CGxDeviceOpenGl::DeviceDestroy() {
    CGxDevice::DeviceDestroy();
    this->DeviceDestroyPbuffer();
    s_deviceChanging = 1;

    if (this->m_hwnd && this->m_ownhwnd) {
        this->IWindowDestroy(&this->m_hwnd);
    }

    if (this->m_hwnd && !this->m_ownhwnd) {
        this->IDevRemoveGlContext();
    }

    if (this->m_hwndClass) {
        this->IWindowClassDestroy(&this->m_hwndClass);
    }

    s_deviceChanging = 0;
}

// PROVEN: dd7e1d2e IDENTICAL/IDENTICAL
// OFFSET: 0x00850BA0/0x140580180
void CGxDeviceOpenGl::DeviceSetBaseMipLevel(uint32_t level) {
    this->ITexForceRecreation();
    CGxDevice::DeviceSetBaseMipLevel(level);
}

// PROVEN: 3cce8327 IDENTICAL/IDENTICAL
// OFFSET: 0x00850BC0/0x1405801B0
void CGxDeviceOpenGl::DeviceSetGamma(float gamma) {
    CGxDevice::DeviceSetGamma(gamma);

    if (!this->m_format.m_window) {
        SetDeviceGammaRamp(this->m_hdc, &this->m_gammaRamp);
    }
}

// PROVEN: eda780ed IDENTICAL/IDENTICAL
// OFFSET: 0x00850C00/0x1405801E0
void CGxDeviceOpenGl::DeviceSetGamma(const CGxGammaRamp& ramp) {
    CGxDevice::DeviceSetGamma(ramp);

    if (!this->m_format.m_window) {
        SetDeviceGammaRamp(this->m_hdc, &this->m_gammaRamp);
    }
}

// PROVEN: eefb30c2 IDENTICAL/NONE
// OFFSET: 0x00850C30
void* CGxDeviceOpenGl::DeviceWindow() {
    return this->m_hwnd;
}

// PROVEN: a516afd1 IDENTICAL/UNPROVEN
// OFFSET: 0x00850C60/0x1405824B0
void CGxDeviceOpenGl::CursorSetVisible(int32_t visible) {
    if (visible == CGxDevice::s_cursorVisible) {
        return;
    }

    CGxDevice::CursorSetVisible(visible);

    if (this->m_hwCursor) {
        ShowCursor(CGxDevice::s_cursorVisible);
    }
}

// PROVEN: 0c49ede9 IDENTICAL/NONE
// OFFSET: 0x00850CA0
void CGxDeviceOpenGl::CursorUnlock(uint32_t x, uint32_t y) {
    CGxDevice::CursorUnlock(x, y);
    this->IHwCursorCreate();
}

// PROVEN: 7d2814df IDENTICAL/NONE
// OFFSET: 0x00850CD0
void CGxDeviceOpenGl::IHwCursorCreate() {
    if (this->m_hwCursor) {
        this->IWindowCursorCreate(this->m_hwnd, &this->m_cursor);
    }
}

// PROVEN: e7d188f5 IDENTICAL/IDENTICAL
// OFFSET: 0x00850CF0/0x140580280
void CGxDeviceOpenGl::ICursorCreate(const CGxFormat& format) {
    CGxDevice::ICursorCreate(format);
    this->IHwCursorCreate();
}

// PROVEN: 5cac136f IDENTICAL/NONE
// OFFSET: 0x00850D10
void CGxDeviceOpenGl::ICursorDestroy() {
    CGxDevice::ICursorDestroy();
    this->IWindowCursorDestroy(&this->m_cursor);
}

// OFFSET: %0x007B7100
void CGxDeviceOpenGl::DeviceCreatePbuffer() {
    if (!wglArbPbuffer) {
        return;
    }

    int32_t pixelFormat = GetPixelFormat(this->m_hdc);
    const CRect& window = this->DeviceDefWindow();

    int32_t attributes[2];
    attributes[0] = 0;
    attributes[1] = 0;

    this->m_pbuffer = wglCreatePbufferARB(this->m_hdc, pixelFormat, static_cast<int32_t>(window.maxX - window.minX), static_cast<int32_t>(window.maxY - window.minY), attributes);
    this->m_pbufferDC = wglGetPbufferDCARB(this->m_pbuffer);

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(this->m_pbufferDC, pixelFormat, sizeof(pfd), &pfd);

    if (pfd.cRedBits == 8) {
        if (pfd.cGreenBits == 8 && pfd.cBlueBits == 8) {
            this->m_caps.m_rttFormat[GxTex_Argb8888] = 1;
        }
    } else if (pfd.cRedBits == 5 && pfd.cGreenBits == 6 && pfd.cBlueBits == pfd.cRedBits) {
        this->m_caps.m_rttFormat[GxTex_Rgb565] = 1;
    }

    this->m_pbufferRC = wglGetCurrentContext();
}

// PROVEN: ce162643 IDENTICAL/DIFFERENT
// OFFSET: 0x00850E50/0x14057FCA0
void CGxDeviceOpenGl::DeviceQueryPbuffer() {
    if (!this->m_pbuffer) {
        return;
    }

    int32_t lost;
    wglQueryPbufferARB(this->m_pbuffer, WGL_PBUFFER_LOST_ARB, &lost);

    if (lost) {
        this->DeviceDestroyPbuffer();
        this->DeviceCreatePbuffer();
    }
}

// PROVEN: e2a62890 IDENTICAL/NONE
// OFFSET: 0x00850C40
void CGxDevice::OpenGlApiQuery(int32_t& supported, int32_t& selectable) {
    supported = 1;
    selectable = 0;
}

// OFFSET: %0x007B7230
int32_t CGxDeviceOpenGl::IDevAttachGlContext(const CGxFormat& format) {
    CreateDummyContext();

    this->m_hdc = GetDC(this->m_hwnd);

    if (!this->m_hdc || (this->m_hglrc = CreateGLContext(this->m_hdc, format)) == NULL) {
        this->IDevRemoveGlContext();
        return 0;
    }

    wglMakeCurrent(this->m_hdc, this->m_hglrc);
    this->m_context = 1;

    if (wglExtSwapControl) {
        wglSwapIntervalEXT(format.m_vsync != 0);
    }

    if (!glExtClampToEdge) {
        s_glWrapMode[1] = GL_REPEAT;
    }

    this->ISetCaps(format);
    this->DeviceCreatePbuffer();
    this->ICreateBackBufferFB(format.m_size.x, format.m_size.y, NULL, NULL);
    GxFrameBufferSet(g_theGxDevicePtr->BackBuffer());
    this->ICursorCreate(format);
    this->IStateSetContextDefaults();
    this->IRsSync(1);
    this->IShaderConstantsClear();
    this->m_glVendor = glGetString(GL_VENDOR);
    this->m_glRenderer = glGetString(GL_RENDERER);
    this->m_glVersion = glGetString(GL_VERSION);

    if (s_firstAttach) {
        this->CursorSetVisible(0);
        s_firstAttach = 0;
    }

    return 1;
}

// OFFSET: %0x007B7350
int32_t CGxDeviceOpenGl::DeviceWM(EGxWM wm, uintptr_t param1, uintptr_t param2) {
    switch (wm) {
        case GxWM_Size:
            if (param2 == 1 || param2 == 2) {
                this->m_windowVisible = 0;
            } else {
                this->m_windowVisible = 1;

                const CRect& window = this->DeviceDefWindow();
                const CRect* rect = reinterpret_cast<const CRect*>(param1);
                bool grew = window.maxX - window.minX < rect->maxX - rect->minX || window.maxY - window.minY < rect->maxY - rect->minY;
                this->DeviceSetDefWindow(*rect);

                if (grew) {
                    this->DeviceDestroyPbuffer();
                    this->DeviceCreatePbuffer();
                } else {
                    this->DeviceQueryPbuffer();
                }

                this->m_viewportDirty = 1;
            }

            break;

        case GxWM_DisplayChange:
            if (this->m_windowVisible) {
                this->DeviceSetDefWindow(*reinterpret_cast<const CRect*>(param1));
                this->m_viewportDirty = 1;
            }

            break;

        case GxWM_Destroy:
            this->IDevRemoveGlContext();
            this->m_windowFocus = 0;

            if (!this->m_format.m_window) {
                ChangeDisplaySettingsExA(NULL, NULL, NULL, 0, NULL);
                ShowWindow(this->m_hwnd, SW_MINIMIZE);
            }

            break;

        case GxWM_SetFocus:
            if (!s_deviceChanging) {
                this->IDevSetFocus(1, this->m_format);
            }

            break;

        case GxWM_KillFocus:
            if (!s_deviceChanging) {
                this->m_windowFocus = 0;

                if (!this->m_format.m_window) {
                    ChangeDisplaySettingsExA(NULL, NULL, NULL, 0, NULL);
                    ShowWindow(this->m_hwnd, SW_MINIMIZE);
                }
            }

            break;

        default:
            break;
    }

    return 0;
}

// OFFSET: %0x007B7580
int32_t CGxDeviceOpenGl::DeviceCreate(void* window, const CGxFormat& format) {
    s_deviceChanging = 1;
    this->m_ownhwnd = 0;

    HDC dc = GetDC(NULL);

    if (GetDeviceGammaRamp(dc, &this->m_systemGammaRamp) && &this->m_gammaRamp != &this->m_systemGammaRamp) {
        this->m_gammaRamp = this->m_systemGammaRamp;
    }

    ReleaseDC(NULL, dc);

    this->m_hwnd = static_cast<HWND>(window);

    if (this->IDevAttachGlContext(format)) {
        s_deviceChanging = 0;

        if (CGxDevice::DeviceCreate(window, format)) {
            return 1;
        }
    }

    this->DeviceDestroy();
    s_deviceChanging = 0;
    return 0;
}

// OFFSET: %0x007B7640
int32_t CGxDeviceOpenGl::DeviceSetFormat(const CGxFormat& format) {
    CGxDevice::Log("CGxDeviceOpenGl::DeviceSetFormat():");
    CGxDevice::Log(format);

    s_deviceChanging = 1;
    this->IDevRemoveGlContext();
    this->IWindowDestroy(&this->m_hwnd);

    CGxFormat newFormat = format;

    if (CGxDeviceOpenGl::SetFormatMode(newFormat) && this->IWindowCreate(&this->m_hwnd, newFormat, 0)) {
        this->IDevSetFocus(1, newFormat);

        if (this->IDevAttachGlContext(newFormat)) {
            s_deviceChanging = 0;

            if (CGxDevice::DeviceSetFormat(newFormat)) {
                this->DeviceSetGamma(this->m_gammaRamp);
                return 1;
            }
        }
    }

    s_deviceChanging = 0;
    return 0;
}

// OFFSET: %0x007B7720
void CGxDeviceOpenGl::IDeviceSetRenderTargetCopy(EGxBuffer buffer, CGxTex* texture, uint32_t plane) {
    if (!this->m_context || !this->m_windowVisible) {
        return;
    }

    CGxDevice::TextureTarget& target = this->m_textureTarget[buffer];

    if (target.m_texture == texture && target.m_plane == plane) {
        return;
    }

    CGxDevice::DeviceSetRenderTarget(buffer, texture, plane);

    if (this->m_textureTarget[GxBuffers_Color].m_texture || this->m_textureTarget[GxBuffers_Depth].m_texture) {
        this->DeviceSetCurWindow(this->DeviceDefWindow());
    }

    CGxTex* previous = static_cast<CGxTex*>(target.m_apiSpecific);

    if (previous) {
        if (!previous->m_apiSpecificData) {
            glGenTextures(1, reinterpret_cast<GLuint*>(&previous->m_apiSpecificData));
        }

        this->BindTexture(previous, 0xFFFFFFFF);

        if (previous->m_needsFlagUpdate) {
            this->ITexSetFlags(previous);
        }

        GLenum glTarget = s_glTexTarget[previous->m_target];

        if (previous->m_needsCreation) {
            glCopyTexImage2D(glTarget, 0, s_glInternalFormat[previous->m_format], 0, 0, previous->m_width, previous->m_height, 0);
        } else {
            glCopyTexSubImage2D(glTarget, 0, 0, 0, 0, 0, previous->m_width, previous->m_height);
        }

        this->IResourceTouch(previous);
        previous->m_needsCreation = 0;
    }

    target.m_apiSpecific = texture;

    HDC dc = this->m_hdc;
    HGLRC context = this->m_hglrc;

    if (this->m_textureTarget[GxBuffers_Color].m_apiSpecific || this->m_textureTarget[GxBuffers_Depth].m_apiSpecific) {
        dc = this->m_pbufferDC;
        context = this->m_pbufferRC;
    }

    wglMakeCurrent(dc, context);

    GxXformSetViewport(this->m_viewport.m_x.m_l, this->m_viewport.m_x.m_h, this->m_viewport.m_y.m_l, this->m_viewport.m_y.m_h, this->m_viewport.m_z.m_l, this->m_viewport.m_z.m_h);
    g_theGxDevicePtr->XformSetProjection(this->m_projection);
}

// OFFSET: %0x007B78C0
void CGxDeviceOpenGl::CapsWindowSizeInScreenCoords(CRect& rect) {
    if (this->IDevIsWindowed()) {
        const CRect& window = this->DeviceCurWindow();

        POINT points[2];
        points[0].x = 0;
        points[0].y = 0;
        points[1].x = static_cast<LONG>(window.maxX);
        points[1].y = static_cast<LONG>(window.maxY);
        MapWindowPoints(this->m_hwnd, NULL, points, 2);

        rect.minY = static_cast<float>(points[0].y);
        rect.minX = static_cast<float>(points[0].x);
        rect.maxY = static_cast<float>(points[1].y);
        rect.maxX = static_cast<float>(points[1].x);
    } else {
        const CRect& window = this->DeviceCurWindow();
        rect.minY = window.minY;
        rect.minX = window.minX;
        rect.maxY = window.maxY;
        rect.maxX = window.maxX;
    }
}

// OFFSET: %0x007B74E0
int32_t CGxDeviceOpenGl::DeviceCreate(GxWindowProc windowProc, const CGxFormat& format) {
    this->m_ownhwnd = 1;

    HDC dc = GetDC(NULL);

    if (GetDeviceGammaRamp(dc, &this->m_systemGammaRamp) && &this->m_gammaRamp != &this->m_systemGammaRamp) {
        this->m_gammaRamp = this->m_systemGammaRamp;
    }

    ReleaseDC(NULL, dc);

    this->m_hwndClass = this->IWindowClassCreate();

    if (this->m_hwndClass && CGxDevice::DeviceCreate(windowProc, format)) {
        return 1;
    }

    this->DeviceDestroy();
    return 0;
}

// OFFSET: %0x007B7980
static void WglAdapterFormats(HDC dc, TSGrowableArray<CGxFormat>& formats) {
    wglGetPixelFormatAttribivARB = reinterpret_cast<PFNWGLGETPIXELFORMATATTRIBIVARBPROC>(wglGetProcAddress("wglGetPixelFormatAttribivARB"));

    if (!wglGetPixelFormatAttribivARB) {
        return;
    }

    int32_t numberAttrib = WGL_NUMBER_PIXEL_FORMATS_ARB;
    int32_t supportAttrib = WGL_SUPPORT_OPENGL_ARB;
    int32_t accelerationAttrib = WGL_ACCELERATION_ARB;
    int32_t pixelTypeAttrib = WGL_PIXEL_TYPE_ARB;
    int32_t colorBitsAttrib = WGL_COLOR_BITS_ARB;
    int32_t alphaBitsAttrib = WGL_ALPHA_BITS_ARB;
    int32_t depthBitsAttrib = WGL_DEPTH_BITS_ARB;
    int32_t stencilBitsAttrib = WGL_STENCIL_BITS_ARB;
    int32_t samplesAttrib = WGL_SAMPLES_ARB;

    int32_t count;

    if (!wglGetPixelFormatAttribivARB(dc, 0, 0, 1, &numberAttrib, &count)) {
        return;
    }

    for (int32_t i = 1; i < count; i++) {
        CGxFormat format;
        memset(&format, 0, sizeof(format));

        int32_t support;
        int32_t acceleration;
        int32_t pixelType;
        int32_t colorBits;
        int32_t alphaBits;
        int32_t depthBits;
        int32_t stencilBits;
        int32_t samples;

        if (!wglGetPixelFormatAttribivARB(dc, i, 0, 1, &supportAttrib, &support) || !support) {
            continue;
        }

        if (!wglGetPixelFormatAttribivARB(dc, i, 0, 1, &accelerationAttrib, &acceleration) || acceleration != WGL_FULL_ACCELERATION_ARB) {
            continue;
        }

        if (!wglGetPixelFormatAttribivARB(dc, i, 0, 1, &pixelTypeAttrib, &pixelType) || pixelType != WGL_TYPE_RGBA_ARB) {
            continue;
        }

        if (!wglGetPixelFormatAttribivARB(dc, i, 0, 1, &colorBitsAttrib, &colorBits)) {
            continue;
        }

        if (!wglGetPixelFormatAttribivARB(dc, i, 0, 1, &depthBitsAttrib, &depthBits)) {
            continue;
        }

        if (!wglGetPixelFormatAttribivARB(dc, i, 0, 1, &stencilBitsAttrib, &stencilBits)) {
            continue;
        }

        if (!wglGetPixelFormatAttribivARB(dc, i, 0, 1, &samplesAttrib, &samples) || !samples) {
            samples = 1;
        }

        if (colorBits == 16) {
            format.m_colorFormat = CGxFormat::Fmt_Rgb565;
        } else {
            if (colorBits != 32) {
                continue;
            }

            if (!wglGetPixelFormatAttribivARB(dc, i, 0, 1, &alphaBitsAttrib, &alphaBits)) {
                continue;
            }

            if (alphaBits) {
                if (alphaBits != 8) {
                    continue;
                }

                format.m_colorFormat = CGxFormat::Fmt_Argb8888;
            } else {
                format.m_colorFormat = CGxFormat::Fmt_ArgbX888;
            }
        }

        if (depthBits == 16) {
            if (stencilBits) {
                continue;
            }

            format.m_depthFormat = CGxFormat::Fmt_Ds160;
        } else if (depthBits == 24) {
            if (!stencilBits) {
                format.m_depthFormat = CGxFormat::Fmt_Ds24X;
            } else if (stencilBits == 8) {
                format.m_depthFormat = CGxFormat::Fmt_Ds248;
            } else {
                continue;
            }
        } else {
            if (depthBits != 32 || stencilBits) {
                continue;
            }

            format.m_depthFormat = CGxFormat::Fmt_Ds320;
        }

        format.m_multisampleCount = samples;

        if (!CGxDevice::FormatExists(formats, format)) {
            *formats.New() = format;
        }
    }
}

// OFFSET: %0x007B7C00
static void PfdAdapterFormats(HDC dc, TSGrowableArray<CGxFormat>& formats) {
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd.dwFlags, 0, sizeof(pfd) - offsetof(PIXELFORMATDESCRIPTOR, dwFlags));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;

    int32_t count = DescribePixelFormat(dc, 0, 0, NULL);

    for (int32_t i = 1; i < count; i++) {
        DescribePixelFormat(dc, i, sizeof(pfd), &pfd);

        if (pfd.iPixelType != PFD_TYPE_RGBA) {
            continue;
        }

        if (!(pfd.dwFlags & (PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL)) || (pfd.dwFlags & (PFD_GENERIC_FORMAT | PFD_SUPPORT_GDI))) {
            continue;
        }

        CGxFormat format;
        memset(&format, 0, sizeof(format));

        if (pfd.cRedBits == 8) {
            if (pfd.cBlueBits != 8 || pfd.cGreenBits != pfd.cRedBits) {
                continue;
            }

            if (pfd.cAlphaBits == 8) {
                format.m_colorFormat = CGxFormat::Fmt_Argb8888;
            } else if (!pfd.cAlphaBits) {
                format.m_colorFormat = CGxFormat::Fmt_ArgbX888;
            }
        } else {
            if (pfd.cRedBits != 5) {
                continue;
            }

            if (pfd.cGreenBits != 5 && pfd.cGreenBits != 6) {
                continue;
            }

            if (pfd.cBlueBits != 5) {
                continue;
            }

            format.m_colorFormat = CGxFormat::Fmt_Rgb565;
        }

        if (pfd.cDepthBits == 16) {
            if (pfd.cStencilBits) {
                continue;
            }

            format.m_depthFormat = CGxFormat::Fmt_Ds160;
        } else if (pfd.cDepthBits == 24) {
            if (!pfd.cStencilBits) {
                format.m_depthFormat = CGxFormat::Fmt_Ds24X;
            } else if (pfd.cStencilBits == 8) {
                format.m_depthFormat = CGxFormat::Fmt_Ds248;
            } else {
                continue;
            }
        } else {
            if (pfd.cDepthBits != 32 || pfd.cStencilBits) {
                continue;
            }

            format.m_depthFormat = CGxFormat::Fmt_Ds320;
        }

        format.m_multisampleCount = 1;

        if (!CGxDevice::FormatExists(formats, format)) {
            *formats.New() = format;
        }
    }
}

// OFFSET: %0x007B7D90
int32_t CGxDevice::OpenGlAdapterFormats(TSGrowableArray<CGxFormat>& formats) {
    HINSTANCE instance = GetModuleHandleA(NULL);

    WNDCLASSEXA windowClass;
    memset(&windowClass, 0, sizeof(windowClass));
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = DefWindowProcA;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = "adfmt";

    ATOM atom = RegisterClassExA(&windowClass);

    if (!atom) {
        return formats.Count() != 0;
    }

    HWND window = CreateWindowExA(0, reinterpret_cast<LPCSTR>(static_cast<uintptr_t>(atom)), "adfmt", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);

    if (window) {
        HDC dc = GetDC(window);

        if (dc) {
            int32_t count = DescribePixelFormat(dc, 0, 0, NULL);

            PIXELFORMATDESCRIPTOR pfd;
            memset(&pfd.dwFlags, 0, sizeof(pfd) - offsetof(PIXELFORMATDESCRIPTOR, dwFlags));
            pfd.nSize = sizeof(pfd);
            pfd.nVersion = 1;

            for (int32_t i = 1; i <= count; i++) {
                DescribePixelFormat(dc, i, sizeof(pfd), &pfd);

                if (!(pfd.dwFlags & (PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL)) || (pfd.dwFlags & (PFD_GENERIC_FORMAT | PFD_SUPPORT_GDI))) {
                    continue;
                }

                SetPixelFormat(dc, i, &pfd);

                HGLRC context = wglCreateContext(dc);

                if (!context) {
                    continue;
                }

                HGLRC currentContext = wglGetCurrentContext();
                HDC currentDC = wglGetCurrentDC();
                wglMakeCurrent(dc, context);

                wglGetExtensionsStringARB = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(wglGetProcAddress("wglGetExtensionsStringARB"));

                if (HasWGLExtension("WGL_ARB_pixel_format")) {
                    WglAdapterFormats(dc, formats);
                } else {
                    PfdAdapterFormats(dc, formats);
                }

                wglMakeCurrent(currentDC, currentContext);
                wglDeleteContext(context);
                break;
            }

            ReleaseDC(window, dc);
        }

        DestroyWindow(window);
    }

    UnregisterClassA(reinterpret_cast<LPCSTR>(static_cast<uintptr_t>(atom)), instance);

    return formats.Count() != 0;
}

// PROVEN: c3594285 IDENTICAL/NONE
// OFFSET: 0x00870160
void CGxDeviceOpenGl::CapsWindowSize(CRect& rect) {
    rect = this->DeviceCurWindow();
}

// PROVEN: 1ac188a5 IDENTICAL/NONE
// OFFSET: 0x008692C0
void CGxDeviceOpenGl::SceneClearToViewport(uint32_t mask, CImVector color) {
    this->SceneClear(mask, color);
}

// PROVEN: b426fc55 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CGxDeviceOpenGl::DrawInstancedNonIndexed(const CGxBatch& batch) {
}

// PROVEN: cb2eacae IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CGxDeviceOpenGl::DrawInstanced(const CGxBatch& batch) {
}

// PROVEN: e97dae37 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CGxDeviceOpenGl::SetInstanceCount(uint32_t count) {
}

// PROVEN: e4f62a13 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CGxDeviceOpenGl::StereoSetConvergence(float convergence) {
}

// PROVEN: 35b1a782 IDENTICAL/IDENTICAL
// OFFSET: 0x00599E10/0x1402082B0
float CGxDeviceOpenGl::StereoGetConvergence() {
    return 1.0f;
}

// PROVEN: fcfaab51 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CGxDeviceOpenGl::StereoSetSeparation(float separation) {
}

// PROVEN: 1fabd306 IDENTICAL/IDENTICAL
// OFFSET: 0x0076CA20/0x140586E10
float CGxDeviceOpenGl::StereoGetSeparation() {
    return 0.0f;
}

// PROVEN: f71ea9a2 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGxDeviceOpenGl::StereoEnabled() {
    return false;
}

// PROVEN: 5e33e863 IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void CGxDeviceOpenGl::DebugGroupBegin(const char* name, char* buffer) {
}

// PROVEN: 4ea2707c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGxDeviceOpenGl::DebugGroupEnd() {
}

// PROVEN: b06a92be IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void CGxDeviceOpenGl::DebugEvent(const char* name, char* buffer) {
}
