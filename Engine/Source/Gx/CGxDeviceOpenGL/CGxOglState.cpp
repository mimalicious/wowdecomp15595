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

struct GlTexEnvOp {
    GLenum m_mode;
    GLfloat m_scale;
    GLenum m_source0;
    GLenum m_source1;
    GLenum m_source2;
    GLenum m_combine;
};

// STATIC: %0x00C0E848
static GlTexEnvOp s_texEnvOps[6] = {
    { GL_REPLACE, 1.0f, 0x8578, GL_TEXTURE, 0x8576, GL_MODULATE },
    { GL_MODULATE, 2.0f, 0x8578, GL_TEXTURE, 0x8576, GL_MODULATE },
    { GL_ADD, 1.0f, 0x8578, GL_TEXTURE, 0x8576, GL_ADD },
    { GL_REPLACE, 1.0f, 0x8578, GL_TEXTURE, 0x8576, GL_REPLACE },
    { GL_DECAL, 1.0f, 0x8578, GL_TEXTURE, 0x8578, 0x8575 },
    { GL_REPLACE, 1.0f, GL_TEXTURE, 0x8578, 0x8578, 0x8575 },
};

// STATIC: %0x00C0E8F4
static GLfloat s_texEnvColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// STATIC: %0x00AFBFB8
static const GLint s_glAttribSize[8] = { 4, 4, 4, 2, 3, 2, 1, 4 };

// STATIC: %0x00AFBFD8
static const GLenum s_glAttribType[8] = { GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_FLOAT, GL_FLOAT, GL_SHORT, GL_FLOAT, GL_FLOAT };

// STATIC: %0x00AFBFF8
static const GLboolean s_glAttribNormalized[8] = { GL_TRUE, GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE };

// STATIC: %0x00C0E7E0
static GLenum s_glBlendSrc[13] = { GL_ONE, GL_ONE, GL_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_COLOR, GL_DST_COLOR, GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE, GL_CONSTANT_ALPHA_EXT, GL_SRC_ALPHA_SATURATE };

// STATIC: %0x00C0E814
static GLenum s_glBlendDst[13] = { GL_ZERO, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO, GL_SRC_COLOR, GL_ONE, GL_ONE, GL_ZERO, GL_ZERO, GL_ONE, GL_ONE_MINUS_CONSTANT_ALPHA_EXT, GL_ONE };

// STATIC: %0x00C0E8D8
static GLenum s_glDepthFunc[4] = { GL_LEQUAL, GL_EQUAL, GL_GEQUAL, GL_LESS };

// STATIC: %0x00C0E8E8
static GLenum s_glColorMaterial[3] = { GL_AMBIENT_AND_DIFFUSE, GL_SPECULAR, GL_EMISSION };

// PROVEN: 6d58ef19 IDENTICAL/IDENTICAL
// OFFSET: 0x0085C570/0x14058E5B0
void CGxDeviceOpenGl::DsInit() {
    memset(this->m_deviceStates, 0, sizeof(this->m_deviceStates));
    this->m_deviceStates[Ds_DepthMask] = 1;
    this->m_deviceStates[Ds_ColorMask] = 1;
}

