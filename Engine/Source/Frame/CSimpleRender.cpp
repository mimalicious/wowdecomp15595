#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleRender.h"
#include <malloc.h>
#include <cctype>
#include <cstring>
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Frame/CSimpleEmbeddedTexture.h"
#include "Engine/Source/Gxu/GxuFontString.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxVertexPCT.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include <cmath>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/Frame/CSimpleFontStringAttributes.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameScript/Types.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Frame/CFramePoint.h"
#include "Engine/Source/Frame/CSimpleRegion.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CRect.h"

// STATIC: %0x00AD6620
static const uint8_t s_letterHasFinalConsonant[26] = { 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: %0x00AD663C
static const uint8_t s_digitHasFinalConsonant[10] = { 1, 1, 0, 1, 0, 0, 1, 1, 1, 0 };

// STATIC: %0x00CC91F0
static uint16_t s_languageWideBuffer[0x2000];

// STATIC: %0x00CCD1F0
static char s_languageBuffer[0x2000];

// STATIC: %0x00BB8100
static TSExplicitList<CSimpleEmbeddedTexture, offsetof(CSimpleEmbeddedTexture, m_link)> s_embeddedTextureStorage;

// STATIC: %0x00CCF208
CGxShader* CSimpleRender::s_vertexShader;

// STATIC: %0x00CCF20C
int32_t CSimpleRender::s_mergeEnable;

// STATIC: %0x00CCF278
static char s_displayText[0x2000];

// STATIC: %0x00BB80E0
const char* CSimpleTexture::s_objectTypeName = "Texture";

// STATIC: %0x00BB80E4
const char* CSimpleTexture::s_imageModePixelShaderNames[2] = { "UI", "Desaturate" };

// STATIC: %0x00BB80EC
EGxTexFilter CSimpleTexture::s_textureFilterMode = GxTex_Linear;

// STATIC: %0x00CCF1F8
int32_t CSimpleTexture::s_count;

// STATIC: %0x00CCF1FC
CGxShader* CSimpleTexture::s_imageModePixelShaders[2];

// STATIC: %0x00CCF204
int32_t CSimpleTexture::s_objectType;

// STATIC: %0x00CCF224
CDataAllocator CSimpleTexture::s_allocator(sizeof(CSimpleTexture), 0x381);

// STATIC: %0x00CCF210
CDataAllocator CTextureExtension::s_allocator(sizeof(CTextureExtension), 0x14);

// STATIC: %0x00CCF24C
TSHashTable<FILENAMEHASH, HASHKEY_STR> CSimpleTexture::s_fileNameHash;

// STATIC: %0x00CD1278
int32_t CSimpleTexture::s_maskCount;

// STATIC: %0x00CD127D
bool CSimpleTexture::s_defaultNonBlocking;

// STATIC: %0x00CD127E
bool CSimpleTexture::s_unloadHiddenTextures;

// STATIC: %0x00BB8D8C
int32_t CSimpleFontString::s_metatable = LUA_NOREF;

// STATIC: %0x00CCF1F4
int32_t CSimpleFontString::s_objectType;

// STATIC: %0x00BB80FC
const char* CSimpleFontString::s_objectTypeName = "FontString";

// STATIC: %0x00CCF238
CDataAllocator CSimpleFontString::s_allocator(sizeof(CSimpleFontString), 0x35E);

// STATIC: %0x00CCF1F0
static int32_t s_fontStringCount;

// INLINED
static int32_t IsRectOffscreen(const CRect& rect) {
    return rect.maxY < 0.0f || rect.minY > 1.0f || rect.maxX < 0.0f || rect.minX > 1.0f;
}

// OFFSET: %0x00507A00
static const uint16_t* LanguageRule1ChooseParticle(uint32_t lastChar, const uint16_t* consonantForm, const uint16_t* vowelForm) {
    if (lastChar & 0xFF00) {
        if (static_cast<uint16_t>(lastChar + 0x5400) > 0x2BA3) {
            return consonantForm;
        }

        int32_t syllable = static_cast<uint16_t>(lastChar);

        if ((syllable - 0xAC08) % 28 == 0 && *consonantForm == 0xC73C) {
            return vowelForm;
        }

        return (syllable - 0xAC00) % 28 ? consonantForm : vowelForm;
    }

    if (static_cast<uint16_t>(lastChar - '0') <= 9) {
        return s_digitHasFinalConsonant[static_cast<uint16_t>(lastChar) - '0'] ? consonantForm : vowelForm;
    }

    if ((lastChar >= 'a' && lastChar <= 'z') || static_cast<uint16_t>(lastChar - 'A') <= 25) {
        return s_letterHasFinalConsonant[tolower(static_cast<uint16_t>(lastChar)) - 'a'] ? consonantForm : vowelForm;
    }

    return consonantForm;
}

// OFFSET: %0x00507AB0
static char* LanguageRule1(const char* text) {
    uint16_t buffer[0x2000];
    uint32_t lastChar = 0;

    SUniConvertUTF8to16(buffer, sizeof(buffer) / sizeof(buffer[0]), text, STORM_MAX_STR, NULL, NULL);

    uint16_t* dest = s_languageWideBuffer;
    const uint16_t* src = buffer;

    if (buffer[0]) {
        do {
            uint16_t ch = *src;

            if (ch == '|') {
                src++;
                ch = *src;

                if (ch == '1' && (src[1] > '9' || src[1] < '0')) {
                    const uint16_t* consonantForm = src + 1;
                    const uint16_t* vowelForm = consonantForm;

                    if (*vowelForm) {
                        uint16_t c;

                        do {
                            c = *vowelForm;
                            vowelForm++;
                        } while (c != ';' && *vowelForm);
                    }

                    const uint16_t* end = vowelForm;

                    if (*end) {
                        uint16_t c;

                        do {
                            c = *end;
                            end++;
                        } while (c != ';' && *end);
                    }

                    src = end - 1;

                    const uint16_t* particle = LanguageRule1ChooseParticle(lastChar, consonantForm, vowelForm);

                    for (uint16_t c = *particle; c && c != ';'; c = *particle) {
                        particle++;
                        *dest = c;
                        dest++;
                    }
                } else {
                    *dest = '|';
                    dest++;
                    *dest = ch;
                    dest++;

                    if (ch == 'c' || ch == 'C') {
                        for (uint32_t i = 0; i < 8 && src[1]; i++) {
                            src++;
                            *dest = *src;
                            dest++;
                        }
                    }
                }
            } else {
                if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || static_cast<uint16_t>(ch + 0x5400) <= 0x2BA3) {
                    lastChar = ch;
                }

                *dest = ch;
                dest++;
            }

            src++;
        } while (*src);
    }

    *dest = 0;
    SUniConvertUTF16to8(s_languageBuffer, sizeof(s_languageBuffer), s_languageWideBuffer, STORM_MAX_STR, NULL, NULL);

    return s_languageBuffer;
}

// OFFSET: %0x00507C70
static bool LanguageRule2IsVowel(uint16_t ch) {
    switch (ch) {
        case 'A':
        case 'E':
        case 'I':
        case 'O':
        case 'U':
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':
            return true;

        default:
            return (ch >= 0xC0 && ch <= 0xC6) || (ch >= 0xC8 && ch <= 0xCF) || (ch >= 0xD2 && ch <= 0xD6) || (ch >= 0xD8 && ch <= 0xDC) || (ch >= 0xE0 && ch <= 0xE6) || (ch >= 0xE8 && ch <= 0xEF) || (ch >= 0xF2 && ch <= 0xF6) || static_cast<uint16_t>(ch - 0xF8) <= 4;
    }
}

// OFFSET: %0x00507D70
static char* LanguageRule2(const char* text) {
    uint16_t buffer[0x2000];

    SUniConvertUTF8to16(buffer, sizeof(buffer) / sizeof(buffer[0]), text, STORM_MAX_STR, NULL, NULL);

    uint16_t* dest = s_languageWideBuffer;
    const uint16_t* src = buffer;

    if (buffer[0]) {
        do {
            if (*src == '|') {
                src++;

                if (*src == '2' && (src[1] > '9' || src[1] < '0')) {
                    src++;

                    while (*src == ' ') {
                        src++;
                    }

                    uint16_t ch = *src;

                    if (ch == 'L' && src[1] == '\'') {
                        *dest++ = 'd';
                        *dest++ = 'e';
                        *dest++ = ' ';
                        *dest++ = 'l';
                        *dest = '\'';
                        src += 2;
                    } else if (ch == 'L' && src[1] == 'e' && src[2] == ' ') {
                        *dest++ = 'd';
                        *dest++ = 'u';
                        *dest = ' ';
                        src += 3;
                    } else if (ch == 'L' && src[1] == 'a' && src[2] == ' ') {
                        *dest++ = 'd';
                        *dest++ = 'e';
                        *dest++ = ' ';
                        *dest++ = 'l';
                        *dest++ = 'a';
                        *dest = ' ';
                        src += 3;
                    } else if (ch == 'L' && src[1] == 'e' && src[2] == 's' && src[3] == ' ') {
                        *dest++ = 'd';
                        *dest++ = 'e';
                        *dest++ = 's';
                        *dest = ' ';
                        src += 4;
                    } else if (LanguageRule2IsVowel(ch) || ch == 'h' || ch == 'H' || (ch == 'y' && LanguageRule2IsVowel(src[1])) || (ch == 'Y' && LanguageRule2IsVowel(src[1]))) {
                        *dest++ = 'd';
                        *dest = '\'';
                    } else {
                        *dest++ = 'd';
                        *dest++ = 'e';
                        *dest = ' ';
                    }
                } else {
                    *dest = '|';
                }

                dest++;
            }

            *dest = *src;
            src++;
            dest++;
        } while (*src);
    }

    *dest = 0;
    SUniConvertUTF16to8(s_languageBuffer, sizeof(s_languageBuffer), s_languageWideBuffer, STORM_MAX_STR, NULL, NULL);

    return s_languageBuffer;
}

// OFFSET: %0x00509890
static char* LanguageRule3(const char* text) {
    char copy[0x2000];
    char word[0x400];

    if (text == s_languageBuffer) {
        SStrCopy(copy, text, sizeof(copy));
        text = copy;
    }

    const char* src = text;
    char* dest = s_languageBuffer;

    while (*src) {
        if (*src == '|') {
            src++;

            if (*src == '3' && (src[1] > '9' || src[1] < '0')) {
                if (src[1] == '-' && src[2] >= '0' && src[2] <= '9') {
                    src += 2;
                }

                uint32_t declension = SStrToUnsigned(&src);

                if (*src == '(') {
                    src++;
                }

                char* wordEnd = word;

                while (*src && *src != ')') {
                    if (wordEnd < &word[sizeof(word) - 1]) {
                        *wordEnd++ = *src;
                    }

                    src++;
                }

                if (!*src) {
                    src--;
                }

                *wordEnd = 0;
                dest += GetDeclinedWord(word, declension, dest, &s_languageBuffer[sizeof(s_languageBuffer)] - dest);
            } else {
                *dest = '|';
                dest++;
                *dest = *src;
                dest++;
            }
        } else {
            *dest = *src;
            dest++;
        }

        src++;
    }

    *dest = 0;

    return s_languageBuffer;
}

// OFFSET: %0x00507FC0
static char* LanguageRule4(const char* text) {
    bool wasDigit = false;
    int32_t number = 0;
    const char* src = text;
    char* dest = s_languageBuffer;

    while (*src) {
        char ch = *src;
        bool isDigit;

        if (ch >= '0' && ch <= '9') {
            number = wasDigit ? number * 10 + ch - '0' : ch - '0';
            isDigit = true;
        } else {
            isDigit = false;
        }

        if (ch == '|') {
            src++;

            if (*src == '4' && (src[1] > '9' || src[1] < '0')) {
                src++;

                uint32_t index = FrameScript_GetPluralIndex(number);

                for (uint32_t i = 0; i < index && *src; i++) {
                    while (*src != ':' && *src != ';') {
                        src++;

                        if (!*src) {
                            break;
                        }
                    }

                    if (!*src || *src == ';') {
                        break;
                    }

                    src++;
                }

                while (*src == ' ') {
                    src++;
                }

                if (!*src) {
                    break;
                }

                while (*src != ':' && *src != ';') {
                    *dest = *src;
                    src++;
                    dest++;

                    if (!*src) {
                        *dest = 0;
                        return s_languageBuffer;
                    }
                }

                while (*src != ';') {
                    src++;

                    if (!*src) {
                        *dest = 0;
                        return s_languageBuffer;
                    }
                }
            } else {
                *dest = '|';
                dest++;
                *dest = *src;
                dest++;
            }
        } else {
            *dest = ch;
            dest++;
        }

        src++;
        wasDigit = isDigit;
    }

    *dest = 0;

    return s_languageBuffer;
}

