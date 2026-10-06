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

// STATIC: %0x00AFD398
static const GLint s_glMinFilter[GxTexFilters_Last] = { GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR };

// STATIC: %0x00AFD3B0
static const GLint s_glMagFilter[GxTexFilters_Last] = { GL_NEAREST, GL_LINEAR, GL_LINEAR, GL_LINEAR, GL_LINEAR, GL_LINEAR };

// STATIC: %0x00AFD428
static const uint32_t s_glBytesPerPixel[GxTexFormats_Last] = { 0, 4, 4, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: %0x00AFD548
static const GLenum s_glCubeFaces[6] = { 0x8515, 0x8516, 0x8517, 0x8518, 0x8519, 0x851A };

// STATIC: 0x00D096A0/0x140BBB510
static GLenum s_glPrim[GxPrims_Last] = { GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP };

// PROVEN: d2465d58 IDENTICAL/NONE
// OFFSET: 0x008691D0
void CGxDeviceOpenGl::IShaderConstantsClear() {
    this->ShaderConstantsClear();
    this->IShaderConstantsReset();
}

// PROVEN: 18fe02d3 IDENTICAL/IDENTICAL
// OFFSET: 0x008691E0/0x14059CCC0
void CGxDeviceOpenGl::SceneClear(uint32_t mask, CImVector color) {
    glClearColor(color.r * (1.0f / 255.0f), color.g * (1.0f / 255.0f), color.b * (1.0f / 255.0f), color.a * (1.0f / 255.0f));

    uint32_t depthMask = this->m_deviceStates[Ds_DepthMask];
    this->DsSet(Ds_DepthMask, 1, 0);
    uint32_t colorMask = this->m_deviceStates[Ds_ColorMask];
    this->DsSet(Ds_ColorMask, 0xF, 0);

    GLbitfield bits = 0;

    if (mask & 0xF) {
        bits = GL_COLOR_BUFFER_BIT;
    }

    if (mask & 0x10) {
        bits |= GL_DEPTH_BUFFER_BIT;
    }

    this->IStateSyncViewport();
    glClear(bits);
    this->DsSet(Ds_DepthMask, depthMask, 0);
    this->DsSet(Ds_ColorMask, colorMask, 0);
}

// OFFSET: %0x007C5BA0
void CGxDeviceOpenGl::ScenePresent(void* window) {
    int32_t screenShot = this->m_scrShotClick;
    CGxDevice::ScenePresent(window);

    if (screenShot) {
        this->DeviceScreenShot();
    }

    this->ICursorDraw();

    if (this->m_hwMasterEnables & 0x40) {
        wglSwapLayerBuffers(this->m_hdc, WGL_SWAP_MAIN_PLANE);
    } else {
        glFinish();
    }

    this->WaitForFPSCap();
    this->IShaderConstantsClear();
}

// OFFSET: %0x007C5C10
void CGxDeviceOpenGl::IXformSetProjection(const C44Matrix& matrix) {
    C44Matrix projection(matrix.a0, matrix.a1, matrix.a2, matrix.a3, matrix.b0, matrix.b1, matrix.b2, matrix.b3, -matrix.c0, -matrix.c1, -matrix.c2, -matrix.c3, matrix.d0, matrix.d1, matrix.d2, matrix.d3);

    if (static_cast<int32_t>(this->m_appMasterEnables) >= 0 && 1.0f != matrix.d3) {
        C44Matrix scale(0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        projection = projection * scale;
    }

    if (this->m_textureTarget[GxBuffers_Color].m_apiSpecific || this->m_textureTarget[GxBuffers_Depth].m_apiSpecific) {
        if (!g_theGxDevicePtr->Caps().unk_01D0) {
            projection.a1 = projection.a1 * -1.0f;
            projection.b1 = projection.b1 * -1.0f;
            projection.c1 = projection.c1 * -1.0f;
            projection.d1 = -1.0f * projection.d1;
            this->DsSet(Ds_CullFace, GL_FRONT, 0);
        }
    } else if (!g_theGxDevicePtr->Caps().unk_01D0) {
        this->DsSet(Ds_CullFace, GL_BACK, 0);
    }

    this->DsSet(Ds_MatrixMode, GL_PROJECTION, 0);
    glLoadMatrixf(&projection.a0);
    this->m_projNative = projection;
}

// OFFSET: %0x007C5E30
void CGxDeviceOpenGl::IXformGLModelView(const C44Matrix& view, C44Matrix& world) {
    world = C44Matrix(view.a0, view.a1, -view.a2, view.a3, view.b0, view.b1, -view.b2, view.b3, view.c0, view.c1, -view.c2, view.c3, view.d0, view.d1, -view.d2, view.d3);
}

// OFFSET: %0x007C5F00
void CGxDeviceOpenGl::IXformSetModelView(const C44Matrix& matrix) {
    C44Matrix modelView;
    this->IXformGLModelView(matrix, modelView);
    this->DsSet(Ds_MatrixMode, GL_MODELVIEW, 0);
    glLoadMatrixf(&modelView.a0);
}

// PROVEN: 14593c6e IDENTICAL/NONE
// OFFSET: 0x00869790
void CGxDeviceOpenGl::XformSetProjection(const C44Matrix& matrix) {
    this->m_projection = matrix;
    this->IXformSetProjection(matrix);
}

// PROVEN: 2c044fa7 IDENTICAL/NONE
// OFFSET: 0x008697C0
void CGxDeviceOpenGl::XformSetView(const C44Matrix& matrix) {
    CGxDevice::XformSetView(matrix);
    this->m_viewDirty = 1;
}

// PROVEN: e2773712 IDENTICAL/IDENTICAL
// OFFSET: 0x008697E0/0x14059D4A0
void CGxDeviceOpenGl::DrawNonIndexed(const CGxBatch& batch) {
    if (this->m_intF5C) {
        return;
    }

    this->IStateSync();
    glDrawArrays(s_glPrim[batch.m_primType], batch.m_start, batch.m_count);
}

// PROVEN: 6be4ef3f IDENTICAL/IDENTICAL
// OFFSET: 0x00869810/0x14059D4E0
void CGxDeviceOpenGl::Draw(const CGxBatch& batch) {
    if (this->m_intF5C) {
        return;
    }

    this->IStateSync();

    if (glExtDrawRangeElements) {
        glDrawRangeElementsEXT(s_glPrim[batch.m_primType], batch.m_minIndex, batch.m_maxIndex, batch.m_count, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid*>(this->m_indexPtr + sizeof(uint16_t) * batch.m_start));
    } else {
        glDrawElements(s_glPrim[batch.m_primType], batch.m_count, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid*>(this->m_indexPtr + sizeof(uint16_t) * batch.m_start));
    }
}

// OFFSET: %0x007C4FC0
void CGxDeviceOpenGl::BindTexture(CGxTex* texId, uint32_t tmu) {
    if (tmu == 0xFFFFFFFF) {
        this->DsSet(Ds_ActiveTexture, 0, 0);
        this->IRsForceUpdate(GxRs_Texture0);
    }

    GLuint name = static_cast<GLuint>(reinterpret_cast<uintptr_t>(texId->m_apiSpecificData));
    glBindTexture(s_glTexTarget[texId->m_target], name);
    this->m_boundTexture = name;
}

// PROVEN: 21fd04a5 IDENTICAL/UNPROVEN
// OFFSET: 0x00868730/0x14059BDE0
void CGxDeviceOpenGl::ITexSetFlags(CGxTex* texId) {
    GLenum target = s_glTexTarget[texId->m_target];
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, s_glMagFilter[texId->m_flags.m_filter]);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, s_glMinFilter[texId->m_flags.m_filter]);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, s_glWrapMode[texId->m_flags.m_wrapU]);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, s_glWrapMode[texId->m_flags.m_wrapV]);

    if (glExtTextureFilterAnisotropic) {
        glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, texId->m_flags.m_maxAnisotropy);
    }

    texId->m_needsFlagUpdate = 0;
}