// OFFSET: 0x0085C5A0/0x14058E5F0
void CGxDeviceOpenGl::DsSet(EDeviceState state, uint32_t value, int32_t force) {
    uint32_t current = this->m_deviceStates[state];

    if (current == value && !force) {
        return;
    }

    switch (state) {
        case Ds_DepthMask:
            if (value) {
                glDepthMask(GL_TRUE);
            } else {
                glDepthMask(GL_FALSE);
            }

            break;

        case Ds_ColorMask:
            glColorMask(value & 1, (value & 4) != 0, (value & 2) != 0, (value & 8) != 0);
            break;

        case Ds_ActiveTexture:
            if (static_cast<uint32_t>(this->m_caps.m_numShaderTmus) > 1) {
                glActiveTextureARB(GL_TEXTURE0_ARB + value);

                if (value < 8) {
                    glClientActiveTextureARB(GL_TEXTURE0_ARB + value);
                }
            }

            break;

        case Ds_TexTarget0 + 0:
        case Ds_TexTarget0 + 1:
        case Ds_TexTarget0 + 2:
        case Ds_TexTarget0 + 3:
        case Ds_TexTarget0 + 4:
        case Ds_TexTarget0 + 5:
        case Ds_TexTarget0 + 6:
        case Ds_TexTarget0 + 7:
        case Ds_TexTarget0 + 8:
        case Ds_TexTarget0 + 9:
        case Ds_TexTarget0 + 10:
        case Ds_TexTarget0 + 11:
        case Ds_TexTarget0 + 12:
        case Ds_TexTarget0 + 13:
        case Ds_TexTarget0 + 14:
        case Ds_TexTarget0 + 15:
            if (static_cast<int32_t>(state - Ds_TexTarget0) < glMaxTextureUnits) {
                if (current) {
                    glDisable(current);
                }

                if (value) {
                    glEnable(value);
                }
            }

            break;

        case Ds_TexGenS0 + 0:
        case Ds_TexGenS0 + 1:
        case Ds_TexGenS0 + 2:
        case Ds_TexGenS0 + 3:
        case Ds_TexGenS0 + 4:
        case Ds_TexGenS0 + 5:
        case Ds_TexGenS0 + 6:
        case Ds_TexGenS0 + 7:
            if (value) {
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, value);
            } else {
                glDisable(GL_TEXTURE_GEN_S);
            }

            if (!current) {
                glEnable(GL_TEXTURE_GEN_S);
            }

            break;

        case Ds_TexGenT0 + 0:
        case Ds_TexGenT0 + 1:
        case Ds_TexGenT0 + 2:
        case Ds_TexGenT0 + 3:
        case Ds_TexGenT0 + 4:
        case Ds_TexGenT0 + 5:
        case Ds_TexGenT0 + 6:
        case Ds_TexGenT0 + 7:
            if (value) {
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, value);
            } else {
                glDisable(GL_TEXTURE_GEN_T);
            }

            if (!current) {
                glEnable(GL_TEXTURE_GEN_T);
            }

            break;

        case Ds_TexGenR0 + 0:
        case Ds_TexGenR0 + 1:
        case Ds_TexGenR0 + 2:
        case Ds_TexGenR0 + 3:
        case Ds_TexGenR0 + 4:
        case Ds_TexGenR0 + 5:
        case Ds_TexGenR0 + 6:
        case Ds_TexGenR0 + 7:
            if (value) {
                glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, value);
            } else {
                glDisable(GL_TEXTURE_GEN_R);
            }

            if (!current) {
                glEnable(GL_TEXTURE_GEN_R);
            }

            break;

        case Ds_TexGenQ0 + 0:
        case Ds_TexGenQ0 + 1:
        case Ds_TexGenQ0 + 2:
        case Ds_TexGenQ0 + 3:
        case Ds_TexGenQ0 + 4:
        case Ds_TexGenQ0 + 5:
        case Ds_TexGenQ0 + 6:
        case Ds_TexGenQ0 + 7:
            if (value) {
                glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, value);
            } else {
                glDisable(GL_TEXTURE_GEN_Q);
            }

            if (!current) {
                glEnable(GL_TEXTURE_GEN_Q);
            }

            break;

        case Ds_ColorOp0 + 0:
        case Ds_ColorOp0 + 1:
        case Ds_ColorOp0 + 2:
        case Ds_ColorOp0 + 3:
        case Ds_ColorOp0 + 4:
        case Ds_ColorOp0 + 5:
        case Ds_ColorOp0 + 6:
        case Ds_ColorOp0 + 7:
            if (static_cast<int32_t>(state - Ds_ColorOp0) < glMaxTextureUnits) {
                if (glARBTextureEnvCombine) {
                    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
                    glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, s_texEnvOps[value].m_scale);
                    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, s_texEnvOps[value].m_source0);
                    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, s_texEnvOps[value].m_source1);
                    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, s_texEnvOps[value].m_source2);
                    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, s_texEnvOps[value].m_combine);
                    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, s_texEnvColor);
                } else {
                    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, s_texEnvOps[value].m_mode);
                }
            }

            break;

        case Ds_AlphaOp0 + 0:
        case Ds_AlphaOp0 + 1:
        case Ds_AlphaOp0 + 2:
        case Ds_AlphaOp0 + 3:
        case Ds_AlphaOp0 + 4:
        case Ds_AlphaOp0 + 5:
        case Ds_AlphaOp0 + 6:
        case Ds_AlphaOp0 + 7:
            if (static_cast<int32_t>(state - Ds_AlphaOp0) < glMaxTextureUnits && glARBTextureEnvCombine) {
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
                glTexEnvf(GL_TEXTURE_ENV, GL_ALPHA_SCALE, s_texEnvOps[value].m_scale);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, s_texEnvOps[value].m_source0);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, s_texEnvOps[value].m_source1);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_ARB, s_texEnvOps[value].m_source2);
                glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, s_texEnvOps[value].m_combine);
            }

            break;

        case Ds_VertexArray0 + 0:
        case Ds_VertexArray0 + 1:
        case Ds_VertexArray0 + 2:
        case Ds_VertexArray0 + 3:
        case Ds_VertexArray0 + 4:
        case Ds_VertexArray0 + 5:
        case Ds_TexCoordArray0 + 0:
        case Ds_TexCoordArray0 + 1:
        case Ds_TexCoordArray0 + 2:
        case Ds_TexCoordArray0 + 3:
        case Ds_TexCoordArray0 + 4:
        case Ds_TexCoordArray0 + 5:
        case Ds_TexCoordArray0 + 6:
        case Ds_TexCoordArray0 + 7:
            if (this->m_deviceStates[Ds_VertexProgram]) {
                GLuint index = state - Ds_VertexArray0;

                if (value) {
                    glEnableVertexAttribArrayARB(index);
                } else {
                    glDisableVertexAttribArrayARB(index);
                }
            } else {
                GLenum clientStates[14] = { GL_VERTEX_ARRAY, 0, 0, GL_NORMAL_ARRAY, GL_COLOR_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY, GL_TEXTURE_COORD_ARRAY, GL_TEXTURE_COORD_ARRAY, GL_TEXTURE_COORD_ARRAY, GL_TEXTURE_COORD_ARRAY, GL_TEXTURE_COORD_ARRAY, GL_TEXTURE_COORD_ARRAY, GL_TEXTURE_COORD_ARRAY };
                GLenum clientState = clientStates[state - Ds_VertexArray0];

                if (value) {
                    glEnableClientState(clientState);
                } else {
                    glDisableClientState(clientState);
                }
            }

            break;

        case Ds_PolygonOffsetFill:
            if (value) {
                glEnable(GL_POLYGON_OFFSET_FILL);
            } else {
                glDisable(GL_POLYGON_OFFSET_FILL);
            }

            break;

        case Ds_PolygonOffset:
            glPolygonOffset(-4.0f, -1.0f * (reinterpret_cast<float&>(value) * 65536.0f));
            break;

        case Ds_Blend:
            if (value) {
                glEnable(GL_BLEND);
            } else {
                glDisable(GL_BLEND);
            }

            break;

        case Ds_AlphaTest:
            if (value) {
                glEnable(GL_ALPHA_TEST);
            } else {
                glDisable(GL_ALPHA_TEST);
            }

            break;

        case Ds_RegisterCombiners:
            if (glNVRegisterCombiners) {
                if (value) {
                    glEnable(GL_REGISTER_COMBINERS_NV);
                } else {
                    glDisable(GL_REGISTER_COMBINERS_NV);
                }
            }

            break;

        case Ds_PerStageConstants:
            if (glNVRegisterCombiners2) {
                if (value) {
                    glEnable(GL_PER_STAGE_CONSTANTS_NV);
                } else {
                    glDisable(GL_PER_STAGE_CONSTANTS_NV);
                }
            }

            break;

        case Ds_TextureShader:
            if (glNVTextureShader) {
                if (value) {
                    glEnable(GL_TEXTURE_SHADER_NV);
                } else {
                    glDisable(GL_TEXTURE_SHADER_NV);
                }
            }

            break;

        case Ds_FragmentProgram:
            if (glARBFragmentProgram) {
                if (value) {
                    glEnable(GL_FRAGMENT_PROGRAM_ARB);
                } else {
                    glDisable(GL_FRAGMENT_PROGRAM_ARB);
                }
            }

            break;

        case Ds_VertexProgram:
            if (glARBVertexProgram) {
                for (int32_t i = Ds_VertexArray0; i < Ds_TexCoordArray0; i++) {
                    this->DsSet(static_cast<EDeviceState>(i), 0, 0);
                }

                for (uint32_t tmu = 0; tmu < static_cast<uint32_t>(this->m_caps.m_numTmus); tmu++) {
                    this->DsSet(Ds_ActiveTexture, tmu, 0);
                    this->DsSet(static_cast<EDeviceState>(Ds_TexCoordArray0 + tmu), 0, 0);
                }

                this->m_attribDirty = 0xFFFFFFFF;

                if (value) {
                    glEnable(GL_VERTEX_PROGRAM_ARB);
                } else {
                    glDisable(GL_VERTEX_PROGRAM_ARB);
                }
            }

            break;

        case Ds_MatrixMode:
            glMatrixMode(value);
            break;

        case Ds_BlendFunc:
            glBlendFunc(value >> 16, value & 0xFFFF);
            break;

        case Ds_ArrayBuffer:
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, value);
            break;

        case Ds_ElementArrayBuffer:
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, value);
            break;

        case Ds_CullFace:
            glCullFace(value);
            break;

        case Ds_ClipPlanes: {
            uint32_t changed = current ^ value;

            for (uint32_t plane = 0; plane < 6; plane++) {
                if ((1 << plane) & changed) {
                    if ((1 << plane) & value) {
                        glEnable(GL_CLIP_PLANE0 + plane);
                    } else {
                        glDisable(GL_CLIP_PLANE0 + plane);
                    }
                }
            }

            break;
        }

        default:
            break;
    }

    this->m_deviceStates[state] = value;
}