// OFFSET: %0x005099D0
const char* LanguageProcess(const char* text) {
    if (SStrLen(text) > sizeof(s_languageBuffer)) {
        memcpy(s_languageBuffer, text, sizeof(s_languageBuffer));
        s_languageBuffer[sizeof(s_languageBuffer) - 3] = 0;
        s_languageBuffer[sizeof(s_languageBuffer) - 2] = 0;
        s_languageBuffer[sizeof(s_languageBuffer) - 1] = 0;
        text = s_languageBuffer;
    }

    for (;;) {
        const uint8_t* src = reinterpret_cast<const uint8_t*>(text);
        int32_t chars;
        uint32_t ch;
        int32_t rule = 0;

        while ((ch = sgetu8(src, &chars)) != 0xFFFFFFFF) {
            src += chars;

            if (ch != '|') {
                continue;
            }

            if (*src == '|') {
                src++;
                continue;
            }

            if (*src >= '0' && *src <= '9') {
                rule = SStrToInt(reinterpret_cast<const char*>(src));

                if (rule >= 1 && rule <= 4) {
                    break;
                }
            }
        }

        if (ch == 0xFFFFFFFF) {
            return text;
        }

        switch (rule) {
            case 1:
                text = LanguageRule1(text);
                break;

            case 2:
                text = LanguageRule2(text);
                break;

            case 3:
                text = LanguageRule3(text);
                break;

            case 4:
                text = LanguageRule4(text);
                break;
        }
    }
}

// PROVEN: 5733db64 IDENTICAL/NONE
// OFFSET: 0x004FBE30/0x140140060
bool CSimpleFontString::IsA(int32_t type) {
    return type == CSimpleFontString::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: 8be38cd3 IDENTICAL/NONE
// OFFSET: 0x004FBE60
bool CSimpleFontString::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleFontString::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptRegion::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// PROVEN: 1ecc490c IDENTICAL/NONE
// OFFSET: 0x004FBEC0
const char* CSimpleFontString::GetObjectTypeName() {
    return CSimpleFontString::s_objectTypeName;
}

// PROVEN: d216810a IDENTICAL/NONE
// OFFSET: 0x004FBED0/0x1401400F0
int32_t CSimpleFontString::GetScriptMetaTable() {
    return CSimpleFontString::s_metatable;
}

// PROVEN: 7140aebe IDENTICAL/NONE
// OFFSET: 0x004F7FD0
const char* CSimpleFontString::GetFontName() {
    return this->m_fontHandle ? TextBlockGetFontName(this->m_fontHandle) : NULL;
}

// PROVEN: 526e6201 IDENTICAL/NONE
// OFFSET: 0x004F8060
uint32_t CSimpleFontString::GetFontFlags() {
    return this->m_fontHandle ? TextBlockGetFontFlags(this->m_fontHandle) : 0;
}

// OFFSET: %0x00508BE0
void CSimpleRender::Init() {
    CSimpleRender::s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "UI");
}

// OFFSET: %0x00508C00
void CSimpleRender::Destroy() {
    if (CSimpleRender::s_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(CSimpleRender::s_vertexShader);
        CSimpleRender::s_vertexShader = NULL;
    }
}

// OFFSET: %0x00508C20
int32_t CSimpleRender::CalcBatchCounts(CRenderBatch* batch, uint32_t* vertCounts, uint32_t* idxCounts) {
    CGxTex* textureID = reinterpret_cast<CGxTex*>(-1);
    int32_t alphaMode = GxBlends_Last;
    CGxShader* shader = NULL;
    int32_t batchIndex = -1;
    uint32_t vertTotal = 0;
    uint32_t idxTotal = 0;

    for (uint32_t i = 0; i < batch->m_texturelist.Count(); i++) {
        CSimpleBatchedMesh* mesh = &batch->m_texturelist[i];
        int32_t changed = 0;

        if (mesh->m_textureID != textureID) {
            textureID = mesh->m_textureID;
            changed = 1;
        }

        int32_t meshAlphaMode = (mesh->m_color && mesh->m_alphaMode < GxBlend_Alpha) ? GxBlend_Alpha : mesh->m_alphaMode;

        if (meshAlphaMode != alphaMode) {
            alphaMode = meshAlphaMode;
            changed = 1;
        }

        if (mesh->m_shader != shader) {
            shader = mesh->m_shader;
            changed = 1;
        }

        if (!CSimpleRender::s_mergeEnable || changed || idxTotal + mesh->m_idxCount >= 0x20000 || vertTotal + mesh->m_posCount >= 0x10000) {
            if (batchIndex > -1) {
                vertCounts[batchIndex] = vertTotal;
                idxCounts[batchIndex] = idxTotal;
            }

            batchIndex++;
            vertTotal = 0;
            idxTotal = 0;
        }

        vertTotal += mesh->m_posCount;
        idxTotal += mesh->m_idxCount;
    }

    if (idxTotal) {
        vertCounts[batchIndex] = vertTotal;
        idxCounts[batchIndex] = idxTotal;
        batchIndex++;
    }

    return batchIndex;
}

// OFFSET: %0x0050B520
void CSimpleRender::DrawBatch(CRenderBatch* batch) {
    uint32_t meshCount = batch->m_texturelist.Count();

    if (meshCount) {
        CGxTex* textureID = reinterpret_cast<CGxTex*>(-1);
        int32_t alphaMode = GxBlends_Last;
        CGxShader* shader = NULL;

        g_theGxDevicePtr->RsPush();

        uint32_t permute = g_theGxDevicePtr->StereoEnabled() != 0;

        if (CSimpleRender::s_vertexShader && CSimpleRender::s_vertexShader->m_permutes[permute].Valid()) {
            C44Matrix viewProj;
            GxRsSet(GxRs_VertexShader, CSimpleRender::s_vertexShader, permute);
            GxXformViewProjNativeTranspose(viewProj);
            g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&viewProj), 4);
        } else {
            GxRsSet(GxRs_Lighting, 0);
        }

        GxRsSet(GxRs_Fog, 0);
        GxRsSet(GxRs_DepthTest, 0);
        GxRsSet(GxRs_DepthWrite, 0);

        uint32_t* vertCounts = static_cast<uint32_t*>(alloca(sizeof(uint32_t) * meshCount));
        uint32_t* idxCounts = static_cast<uint32_t*>(alloca(sizeof(uint32_t) * meshCount));
        CSimpleRender::CalcBatchCounts(batch, vertCounts, idxCounts);

        CGxBuf* vertexBuf = NULL;
        CGxBuf* indexBuf = NULL;
        CGxVertexPCT* vertices = NULL;
        uint16_t* indices = NULL;
        uint32_t vertTotal = 0;
        int32_t idxTotal = 0;
        int32_t batchIndex = -1;

        for (uint32_t i = 0; i < meshCount; i++) {
            CSimpleBatchedMesh* mesh = &batch->m_texturelist[i];
            int32_t changed = 0;

            if (mesh->m_textureID != textureID) {
                textureID = mesh->m_textureID;
                changed = 1;
            }

            int32_t meshAlphaMode = (mesh->m_color && mesh->m_alphaMode < GxBlend_Alpha) ? GxBlend_Alpha : mesh->m_alphaMode;

            if (meshAlphaMode != alphaMode) {
                alphaMode = meshAlphaMode;
                changed = 1;
            }

            if (mesh->m_shader != shader) {
                shader = mesh->m_shader;
                changed = 1;
            }

            bool newBatch = !CSimpleRender::s_mergeEnable || changed || idxTotal + mesh->m_idxCount >= 0x20000 || vertTotal + mesh->m_posCount >= 0x10000;

            if (newBatch) {
                if (idxTotal > 0) {
                    GxBufUnlock(vertexBuf, 0);
                    GxBufUnlock(indexBuf, 0);
                    GxPrimVertexPtr(vertexBuf, GxVBF_PCT);
                    g_theGxDevicePtr->PrimIndexPtr(indexBuf);

                    CGxBatch gxBatch;
                    gxBatch.m_primType = GxPrim_Triangles;
                    gxBatch.m_start = 0;
                    gxBatch.m_count = idxCounts[batchIndex];
                    gxBatch.m_minIndex = 0;
                    gxBatch.m_maxIndex = vertCounts[batchIndex] - 1;
                    GxDraw(gxBatch);
                }

                batchIndex++;
                idxTotal = 0;
                vertTotal = 0;

                GxRsSet(GxRs_Texture0, TextureGetGxTex(mesh->m_texture));
                GxRsSet(GxRs_BlendingMode, alphaMode);
                GxUpdateAlphaRef(GxRs_AlphaRef);
                GxRsSet(GxRs_PixelShader, shader ? shader->m_permutes.Ptr() : NULL);
            }

            if (newBatch || !vertices) {
                vertexBuf = GxBufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPCT), vertCounts[batchIndex]);
                indexBuf = GxBufStream(GxPoolTarget_Index, sizeof(uint16_t), idxCounts[batchIndex]);
                vertices = reinterpret_cast<CGxVertexPCT*>(GxBufLock(vertexBuf));
                indices = reinterpret_cast<uint16_t*>(GxBufLock(indexBuf));
            }

            if (!mesh->m_indices) {
                continue;
            }

            for (uint32_t v = 0; v < mesh->m_posCount; v++) {
                vertices->p = mesh->m_position[v];

                if (mesh->m_color) {
                    CImVector color = mesh->m_color[(v * mesh->m_colorCount) >> 2];
                    GxFormatColor(color);
                    vertices->c = color;
                } else {
                    CImVector white;
                    white.b = 0xFF;
                    white.g = 0xFF;
                    white.r = 0xFF;
                    white.a = 0xFF;
                    vertices->c = white;
                }

                if (mesh->m_onAtlas) {
                    vertices->tc.x = mesh->m_atlasScale * mesh->m_texCoord[v].x + mesh->m_atlasOffset.x;
                    vertices->tc.y = mesh->m_atlasScale * mesh->m_texCoord[v].y + mesh->m_atlasOffset.y;
                } else {
                    vertices->tc = mesh->m_texCoord[v];
                }

                vertices++;
            }

            for (uint32_t j = 0; j < mesh->m_idxCount; j++) {
                *indices = vertTotal + mesh->m_indices[j];
                indices++;
            }

            vertTotal += mesh->m_posCount;
            idxTotal += mesh->m_idxCount;
        }

        if (idxTotal > 0) {
            GxBufUnlock(vertexBuf, sizeof(CGxVertexPCT) * vertTotal);
            GxBufUnlock(indexBuf, sizeof(uint16_t) * idxTotal);
            GxPrimVertexPtr(vertexBuf, GxVBF_PCT);
            g_theGxDevicePtr->PrimIndexPtr(indexBuf);

            CGxBatch gxBatch;
            gxBatch.m_primType = GxPrim_Triangles;
            gxBatch.m_start = 0;
            gxBatch.m_count = idxCounts[batchIndex];
            gxBatch.m_minIndex = 0;
            gxBatch.m_maxIndex = vertCounts[batchIndex] - 1;
            GxDraw(gxBatch);
        }

        g_theGxDevicePtr->RsPop();
    }

    if (batch->m_stringbatch) {
        GxuFontRenderBatch(batch->m_stringbatch);
    }

    for (RENDERCALLBACKNODE* node = batch->m_callbacks.Head(); !TSList<RENDERCALLBACKNODE, TSGetLink<RENDERCALLBACKNODE> >::IsEnd(node); node = node->RawNext()) {
        node->m_callback(node->m_param);
    }
}

// PROVEN: e0d2364d IDENTICAL/NONE
// OFFSET: 0x004F7E60/0x14013B6D0
void CSimpleFontString::RemoveShadow() {
    if (this->m_styleFlags & 0x100) {
        this->m_styleFlags &= ~0x100u;

        if (this->m_string) {
            TextBlockRemoveShadow(this->m_string);
        }
    }
}

// PROVEN: aa2f921a IDENTICAL/NONE
// OFFSET: 0x004F7FF0/0x14029E930
float CSimpleFontString::GetFontHeight(bool recalculate) {
    float fontHeight = this->m_fontHeight;

    if (recalculate && this->m_fontHandle && (this->m_styleFlags & 0x200)) {
        float ndcHeight = GxuFontGetOneToOneHeight(TextBlockGetFontPtr(this->m_fontHandle)) / this->m_layoutScale;
        NDCToDDC(0.0f, ndcHeight, NULL, &fontHeight);
    }

    return fontHeight;
}

// OFFSET: %0x00509C40
const char* CSimpleFontString::GetDisplayText(float width, float height) {
    const char* text = this->m_text && *this->m_text ? this->m_text : NULL;

    if (!this->m_fontHandle || !text || !*text || width <= 0.0f) {
        return text;
    }

    if (!(this->m_maxLines & 0xFFFFFF) && height <= 0.0f) {
        return text;
    }

    float fontHeight = this->GetFontHeight(true);
    float lineHeight = (CSimpleTop::RoundToPixelHeight(this->m_spacing) + fontHeight) * this->m_layoutScale;

    if (this->m_maxLines & 0xFFFFFF) {
        height = lineHeight * (this->m_maxLines & 0xFFFFFF);
    } else if (lineHeight >= height) {
        height = lineHeight;
    }

    uint32_t numBytes = SStrLen(text);

    if (TextBlockGetMaxCharsWithinWidthAndHeight(this->m_fontHandle, text, this->m_layoutScale * fontHeight, width, height, numBytes, this->m_layoutScale * this->m_shadowOffset.x, this->m_layoutScale, this->m_spacing * this->m_layoutScale, this->m_styleFlags) >= numBytes) {
        return text;
    }

    if (numBytes >= sizeof(s_displayText) - 4) {
        numBytes = sizeof(s_displayText) - 4;
    }

    SStrCopy(s_displayText, text, sizeof(s_displayText));

    uint32_t count;

    do {
        if (numBytes >= 3 && s_displayText[numBytes - 3] == '0' && s_displayText[numBytes - 2] == '|' && s_displayText[numBytes - 1] == 'k') {
            s_displayText[numBytes - 3] = '|';
            s_displayText[numBytes - 2] = 'k';
            numBytes--;
        } else if (numBytes) {
            char ch;

            do {
                ch = s_displayText[numBytes - 1];
                numBytes--;
            } while ((ch & 0xC0) == 0x80 && numBytes);
        }

        SStrCopy(&s_displayText[numBytes], "...", STORM_MAX_STR);
        count = TextBlockGetMaxCharsWithinWidthAndHeight(this->m_fontHandle, s_displayText, this->m_layoutScale * fontHeight, width, height, numBytes + 3, this->m_layoutScale * this->m_shadowOffset.x, this->m_layoutScale, this->m_spacing * this->m_layoutScale, this->m_styleFlags);
    } while (numBytes && count < numBytes + 3);

    return s_displayText;
}

