#ifndef ENGINE_SOURCE_FRAME_CSIMPLETEXTURE_H
#define ENGINE_SOURCE_FRAME_CSIMPLETEXTURE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleRegion.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"

class CImVector;
class CRect;
class CRenderBatch;
class CSimpleFrame;
class CGxShader;
class CGxTex;
class CStatus;
class CTexture;
struct MipBits;
class linkObject;

struct FILENAMEHASH : public TSHashObject<FILENAMEHASH, HASHKEY_STR> {
};

struct CSimpleTextureImage {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ MipBits* m_bits;
    /* 0010 */ uint32_t m_width;
    /* 0014 */ uint32_t m_height;
    /* 0018 */ PIXEL_FORMAT m_format;

    void Free();
    int32_t Load(const char* fileName, int32_t a3);
};

struct CTextureExtension {
    static CDataAllocator s_allocator;

    /* 0000 */ uint32_t m_flags;
    /* 0004 */ char m_maskFile[260];
    /* 0108 */ TSGrowableArray<CImVector> m_maskData;
    /* 0118 */ CTexture* m_maskTexture;
    /* 011C */ uint8_t unk_011C[20];

    ~CTextureExtension();
    void ReleaseMaskTexture();
};

class CSimpleTexture : public CSimpleRegion {
    public:
    static CDataAllocator s_allocator;
    static int32_t s_count;
    static bool s_defaultNonBlocking;
    static TSHashTable<FILENAMEHASH, HASHKEY_STR> s_fileNameHash;
    static CGxShader* s_imageModePixelShaders[2];
    static const char* s_imageModePixelShaderNames[2];
    static int32_t s_maskCount;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;
    static EGxTexFilter s_textureFilterMode;
    static bool s_unloadHiddenTextures;

    static void operator delete(void* ptr);
    static void CreateScriptMetaTable();
    static void Destroy();
    static void DestroyScriptMetaTable();
    static CGxShader* GetImageModePixelShader(TextureImageMode mode);
    static int32_t GetObjectType();
    static void Init();
    static void RegisterScriptMethods(lua_State* L);

    /* 00BC */ FILENAMEHASH* m_fileName;
    /* 00C0 */ CTextureExtension* m_extension;
    /* 00C4 */ CTexture* m_texture;
    /* 00C8 */ EGxBlend m_alphaMode;
    /* 00CC */ CGxShader* m_shader;
    /* 00D0 */ C3Vector m_positions [4];
    /* 0100 */ C2Vector m_texCoords [4];
    /* 0120 */ uint32_t m_textureFlags;

    virtual ~CSimpleTexture();
    virtual int32_t GetScriptMetaTable();
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void PreOnAnimUpdate();
    virtual void AddAnimTranslation(CScriptRegion* region, const C2Vector& translation);
    virtual void AddAnimRotation(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, float radians);
    virtual void AddAnimScale(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, const C2Vector& scale);
    virtual void AddAnimAlpha(CScriptRegion* region, int16_t alpha);
    virtual void Draw(CRenderBatch* batch);
    virtual void LoadXML(linkObject* node, CStatus* status);
    virtual float GetWidth();
    virtual float GetHeight();
    virtual int32_t IsObjectLoaded();
    virtual void DumpInfo(int32_t depth, void (*print)(char*, void*), void* param);
    virtual void HideThis();
    virtual void ShowThis(bool loadPending);
    virtual void OnFrameSizeChanged(const CRect& rect);
    virtual void AnimData_Init();
    virtual void AnimData_ResetPositionData();

    CSimpleTexture(CSimpleFrame* frame, uint32_t drawLayer, int8_t subLevel, bool show);
    void AllocateExtension();
    CGxTex* GetGxTex();
    void GetTexCoord(C2Vector* texCoords);
    void GetTexCoord(CRect& texRect);
    int32_t GetTexture(char* buffer, uint32_t bufferLen);
    int32_t LoadTexture();
    void PostLoadXML(linkObject* node, CStatus* status);
    void ResetTexCoord();
    void SetBlendMode(EGxBlend blend);
    void SetDesaturated(bool desaturated);
    void SetPosition(const CRect& rect, C3Vector* position);
    void SetRotation(float radians, float centerX, float centerY);
    void SetShader(CGxShader* shader);
    void SetTexCoord(const C2Vector* texCoords);
    void SetTexCoord(const CRect& texRect);
    int32_t SetTexture(CTexture* texture);
    int32_t SetTexture(const CImVector& color);
    int32_t SetTexture(const char* fileName, int32_t wrapU, int32_t wrapV, EGxTexFilter filter, TextureImageMode mode);
};

#include "Engine/Source/Frame/CSimpleTexture.inl"

#endif