static void ColorToFloats(GLfloat* out, const CImVector& color) {
    out[0] = color.r * (1.0f / 255.0f);
    out[1] = color.g * (1.0f / 255.0f);
    out[2] = color.b * (1.0f / 255.0f);
    out[3] = color.a * (1.0f / 255.0f);
}

// OFFSET: 0x0085CC80/0x14058EE10
void CGxDeviceOpenGl::IStateSyncIndexPtr() {
    CGxBuf* buf = this->m_primIndexBuf;

    if (!buf) {
        return;
    }

    CGxPool* pool = buf->m_pool;

    if (glARBVertexBufferObject) {
        if (this->m_useVboBroker[pool->m_target][pool->m_usage]) {
            GLuint vbo;
            uint32_t offset;
            static_cast<CGxVboBroker*>(pool->m_apiSpecific)->GetLastSelectedVbo(vbo, offset);

            if (this->m_deviceStates[Ds_ElementArrayBuffer] != vbo) {
                glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo);
                this->m_deviceStates[Ds_ElementArrayBuffer] = vbo;
            }

            this->m_indexPtr = this->m_primIndexBuf->m_itemSize * this->m_primIndexBuf->m_index;
            return;
        }

        if (this->m_useVbo[pool->m_target][pool->m_usage]) {
            GLuint name = static_cast<GLuint>(reinterpret_cast<uintptr_t>(pool->m_apiSpecific));

            if (this->m_deviceStates[Ds_ElementArrayBuffer] != name) {
                glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, name);
                this->m_deviceStates[Ds_ElementArrayBuffer] = name;
            }

            this->m_indexPtr = this->m_primIndexBuf->m_itemSize * this->m_primIndexBuf->m_index;
            return;
        }
    }

    this->m_indexPtr = reinterpret_cast<uintptr_t>(pool->m_apiSpecific) + buf->m_itemSize * buf->m_index;
}

// PROVEN: 0b438639 IDENTICAL/DIFFERENT
// OFFSET: 0x0085CD60/0x14058EEE0
void CGxDeviceOpenGl::IStateSyncVertexFormat() {
    EGxVertexBufferFormat format = this->m_primVertexFormats[0];
    CGxBuf* buf = this->m_primVertexBufs[0];
    const VertexBufDesc& desc = Buffer::s_vertexBufDesc[format];

    if (this->m_curVertexBuf != buf) {
        this->m_attribDirty |= desc.mask;
        this->m_curVertexBuf = buf;
    }

    if (this->m_curVertexFormat == format) {
        return;
    }

    if (this->m_curVertexFormat != GxVertexBufferFormats_Last) {
        const VertexBufDesc& prev = Buffer::s_vertexBufDesc[this->m_curVertexFormat];

        for (uint32_t i = 0; i < desc.attribCount; i++) {
            const CGxVertexAttrib& attrib = desc.attribs[i];
            const CGxVertexAttrib* prevAttrib = prev.attribPtrs[attrib.m_attrib];

            if (prevAttrib && (attrib.m_type != prevAttrib->m_type || attrib.m_offset != prevAttrib->m_offset)) {
                this->m_attribDirty |= 1 << attrib.m_attrib;
            }
        }

        this->m_attribDirty |= desc.mask ^ prev.mask;
    }

    this->m_curVertexFormat = this->m_primVertexFormats[0];
}

// OFFSET: 0x0085CE30
int32_t CGxDeviceOpenGl::XformIsValidViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    const CRect& window = this->DeviceCurWindow();
    float height = window.maxY;
    float width = window.maxX;

    if (this->m_textureTarget[GxBuffers_Color].m_texture) {
        CGxTex* target = static_cast<CGxTex*>(this->m_textureTarget[GxBuffers_Color].m_apiSpecific);
        width = static_cast<float>(target->m_width);
        height = static_cast<float>(target->m_height);
    }

    int32_t left = static_cast<int32_t>(width * minX + 0.5f);
    int32_t top = static_cast<int32_t>(height * minY + 0.5f);
    int32_t right = static_cast<int32_t>(width * maxX + 0.5f);
    int32_t bottom = static_cast<int32_t>(height * maxY + 0.5f);

    return right - left > 0 && bottom - top > 0 && minZ >= 0.0f && maxZ <= 1.0f && minZ <= maxZ && left >= 0 && top >= 0 && static_cast<float>(right) <= width && static_cast<float>(bottom) <= height;
}

// OFFSET: 0x0085CF40/0x14058F120
void CGxDeviceOpenGl::IStateSyncViewport() {
    const CRect& window = this->DeviceCurWindow();

    float width;
    float height;

    if (this->m_textureTarget[GxBuffers_Color].m_texture) {
        CGxTex* target = static_cast<CGxTex*>(this->m_textureTarget[GxBuffers_Color].m_apiSpecific);
        width = static_cast<float>(target->m_width);
        height = static_cast<float>(target->m_height);
    } else {
        width = window.maxX;
        height = window.maxY;
    }

    GLint x = static_cast<GLint>(this->m_viewport.m_x.m_l * width + 0.5f);
    GLint y = static_cast<GLint>(this->m_viewport.m_y.m_l * height + 0.5f);
    glViewport(x, y, static_cast<GLint>(width * this->m_viewport.m_x.m_h + 0.5f) - x, static_cast<GLint>(height * this->m_viewport.m_y.m_h + 0.5f) - y);
    glDepthRange(this->m_viewport.m_z.m_l, this->m_viewport.m_z.m_h);
    this->m_viewportDirty = 0;
}