// OFFSET: %0x0050A5F0
bool CSimpleFontString::SetLayoutScale(float scale, bool force) {
    if (!this->CLayoutFrame::SetLayoutScale(scale, force)) {
        return false;
    }

    if (this->m_fontHandle) {
        char fontName[1024];
        SStrCopy(fontName, this->GetFontName(), sizeof(fontName));

        uint32_t fontFlags = this->GetFontFlags();
        float fontHeight = this->m_fontHeight;
        HTEXTFONT font = NULL;

        if (fontHeight == 0.0f || (font = TextBlockGenerateFont(fontName, fontFlags, fontHeight * this->m_layoutScale)) != NULL) {
            this->m_fontHeight = fontHeight;

            if (this->m_string) {
                HandleClose(reinterpret_cast<HOBJECT>(this->m_string));
                this->m_string = NULL;
            }

            if (this->m_fontHandle) {
                HandleClose(reinterpret_cast<HOBJECT>(this->m_fontHandle));
            }

            this->m_fontHandle = font;

            if (font) {
                this->m_textWidth = 0.0f;
                this->m_textHeight = 0.0f;

                if (this->m_string) {
                    HandleClose(reinterpret_cast<HOBJECT>(this->m_string));
                    this->m_string = NULL;
                }

                this->m_layoutFlags &= ~0x1u;
                this->Resize(0);
            }
        }
    }

    return true;
}

// OFFSET: %0x0050C840
void CSimpleFontString::QueueTextures(CRenderBatch* batch) {
    for (CSimpleEmbeddedTexture* embedded = this->m_textures.Head(); !TSList<CSimpleEmbeddedTexture, TSGetExplicitLink<CSimpleEmbeddedTexture> >::IsEnd(embedded); embedded = this->m_textures.RawNext(embedded)) {
        if (embedded->m_texture && embedded->m_texture->m_texture) {
            batch->QueueTexture(embedded->m_texture);
        }
    }
}

// PROVEN: c69419c8 IDENTICAL/NONE
// OFFSET: 0x004FBEE0
void CSimpleFontString::Draw(CRenderBatch* batch) {
    if (this->m_fontHandle && this->m_text && *this->m_text) {
        batch->QueueFontString(this);
        this->QueueTextures(batch);
    }
}

// OFFSET: %0x0050C0B0
void CSimpleFontString::FreeEmbeddedTextures() {
    CSimpleEmbeddedTexture* embedded = this->m_textures.Head();

    while (!TSList<CSimpleEmbeddedTexture, TSGetExplicitLink<CSimpleEmbeddedTexture> >::IsEnd(embedded)) {
        CSimpleEmbeddedTexture* next = this->m_textures.RawNext(embedded);
        this->m_textures.UnlinkNode(embedded);
        embedded->m_texture->m_shown = 0;
        embedded->m_texture->HideThis();
        s_embeddedTextureStorage.LinkToTail(embedded);
        embedded = next;
    }
}

// OFFSET: %0x0050C8A0
void CSimpleFontString::FreeEmbededTextureStorage() {
    CSimpleEmbeddedTexture* embedded = s_embeddedTextureStorage.Head();

    while (!TSList<CSimpleEmbeddedTexture, TSGetExplicitLink<CSimpleEmbeddedTexture> >::IsEnd(embedded)) {
        embedded = s_embeddedTextureStorage.DeleteNode(embedded);
    }
}

// INLINED
CSimpleEmbeddedTexture* CSimpleFontString::GetEmbeddedTextureFromStorage() {
    CSimpleEmbeddedTexture* embedded = s_embeddedTextureStorage.Head();

    if (embedded) {
        s_embeddedTextureStorage.UnlinkNode(embedded);
        return embedded;
    }

    void* mem = SMemAlloc(sizeof(CSimpleEmbeddedTexture), __FILE__, __LINE__, 0);
    return mem ? new (mem) CSimpleEmbeddedTexture : NULL;
}

// OFFSET: %0x0050D960
void CSimpleFontString::RefreshEmbeddedTextures() {
    const GXUEMBEDDEDTEXTUREINFO* info;
    uint32_t count = GxuFontStringEmbeddedTextureInfo(this->m_string ? TextBlockGetStringPtr(this->m_string) : NULL, info);

    for (uint32_t i = 0; i < count; i++) {
        CSimpleEmbeddedTexture* embedded = CSimpleFontString::GetEmbeddedTextureFromStorage();
        this->m_textures.LinkToTail(embedded);
        embedded->InitEmbeddedTexture(this, &info[i]);
    }
}

// INLINED
void CSimpleFontString::operator delete(void* ptr) {
    CSimpleFontString::s_allocator.PutData(ptr, NULL, 0);
}

// OFFSET: %0x0050BE30
CSimpleFontString::CSimpleFontString(CSimpleFrame* frame, uint32_t drawLayer, bool show) : CSimpleRegion(frame, drawLayer, 0, show) {
    this->m_fontHandle = NULL;
    this->m_text = NULL;
    this->m_string = NULL;
    this->m_rotation = 0.0f;
    this->m_scale = 1.0f;
    this->m_spacing = 0.0f;
    this->m_textWidth = 0.0f;
    this->m_textHeight = 0.0f;
    this->m_textMaxSize = 0;
    this->m_textCurSize = 0;
    this->m_shadowColor.b = 0x00;
    this->m_shadowColor.g = 0x00;
    this->m_shadowColor.r = 0x00;
    this->m_shadowColor.a = 0xFF;
    this->m_shadowOffset.x = 0.0f;
    this->m_shadowOffset.y = 0.0f;
    this->m_justificationOffset.x = 0.0f;
    this->m_justificationOffset.y = 0.0f;
    this->m_alphaGradientStart = -1;
    this->m_styleFlags = 0x212;
    this->m_settableStyleFlags = 0x2107F;
    this->m_maxLines = (this->m_maxLines & 0xFE000000) | 0x1000000;
    s_fontStringCount++;
}

// OFFSET: [%0x0050C5E0, %0x0050BF80, %0x0050BF90, %0x0050CA40]
CSimpleFontString::~CSimpleFontString() {
    this->FreeEmbeddedTextures();

    if (this->m_text) {
        SMemFree(this->m_text, __FILE__, __LINE__, 0);
        this->m_text = NULL;
    }

    this->SetFont(NULL, 0.0f, 0, false);

    s_fontStringCount--;
}

// PROVEN: 09a65cd4 IDENTICAL/NONE
// OFFSET: 0x004F7630/0x14013AD40
void CSimpleFontString::SetTextLength(uint32_t length) {
    if (length == this->m_textMaxSize) {
        return;
    }

    if (length) {
        char* text = static_cast<char*>(SMemAlloc(length + 1, __FILE__, __LINE__, 0));
        *text = 0;

        if (this->m_text) {
            SMemFree(this->m_text, __FILE__, __LINE__, 0);
        }

        this->m_text = text;
        this->m_textCurSize = length;
    }

    this->m_textMaxSize = length;
}

// OFFSET: %0x005081D0
bool CSimpleFontString::SetAlphaGradient(int32_t start, int32_t length) {
    this->m_alphaGradientStart = start;
    this->m_alphaGradientLength = length;

    return !this->m_string || TextBlockSetGradient(this->m_string, start, length) < 2;
}

// PROVEN: 424e4fda IDENTICAL/NONE
// OFFSET: 0x004F8080/0x14013B8F0
void CSimpleFontString::SetWidth(float width) {
    this->CLayoutFrame::SetWidth(width);
    this->m_textWidth = 0.0f;
    this->m_textHeight = 0.0f;
}

// PROVEN: f662e189 IDENTICAL/NONE
// OFFSET: 0x004F80B0/0x14013B920
void CSimpleFontString::SetHeight(float height) {
    this->CLayoutFrame::SetHeight(height);
    this->m_textWidth = 0.0f;
    this->m_textHeight = 0.0f;
}

// OFFSET: %0x00508320
float CSimpleFontString::GetTextWidth(const char* text, uint32_t numBytes) {
    float extent = 0.0f;

    if (!this->m_fontHandle) {
        return extent;
    }

    if (!numBytes) {
        numBytes = SStrLen(text);
    }

    TextBlockGetTextExtent(this->m_fontHandle, text, numBytes, this->m_layoutScale * this->GetFontHeight(true), &extent, this->m_shadowOffset.x * this->m_layoutScale, this->m_layoutScale, 0.0f, this->m_styleFlags);

    return extent / this->m_layoutScale;
}

// OFFSET: %0x00508420
uint32_t CSimpleFontString::WrapText(const char* text, float width, uint32_t* lineStarts, uint32_t maxLines) {
    if (!this->m_fontHandle) {
        return 0;
    }

    return TextBlockWrapText(this->m_fontHandle, text, this->GetFontHeight(true) * this->m_layoutScale, width * this->m_layoutScale, lineStarts, maxLines, this->m_shadowOffset.x * this->m_layoutScale, this->m_layoutScale, 0.0f, this->m_styleFlags);
}

// OFFSET: %0x00508500
uint32_t CSimpleFontString::GetNumCharsWithinWidth(const char* text, uint32_t numBytes, float maxWidth) {
    if (!this->m_fontHandle) {
        return 0;
    }

    if (!numBytes) {
        numBytes = SStrLen(text);
    }

    float extent;

    return TextBlockGetMaxCharsWithinWidth(this->m_fontHandle, text, this->m_layoutScale * this->GetFontHeight(true), this->m_layoutScale * maxWidth, numBytes, &extent, this->m_shadowOffset.x * this->m_layoutScale, this->m_layoutScale, 0.0f, this->m_styleFlags);
}

// OFFSET: %0x005085F0
uint32_t CSimpleFontString::GetNumCharsWithinWidthFromEnd(const char* text, uint32_t numBytes, float maxWidth) {
    if (!this->m_fontHandle) {
        return 0;
    }

    if (!numBytes) {
        numBytes = SStrLen(text);
    }

    float extent;

    return TextBlockGetMaxCharsWithinWidthFromEnd(this->m_fontHandle, text, this->m_layoutScale * this->GetFontHeight(true), this->m_layoutScale * maxWidth, numBytes, &extent, this->m_shadowOffset.x * this->m_layoutScale, this->m_layoutScale, 0.0f, this->m_styleFlags);
}

// PROVEN: d1ad3519 IDENTICAL/NONE
// OFFSET: 0x004F8520
void CSimpleFontString::OnScreenSizeChanged() {
    this->m_textWidth = 0.0f;
    this->m_textHeight = 0.0f;
    this->Resize(0);
}

// PROVEN: 79139458 IDENTICAL/NONE
// OFFSET: 0x004F8540
void CSimpleFontString::AddAnimTranslation(CScriptRegion* region, const C2Vector& translation) {
    if (region && region != this) {
        return;
    }

    if (this->m_string) {
        C3Vector position = TextBlockGetStringPos(this->m_string);
        ApplyTranslationAnim(position, translation);
        TextBlockSetStringPos(this->m_string, position);
        this->OnRegionChanged();
    }
}

// PROVEN: 26cb9431 IDENTICAL/NONE
// OFFSET: 0x004FA440
void CSimpleFontString::AddAnimRotation(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, float radians) {
    if (region && region != this) {
        return;
    }

    if (!this->m_string) {
        return;
    }

    CGxString* string = TextBlockGetStringPtr(this->m_string);

    if (string) {
        this->m_rotation = this->m_animBaseRotation + radians;
        GxuFontSetRotation(string, this->m_rotation);
        GxuFontStringInvalidate(string);
    }
}

// PROVEN: a6036cbc IDENTICAL/NONE
// OFFSET: 0x004FA4B0
void CSimpleFontString::AddAnimScale(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, const C2Vector& scale) {
    if (region && region != this) {
        return;
    }

    if (!this->m_string) {
        return;
    }

    CGxString* string = TextBlockGetStringPtr(this->m_string);

    if (string) {
        this->m_scale = (1.0f - scale.x) * this->m_animBaseScale;
        GxuFontSetScale(string, this->m_scale);
        GxuFontStringInvalidate(string);
    }
}

// OFFSET: %0x005087E0
void CSimpleFontString::StoreAnimPosition() {
    if (!this->m_animPosition) {
        return;
    }

    C3Vector& position = this->m_animPosition->m_position[0];
    position.x = this->m_justificationOffset.x * this->m_layoutScale + this->m_rect.minX;
    position.y = this->m_justificationOffset.y * this->m_layoutScale + this->m_rect.minY;
    position.z = this->m_layoutDepth;

    if (this->m_string) {
        TextBlockSetStringPos(this->m_string, this->m_animPosition->m_position[0]);
    }
}