// PROVEN: 4a90de58 IDENTICAL/UNPROVEN
// OFFSET: 0x008687D0/0x14059BEC0
void CGxDeviceOpenGl::TexDestroy(CGxTex* texId) {
    if (texId->m_apiSpecificData) {
        glDeleteTextures(1, reinterpret_cast<GLuint*>(&texId->m_apiSpecificData));
        this->IResourceTouch(texId);
    }

    CGxDevice::TexDestroy(texId);
}

// OFFSET: %0x007C50F0
int32_t CGxDeviceOpenGl::TexCopy(CGxTex* source, CGxTex* dest, const C2iVector& destPos, const C2iVector& sourcePos, const C2iVector& size, uint32_t level, uint32_t plane) {
    if (!this->m_context) {
        return 0;
    }

    GLenum target = s_glTexTarget[dest->m_target];

    if (dest->m_target == GxTex_CubeMap) {
        target = s_glCubeFaces[plane];
    }

    if (!dest->m_apiSpecificData) {
        glGenTextures(1, reinterpret_cast<GLuint*>(&dest->m_apiSpecificData));
    }

    this->BindTexture(dest, 0xFFFFFFFF);

    if (dest->m_needsFlagUpdate) {
        this->ITexSetFlags(dest);
    }

    if (dest->m_needsCreation) {
        this->IResourceTouch(dest);
        glCopyTexImage2D(target, 0, s_glInternalFormat[dest->m_format], 0, 0, dest->m_width, dest->m_height, 0);
        dest->m_needsCreation = 0;
    }

    glCopyTexSubImage2D(target, level, destPos.x, destPos.y, sourcePos.x, sourcePos.y, size.x, size.y);

    return 1;
}