// OFFSET: 0x0085D040/0x14058F210
void CGxDeviceOpenGl::IStateSyncEnables() {
    if (this->m_appMasterEnables == this->m_hwMasterEnables) {
        return;
    }

    int32_t enable = 0;

    if (this->NeedsUpdate(this->m_appMasterEnables, this->m_hwMasterEnables, 0, 0, 8, enable)) {
        if (enable) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    this->m_hwMasterEnables = this->m_appMasterEnables;
}

// PROVEN: a87f1cd3 IDENTICAL/IDENTICAL
// OFFSET: 0x0085D0B0/0x14058F290
void CGxDeviceOpenGl::IStateSyncClipPlanes() {
    if (!this->m_clipPlaneMask) {
        return;
    }

    for (uint32_t i = 0; i < 6; i++) {
        if ((1 << i) & this->m_clipPlaneMask) {
            GLdouble equation[4];
            equation[0] = this->m_clipPlane[i].n.x;
            equation[1] = this->m_clipPlane[i].n.y;
            equation[2] = this->m_clipPlane[i].n.z;
            equation[3] = this->m_clipPlane[i].d;
            glClipPlane(GL_CLIP_PLANE0 + i, equation);
        }
    }

    this->m_clipPlaneMask = 0;
}

// OFFSET: 0x0085D130/0x14058F340
void CGxDeviceOpenGl::IStateSyncScissorRect() {
    if (!this->m_scissorRectDirty) {
        return;
    }

    const CRect& window = this->DeviceCurWindow();

    float width;
    float height;

    width = window.maxX;
    height = window.maxY;

    CGxTex* target = static_cast<CGxTex*>(this->m_textureTarget[GxBuffers_Color].m_apiSpecific);

    if (target || (target = static_cast<CGxTex*>(this->m_textureTarget[GxBuffers_Depth].m_apiSpecific)) != NULL) {
        width = static_cast<float>(target->m_width);
        height = static_cast<float>(target->m_height);
    }

    GLint x = static_cast<GLint>(this->m_scissorRect.minX * width + 0.5f);
    GLint y = static_cast<GLint>(this->m_scissorRect.minY * height + 0.5f);
    glScissor(x, y, static_cast<GLint>(width * this->m_scissorRect.maxX + 0.5f) - x, static_cast<GLint>(height * this->m_scissorRect.maxY + 0.5f) - y);
    this->m_scissorRectDirty = 0;
}

// OFFSET: 0x0085D220/0x14058F430
void CGxDeviceOpenGl::ISetTexture(uint32_t tmu, CGxTex* texId) {
    if (tmu >= static_cast<uint32_t>(this->m_caps.m_numShaderTmus)) {
        return;
    }

    this->DsSet(Ds_ActiveTexture, tmu, 0);

    if (texId) {
        this->DsSet(static_cast<EDeviceState>(Ds_TexTarget0 + tmu), s_glTexTarget[texId->m_target], 0);
        this->BindTexture(texId, tmu);
        this->ITexMarkAsUpdated(texId, tmu);
        this->IResourceTouch(texId);
    } else {
        this->DsSet(static_cast<EDeviceState>(Ds_TexTarget0 + tmu), 0, 0);
    }
}

// OFFSET: 0x0085DEF0/0x140590240
void CGxDeviceOpenGl::ISetColorOp(uint32_t tmu, uint32_t op) {
    if (tmu >= static_cast<uint32_t>(this->m_caps.m_numTmus)) {
        return;
    }

    this->DsSet(Ds_ActiveTexture, tmu, 0);
    this->DsSet(static_cast<EDeviceState>(Ds_ColorOp0 + tmu), op, 0);
}

// OFFSET: 0x0085E610/0x140590A50
void CGxDeviceOpenGl::ISetAlphaOp(uint32_t tmu, uint32_t op) {
    if (tmu >= static_cast<uint32_t>(this->m_caps.m_numTmus)) {
        return;
    }

    this->DsSet(Ds_ActiveTexture, tmu, 0);
    this->DsSet(static_cast<EDeviceState>(Ds_AlphaOp0 + tmu), op, 0);
}

// OFFSET: 0x0085ED30/0x140591260
void CGxDeviceOpenGl::IStateSyncVertexPtrs() {
    if (!this->m_attribDirty) {
        return;
    }

    uint32_t mask = Buffer::s_vertexBufDesc[this->m_primVertexFormats[0]].mask;
    uint32_t bit = 1;

    for (uint32_t attrib = 0; attrib < GxVAs_Last; attrib++, bit <<= 1) {
        if (!(bit & this->m_attribDirty)) {
            continue;
        }

        if (!(bit & mask)) {
            switch (attrib) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    this->DsSet(static_cast<EDeviceState>(Ds_VertexArray0 + attrib), 0, 0);
                    break;

                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13: {
                    uint32_t tmu = attrib - 6;

                    if (tmu < static_cast<uint32_t>(this->m_caps.m_numTmus)) {
                        this->DsSet(Ds_ActiveTexture, tmu, 0);
                        this->DsSet(static_cast<EDeviceState>(Ds_TexCoordArray0 + tmu), 0, 0);
                    }

                    break;
                }

                default:
                    break;
            }

            continue;
        }

        const VertexBufDesc& desc = Buffer::s_vertexBufDesc[this->m_primVertexFormats[0]];
        GLsizei stride = desc.size;
        const CGxVertexAttrib* vertexAttrib = desc.attribPtrs[attrib];
        CGxBuf* buf = this->m_primVertexBufs[0];
        CGxPool* pool = buf->m_pool;
        uintptr_t base;

        if (glARBVertexBufferObject && this->m_useVboBroker[pool->m_target][pool->m_usage]) {
            GLuint vbo;
            uint32_t offset;
            static_cast<CGxVboBroker*>(pool->m_apiSpecific)->GetLastSelectedVbo(vbo, offset);

            if (this->m_deviceStates[Ds_ArrayBuffer] != vbo) {
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
                this->m_deviceStates[Ds_ArrayBuffer] = vbo;
            }

            base = buf->m_itemSize * buf->m_index;
        } else if (glARBVertexBufferObject && this->m_useVbo[pool->m_target][pool->m_usage]) {
            GLuint name = static_cast<GLuint>(reinterpret_cast<uintptr_t>(pool->m_apiSpecific));

            if (this->m_deviceStates[Ds_ArrayBuffer] != name) {
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, name);
                this->m_deviceStates[Ds_ArrayBuffer] = name;
            }

            base = buf->m_itemSize * buf->m_index;
        } else {
            base = reinterpret_cast<uintptr_t>(pool->m_apiSpecific) + buf->m_itemSize * buf->m_index;
        }

        if (!this->m_deviceStates[Ds_VertexProgram]) {
            switch (attrib) {
                case 0:
                    this->DsSet(Ds_VertexArray0, 1, 0);
                    glVertexPointer(s_glAttribSize[vertexAttrib->m_type], s_glAttribType[vertexAttrib->m_type], stride, reinterpret_cast<const GLvoid*>(base + vertexAttrib->m_offset));
                    break;

                case 3:
                    glNormalPointer(s_glAttribType[vertexAttrib->m_type], stride, reinterpret_cast<const GLvoid*>(base + vertexAttrib->m_offset));
                    this->DsSet(static_cast<EDeviceState>(Ds_VertexArray0 + 3), 1, 0);
                    break;

                case 4:
                    glColorPointer(s_glAttribSize[vertexAttrib->m_type], s_glAttribType[vertexAttrib->m_type], stride, reinterpret_cast<const GLvoid*>(base + vertexAttrib->m_offset));
                    this->DsSet(static_cast<EDeviceState>(Ds_VertexArray0 + 4), 1, 0);
                    break;

                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13: {
                    uint32_t tmu = 0;

                    for (uint32_t i = 0; i < 8; i++) {
                        if (this->m_appRenderStates[GxRs_TextureCoord0 + i].m_value.m_data.u[0] == attrib - 6) {
                            tmu = i;
                            break;
                        }
                    }

                    if (tmu < static_cast<uint32_t>(this->m_caps.m_numTmus)) {
                        this->DsSet(Ds_ActiveTexture, tmu, 0);
                        glTexCoordPointer(s_glAttribSize[vertexAttrib->m_type], s_glAttribType[vertexAttrib->m_type], stride, reinterpret_cast<const GLvoid*>(base + vertexAttrib->m_offset));
                        this->DsSet(static_cast<EDeviceState>(Ds_TexCoordArray0 + tmu), 1, 0);
                    }

                    break;
                }

                default:
                    break;
            }
        } else {
            switch (attrib) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    glVertexAttribPointerARB(attrib, s_glAttribSize[vertexAttrib->m_type], s_glAttribType[vertexAttrib->m_type], s_glAttribNormalized[vertexAttrib->m_type], stride, reinterpret_cast<const GLvoid*>(base + vertexAttrib->m_offset));
                    this->DsSet(static_cast<EDeviceState>(Ds_VertexArray0 + attrib), 1, 0);
                    break;

                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13: {
                    uint32_t tmu = 0;

                    for (uint32_t i = 0; i < 8; i++) {
                        if (this->m_appRenderStates[GxRs_TextureCoord0 + i].m_value.m_data.u[0] == attrib - 6) {
                            tmu = i;
                            break;
                        }
                    }

                    if (tmu < static_cast<uint32_t>(this->m_caps.m_numTmus)) {
                        this->DsSet(Ds_ActiveTexture, tmu, 0);
                        glVertexAttribPointerARB(attrib, s_glAttribSize[vertexAttrib->m_type], s_glAttribType[vertexAttrib->m_type], s_glAttribNormalized[vertexAttrib->m_type], stride, reinterpret_cast<const GLvoid*>(base + vertexAttrib->m_offset));
                        this->DsSet(static_cast<EDeviceState>(Ds_VertexArray0 + attrib), 1, 0);
                    }

                    break;
                }

                default:
                    break;
            }
        }
    }

    this->m_attribDirty = 0;
}