// OFFSET: %0x00508840
void CSimpleFontString::AnimData_Init() {
    CLayoutFrame::AnimData_Init();
    this->m_animBaseRotation = this->m_rotation;
    this->m_animBaseScale = this->m_scale;
    this->StoreAnimPosition();
}

// OFFSET: %0x0050A8F0
void CSimpleFontString::AnimData_ResetPositionData() {
    CLayoutFrame::AnimData_ResetPositionData();
    this->m_rotation = this->m_animBaseRotation;
    this->m_scale = this->m_animBaseScale;

    if (this->m_string) {
        CGxString* string = TextBlockGetStringPtr(this->m_string);

        if (string) {
            GxuFontSetRotation(string, this->m_animBaseRotation);
            GxuFontSetScale(string, this->m_animBaseScale);
            GxuFontStringInvalidate(string);
        }
    }

    this->StoreAnimPosition();
}

// PROVEN: 719c2258 IDENTICAL/NONE
// OFFSET: 0x004F8920
void CSimpleFontString::ClearString() {
    this->m_textWidth = 0.0f;
    this->m_textHeight = 0.0f;

    if (this->m_string) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_string));
        this->m_string = NULL;
    }

    this->m_layoutFlags &= ~0x1u;
    this->Resize(0);
}

// OFFSET: %0x00509630
void CSimpleFontString::PostLoadXML(linkObject* node, CStatus* status) {
    if (!this->m_parent || this->HasAnyFramepoints()) {
        return;
    }

    if (this->m_styleFlags & 0x1) {
        this->SetPoint(FRAMEPOINT_LEFT, this->m_parent, FRAMEPOINT_LEFT, 0.0f, 0.0f, 1);
    } else if (this->m_styleFlags & 0x4) {
        this->SetPoint(FRAMEPOINT_RIGHT, this->m_parent, FRAMEPOINT_RIGHT, 0.0f, 0.0f, 1);
    } else {
        this->SetPoint(FRAMEPOINT_CENTER, this->m_parent, FRAMEPOINT_CENTER, 0.0f, 0.0f, 1);
    }
}

// PROVEN: 66a90242 IDENTICAL/NONE
// OFFSET: 0x004F93E0/0x14013CD80
int32_t CSimpleFontString::SetFont(const char* fontName, float fontHeight, uint32_t fontFlags, bool force) {
    if (!force) {
        const char* currentName = this->GetFontName();

        if (fontName && currentName && !SStrCmpI(fontName, currentName, STORM_MAX_STR) && fequal(fontHeight, this->m_fontHeight) && fontFlags == this->GetFontFlags()) {
            return 1;
        }
    }

    HTEXTFONT font = NULL;

    if (fontName && fontHeight != 0.0f) {
        font = TextBlockGenerateFont(fontName, fontFlags, fontHeight * this->m_layoutScale);

        if (!font) {
            return 0;
        }
    }

    this->m_fontHeight = fontHeight;

    if (this->m_string) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_string));
        this->m_string = NULL;
    }

    if (this->m_fontHandle) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_fontHandle));
    }

    this->m_fontHandle = font;

    if (font) {
        this->ClearString();
    }

    return 1;
}

// OFFSET: %0x00509810
void CSimpleFontString::SetTextHeight(float height) {
    if (fabsf(height - this->m_fontHeight) >= 0.00000023841858f) {
        this->m_styleFlags &= ~0x200u;
        this->m_fontHeight = height;
        this->ClearString();
    }
}

// OFFSET: %0x00509AC0
void CSimpleFontString::SetText(const char* text, int32_t process) {
    if (text && process) {
        text = LanguageProcess(text);
    }

    if ((!text || !*text) && (!this->m_text || !*this->m_text)) {
        return;
    }

    if (text) {
        if (this->m_text && !SStrCmp(text, this->m_text, STORM_MAX_STR)) {
            return;
        }

        if (*text) {
            if (this->m_textMaxSize) {
                SStrCopy(this->m_text, text, this->m_textMaxSize);
            } else {
                uint32_t length = SStrLen(text);

                if (length > this->m_textCurSize) {
                    if (this->m_text) {
                        SMemFree(this->m_text, __FILE__, __LINE__, 0);
                    }

                    this->m_text = SStrDupA(text, __FILE__, __LINE__);
                    this->m_textCurSize = length;
                } else {
                    SStrCopy(this->m_text, text, STORM_MAX_STR);
                }
            }

            this->ClearString();
            return;
        }
    }

    if (this->m_text) {
        *this->m_text = 0;
    }

    this->ClearString();
}

// OFFSET: %0x00508100
void CSimpleFontString::SetJustificationOffset(float x, float y) {
    if (x == this->m_justificationOffset.x && this->m_justificationOffset.y == y) {
        return;
    }

    this->m_justificationOffset.x = x;
    this->m_justificationOffset.y = y;

    if (this->m_string) {
        C3Vector position;
        position.x = this->m_layoutScale * this->m_justificationOffset.x + this->m_rect.minX;
        position.y = this->m_justificationOffset.y * this->m_layoutScale + this->m_rect.minY;
        position.z = this->m_layoutDepth;
        TextBlockSetStringPos(this->m_string, position);
    }
}

// OFFSET: %0x00509ED0
void CSimpleFontString::AddShadow(const CImVector& color, const C2Vector& offset) {
    if ((this->m_styleFlags & 0x100) && *reinterpret_cast<const uint32_t*>(&color) == *reinterpret_cast<uint32_t*>(&this->m_shadowColor) && this->m_shadowOffset.x == offset.x && this->m_shadowOffset.y == offset.y) {
        return;
    }

    this->m_styleFlags |= 0x100;
    this->m_shadowColor = color;
    this->m_shadowOffset = offset;

    if (this->m_string) {
        CImVector shadowColor = this->m_shadowColor;
        C2Vector shadowOffset;
        shadowOffset.x = this->m_shadowOffset.x * this->m_layoutScale;
        shadowOffset.y = this->m_shadowOffset.y * this->m_layoutScale;
        uint8_t alpha = this->m_parent->m_alpha * this->m_parent->m_parentAlpha / 255;
        shadowColor.a = this->m_shadowColor.a * alpha / 255;
        TextBlockAddShadow(this->m_string, shadowColor, shadowOffset);
    }
}

// OFFSET: %0x00509FE0
void CSimpleFontString::SetShadowOffset(const C2Vector& offset) {
    if (this->m_shadowOffset.x == offset.x && this->m_shadowOffset.y == offset.y) {
        return;
    }

    this->m_shadowOffset = offset;

    if (this->m_shadowOffset.x == 0.0f && this->m_shadowOffset.y == 0.0f) {
        this->RemoveShadow();
    } else {
        this->AddShadow(this->m_shadowColor, this->m_shadowOffset);
    }

    this->ClearString();
}

// OFFSET: %0x0050A0A0
float CSimpleFontString::GetTextBlockWidth() {
    float width = this->CLayoutFrame::GetWidth();
    width = width != 0.0f ? this->m_layoutScale * width : this->m_textWidth;

    if (fabsf(width - (this->m_rect.maxX - this->m_rect.minX)) >= 0.0000099999997f) {
        return this->m_rect.maxX - this->m_rect.minX;
    }

    return width;
}

// OFFSET: %0x0050A120
float CSimpleFontString::GetTextBlockHeight() {
    float height = this->CLayoutFrame::GetHeight();
    height = height != 0.0f ? this->m_layoutScale * height : this->m_textHeight;

    if (fabsf(height - (this->m_rect.maxY - this->m_rect.minY)) >= 0.0000099999997f) {
        return this->m_rect.maxY - this->m_rect.minY;
    }

    return height;
}

// PROVEN: 5af701b0 IDENTICAL/NONE
// OFFSET: 0x004F9DC0/0x14013D900
float CSimpleFontString::GetStringWidth() {
    if (this->m_textWidth == 0.0f && this->m_fontHandle) {
        const char* text = this->GetDisplayText(this->CLayoutFrame::GetWidth() * this->m_layoutScale, this->CLayoutFrame::GetHeight() * this->m_layoutScale);

        if (text && *text) {
            TextBlockGetTextExtent(this->m_fontHandle, text, SStrLen(text), this->GetFontHeight(true) * this->m_layoutScale, &this->m_textWidth, this->m_shadowOffset.x * this->m_layoutScale, this->m_layoutScale, 0.0f, this->m_styleFlags);
        }
    }

    return this->m_textWidth / this->m_layoutScale;
}

// OFFSET: %0x0050A290
bool CSimpleFontString::IsTruncated() {
    float height = this->GetTextBlockHeight();
    float width = this->GetTextBlockWidth();
    const char* text = this->GetDisplayText(width, height);

    return text && this->m_text && SStrCmp(text, this->m_text, STORM_MAX_STR) != 0;
}

// OFFSET: %0x0050A520
float CSimpleFontString::GetWrappedWidth(float width) {
    const char* text = this->m_text && *this->m_text ? this->m_text : NULL;
    uint32_t lineStarts[30];
    uint32_t lineCount = this->WrapText(text, width, lineStarts, 30) >= 30 ? 30 : this->WrapText(text, width, lineStarts, 30);
    float maxWidth = 0.0f;

    for (uint32_t i = 0; i < lineCount; i++) {
        uint32_t end = i >= lineCount - 1 ? SStrLen(text) : lineStarts[i + 1];
        float lineWidth = this->GetTextWidth(&text[lineStarts[i]], end - lineStarts[i]);

        if (lineWidth > maxWidth) {
            maxWidth = lineWidth;
        }
    }

    return maxWidth;
}

// OFFSET: %0x0050AC60
float CSimpleFontString::GetWrappedWidth() {
    return this->GetWrappedWidth(this->GetWidth());
}

// PROVEN: 75ee4951 IDENTICAL/NONE
// OFFSET: 0x004F9F20/0x14013DB00
float CSimpleFontString::GetWidth() {
    float width = this->CLayoutFrame::GetWidth();

    if (width == 0.0f) {
        width = this->GetStringWidth();
    }

    return width > NDCToDDCWidth(1.0f / (CoordinateGetAspectCompensation() * 1024.0f)) ? width : NDCToDDCWidth(1.0f / (CoordinateGetAspectCompensation() * 1024.0f));
}

// PROVEN: b6ba25f1 IDENTICAL/NONE
// OFFSET: 0x004F9FA0/0x14013DBA0
float CSimpleFontString::GetStringHeight() {
    if (this->m_textHeight == 0.0f && this->m_fontHandle) {
        const char* text = this->GetDisplayText(this->CLayoutFrame::GetWidth() * this->m_layoutScale, this->CLayoutFrame::GetHeight() * this->m_layoutScale);

        if (text && *text) {
            C2Vector shadowOffset;
            shadowOffset.x = this->m_shadowOffset.x * this->m_layoutScale;
            shadowOffset.y = this->m_layoutScale * this->m_shadowOffset.y;
            this->m_textHeight = TextBlockGetWrappedTextHeight(this->m_fontHandle, text, this->GetFontHeight(true) * this->m_layoutScale, this->GetWidth() * this->m_layoutScale, shadowOffset, this->m_layoutScale, this->m_spacing * this->m_layoutScale, this->m_styleFlags);
        }
    }

    return this->m_textHeight / this->m_layoutScale;
}

// PROVEN: f2f96f72 IDENTICAL/NONE
// OFFSET: 0x004FA0C0/0x14013DD60
float CSimpleFontString::GetHeight() {
    float height = this->CLayoutFrame::GetHeight();

    if (height == 0.0f) {
        height = this->GetStringHeight();
    }

    return height > NDCToDDCWidth(1.0f / (CoordinateGetAspectCompensation() * 1024.0f)) ? height : NDCToDDCWidth(1.0f / (CoordinateGetAspectCompensation() * 1024.0f));
}

// PROVEN: e8d3ef7b IDENTICAL/NONE
// OFFSET: 0x004F84A0/0x14013BDB0
bool CSimpleFontString::SetLayoutDepth(float depth, bool force) {
    if (!this->CLayoutFrame::SetLayoutDepth(depth, force)) {
        return false;
    }

    if (this->m_string) {
        C3Vector position;
        position.x = this->m_justificationOffset.x * this->m_layoutScale + this->m_rect.minX;
        position.y = this->m_justificationOffset.y * this->m_layoutScale + this->m_rect.minY;
        position.z = this->m_layoutDepth;
        TextBlockSetStringPos(this->m_string, position);
    }

    return true;
}

// OFFSET: %0x0050A760
void CSimpleFontString::PreOnAnimUpdate() {
    if (!this->m_animating) {
        return;
    }

    if (this->m_string) {
        TextBlockSetStringPos(this->m_string, this->m_animPosition->m_position[0]);
    }

    if (this->m_animAlpha) {
        this->SetVertexAlpha(this->m_animSavedAlpha);
    }

    if (this->m_string) {
        CGxString* string = TextBlockGetStringPtr(this->m_string);

        if (string) {
            GxuFontSetRotation(string, this->m_animBaseRotation);
            GxuFontSetScale(string, this->m_animBaseScale);
            GxuFontStringInvalidate(string);
        }
    }
}