// PROVEN: 0fe72bb7 IDENTICAL/NONE
// OFFSET: 0x008688E0
int32_t CGxDeviceOpenGl::TexStretch(CGxTex* source, CGxTex* dest, const CiRect* destRect, const CiRect* sourceRect, uint32_t level, uint32_t face) {
    return 0;
}

// OFFSET: %0x007C51D0
void CGxDeviceOpenGl::ITexForceRecreation() {
    for (CGxTex* texId = CGxDevice::s_textureList.Head(); texId; texId = CGxDevice::s_textureList.Next(texId)) {
        if (!texId->m_apiSpecificData) {
            continue;
        }

        // STATIC: %0x00D9310C
        static CiRect s_updateRect;

        if (s_glFormat[texId->m_format] == GL_DEPTH_COMPONENT) {
            glTexParameteri(s_glTexTarget[texId->m_target], GL_TEXTURE_COMPARE_MODE_ARB, 0);
        }

        glDeleteTextures(1, reinterpret_cast<GLuint*>(&texId->m_apiSpecificData));
        this->IResourceTouch(texId);
        texId->m_apiSpecificData = NULL;
        texId->m_needsCreation = 1;
        texId->m_needsFlagUpdate = 1;
        this->TexMarkForUpdate(texId, s_updateRect, 0);
    }

    this->NotifyOnTextureRecreation();
}