// OFFSET: 0x00860370/0x140592FE0
void CGxDeviceOpenGl::IStateSyncLights() {
    if (this->m_appRenderStates[GxRs_Lighting].m_value.m_data.i[0]) {
        if (this->m_ambientOnlyMode) {
            this->m_ambientOnlyMode = 0;
        }

        if (this->m_appMasterEnables & 0x1) {
            glEnable(GL_LIGHTING);
        } else {
            glDisable(GL_LIGHTING);
        }

        GLfloat ambient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

        int32_t modelViewSet = 0;
        uint16_t mask = this->m_appRenderStates[GxRs_MatSpecularExp].m_value.m_data.f[0] == 0.0f ? 0xFFEF : 0xFFFF;
        uint16_t force = this->m_viewDirty ? 2 : 0;
        GLenum glLight = GL_LIGHT0;

        for (uint32_t i = 0; i < 4; i++, glLight++) {
            CGxApiLight& light = this->m_lights[i];
            uint16_t flags = force | (mask & light.m_flags & light.unk_0046);

            if (!flags) {
                continue;
            }

            if (flags & 0x1) {
                if (!light.m_enable) {
                    light.m_flags &= ~0x1;
                    glDisable(glLight);
                    continue;
                }

                glEnable(glLight);
            } else if (!light.m_enable) {
                continue;
            }

            if (flags & 0x2) {
                if (!modelViewSet) {
                    CGxMatrixStack& view = this->m_xforms[GxXform_View];
                    this->IXformSetModelView(view.m_mtx[view.m_level]);
                    modelViewSet = 1;
                }

                if (light.m_dir.w == 1.0f) {
                    glLightfv(glLight, GL_POSITION, &light.m_dir.x);
                } else {
                    GLfloat direction[4];
                    direction[0] = -light.m_dir.x;
                    direction[1] = -light.m_dir.y;
                    direction[2] = -light.m_dir.z;
                    direction[3] = -light.m_dir.w;
                    glLightfv(glLight, GL_POSITION, direction);
                }
            }

            if (flags & 0x20) {
                glLightf(glLight, GL_CONSTANT_ATTENUATION, light.m_constantAttenuation);
            }

            if (flags & 0x40) {
                glLightf(glLight, GL_LINEAR_ATTENUATION, light.m_linearAttenuation);
            }

            if (flags & 0x80) {
                glLightf(glLight, GL_QUADRATIC_ATTENUATION, light.m_quadraticAttenuation);
            }

            if (flags & 0x4) {
                GLfloat color[4];
                color[0] = light.m_ambColor.x;
                color[1] = light.m_ambColor.y;
                color[2] = light.m_ambColor.z;
                color[3] = 0.0f;
                glLightfv(glLight, GL_AMBIENT, color);
            }

            if (flags & 0x8) {
                GLfloat color[4];
                color[0] = light.m_dirColor.x;
                color[1] = light.m_dirColor.y;
                color[2] = light.m_dirColor.z;
                color[3] = 1.0f;
                glLightfv(glLight, GL_DIFFUSE, color);
            }

            if (flags & 0x10) {
                GLfloat color[4];
                color[0] = light.m_specColor.x;
                color[1] = light.m_specColor.y;
                color[2] = light.m_specColor.z;
                color[3] = 0.0f;
                glLightfv(glLight, GL_SPECULAR, color);
            }

            light.m_flags &= ~flags;
        }

        this->m_viewDirty |= modelViewSet;
        return;
    }

    if ((Buffer::s_vertexBufDesc[this->m_primVertexFormats[0]].mask & (1 << GxVA_Color0)) || this->m_appRenderStates[GxRs_MatDiffuse].m_value.m_data.u[0] == 0xFFFFFFFF) {
        if (this->m_ambientOnlyMode) {
            this->m_ambientOnlyMode = 0;
        }

        glDisable(GL_LIGHTING);

        GLfloat ambient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    } else {
        if (!this->m_ambientOnlyMode) {
            this->m_ambientOnlyMode = 1;

            for (uint32_t i = 0; i < 4; i++) {
                glDisable(GL_LIGHT0 + i);

                if (this->m_lights[i].m_enable) {
                    this->m_lights[i].m_flags |= 0x1;
                } else {
                    this->m_lights[i].m_flags &= ~0x1;
                }
            }
        }

        glEnable(GL_LIGHTING);

        GLfloat ambient[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    }
}

// OFFSET: 0x00860730/0x140593440
void CGxDeviceOpenGl::IStateSyncMaterial() {
    int32_t colorMaterial = this->m_appRenderStates[GxRs_ColorMaterial].m_value.m_data.i[0];

    if (this->m_attribDirty & (1 << GxVA_Color0)) {
        this->m_materialDirty |= 1 << colorMaterial;
    }

    if (!this->m_materialDirty) {
        return;
    }

    uint32_t mask = Buffer::s_vertexBufDesc[this->m_primVertexFormats[0]].mask;
    GLfloat color[4];

    if (this->m_materialDirty & 0x1) {
        ColorToFloats(color, reinterpret_cast<const CImVector&>(this->m_appRenderStates[GxRs_MatDiffuse].m_value.m_data.u[0]));

        if (colorMaterial) {
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
        } else if (!(mask & (1 << GxVA_Color0))) {
            glColor4fv(color);
        }
    }

    if (this->m_materialDirty & 0x2) {
        ColorToFloats(color, reinterpret_cast<const CImVector&>(this->m_appRenderStates[GxRs_MatSpecular].m_value.m_data.u[0]));

        if (colorMaterial == 1) {
            if (!(mask & (1 << GxVA_Color0))) {
                glColor4fv(color);
            }
        } else {
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
        }
    }

    if (this->m_materialDirty & 0x4) {
        ColorToFloats(color, reinterpret_cast<const CImVector&>(this->m_appRenderStates[GxRs_MatEmissive].m_value.m_data.u[0]));

        if (colorMaterial == 2) {
            if (!(mask & (1 << GxVA_Color0))) {
                glColor4fv(color);
                this->m_materialDirty = 0;
                return;
            }
        } else {
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color);
        }
    }

    this->m_materialDirty = 0;
}