// OFFSET: %0x0050C6D0
void CSimpleFontString::OnColorChanged(bool changed) {
    this->CSimpleRegion::OnColorChanged(changed);

    CImVector color;
    color.b = 0xFF;
    color.g = 0xFF;
    color.r = 0xFF;
    color.a = 0xFF;

    if (this->m_hasColor) {
        const CImVector* source = this->m_hasGradient ? this->Gradient()->m_colors : &this->m_color;

        if (source) {
            color = *source;
        }
    }

    if (this->m_string) {
        TextBlockUpdateColor(this->m_string, color);

        if (this->m_styleFlags & 0x100) {
            CImVector shadowColor = this->m_shadowColor;
            C2Vector shadowOffset;
            shadowOffset.x = this->m_shadowOffset.x * this->m_layoutScale;
            shadowOffset.y = this->m_shadowOffset.y * this->m_layoutScale;
            uint8_t alpha = this->m_parent->m_alpha * this->m_parent->m_parentAlpha / 255;
            shadowColor.a = this->m_shadowColor.a * alpha / 255;
            TextBlockAddShadow(this->m_string, shadowColor, shadowOffset);
        }
    }

    for (CSimpleEmbeddedTexture* embedded = this->m_textures.Head(); !TSList<CSimpleEmbeddedTexture, TSGetExplicitLink<CSimpleEmbeddedTexture> >::IsEnd(embedded); embedded = this->m_textures.RawNext(embedded)) {
        if (embedded->m_texture && embedded->m_texture->m_texture) {
            CImVector textureColor(0, 0, 0, 0);
            embedded->m_texture->GetVertexColor(textureColor);
            textureColor.a = color.a;
            embedded->m_texture->SetVertexColor(textureColor);
        }
    }
}

// OFFSET: %0x0050E320
void CSimpleFontString::UpdateString() {
    if (!(this->m_layoutFlags & 0x1)) {
        return;
    }

    if (this->m_string) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_string));
        this->m_string = NULL;
    }

    this->FreeEmbeddedTextures();

    if (!(this->m_rect.maxY > this->m_rect.minY) || !(this->m_rect.maxX > this->m_rect.minX)) {
        return;
    }

    if (this->m_fontHandle && this->m_text && *this->m_text) {
        C3Vector position;
        position.x = this->m_justificationOffset.x * this->m_layoutScale + this->m_rect.minX;
        position.y = this->m_justificationOffset.y * this->m_layoutScale + this->m_rect.minY;
        position.z = this->m_layoutDepth;

        float width = this->GetTextBlockWidth();
        float height = this->GetTextBlockHeight();

        const char* text = this->GetDisplayText(width, height);
        uint32_t styleFlags = this->m_styleFlags;

        CImVector color;
        color.b = 0xFF;
        color.g = 0xFF;
        color.r = 0xFF;
        color.a = 0xFF;

        if (!(styleFlags & 0x400)) {
            bool hasColorCode = false;

            for (int32_t i = 0; text[i]; i++) {
                if (text[i] == '|') {
                    if (text[i + 1] == '|') {
                        i++;
                    } else if (text[i + 1] == 'C' || text[i + 1] == 'c') {
                        hasColorCode = true;
                        break;
                    }
                }
            }

            if (!hasColorCode) {
                styleFlags |= 0x400;
            }
        }

        if (this->m_hasGradient || this->m_hasColor) {
            this->GetBaseColor(color);
        }

        this->m_string = TextBlockCreate(this->m_fontHandle, text, color, position, this->GetFontHeight(true) * this->m_layoutScale, width, height, styleFlags, 0.0f, this->m_spacing * this->m_layoutScale, this->m_layoutScale, this->m_scale, this->m_rotation);

        if (this->m_styleFlags & 0x100) {
            CImVector shadowColor = this->m_shadowColor;
            uint8_t alpha = this->m_parent->GetEffectiveAlpha();
            C2Vector shadowOffset;
            shadowOffset.x = this->m_shadowOffset.x * this->m_layoutScale;
            shadowOffset.y = this->m_shadowOffset.y * this->m_layoutScale;
            shadowColor.a = this->m_shadowColor.a * alpha / 255;
            TextBlockAddShadow(this->m_string, shadowColor, shadowOffset);
        }

        if (this->m_alphaGradientStart >= 0) {
            TextBlockSetGradient(this->m_string, this->m_alphaGradientStart, this->m_alphaGradientLength);
        }

        this->RefreshEmbeddedTextures();

        if (this->m_animating) {
            this->m_animPosition->m_position[0] = position;
            TextBlockSetStringPos(this->m_string, position);
        }
    }

    this->OnRegionChanged();
}

// OFFSET: %0x0050E600
void CSimpleFontString::OnFrameSizeChanged(const CRect& rect) {
    this->CLayoutFrame::OnFrameSizeChanged(rect);

    if (this->m_string && fabsf((rect.maxX - rect.minX) - (this->m_rect.maxX - this->m_rect.minX)) < 0.00001f && fabsf((rect.maxY - rect.minY) - (this->m_rect.maxY - this->m_rect.minY)) < 0.00001f) {
        C3Vector position;
        position.x = this->m_justificationOffset.x * this->m_layoutScale + this->m_rect.minX;
        position.y = this->m_justificationOffset.y * this->m_layoutScale + this->m_rect.minY;
        position.z = this->m_layoutDepth;
        TextBlockSetStringPos(this->m_string, position);

        if (IsRectOffscreen(this->m_rect) != IsRectOffscreen(rect)) {
            this->OnRegionChanged();
        }
    } else {
        this->UpdateString();
    }

    if (this->m_animating) {
        C3Vector& stored = this->m_animPosition->m_position[0];
        C3Vector previous = stored;
        stored.x = this->m_justificationOffset.x * this->m_layoutScale + this->m_rect.minX;
        stored.y = this->m_justificationOffset.y * this->m_layoutScale + this->m_rect.minY;
        stored.z = this->m_layoutDepth;

        if (this->m_string) {
            C3Vector delta = stored - previous;
            C3Vector position = TextBlockGetStringPos(this->m_string) + delta;
            TextBlockSetStringPos(this->m_string, position);
        }
    }

    if (this->m_parent) {
        this->m_parent->NotifyScrollParent();
    }
}

// PROVEN: a03f5185 IDENTICAL/NONE
// OFFSET: 0x004FE570
void CSimpleFontString::SetStyleFlags(uint32_t styleFlags) {
    uint32_t flags = (styleFlags & this->m_settableStyleFlags) | (this->m_styleFlags & ~this->m_settableStyleFlags);

    if (flags != this->m_styleFlags) {
        this->m_styleFlags = flags;

        if (this->m_string) {
            this->UpdateString();
        }
    }
}

// OFFSET: %0x0050E850
void CSimpleFontString::SetSpacing(float spacing) {
    if (spacing < 0.0f) {
        spacing = 0.0f;
    }

    if (fabsf(spacing - this->m_spacing) >= 0.00000023841858f) {
        this->m_spacing = spacing;
        this->m_textHeight = 0.0f;

        if (this->m_string) {
            this->UpdateString();
        }
    }
}

// OFFSET: %0x0050E8B0
void CSimpleFontString::SetShadowColor(const CImVector& color) {
    if (*reinterpret_cast<const uint32_t*>(&color) != *reinterpret_cast<uint32_t*>(&this->m_shadowColor)) {
        this->m_shadowColor = color;

        if (this->m_string) {
            this->UpdateString();
        }
    }
}

// OFFSET: %0x0050E8E0
void CSimpleFontString::SetJustifyH(uint8_t justify) {
    this->m_settableStyleFlags &= ~0x7u;
    uint32_t styleFlags = (this->m_styleFlags & ~0x7u) | (justify & 0x7);

    if (styleFlags != this->m_styleFlags) {
        this->m_styleFlags = styleFlags;

        if (this->m_string) {
            this->UpdateString();
        }
    }
}

// OFFSET: %0x0050E920
void CSimpleFontString::SetJustifyV(uint8_t justify) {
    this->m_settableStyleFlags &= ~0x38u;
    uint32_t styleFlags = (this->m_styleFlags & ~0x38u) | (justify & 0x38);

    if (styleFlags != this->m_styleFlags) {
        this->m_styleFlags = styleFlags;

        if (this->m_string) {
            this->UpdateString();
        }
    }
}

// PROVEN: a5c64e3b IDENTICAL/NONE
// OFFSET: 0x004FE6C0
void CSimpleFontString::SetNonWordWrap(int32_t noWrap) {
    this->m_settableStyleFlags &= ~0x40u;
    uint32_t styleFlags = noWrap ? this->m_styleFlags | 0x40 : this->m_styleFlags & ~0x40u;

    if (styleFlags != this->m_styleFlags) {
        this->m_styleFlags = styleFlags;

        if (this->m_string) {
            this->UpdateString();
        }
    }
}

// PROVEN: c6e2cf7b IDENTICAL/NONE
// OFFSET: 0x004FE700
void CSimpleFontString::SetNonSpaceWrap(int32_t wrap) {
    this->m_settableStyleFlags &= ~0x1000u;
    uint32_t styleFlags = wrap ? this->m_styleFlags | 0x1000 : this->m_styleFlags & ~0x1000u;

    if (styleFlags != this->m_styleFlags) {
        this->m_styleFlags = styleFlags;

        if (this->m_string) {
            this->UpdateString();
        }
    }
}

// PROVEN: f8b2fcfe IDENTICAL/NONE
// OFFSET: 0x004FE750
void CSimpleFontString::SetIndented(int32_t indented) {
    this->m_settableStyleFlags &= ~0x20000u;
    uint32_t styleFlags = indented ? this->m_styleFlags | 0x20000 : this->m_styleFlags & ~0x20000u;

    if (styleFlags != this->m_styleFlags) {
        this->m_styleFlags = styleFlags;

        if (this->m_string) {
            this->UpdateString();
        }
    }
}

// OFFSET: %0x0050EB30
void CSimpleFontString::LoadXML(linkObject* node, CStatus* status) {
    if (node->m_flags & 0x2000000) {
        CSimpleFont* font = CSimpleFont::GetFont(node->m_inherits.m_string, 0);

        if (font) {
            if (this->m_maxLines & 0x1000000) {
                this->SetFontObject(font);
            }
        } else {
            XMLHashNodeRef ref;

            if (!ref.Acquire(node->m_inherits.m_string)) {
                status->Add(STATUS_WARNING, "Couldn't find inherited node: %s", ref.m_name);
            } else if (ref.m_locked) {
                status->Add(STATUS_WARNING, "Recursively inherited node: %s", ref.m_name);
            } else {
                this->LoadXML(ref.m_node, status);
            }
        }
    }

    this->CScriptRegion::LoadXML(node, status);

    if (!(node->m_flags & 0x10000)) {
        return;
    }

    FontString* fontString = static_cast<FontString*>(node);

    if (fontString->m_regionFlags & 0x2) {
        if (fontString->m_regionFlags & 0x8) {
            this->m_shown = 0;
            this->HideThis();
        } else {
            this->m_shown = 1;
            this->ShowThis(false);
        }
    }

    this->m_scale = (fontString->m_stringFlags & 0x40) ? fontString->m_scale : 1.0f;
    this->m_rotation = (fontString->m_stringFlags & 0x20) ? fontString->m_rotation : 0.0f;

    if (fontString->m_stringFlags & 0x4) {
        const char* text = FrameScript_GetText(fontString->m_text.m_string, -1, GENDER_NOT_APPLICABLE);

        if (!text || !*text) {
            text = fontString->m_text.m_string;
        }

        this->SetText(text, 1);
    }

    if (fontString->m_fontFlags & 0x4) {
        this->SetNonSpaceWrap((fontString->m_fontFlags >> 6) & 1);
    }

    if (fontString->m_stringFlags & 0x8) {
        this->SetNonWordWrap((fontString->m_stringFlags & 0x10) == 0);
    }

    if (fontString->m_stringFlags & 0x1) {
        this->SetTextLength(fontString->m_bytes);
    }

    if (fontString->m_stringFlags & 0x2) {
        this->m_maxLines = (this->m_maxLines & ~0xFFFFFFu) | (fontString->m_maxLines & 0xFFFFFF);
    }

    if (!(this->m_maxLines & 0x1000000)) {
        return;
    }

    if (fontString->m_fontFlags & 0x80) {
        const char* fontName = fontString->m_font.m_string;
        CSimpleFont* font = CSimpleFont::GetFont(fontName, 0);

        if (font) {
            this->SetFontObject(font);
        } else {
            float fontHeight = 0.0f;
            linkObject* heightNode = node->GetChild(XML_FONTHEIGHT);

            if (!heightNode || (LoadXML_Value(heightNode, fontHeight, status), fontHeight <= 0.0f)) {
                status->Add(STATUS_WARNING, "Font %s: Missing font height in %s element", this->GetDisplayName(), node->m_type.m_string);
                return;
            }

            uint32_t fontFlags = 0;

            if (fontString->m_fontFlags & 0x100) {
                if (!SStrCmpI(fontString->m_outline.m_string, "NORMAL", STORM_MAX_STR)) {
                    fontFlags = 1;
                } else if (!SStrCmpI(fontString->m_outline.m_string, "THICK", STORM_MAX_STR)) {
                    fontFlags = 5;
                }
            }

            if ((fontString->m_fontFlags & 0x200) && (fontString->m_fontFlags & 0x400)) {
                fontFlags |= 2;
            }

            if (this->SetFont(fontName, fontHeight, fontFlags, false)) {
                this->m_fontableFlags &= ~0x1u;
            } else {
                status->Add(STATUS_WARNING, "Font %s: Unable to load font file %s", this->GetDisplayName(), fontName);
            }
        }
    }

    if (fontString->m_fontFlags & 0x20) {
        this->SetSpacing(NDCToDDCWidth(fontString->m_spacing / (CoordinateGetAspectCompensation() * 1024.0f)));
        this->m_fontableFlags &= ~0x10u;
    }

    uint32_t justify;

    if ((fontString->m_fontFlags & 0x1) && StringToJustify(fontString->m_justifyV.m_string, justify)) {
        this->SetJustifyV(justify);
        this->m_fontableFlags &= ~0x2u;
    }

    if ((fontString->m_fontFlags & 0x2) && StringToJustify(fontString->m_justifyH.m_string, justify)) {
        this->SetJustifyH(justify);
        this->m_fontableFlags &= ~0x2u;
    }

    if (fontString->m_fontFlags & 0x8) {
        this->SetIndented((fontString->m_fontFlags >> 4) & 1);
    }

    if (fontString->m_regionFlags & 0x1) {
        float alpha = fontString->m_alpha;

        if (alpha < 0.0f) {
            alpha = 0.0f;
        } else if (alpha >= 1.0f) {
            alpha = 1.0f;
        }

        this->SetVertexAlpha(static_cast<uint8_t>(alpha * 255.0f));
    }

    if (fontString->m_regionFlags & 0x10) {
        this->SetIgnoreParentAlpha((fontString->m_regionFlags & 0x20) != 0);
    }

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        if (child->m_element == XML_COLOR) {
            CImVector color(0, 0, 0, 0);
            LoadXML_Color(child, color, status);
            this->SetVertexColor(color);
            this->m_fontableFlags &= ~0x4u;
        } else if (child->m_element == XML_SHADOW) {
            CImVector color(0x00, 0x00, 0x00, 0xFF);
            float offsetX = 0.001f;
            float offsetY = -0.001f;

            linkObject* colorNode = child->GetChild(XML_COLOR);

            if (colorNode) {
                LoadXML_Color(colorNode, color, status);
            }

            linkObject* offsetNode = child->GetChild(XML_OFFSET);

            if (offsetNode) {
                LoadXML_Dimensions(offsetNode, offsetX, offsetY, status);
            }

            C2Vector offset;
            offset.x = offsetX;
            offset.y = offsetY;
            this->AddShadow(color, offset);
            this->m_fontableFlags &= ~0x8u;
        }
    }
}