// OFFSET: %0x007C52B0
void CGxDeviceOpenGl::ITexDownload(CGxTex* texId, uint32_t width, uint32_t height, uint32_t face, uint32_t mip, uint32_t baseMip, uint32_t stride, const void* texels) {
    // STATIC: %0x00D93120
    static TSGrowableArray<uint8_t> s_buffer;

    uint32_t minY = texId->m_updateRect.minY >> mip;
    uint32_t minX = texId->m_updateRect.minX >> mip;
    uint32_t maxY = minY + 1 > static_cast<uint32_t>(texId->m_updateRect.maxY >> mip) ? minY + 1 : texId->m_updateRect.maxY >> mip;
    uint32_t maxX = minX + 1 > static_cast<uint32_t>(texId->m_updateRect.maxX >> mip) ? minX + 1 : texId->m_updateRect.maxX >> mip;

    GLint internalFormat = s_glInternalFormat[texId->m_format];
    uint32_t updateWidth = maxX - minX;
    uint32_t updateHeight = maxY - minY;
    GLint level = mip - baseMip;

    GLenum target;

    if (texId->m_target == GxTex_CubeMap) {
        target = s_glCubeFaces[face];
    } else {
        target = s_glTexTarget[texId->m_target];
    }

    const uint8_t* bits = static_cast<const uint8_t*>(texels);

    switch (texId->m_format) {
        case GxTex_Argb8888:
        case GxTex_Argb4444:
        case GxTex_Argb1555:
        case GxTex_Rgb565:
        case GxTex_Uv88: {
            EGxTexFormat dataFormat = texId->m_dataFormat;

            if (dataFormat == GxTex_Dxt1 || dataFormat == GxTex_Dxt3 || dataFormat == GxTex_Dxt5) {
                dataFormat = GxTex_Rgb565;

                uint32_t bufferSize = (updateHeight * 16 * updateWidth) >> 3;

                if (s_buffer.Count() < bufferSize) {
                    s_buffer.SetCount(bufferSize);
                }

                const uint8_t* source = bits + ((minX * CGxDevice::s_texFormatBitDepth[texId->m_dataFormat]) >> 3) + stride * minY;
                BlitFormat destFormat = GxGetBlitFormat(GxTex_Rgb565);
                uint8_t* dest = s_buffer.Ptr();

                C2iVector size;
                size.x = updateWidth;
                size.y = updateHeight;

                uint32_t destPitch = CalcRowStride(destFormat, updateWidth, updateHeight);
                Blit(size, BlitAlpha_0, source, stride, GxGetBlitFormat(texId->m_dataFormat), dest, destPitch, destFormat);
                bits = s_buffer.Ptr();
            }

            GLenum format = s_glFormat[dataFormat];
            GLenum type = s_glType[dataFormat];

            if (texId->m_flags.m_bit15) {
                glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            } else {
                glPixelStorei(GL_UNPACK_ROW_LENGTH, stride / s_glBytesPerPixel[dataFormat]);
            }

            if (texId->m_needsCreation) {
                if (!texId->m_flags.m_bit15 && updateWidth == width && updateHeight == height) {
                    glTexImage2D(target, level, internalFormat, width, height, 0, format, type, bits);
                } else {
                    glTexImage2D(target, level, internalFormat, width, height, 0, format, type, NULL);
                    glTexSubImage2D(target, level, minX, minY, updateWidth, updateHeight, format, type, bits);
                }
            } else if (texId->m_flags.m_bit14 || (!minX && !minY && maxY == height && maxX == width)) {
                glTexSubImage2D(target, level, 0, 0, width, height, format, type, bits);
            } else {
                glTexSubImage2D(target, level, minX, minY, updateWidth, updateHeight, format, type, bits + minX * s_glBytesPerPixel[dataFormat] + stride * minY);
            }

            break;
        }

        case GxTex_Dxt1:
        case GxTex_Dxt3:
        case GxTex_Dxt5: {
            uint32_t blockX = minX & ~3;
            uint32_t blockY = minY & ~3;
            uint32_t blockWidth = ((maxX + 3) & ~3) - blockX;

            if (blockWidth <= 4) {
                blockWidth = 4;
            }

            uint32_t blockHeight = ((maxY + 3) & ~3) - blockY;

            if (blockHeight <= 4) {
                blockHeight = 4;
            }

            uint32_t bitDepth = CGxDevice::s_texFormatBitDepth[texId->m_format];
            uint32_t imageSize = (blockWidth * blockHeight * bitDepth) >> 3;
            uint32_t offset = 0;

            if (!texId->m_flags.m_bit15) {
                offset = stride * (blockY >> 2) + CGxDevice::s_texFormatBytesPerBlock[texId->m_dataFormat] * (blockX >> 2);
            }

            const uint8_t* source = bits + offset;

            if (blockWidth == 4 * (stride / CGxDevice::s_texFormatBytesPerBlock[texId->m_dataFormat]) && !texId->m_needsCreation) {
                glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

                if (width < 4 || height < 4 || (blockWidth == width && blockHeight == height)) {
                    glCompressedTexImage2DARB(target, level, internalFormat, width, height, 0, imageSize, source);
                } else {
                    glCompressedTexSubImage2DARB(target, level, blockX, blockY, blockWidth, blockHeight, internalFormat, imageSize, source);
                }
            } else {
                uint32_t fullSize;
                uint32_t bufferSize;

                if (texId->m_needsCreation) {
                    fullSize = (height * width * bitDepth) >> 3;
                    bufferSize = fullSize;
                } else {
                    fullSize = 0;
                    bufferSize = 0;
                }

                if (imageSize > bufferSize) {
                    bufferSize = (blockWidth * blockHeight * bitDepth) >> 3;
                }

                if (s_buffer.Count() < bufferSize) {
                    s_buffer.SetCount(bufferSize);
                }

                uint32_t rowSize = CGxDevice::s_texFormatBytesPerBlock[texId->m_dataFormat] * (blockWidth >> 2);
                uint8_t* buffer = s_buffer.Ptr();
                uint8_t* dest = buffer;

                for (uint32_t rows = blockHeight >> 2; rows; rows--) {
                    memcpy(dest, source, rowSize);
                    source += stride;
                    dest += rowSize;
                }

                glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

                if (width < 4 || height < 4 || (blockWidth == width && blockHeight == height)) {
                    glCompressedTexImage2DARB(target, level, internalFormat, width, height, 0, imageSize, buffer);
                } else {
                    if (texId->m_needsCreation) {
                        glCompressedTexImage2DARB(target, level, internalFormat, width, height, 0, fullSize, buffer);
                    }

                    glCompressedTexSubImage2DARB(target, level, blockX, blockY, blockWidth, blockHeight, internalFormat, imageSize, buffer);
                }
            }

            break;
        }

        default:
            break;
    }
}