// OFFSET: 0x00860960/0x1405936D0
void CGxDeviceOpenGl::IStateSyncXforms() {
    if (this->m_viewDirty) {
        CGxMatrixStack& world = this->m_xforms[GxXform_World];
        CGxMatrixStack& view = this->m_xforms[GxXform_View];
        this->IXformSetModelView(world.m_mtx[world.m_level] * view.m_mtx[view.m_level]);
        world.m_dirty = 0;
        view.m_dirty = 0;
    }

    for (uint32_t tmu = 0; tmu < static_cast<uint32_t>(this->m_caps.m_numTmus); tmu++) {
        CGxMatrixStack& xform = this->m_xforms[tmu];
        CGxMatrixStack& texGen = this->m_texGen[tmu];

        if (!this->m_appRenderStates[GxRs_Texture0 + tmu].m_value.m_data.p || (!xform.m_dirty && !texGen.m_dirty)) {
            continue;
        }

        this->DsSet(Ds_ActiveTexture, tmu, 0);
        this->DsSet(Ds_MatrixMode, GL_TEXTURE, 0);

        // STATIC: %0x00D930C8
        static C44Matrix s_matrix;

        int32_t identity;

        if (static_cast<uint32_t>(this->m_appRenderStates[GxRs_TextureShader0 + tmu].m_value.m_data.i[0] - 1) > 1) {
            identity = 1;
            s_matrix.Identity();
        } else {
            identity = 0;
            s_matrix = texGen.m_mtx[texGen.m_level] * xform.m_mtx[xform.m_level];
        }

        xform.m_dirty = 0;
        texGen.m_dirty = 0;

        if (identity) {
            glLoadIdentity();
        } else {
            glLoadMatrixf(&s_matrix.a0);
        }
    }

    this->DsSet(Ds_MatrixMode, GL_MODELVIEW, 0);
}