// PROVEN: daa9d731 IDENTICAL/NONE
// OFFSET: 0x004FEDB0/0x140143BB0
void CSimpleFontString::FontObjectUpdated(CSimpleFontStringAttributes& attributes) {
    attributes.Update(this, this->m_fontableFlags);
}

// PROVEN: bfc87778 IDENTICAL/NONE
// OFFSET: 0x004F8B90/0x1400A0C00
static void TextureUpdate(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    if (command != GxTex_Latch || !userArg) {
        return;
    }

    texelStrideInBytes = 4 * width;
    texels = static_cast<TSGrowableArray<CImVector>*>(userArg)->m_data;
}

// INLINED
void CSimpleTexture::operator delete(void* ptr) {
    CSimpleTexture::s_allocator.PutData(ptr, NULL, 0);
}

// OFFSET: %0x005074B0
int32_t CSimpleTextureImage::Load(const char* fileName, int32_t a3) {
    const char* formats[3];
    formats[0] = "%s.blp";
    formats[1] = "%s.png";
    formats[2] = "%s.tga";

    for (uint32_t i = 0; i < 3; i++) {
        char path[260];
        SStrPrintf(path, sizeof(path), formats[i], fileName);
        this->m_format = PIXEL_ARGB8888;
        this->m_bits = TextureLoadImage(path, &this->m_width, &this->m_height, &this->m_format, NULL, NULL, NULL, 0);

        if (this->m_bits) {
            break;
        }
    }

    return this->m_bits != NULL;
}

// PROVEN: 56bd9566 IDENTICAL/NONE
// OFFSET: 0x004FA890
void CSimpleTextureImage::Free() {
    if (this->m_flags & 1) {
        this->m_flags &= ~1u;
    }

    if (this->m_bits) {
        TextureFreeMippedImg(this->m_bits, this->m_format, this->m_width, this->m_height);
        this->m_bits = NULL;
    }
}

// OFFSET: %0x005088E0
void CTextureExtension::ReleaseMaskTexture() {
    if (this->m_maskTexture) {
        TextureRelease(this->m_maskTexture);
        this->m_maskTexture = NULL;
    }
}

// OFFSET: %0x0050BC00
CTextureExtension::~CTextureExtension() {
    if (this->m_maskTexture) {
        TextureRelease(this->m_maskTexture);
        this->m_maskTexture = NULL;
    }
}

// OFFSET: %0x00507550
int32_t CSimpleTexture::SetTexture(CTexture* texture) {
    if (this->m_texture != texture) {
        if (texture) {
            TextureAddRef(texture);
        }

        if (this->m_texture) {
            TextureRelease(this->m_texture);
        }

        this->m_texture = texture;
        this->m_fileName = NULL;
        this->OnRegionChanged();
    }

    return 1;
}

// OFFSET: %0x005075B0
int32_t CSimpleTexture::SetTexture(const CImVector& color) {
    CTexture* texture = TextureCreateSolid(color);

    if (this->m_texture) {
        TextureRelease(this->m_texture);
    }

    this->m_texture = texture;
    this->m_fileName = NULL;
    this->OnRegionChanged();

    return 1;
}

// OFFSET: %0x00507600
int32_t CSimpleTexture::GetTexture(char* buffer, uint32_t bufferLen) {
    if (!this->m_texture) {
        return 0;
    }

    const char* filename = TextureGetFilename(this->m_texture);

    if (!filename || !*filename) {
        return 0;
    }

    SStrCopy(buffer, filename, bufferLen);
    return 1;
}

// PROVEN: dc325804 IDENTICAL/NONE
// OFFSET: 0x004F71F0/0x14013A960
void CSimpleTexture::SetBlendMode(EGxBlend blend) {
    if (blend != this->m_alphaMode) {
        this->m_alphaMode = blend;
        this->OnRegionChanged();
    }
}

// PROVEN: 7488c7fa IDENTICAL/NONE
// OFFSET: 0x004F7320
void CSimpleTexture::GetTexCoord(CRect& texRect) {
    texRect.minX = this->m_texCoords[0].x;
    texRect.maxX = this->m_texCoords[2].x;
    texRect.minY = this->m_texCoords[0].y;
    texRect.maxY = this->m_texCoords[1].y;
}

// PROVEN: 835bdc57 IDENTICAL/NONE
// OFFSET: 0x004F7350
void CSimpleTexture::SetTexCoord(const C2Vector* texCoords) {
    this->m_texCoords[0] = texCoords[0];
    this->m_texCoords[1] = texCoords[1];
    this->m_texCoords[2] = texCoords[2];
    this->m_texCoords[3] = texCoords[3];
}

// PROVEN: 0206c6e5 IDENTICAL/NONE
// OFFSET: 0x004F73B0
void CSimpleTexture::GetTexCoord(C2Vector* texCoords) {
    texCoords[0] = this->m_texCoords[0];
    texCoords[1] = this->m_texCoords[1];
    texCoords[2] = this->m_texCoords[2];
    texCoords[3] = this->m_texCoords[3];
}

// PROVEN: 5bf2c801 IDENTICAL/NONE
// OFFSET: 0x004F7410
CGxShader* CSimpleTexture::GetImageModePixelShader(TextureImageMode mode) {
    return CSimpleTexture::s_imageModePixelShaders[mode];
}

// OFFSET: %0x00507790
CGxTex* CSimpleTexture::GetGxTex() {
    return this->m_texture ? TextureGetGxTexBlocking(this->m_texture) : NULL;
}

// OFFSET: %0x005077B0
void CSimpleTexture::AddAnimAlpha(CScriptRegion* region, int16_t alpha) {
    CSimpleRegion::AddAnimAlpha(region, alpha);
}

// OFFSET: %0x005077C0
void CSimpleTexture::AddAnimTranslation(CScriptRegion* region, const C2Vector& translation) {
    if (region && region != this) {
        return;
    }

    ApplyTranslationAnim(this->m_positions, translation);
    this->OnRegionChanged();
}

// OFFSET: %0x005077F0
void CSimpleTexture::AddAnimRotation(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, float radians) {
    if (region && region != this) {
        return;
    }

    ApplyRotationAnim(this->m_positions, origin, offset, radians);
    this->OnRegionChanged();
}

// OFFSET: %0x00507830
void CSimpleTexture::AddAnimScale(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, const C2Vector& scale) {
    if (region && region != this) {
        return;
    }

    ApplyScaleAnim(this->m_positions, origin, offset, scale);
    this->OnRegionChanged();
}

// PROVEN: b568ad96 IDENTICAL/NONE
// OFFSET: 0x004F74F0/0x14013ABF0
float CSimpleTexture::GetWidth() {
    float width = this->CLayoutFrame::GetWidth();

    if (width == 0.0f && this->m_texture) {
        uint32_t textureWidth;

        if (TextureGetDimensions(this->m_texture, &textureWidth, NULL, 0)) {
            return NDCToDDCWidth(static_cast<float>(textureWidth) / (CoordinateGetAspectCompensation() * 1024.0f));
        }
    }

    return width;
}

// PROVEN: f6691c78 IDENTICAL/NONE
// OFFSET: 0x004F7570/0x14013AC80
float CSimpleTexture::GetHeight() {
    float height = this->CLayoutFrame::GetHeight();

    if (height == 0.0f && this->m_texture) {
        uint32_t textureHeight;

        if (TextureGetDimensions(this->m_texture, NULL, &textureHeight, 0)) {
            return NDCToDDCWidth(static_cast<float>(textureHeight) / (CoordinateGetAspectCompensation() * 1024.0f));
        }
    }

    return height;
}

// PROVEN: 0c25116a IDENTICAL/NONE
// OFFSET: 0x004F75F0/0x14013AD10
int32_t CSimpleTexture::IsObjectLoaded() {
    if (this->m_texture && !TextureIsLoaded(this->m_texture)) {
        return 0;
    }

    return 1;
}

// OFFSET: %0x00508D30
void CSimpleTexture::Init() {
    for (uint32_t i = 0; i < 2; i++) {
        if (CSimpleTexture::s_imageModePixelShaderNames[i]) {
            CSimpleTexture::s_imageModePixelShaders[i] = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, CSimpleTexture::s_imageModePixelShaderNames[i]);
        }
    }
}

// OFFSET: %0x00508D60
void CSimpleTexture::Destroy() {
    for (uint32_t i = 0; i < 2; i++) {
        if (CSimpleTexture::s_imageModePixelShaders[i]) {
            g_theGxDevicePtr->ShaderRelease(CSimpleTexture::s_imageModePixelShaders[i]);
            CSimpleTexture::s_imageModePixelShaders[i] = NULL;
        }
    }
}

// OFFSET: %0x00508D90
void CSimpleTexture::PostLoadXML(linkObject* node, CStatus* status) {
    CSimpleFrame* parent = this->m_parent;

    if (!parent) {
        return;
    }

    for (uint32_t i = 0; i < FRAMEPOINT_NUMPOINTS; i++) {
        if (this->m_framePoints[i] && !(this->m_framePoints[i]->m_flags & 0x8)) {
            return;
        }
    }

    this->SetAllPoints(parent, 1);
}

// OFFSET: %0x00508DD0
void CSimpleTexture::HideThis() {
    CSimpleRegion::HideThis();

    if (!(this->m_textureFlags & 0x800) && CSimpleTexture::s_unloadHiddenTextures && this->m_fileName && this->m_texture) {
        CSimpleTop::QueueTextureRelease(this->m_texture);
        this->m_texture = NULL;
    }

    this->m_visible = 0;
}

// OFFSET: %0x00508E20
void CSimpleTexture::SetRotation(float radians, float centerX, float centerY) {
    float angle = radians - 0.78539819f;
    float s = sin(angle);
    float c = cos(angle);

    this->m_texCoords[0].x = s + centerX;
    this->m_texCoords[0].y = centerY - c;
    this->m_texCoords[1].x = centerX - c;
    this->m_texCoords[1].y = centerY - s;
    this->m_texCoords[2].x = c + centerX;
    this->m_texCoords[2].y = s + centerY;
    this->m_texCoords[3].x = centerX - s;
    this->m_texCoords[3].y = centerY + c;
}

// OFFSET: %0x00508ED0
void CSimpleTexture::SetTexCoord(const CRect& texRect) {
    this->m_texCoords[0].x = texRect.minX;
    this->m_texCoords[0].y = texRect.minY;
    this->m_texCoords[1].x = texRect.minX;
    this->m_texCoords[1].y = texRect.maxY;
    this->m_texCoords[2].x = texRect.maxX;
    this->m_texCoords[2].y = texRect.minY;
    this->m_texCoords[3].x = texRect.maxX;
    this->m_texCoords[3].y = texRect.maxY;
}