// OFFSET: %0x007C5800
void CGxDeviceOpenGl::ITexMarkAsUpdated(CGxTex* texId, uint32_t tmu) {
    if (!this->m_context) {
        return;
    }

    if (texId->m_needsUpdate && texId->m_userFunc) {
        uint32_t width = texId->m_width;
        uint32_t height = texId->m_height;

        if (!texId->m_apiSpecificData) {
            GLenum target = s_glTexTarget[texId->m_target];
            glGenTextures(1, reinterpret_cast<GLuint*>(&texId->m_apiSpecificData));
            this->BindTexture(texId, tmu);
            this->ITexSetFlags(texId);

            if (glUnkC60498 && (texId->m_format == GxTex_Dxt1 || texId->m_format == GxTex_Dxt3 || texId->m_format == GxTex_Dxt5)) {
                uint32_t size = width >= height ? height : width;
                uint32_t levels = 0;

                while (size > 4) {
                    size >>= 1;
                    levels++;
                }

                glTexParameteri(target, GL_TEXTURE_MAX_LOD, levels);
            }
        } else {
            this->BindTexture(texId, tmu);
        }

        uint32_t baseMip;
        uint32_t mipCount;

        if (texId->m_flags.m_filter >= GxTex_NearestMipNearest) {
            uint32_t size = texId->m_height;

            if (texId->m_width > size) {
                size = texId->m_width;
            }

            mipCount = 1;

            while (size != 1) {
                size >>= 1;
                mipCount++;
            }

            baseMip = mipCount - 1;

            if (this->m_baseMipLevel < baseMip) {
                baseMip = this->m_baseMipLevel;
            }

            width >>= baseMip;
            height >>= baseMip;
        } else {
            baseMip = 0;
            mipCount = 1;
        }

        glTexParameteri(s_glTexTarget[texId->m_target], GL_TEXTURE_MAX_LEVEL, mipCount - baseMip - 1);

        uint32_t texelStrideInBytes;
        const void* texels;
        texId->m_userFunc(GxTex_Lock, texId->m_width, texId->m_height, 0, 0, texId->m_userArg, texelStrideInBytes, texels);

        if (this->m_boundTexture != static_cast<GLuint>(reinterpret_cast<uintptr_t>(texId->m_apiSpecificData))) {
            this->BindTexture(texId, tmu);
        }

        uint32_t faceCount = texId->m_target == GxTex_CubeMap ? 6 : 1;

        for (uint32_t face = 0; face < faceCount; face++) {
            uint32_t mipWidth = width;
            uint32_t mipHeight = height;
            uint32_t levelCount = 0;

            for (uint32_t mip = baseMip; mip != mipCount; mip++, levelCount++) {
                texId->m_userFunc(GxTex_Latch, mipWidth, mipHeight, face, mip, texId->m_userArg, texelStrideInBytes, texels);

                if (!texId->m_flags.m_renderTarget) {
                    this->ITexDownload(texId, mipWidth, mipHeight, face, mip, baseMip, texelStrideInBytes, texels);
                }

                mipWidth >>= 1;
                mipHeight >>= 1;

                if (!mipWidth) {
                    mipWidth = 1;
                }

                if (!mipHeight) {
                    mipHeight = 1;
                }
            }
        }

        texId->m_userFunc(GxTex_Unlock, texId->m_width, texId->m_height, 0, 0, texId->m_userArg, texelStrideInBytes, texels);

        if (!texId->m_flags.m_renderTarget) {
            if (texId->m_needsCreation) {
                this->IResourceTouch(texId);
            }

            texId->m_needsCreation = 0;
        }
    }

    if (texId->m_needsFlagUpdate) {
        this->ITexSetFlags(texId);
    }

    CGxDevice::ITexMarkAsUpdated(texId);
}

// OFFSET: %0x007C5A90
void CGxDeviceOpenGl::ITexMarkAsUpdated(CGxTex* texId) {
    this->ITexMarkAsUpdated(texId, 0xFFFFFFFF);
}