// OFFSET: 0x00860C50/0x140593AC0
void CGxDeviceOpenGl::IStateSetContextDefaults() {
    this->DsInit();

    GLfloat ambient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    glMatrixMode(GL_MODELVIEW);
    this->m_deviceStates[Ds_MatrixMode] = GL_MODELVIEW;
    glLoadIdentity();

    GLfloat planeR[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
    GLfloat planeQ[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    for (uint32_t tmu = 0; tmu != static_cast<uint32_t>(this->m_caps.m_numTmus); tmu++) {
        if (static_cast<uint32_t>(this->m_caps.m_numShaderTmus) > 1) {
            glActiveTextureARB(GL_TEXTURE0_ARB + tmu);

            if (tmu < 8) {
                glClientActiveTextureARB(GL_TEXTURE0_ARB + tmu);
            }
        }

        this->m_deviceStates[Ds_ActiveTexture] = tmu;
        glTexGenfv(GL_R, GL_EYE_PLANE, planeR);
        glTexGenfv(GL_Q, GL_EYE_PLANE, planeQ);
        glTexGenfv(GL_R, GL_OBJECT_PLANE, planeR);
        glTexGenfv(GL_Q, GL_OBJECT_PLANE, planeQ);
    }

    if (glARBPointSprite) {
        for (uint32_t tmu = 0; tmu < static_cast<uint32_t>(this->m_caps.m_numTmus); tmu++) {
            glActiveTextureARB(GL_TEXTURE0_ARB + tmu);
            glClientActiveTextureARB(GL_TEXTURE0_ARB + tmu);
            glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
        }
    }

    this->InitLights();
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

    CGxMatrixStack& world = this->m_xforms[GxXform_World];
    CGxMatrixStack& view = this->m_xforms[GxXform_View];
    this->IXformSetModelView(world.m_mtx[world.m_level] * view.m_mtx[view.m_level]);
    this->m_viewDirty = 0;

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_SCISSOR_TEST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_DENSITY, 0.0f);
}

// OFFSET: 0x00860F20/0x140593DD0
void CGxDeviceOpenGl::ISetTexGen(uint32_t tmu, int32_t texGen) {
    if (tmu >= static_cast<uint32_t>(this->m_caps.m_numTmus)) {
        return;
    }

    this->DsSet(Ds_ActiveTexture, tmu, 0);

    switch (texGen) {
        case GxTexGen_Disabled:
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenS0 + tmu), 0, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenT0 + tmu), 0, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenR0 + tmu), 0, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenQ0 + tmu), 0, 0);
            break;

        case GxTexGen_ObjectLinear:
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenS0 + tmu), GL_OBJECT_LINEAR, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenT0 + tmu), GL_OBJECT_LINEAR, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenR0 + tmu), GL_OBJECT_LINEAR, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenQ0 + tmu), 0, 0);
            break;

        case GxTexGen_EyeLinear:
        case GxTexGen_Unk3:
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenS0 + tmu), GL_EYE_LINEAR, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenT0 + tmu), GL_EYE_LINEAR, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenR0 + tmu), GL_EYE_LINEAR, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenQ0 + tmu), 0, 0);
            break;

        case GxTexGen_ReflectionMap:
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenS0 + tmu), GL_REFLECTION_MAP_ARB, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenT0 + tmu), GL_REFLECTION_MAP_ARB, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenR0 + tmu), GL_REFLECTION_MAP_ARB, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenQ0 + tmu), 0, 0);
            break;

        case GxTexGen_Normal:
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenS0 + tmu), GL_NORMAL_MAP_ARB, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenT0 + tmu), GL_NORMAL_MAP_ARB, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenR0 + tmu), GL_NORMAL_MAP_ARB, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenQ0 + tmu), 0, 0);
            break;

        case GxTexGen_SphereMap:
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenS0 + tmu), this->m_sphereMapMode, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenT0 + tmu), this->m_sphereMapMode, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenR0 + tmu), 0, 0);
            this->DsSet(static_cast<EDeviceState>(Ds_TexGenQ0 + tmu), 0, 0);
            break;

        default:
            break;
    }

    if (texGen == GxTexGen_EyeLinear) {
        CGxMatrixStack& stack = this->m_texGen[tmu];
        stack.m_dirty = 1;
        stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;

        C44Matrix& matrix = stack.m_mtx[stack.m_level];
        CGxMatrixStack& view = this->m_xforms[GxXform_View];
        CGxDeviceOpenGl::IXformGLModelView(view.m_mtx[view.m_level], matrix);
        matrix.AffineInvertInPlace();
    } else {
        this->m_texGen[tmu].Identity();
    }
}