// INLINED
void CSimpleTexture::SetPosition(const CRect& rect, C3Vector* position) {
    position[0].x = rect.minX;
    position[0].y = rect.maxY;
    position[0].z = this->m_layoutDepth;
    position[1].x = rect.minX;
    position[1].y = rect.minY;
    position[1].z = this->m_layoutDepth;
    position[2].x = rect.maxX;
    position[2].y = rect.maxY;
    position[2].z = this->m_layoutDepth;
    position[3].x = rect.maxX;
    position[3].y = rect.minY;
    position[3].z = this->m_layoutDepth;
}

// OFFSET: %0x00508F30
void CSimpleTexture::OnFrameSizeChanged(const CRect& rect) {
    this->CLayoutFrame::OnFrameSizeChanged(rect);

    if (this->m_animating) {
        C3Vector* stored = this->m_animPosition->m_position;
        C3Vector positions[4];
        memcpy(positions, stored, sizeof(positions));
        this->SetPosition(this->m_rect, stored);

        this->m_positions[0].x += stored[0].x - positions[0].x;
        this->m_positions[0].y += stored[0].y - positions[0].y;
        this->m_positions[0].z += stored[0].z - positions[0].z;
        this->m_positions[1].x += stored[1].x - positions[1].x;
        this->m_positions[1].y += stored[1].y - positions[1].y;
        this->m_positions[1].z += stored[1].z - positions[1].z;
        this->m_positions[2].x += stored[2].x - positions[2].x;
        this->m_positions[2].y += stored[2].y - positions[2].y;
        this->m_positions[2].z += stored[2].z - positions[2].z;
        this->m_positions[3].x += stored[3].x - positions[3].x;
        this->m_positions[3].y += stored[3].y - positions[3].y;
        this->m_positions[3].z += stored[3].z - positions[3].z;
    } else {
        this->SetPosition(this->m_rect, this->m_positions);
    }

    if ((this->m_textureFlags & 0x4) || (this->m_textureFlags & 0x8)) {
        this->m_textureFlags |= 0x2;
        this->OnRegionChanged();
    }

    if (this->m_parent) {
        this->m_parent->NotifyScrollParent();
    }
}

// OFFSET: %0x00509210
void CSimpleTexture::PreOnAnimUpdate() {
    if (this->m_animating) {
        memcpy(this->m_positions, this->m_animPosition->m_position, sizeof(this->m_positions));

        if (this->m_animAlpha) {
            this->SetVertexAlpha(this->m_animSavedAlpha);
        }
    }
}

// OFFSET: %0x00509270
void CSimpleTexture::AnimData_Init() {
    CLayoutFrame::AnimData_Init();
    this->SetPosition(this->m_rect, this->m_animPosition->m_position);
    memcpy(this->m_positions, this->m_animPosition->m_position, sizeof(this->m_positions));
}

// OFFSET: %0x00509300
void CSimpleTexture::AnimData_ResetPositionData() {
    CLayoutFrame::AnimData_ResetPositionData();
    this->SetPosition(this->m_rect, this->m_positions);
}

// OFFSET: %0x005093A0
void CSimpleTexture::SetDesaturated(bool desaturated) {
    uint32_t mode = desaturated != 0;
    this->m_textureFlags ^= (this->m_textureFlags ^ (mode << 9)) & 0x600;

    CGxShader* shader = CSimpleTexture::s_imageModePixelShaders[mode];

    if (shader) {
        if (shader != this->m_shader) {
            this->m_shader = shader;
            this->OnRegionChanged();
        }

        return;
    }

    if (this->m_shader) {
        this->m_shader = NULL;
        this->OnRegionChanged();
    }

    CImVector vertexColor;
    this->GetVertexColor(vertexColor);

    CImVector color;

    if (desaturated) {
        color.b = 0x80;
        color.g = 0x80;
        color.r = 0x80;
        color.a = 0xFF;
    } else {
        color.b = 0xFF;
        color.g = 0xFF;
        color.r = 0xFF;
        color.a = 0xFF;
    }

    color.a = vertexColor.a;
    this->SetVertexColor(color);
}

// OFFSET: %0x0050AA40
void CSimpleTexture::DumpInfo(int32_t depth, void (*print)(char*, void*), void* param) {
    char buffer[256];

    if (depth) {
        memset(buffer, '.', depth);
    }

    SStrPrintf(&buffer[depth], sizeof(buffer) - depth, "Texture %p ", this);
    uint32_t length = SStrLen(buffer);

    if (this->m_texture) {
        buffer[length] = '+';
        SStrCopy(&buffer[length + 1], TextureGetFilename(this->m_texture), 251 - (length + 1));
        length = SStrLen(buffer);
    } else if (this->m_fileName) {
        buffer[length] = '-';
        SStrCopy(&buffer[length + 1], this->m_fileName->m_key.m_str, 251 - (length + 1));
        length = SStrLen(buffer);
    } else {
        buffer[length] = '?';
        length++;
    }

    buffer[length] = '\n';
    buffer[length + 1] = '\0';
    print(buffer, param);
}

// PROVEN: e65a0a8c IDENTICAL/NONE
// OFFSET: 0x004F8AE0
void CSimpleTexture::ResetTexCoord() {
    this->m_texCoords[0].x = 0.0f;
    this->m_texCoords[0].y = 0.0f;
    this->m_texCoords[1].x = 0.0f;
    this->m_texCoords[1].y = 1.0f;
    this->m_texCoords[2].x = 1.0f;
    this->m_texCoords[2].y = 0.0f;
    this->m_texCoords[3].x = 1.0f;
    this->m_texCoords[3].y = 1.0f;
}

// OFFSET: %0x0050AD90
CSimpleTexture::CSimpleTexture(CSimpleFrame* frame, uint32_t drawLayer, int8_t subLevel, bool show) : CSimpleRegion(frame, drawLayer, subLevel, show) {
    this->m_alphaMode = GxBlend_Alpha;
    this->m_fileName = NULL;
    this->m_extension = NULL;
    this->m_texture = NULL;
    this->m_shader = CSimpleTexture::s_imageModePixelShaders[ImageMode_Normal];
    this->m_textureFlags &= 0xFFFFC7F1;
    this->m_textureFlags ^= (this->m_textureFlags ^ CSimpleTexture::s_defaultNonBlocking) & 0x1;
    this->m_shader = CSimpleTexture::GetImageModePixelShader(ImageMode_Normal);
    CSimpleTexture::s_count++;
    this->ResetTexCoord();
}