// OFFSET: 0x00867EB0/0x14059B5C0
void CGxDeviceOpenGl::IRsSendToHw(EGxRenderState which) {
    const CGxStateBom& value = this->m_appRenderStates[which].m_value;

    switch (which) {
        case GxRs_PolygonOffset:
            if (value.m_data.f[0] == 0.0f) {
                this->DsSet(Ds_PolygonOffsetFill, 0, 0);
            } else {
                this->DsSet(Ds_PolygonOffset, reinterpret_cast<const uint32_t&>(CGxDevice::s_polygonOffsetValues[value.m_data.u[0]]), 0);
                this->DsSet(Ds_PolygonOffsetFill, 1, 0);
            }

            break;

        case GxRs_MatDiffuse:
            this->m_materialDirty |= 0x1;
            break;

        case GxRs_MatEmissive:
            this->m_materialDirty |= 0x4;
            break;

        case GxRs_MatSpecular:
            this->m_materialDirty |= 0x2;
            break;

        case GxRs_MatSpecularExp:
            glMaterialf(GL_FRONT, GL_SHININESS, value.m_data.f[0]);
            break;

        case GxRs_NormalizeNormals:
            if (value.m_data.i[0]) {
                glEnable(GL_NORMALIZE);
            } else {
                glDisable(GL_NORMALIZE);
            }

            break;

        case GxRs_BlendingMode: {
            uint32_t mode = value.m_data.u[0];

            if (mode < 2) {
                this->DsSet(Ds_Blend, 0, 0);
            } else {
                this->DsSet(Ds_Blend, 1, 0);
                this->DsSet(Ds_BlendFunc, s_glBlendDst[mode] | (s_glBlendSrc[mode] << 16), 0);
            }

            break;
        }

        case GxRs_AlphaRef:
            if (value.m_data.i[0] <= 0) {
                this->DsSet(Ds_AlphaTest, 0, 0);
            } else {
                glAlphaFunc(GL_GEQUAL, value.m_data.i[0] * (1.0f / 255.0f));
                this->DsSet(Ds_AlphaTest, 1, 0);
            }

            break;

        case GxRs_FogStart:
            glFogf(GL_FOG_START, value.m_data.f[0]);
            break;

        case GxRs_FogEnd:
            glFogf(GL_FOG_END, value.m_data.f[0]);
            break;

        case GxRs_FogColor: {
            GLfloat color[4];
            ColorToFloats(color, reinterpret_cast<const CImVector&>(value.m_data.u[0]));
            glFogfv(GL_FOG_COLOR, color);

            if (this->m_deviceStates[Ds_VertexProgram] == 1) {
                glDisable(GL_VERTEX_PROGRAM_ARB);
                glEnable(GL_VERTEX_PROGRAM_ARB);
            } else {
                glEnable(GL_VERTEX_PROGRAM_ARB);
                glDisable(GL_VERTEX_PROGRAM_ARB);
            }

            break;
        }

        case GxRs_Lighting:
            if ((this->m_appMasterEnables & 0x1) && value.m_data.i[0]) {
                glEnable(GL_LIGHTING);
            } else {
                glDisable(GL_LIGHTING);
            }

            break;

        case GxRs_Fog:
            if ((this->m_appMasterEnables & 0x2) && value.m_data.i[0]) {
                glEnable(GL_FOG);
            } else {
                glDisable(GL_FOG);
            }

            break;

        case GxRs_DepthTest:
            if ((this->m_appMasterEnables & 0x4) && value.m_data.i[0]) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }

            break;

        case GxRs_DepthFunc:
            glDepthFunc(s_glDepthFunc[value.m_data.i[0]]);
            break;

        case GxRs_DepthWrite:
            this->DsSet(Ds_DepthMask, (this->m_appMasterEnables & 0x8) ? value.m_data.u[0] : 0, 0);
            break;

        case GxRs_ColorWrite:
            this->DsSet(Ds_ColorMask, (this->m_appMasterEnables & 0x10) ? value.m_data.u[0] : 0, 0);
            break;

        case GxRs_Culling: {
            if (!(this->m_appMasterEnables & 0x20)) {
                glDisable(GL_CULL_FACE);
                break;
            }

            uint32_t culling = value.m_data.u[0];

            if (culling <= 2) {
                if (!culling) {
                    glDisable(GL_CULL_FACE);
                    break;
                }
            } else {
                culling = 2;
            }

            glEnable(GL_CULL_FACE);

            if (culling == 1) {
                glFrontFace(GL_CCW);
            } else {
                glFrontFace(GL_CW);
            }

            break;
        }

        case GxRs_ClipPlaneMask:
            this->DsSet(Ds_ClipPlanes, value.m_data.u[0], 0);
            break;

        case GxRs_Multisample:
            if (glARBMultisample) {
                if (value.m_data.i[0]) {
                    glEnable(GL_MULTISAMPLE_ARB);
                } else {
                    glDisable(GL_MULTISAMPLE_ARB);
                }
            }

            break;

        case GxRs_ScissorTest:
            if (value.m_data.i[0]) {
                glEnable(GL_SCISSOR_TEST);
            } else {
                glDisable(GL_SCISSOR_TEST);
            }

            break;

        case GxRs_Texture0:
        case GxRs_Texture1:
        case GxRs_Texture2:
        case GxRs_Texture3:
        case GxRs_Texture4:
        case GxRs_Texture5:
        case GxRs_Texture6:
        case GxRs_Texture7:
        case GxRs_Texture8:
        case GxRs_Texture9:
        case GxRs_Texture10:
        case GxRs_Texture11:
        case GxRs_Texture12:
        case GxRs_Texture13:
        case GxRs_Texture14:
        case GxRs_Texture15:
            this->ISetTexture(which - GxRs_Texture0, static_cast<CGxTex*>(value.m_data.p));
            break;

        case GxRs_ColorOp0:
        case GxRs_ColorOp1:
        case GxRs_ColorOp2:
        case GxRs_ColorOp3:
        case GxRs_ColorOp4:
        case GxRs_ColorOp5:
        case GxRs_ColorOp6:
        case GxRs_ColorOp7:
            this->ISetColorOp(which - GxRs_ColorOp0, value.m_data.u[0]);
            break;

        case GxRs_AlphaOp0:
        case GxRs_AlphaOp1:
        case GxRs_AlphaOp2:
        case GxRs_AlphaOp3:
        case GxRs_AlphaOp4:
        case GxRs_AlphaOp5:
        case GxRs_AlphaOp6:
        case GxRs_AlphaOp7:
            this->ISetAlphaOp(which - GxRs_AlphaOp0, value.m_data.u[0]);
            break;

        case GxRs_TexGen0:
        case GxRs_TexGen1:
        case GxRs_TexGen2:
        case GxRs_TexGen3:
        case GxRs_TexGen4:
        case GxRs_TexGen5:
        case GxRs_TexGen6:
        case GxRs_TexGen7:
            this->ISetTexGen(which - GxRs_TexGen0, value.m_data.i[0]);
            break;

        case GxRs_VertexShader:
            this->IBindVertexShader(static_cast<CGxShaderPermute*>(value.m_data.p));
            break;

        case GxRs_PixelShader:
            this->IBindPixelShader(static_cast<CGxShaderPermute*>(value.m_data.p));
            break;

        case GxRs_PointScale:
            glPointSize(value.m_data.f[0]);
            break;

        case GxRs_PointScaleAttenuation:
            if (glARBPointParameters) {
                glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, value.m_data.f);
            }

            break;

        case GxRs_PointScaleMin:
            if (glARBPointParameters) {
                glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, value.m_data.f[0]);
            }

            break;

        case GxRs_PointScaleMax:
            if (glARBPointParameters) {
                glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, value.m_data.f[0]);
            }

            break;

        case GxRs_PointSprite:
            if (!glARBPointSprite) {
                break;
            }

            if (value.m_data.i[0]) {
                glEnable(GL_POINT_SPRITE_ARB);
            } else {
                glDisable(GL_POINT_SPRITE_ARB);
            }

            if (value.m_data.i[0]) {
                glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
            } else {
                glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
            }

            break;

        case GxRs_LineWidth:
            glLineWidth(value.m_data.f[0]);
            break;

        case GxRs_BlendFactor:
            if (glExtBlendColor) {
                glBlendColorEXT(value.m_data.f[0], value.m_data.f[0], value.m_data.f[0], value.m_data.f[0]);
            }

            break;

        case GxRs_ColorMaterial:
            glColorMaterial(GL_FRONT_AND_BACK, s_glColorMaterial[value.m_data.i[0]]);
            this->m_materialDirty = 7;
            break;

        default:
            break;
    }
}

// OFFSET: 0x00860E90/0x140593D40
void CGxDeviceOpenGl::IStateSync() {
    this->m_viewDirty |= this->m_xforms[GxXform_World].m_dirty | this->m_xforms[GxXform_View].m_dirty;
    this->IStateSyncVertexFormat();
    this->IShaderConstantsFlush();
    this->IRsSync(0);
    this->IStateSyncLights();
    this->IStateSyncEnables();
    this->IStateSyncMaterial();
    this->IStateSyncXforms();
    this->m_viewDirty = 0;
    this->IStateSyncClipPlanes();
    this->IStateSyncScissorRect();
    this->IStateSyncViewport();
    this->IStateSyncVertexPtrs();
    this->IStateSyncIndexPtr();
}