// PROVEN: 61640e76 IDENTICAL/NONE
// OFFSET: 0x004FAAA0/0x14013E810
bool CSimpleTexture::IsA(int32_t type) {
    return type == CSimpleTexture::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: 19f1512d IDENTICAL/NONE
// OFFSET: 0x004FAAD0
bool CSimpleTexture::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleTexture::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptRegion::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// PROVEN: ded9c6d1 IDENTICAL/NONE
// OFFSET: 0x004FAB30
const char* CSimpleTexture::GetObjectTypeName() {
    return CSimpleTexture::s_objectTypeName;
}

// PROVEN: be51787e IDENTICAL/NONE
// OFFSET: 0x004FAB40/0x14013E8A0
int32_t CSimpleTexture::GetScriptMetaTable() {
    return CSimpleTexture::s_metatable;
}

// OFFSET: %0x0050BC70
void CSimpleTexture::AllocateExtension() {
    if (this->m_extension) {
        return;
    }

    void* mem = CTextureExtension::s_allocator.GetData(0, typeid(CTextureExtension).raw_name(), SLOG_OBJECT);

    if (mem) {
        CTextureExtension* extension = static_cast<CTextureExtension*>(mem);
        extension->m_flags &= ~1u;
        new (&extension->m_maskData) TSGrowableArray<CImVector>();
        extension->m_maskTexture = NULL;
        this->m_extension = extension;
    } else {
        this->m_extension = NULL;
    }
}

// PROVEN: ced4ac0e IDENTICAL/NONE
// OFFSET: 0x004FB3C0
void CSimpleTexture::Draw(CRenderBatch* batch) {
    if (this->m_texture) {
        batch->QueueTexture(this);
    }
}

// OFFSET: [%0x0050BDD0, %0x0050C480, %0x0050AF10]
CSimpleTexture::~CSimpleTexture() {
    if (this->m_texture) {
        TextureRelease(this->m_texture);
    }

    if (this->m_extension) {
        this->m_extension->~CTextureExtension();
        CTextureExtension::s_allocator.PutData(this->m_extension, NULL, 0);
        this->m_extension = NULL;
    }

    CSimpleTexture::s_count--;
}

// OFFSET: %0x0050C4B0
int32_t CSimpleTexture::LoadTexture() {
    if (!(this->m_textureFlags & 0x1000) && CSimpleTexture::s_unloadHiddenTextures) {
        if (!this->m_visible) {
            return 0;
        }

        for (CSimpleFrame* parent = this->m_parent; parent; parent = parent->m_parent) {
            if (!parent->m_visible) {
                return 0;
            }
        }
    }

    const char* fileName = this->m_fileName->m_key.m_str;
    CStatus status;

    CGxTexFlags texFlags;
    texFlags.m_filter = (this->m_textureFlags >> 6) & 7;
    texFlags.m_wrapU = (this->m_textureFlags >> 4) & 1;
    texFlags.m_wrapV = (this->m_textureFlags >> 5) & 1;

    CTexture* texture = TextureCreate(fileName, texFlags, &status, 7);

    if (status.m_highestSeverity >= STATUS_ERROR) {
        TextureRelease(texture);
        SysMsgAdd(&status);
        return 0;
    }

    this->m_shader = CSimpleTexture::s_imageModePixelShaders[(this->m_textureFlags >> 9) & 3];
    this->m_texture = texture;
    return 1;
}

// OFFSET: %0x0050C9E0
void CSimpleTexture::ShowThis(bool loadPending) {
    CSimpleRegion::ShowThis(loadPending);

    if (this->m_shown && (!this->m_parent || this->m_parent->m_visible) && !this->m_visible) {
        this->m_visible = 1;
    }

    if (this->m_fileName && !this->m_texture) {
        this->LoadTexture();
    }
}

// OFFSET: %0x0050D210
int32_t CSimpleTexture::SetTexture(const char* fileName, int32_t wrapU, int32_t wrapV, EGxTexFilter filter, TextureImageMode mode) {
    if (this->m_texture && fileName && TextureIsSame(this->m_texture, fileName)) {
        return 1;
    }

    if (this->m_texture) {
        TextureRelease(this->m_texture);
    }

    this->m_texture = NULL;
    this->m_fileName = NULL;

    if (fileName && *fileName) {
        CTextureExtension* extension = this->m_extension;

        if (extension && (extension->m_flags & 1)) {
            const char* maskFile = extension->m_maskFile;
            extension->ReleaseMaskTexture();

            CSimpleTextureImage image;
            image.unk_0004 = 0;
            image.unk_0008 = 0;

            if (image.Load(fileName, 1)) {
                uint32_t width = image.m_width;
                image.m_flags |= 1;
                uint32_t height = image.m_height;
                uint32_t stride = 4 * width;
                uint8_t* pixels = *reinterpret_cast<uint8_t**>(image.m_bits);

                CSimpleTextureImage mask;
                mask.unk_0004 = 0;
                mask.unk_0008 = 0;

                if (mask.Load(maskFile, 1)) {
                    mask.m_flags |= 1;
                    uint8_t* maskPixels = *reinterpret_cast<uint8_t**>(mask.m_bits);
                    uint32_t maskWidth = mask.m_width;
                    uint32_t maskStride = 4 * maskWidth;
                    uint32_t maskHeight = mask.m_height;

                    CRect texRect = { 0.0f, 0.0f, 0.0f, 0.0f };
                    this->GetTexCoord(texRect);

                    int32_t left = static_cast<int32_t>(static_cast<float>(width) * texRect.minX + 0.5f);
                    int32_t right = static_cast<int32_t>(static_cast<float>(width) * texRect.maxX + 0.5f);
                    int32_t top = static_cast<int32_t>(static_cast<float>(height) * texRect.minY + 0.5f);
                    int32_t bottom = static_cast<int32_t>(static_cast<float>(height) * texRect.maxY + 0.5f);

                    this->ResetTexCoord();

                    int32_t stepX = 2 * (left < right) - 1;
                    uint32_t columns = stepX * (right - left);
                    int32_t stepY = 2 * (top < bottom) - 1;
                    uint32_t rows = stepY * (bottom - top);

                    this->m_extension->m_maskData.SetCount(columns * rows);

                    uint32_t index = 0;
                    uint8_t* maskRow = maskPixels + 3;
                    uint8_t* row = pixels + stride * top + 4 * left;
                    int32_t rowStep = stride * stepY;

                    for (uint32_t y = 0; y < rows; y++) {
                        uint8_t* src = row;
                        uint8_t* maskSrc = maskRow;

                        for (uint32_t x = 0; x < columns; x++) {
                            uint8_t alpha;

                            if (x >= maskWidth || y >= maskHeight) {
                                alpha = 0;
                            } else {
                                alpha = *maskSrc & src[3];
                                maskSrc += 4;
                            }

                            uint8_t b = src[0];
                            uint8_t g = src[1];
                            uint8_t r = src[2];
                            CImVector* dest = &this->m_extension->m_maskData.m_data[index++];
                            dest->a = alpha;
                            dest->r = r;
                            dest->g = g;
                            dest->b = b;
                            src += 4 * stepX;
                        }

                        maskRow += maskStride;
                        row += rowStep;
                    }

                    char name[12];
                    SStrPrintf(name, sizeof(name), "mask%d", CSimpleTexture::s_maskCount++);

                    CGxTexFlags texFlags;
                    texFlags.m_filter = GxTex_Linear;

                    this->m_extension->m_maskTexture = TextureCreate(columns, rows, GxTex_Argb8888, GxTex_Argb8888, texFlags, &this->m_extension->m_maskData, &TextureUpdate, name, 0);
                    this->SetTexture(this->m_extension->m_maskTexture);
                }

                mask.Free();
            }

            image.Free();
        } else {
            this->m_textureFlags = this->m_textureFlags & 0xFFFFF80F | (16 * (wrapU & 1 | (2 * (wrapV & 1 | (2 * (filter & 7 | (8 * (mode & 3))))))));

            FILENAMEHASH* hash = CSimpleTexture::s_fileNameHash.Ptr(fileName);

            if (!hash) {
                hash = CSimpleTexture::s_fileNameHash.New(fileName, 0, 0);
            }

            this->m_fileName = hash;
            this->LoadTexture();
        }
    }

    this->OnRegionChanged();
    return 1;
}

// OFFSET: %0x0050D680
void CSimpleEmbeddedTexture::InitEmbeddedTexture(CSimpleFontString* string, const GXUEMBEDDEDTEXTUREINFO* info) {
    if (string && info) {
        if (!this->m_texture) {
            return;
        }

        char path[256];
        SStrCopy(path, info->m_path, info->m_pathLength + 1);

        if (this->m_texture->SetTexture(path, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal)) {
            float top = 0.0f;
            float left = 0.0f;
            float baseLine = 0.0f;
            float right = 0.0f;
            NDCToDDC(info->m_left, info->m_baseLineHeight, &left, &baseLine);
            NDCToDDC(info->m_right, info->m_lineHeight, &right, &top);

            float offsetX = NDCToDDCWidth(info->m_offsetX);
            float offsetY = NDCToDDCHeight(info->m_offsetY);
            float width = NDCToDDCWidth(info->m_width);
            float height = NDCToDDCHeight(info->m_height);

            uint32_t styleFlags = string->m_styleFlags;
            float x = 0.0f;
            float y = 0.0f;
            uint32_t point = FRAMEPOINT_TOPLEFT;

            switch (styleFlags & 0x7) {
                case 0x1:
                    x = left + offsetX;
                    break;

                case 0x2:
                    point = FRAMEPOINT_TOP;
                    x = (right + left) * 0.5f + offsetX;
                    break;

                case 0x4:
                    point = FRAMEPOINT_TOPRIGHT;
                    x = right + offsetX;
                    break;
            }

            switch (styleFlags & 0x38) {
                case 0x8:
                    y = top + offsetY;
                    break;

                case 0x10:
                    point += 3;
                    y = (string->GetStringHeight() * string->m_layoutScale + height) * 0.5f + baseLine + offsetY;
                    break;

                case 0x20:
                    point += 6;
                    y = string->GetStringHeight() + baseLine + offsetY;
                    break;
            }

            this->m_texture->MarkFramePointsUnused();
            this->m_texture->SetPoint(static_cast<FRAMEPOINT>(point), string, static_cast<FRAMEPOINT>(point), x, y, 1);
            this->m_texture->SetWidth(width);
            this->m_texture->SetHeight(height);
            this->m_texture->SetVertexAlpha(string->GetVertexAlpha());

            if (info->m_hasTexCoords) {
                this->m_texture->SetTexCoord(info->m_texCoords);
            } else {
                CRect texRect = { 0.0f, 0.0f, 1.0f, 1.0f };
                this->m_texture->SetTexCoord(texRect);
            }

            if (info->m_hasColor) {
                this->m_texture->SetVertexColor(info->m_color);
            } else {
                this->m_texture->SetVertexColor(CImVector(0xFF, 0xFF, 0xFF, 0xFF));
            }

            this->m_texture->m_shown = 1;
            this->m_texture->ShowThis(false);
            return;
        }
    }

    if (this->m_texture) {
        this->m_texture->m_shown = 0;
        this->m_texture->HideThis();
    }
}

// OFFSET: %0x0050DAA0
void CSimpleTexture::LoadXML(linkObject* node, CStatus* status) {
    if (node->m_flags & 0x2000000) {
        XMLHashNodeRef ref;

        if (!ref.Acquire(node->m_inherits.m_string)) {
            status->Add(STATUS_WARNING, "Couldn't find inherited node: %s", ref.m_name);
        } else if (ref.m_locked) {
            status->Add(STATUS_WARNING, "Recursively inherited node: %s", ref.m_name);
        } else {
            this->LoadXML(ref.m_node, status);
        }
    }

    this->CScriptRegion::LoadXML(node, status);

    if (!(node->m_flags & 0x8000)) {
        return;
    }

    Texture* texture = static_cast<Texture*>(node);
    CImVector color(0, 0, 0, 0);
    int32_t wrapU = 0;
    int32_t wrapV = 0;

    if (texture->m_textureFlags & 0x8000) {
        this->m_textureFlags ^= (this->m_textureFlags ^ (texture->m_textureFlags >> 3)) & 0x2000;
    }

    if (texture->m_textureFlags & 0x800) {
        this->m_textureFlags ^= (this->m_textureFlags ^ (texture->m_textureFlags >> 2)) & 0x800;
    }

    if (texture->m_textureFlags & 0x1000) {
        this->m_textureFlags ^= (this->m_textureFlags ^ (texture->m_textureFlags >> 2)) & 0x1000;
    }

    if (texture->m_textureFlags & 0x80) {
        wrapU = 1;
        this->m_textureFlags ^= (this->m_textureFlags ^ (4 * ((texture->m_textureFlags >> 8) & 0xFF))) & 0x4;
    }

    if (texture->m_textureFlags & 0x200) {
        wrapV = 1;
        this->m_textureFlags ^= (this->m_textureFlags ^ (8 * ((texture->m_textureFlags >> 10) & 0xFF))) & 0x8;
    }

    if (texture->m_regionFlags & 0x10) {
        this->SetIgnoreParentAlpha((texture->m_regionFlags & 0x20) != 0);
    }

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        switch (child->m_element) {
            case XML_COLOR:
                LoadXML_Color(child, color, status);
                this->SetTexture(color);
                break;

            case XML_GRADIENT: {
                ORIENTATION orientation;
                CImVector minColor(0, 0, 0, 0);
                CImVector maxColor(0, 0, 0, 0);
                LoadXML_Gradient(child, orientation, minColor, maxColor, status);
                this->SetVertexGradient(orientation, minColor, maxColor);
                break;
            }

            case XML_TEXCOORDS: {
                bool horizError = false;
                bool vertError = false;
                float ulx = 0.0f;
                float uly = 0.0f;
                float llx = 0.0f;
                float lly = 1.0f;
                float urx = 1.0f;
                float ury = 0.0f;
                float lrx = 1.0f;
                float lry = 1.0f;

                linkObject* rectNode = child->GetChild(XML_RECT);

                if (rectNode) {
                    Rect* rect = static_cast<Rect*>(rectNode);

                    if (rect->m_rectFlags & 0x1) {
                        ulx = rect->m_ulx;
                    }

                    if (rect->m_rectFlags & 0x2) {
                        uly = rect->m_uly;
                    }

                    if (rect->m_rectFlags & 0x10) {
                        llx = rect->m_llx;
                    }

                    if (rect->m_rectFlags & 0x20) {
                        lly = rect->m_lly;
                    }

                    if (rect->m_rectFlags & 0x4) {
                        urx = rect->m_urx;
                    }

                    if (rect->m_rectFlags & 0x8) {
                        ury = rect->m_ury;
                    }

                    if (rect->m_rectFlags & 0x40) {
                        lrx = rect->m_lrx;
                    }

                    if (rect->m_rectFlags & 0x80) {
                        lry = rect->m_lry;
                    }
                }

                TexCoords* texCoords = static_cast<TexCoords*>(child);

                if (texCoords->m_texCoordFlags & 0x1) {
                    if ((this->m_textureFlags & 0x4) && texCoords->m_left != 0.0f) {
                        horizError = true;
                    }

                    llx = texCoords->m_left;
                    ulx = llx;
                }

                if (texCoords->m_texCoordFlags & 0x2) {
                    if ((this->m_textureFlags & 0x4) && texCoords->m_right != 1.0f) {
                        horizError = true;
                    }

                    urx = texCoords->m_right;
                    lrx = urx;
                }

                if (texCoords->m_texCoordFlags & 0x4) {
                    if ((this->m_textureFlags & 0x8) && texCoords->m_top != 0.0f) {
                        vertError = true;
                    }

                    ury = texCoords->m_top;
                    uly = ury;
                }

                if (texCoords->m_texCoordFlags & 0x8) {
                    if ((this->m_textureFlags & 0x8) && texCoords->m_bottom != 1.0f) {
                        vertError = true;
                    }

                    lry = texCoords->m_bottom;
                    lly = lry;
                }

                bool valid = ulx >= -10000.0f && ulx <= 10000.0f
                    && uly >= -10000.0f && uly <= 10000.0f
                    && urx >= -10000.0f && urx <= 10000.0f
                    && ury >= -10000.0f && ury <= 10000.0f
                    && llx >= -10000.0f && llx <= 10000.0f
                    && lly >= -10000.0f && lly <= 10000.0f
                    && lrx >= -10000.0f && lrx <= 10000.0f
                    && lry >= -10000.0f && lry <= 10000.0f;

                if (horizError) {
                    const char* name = this->GetName();
                    status->Add(STATUS_ERROR, "Texture %s: Invalid TexCoords value (horizTile is on)", name ? name : "<unnamed>");
                } else if (vertError) {
                    const char* name = this->GetName();
                    status->Add(STATUS_ERROR, "Texture %s: Invalid TexCoords value (vertTile is on)", name ? name : "<unnamed>");
                } else if (valid) {
                    C2Vector coords[4];
                    coords[0].x = ulx;
                    coords[0].y = uly;
                    coords[1].x = llx;
                    coords[1].y = lly;
                    coords[2].x = urx;
                    coords[2].y = ury;
                    coords[3].x = lrx;
                    coords[3].y = lry;
                    this->SetTexCoord(coords);

                    if (ulx < 0.0f || ulx > 1.0f || llx < 0.0f || llx > 1.0f || urx < 0.0f || urx > 1.0f || lrx < 0.0f || lrx > 1.0f) {
                        wrapU = 1;
                    }

                    if (uly < 0.0f || uly > 1.0f || lly < 0.0f || lly > 1.0f || ury < 0.0f || ury > 1.0f || lry < 0.0f || lry > 1.0f) {
                        wrapV = 1;
                    }
                } else {
                    const char* name = this->GetName();
                    status->Add(STATUS_ERROR, "Texture %s: Invalid rect value (out of range)", name ? name : "<unnamed>");
                }

                break;
            }
        }
    }

    if (texture->m_textureFlags & 0x1) {
        this->AllocateExtension();
        this->m_extension->m_flags |= 1;
        SStrCopy(this->m_extension->m_maskFile, texture->m_mask.m_string, sizeof(this->m_extension->m_maskFile));
    }

    if (texture->m_textureFlags & 0x2) {
        const char* file = texture->m_file.m_string;

        if (this->SetTexture(file, wrapU, wrapV, CSimpleTexture::s_textureFilterMode, ImageMode_Normal)) {
            if (*reinterpret_cast<uint32_t*>(&color)) {
                this->SetVertexColor(color);
            }
        } else {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "Texture %s: Unable to load texture file %s", name ? name : "<unnamed>", file);

            CImVector missingColor;
            missingColor.b = 0x00;
            missingColor.g = 0xFF;
            missingColor.r = 0x00;
            missingColor.a = 0xFF;
            this->SetTexture(missingColor);
        }
    }

    if (texture->m_textureFlags & 0x8) {
        if (texture->m_textureFlags & 0x10) {
            this->m_shown = 0;
            this->HideThis();
        } else {
            this->m_shown = 1;
            this->ShowThis(false);
        }
    }

    if (texture->m_textureFlags & 0x4) {
        this->SetBlendMode(static_cast<EGxBlend>(texture->m_alphaMode));
    }

    if (texture->m_regionFlags & 0x1) {
        float alpha = texture->m_alpha;

        if (alpha < 0.0f) {
            alpha = 0.0f;
        } else if (alpha >= 1.0f) {
            alpha = 1.0f;
        }

        CImVector vertexColor(0, 0, 0, 0);
        this->GetVertexColor(vertexColor);
        vertexColor.a = static_cast<uint8_t>(alpha * 255.0f);
        this->SetVertexColor(vertexColor);
    }

    if (texture->m_textureFlags & 0x20) {
        this->m_textureFlags ^= (this->m_textureFlags ^ (texture->m_textureFlags >> 6)) & 0x1;
    }

    if (texture->m_textureFlags & 0x20000) {
        this->SetDesaturated((texture->m_textureFlags & 0x40000) != 0);
    }
}
