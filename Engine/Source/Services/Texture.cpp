#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BLPFile/blp.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Images/MippedImg.h"
#include "Engine/Source/Images/StaticImage.h"
#include "Engine/Source/Images/tga.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/FileNotification.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Services/TextureBlob.h"
#include "Engine/Source/Services/TextureInt.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"

// STATIC: #0x009F1028
static const uint16_t s_pixelFormatBytesPerBlock[NUM_PIXEL_FORMATS] = { 8, 16, 0, 0, 0, 0, 0, 16, 0, 0 };

// STATIC: %0x00ADDA54
static const int32_t s_pixelFormatToMipBitsCache[NUM_PIXEL_FORMATS] = { -1, -1, -1, 0, 0, 0, -1, -1, -1, 1 };

// STATIC: %0x00ADDB08
static const uint32_t s_texFormatBytesPerTexel[GxTexFormats_Last] = { 0, 4, 4, 2, 2, 2, 2, 8, 16, 16, 2, 4, 8, 4, 4, 16, 4, 4, 2, 4, 4, 4, 4, 4 };

// STATIC: %0x00ADDB68
static const uint32_t s_texFormatTexelsPerBlock[GxTexFormats_Last] = { 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

// STATIC: %0x00BB9CF0
static EGxTexFormat s_pixelFormatToGxTexFormat[NUM_PIXEL_FORMATS] = { GxTex_Dxt1, GxTex_Dxt3, GxTex_Argb8888, GxTex_Argb1555, GxTex_Argb4444, GxTex_Rgb565, GxTex_Unknown, GxTex_Dxt5, GxTex_Unknown, GxTex_Unknown };

// STATIC: %0x00BB9D78
static EGxTexFilter s_filterMode = GxTex_LinearMipNearest;

// STATIC: %0x00BB9D7C
static uint32_t s_maxAnisotropy = 1;

// STATIC: %0x00BB9D80
static int32_t s_createBlpAsync = 1;

// STATIC: %0x00BB9D84
static uint32_t s_maxTextureWidth = 0xFFFFFFFF;

// STATIC: %0x00BB9D88
static uint32_t s_maxTextureHeight = 0xFFFFFFFF;

// STATIC: %0x00BB9E28
static TSExplicitList<CTexture, offsetof(CTexture, m_link)> s_textureList;

// STATIC: %0x00BB9E34
static uint32_t s_defaultColor = 0xFF00FF00;

// STATIC: %0x00BB9E38
static TSExplicitList<CGxTexCache, offsetof(CGxTexCache, m_link)> s_gxTexCacheFree;

// STATIC: %0x00BB9E44
static TSExplicitList<CMipBitsCache, offsetof(CMipBitsCache, m_link)> s_mipBitsCacheFree;

// STATIC: %0x00BB9E50
static TSExplicitList<CTextureAtlas, offsetof(CTextureAtlas, m_link)> s_atlasList;

// STATIC: %0x00BB9E68
static TSExplicitList<CAsyncObject, offsetof(CAsyncObject, m_link)> s_asyncTextureList;

// STATIC: %0x00CD2900
static uint32_t s_currentTime;

// STATIC: %0x00CD2904
static char* s_substituteFrom;

// STATIC: %0x00CD2908
static char* s_substituteTo;

// STATIC: %0x00CD290C
static int32_t s_atlasEnable;

// STATIC: %0x00CD2918
static MipBits* s_mipBits;

// STATIC: %0x00CD291C
static int32_t s_pumping;

// STATIC: %0x00CD2920
static int32_t s_gxTexCacheSize;

// STATIC: %0x00CD2924
static int32_t s_gxTexCacheMaxSize;

// STATIC: %0x00CD2928
static uint32_t s_asyncLoadBufferUsed;

// STATIC: %0x00CD292C
static TSHashTableReuse<CTexture, HASHKEY_TEXTUREFILE> s_textureCache;

// STATIC: %0x00CD2968
static TSExplicitList<CGxTexCache, offsetof(CGxTexCache, m_link)> s_gxTexCache[6][6];

// STATIC: %0x00CD2B18
static TSExplicitList<CMipBitsCache, offsetof(CMipBitsCache, m_link)> s_mipBitsCache[6][6][2];

// INLINED
HASHKEY_TEXTUREFILE::HASHKEY_TEXTUREFILE() {
    this->m_filename = NULL;
    this->m_anyTexFlags = 0;
}

// INLINED
bool HASHKEY_TEXTUREFILE::operator==(const HASHKEY_TEXTUREFILE& key) const {
    return !SStrCmpI(this->m_filename, key.m_filename, 260) && (this->m_anyTexFlags || key.m_anyTexFlags || this->m_texFlags == key.m_texFlags);
}

// INLINED
CMipBitsCache::CMipBitsCache() {
    this->m_bits = NULL;
}

// INLINED
CMipBitsCache::~CMipBitsCache() {
    if (this->m_bits) {
        SMemFree(this->m_bits, __FILE__, __LINE__, 0);
    }
}

// PROVEN: c74642f9 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GxTexNullUserFunc(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
}

// PROVEN: 450314b2 IDENTICAL/IDENTICAL
// OFFSET: 0x00540720/0x140193230
EGxTexFormat TexturePixelFormatToGxTexFormat(PIXEL_FORMAT format) {
    return s_pixelFormatToGxTexFormat[format];
}

// OFFSET: 0x00540730
void CTextureAtlas::Update(CTexture* texture) {
    this->m_updateSlot = texture->m_atlasBlockIndex;
    GxTexUpdate(this->m_gxTex, (texture->m_atlasBlockIndex & 7) * this->m_width, ((texture->m_atlasBlockIndex >> 3) & 7) * this->m_height, (texture->m_atlasBlockIndex & 7) * this->m_width + this->m_width, ((texture->m_atlasBlockIndex >> 3) & 7) * this->m_height + this->m_height, 1);
    this->m_updateSlot = -1;
}

// OFFSET: %0x00550CC0
void AsyncTextureDestroyedCallback(CAsyncObject* object) {
    s_asyncLoadBufferUsed -= object->m_size;
    void* buffer = object->m_buffer;
    AsyncFileReadDestroyObject(object);
    SMemFree(buffer, __FILE__, __LINE__, 0);
}

// OFFSET: 0x00540850
void TextureReportLoadError(const char* type, const char* fileName, CStatus* status) {
    char buffer[256];
    SErrGetErrorStr(SErrGetLastError(), buffer, sizeof(buffer));
    status->Add(STATUS_FATAL, "Error loading %s file \"%s\": %s\n", type, fileName, buffer);
    SErrSetLastError(0);
}

// OFFSET: 0x005408A0
static void TextureBuildMipFormat(char* buffer, const char* fileName) {
    SStrCopy(buffer, fileName, 260);
    *OsPathFindExtensionWithDot(buffer) = '\0';
    SStrPack(buffer, "_mip%d.tga", 260);
}

// OFFSET: 0x00540910
int32_t TextureBuildAliasPath(char* buffer, const char* fileName) {
    if (!s_substituteFrom) {
        return 0;
    }

    const char* name = OsPathFindFilename(fileName);

    if (_stricmp(s_substituteFrom, name)) {
        return 0;
    }

    SStrCopy(buffer, fileName, 260);
    SStrCopy(&buffer[name - fileName], s_substituteTo, 260 - (name - fileName));

    return 1;
}

// OFFSET: 0x005409C0
MipBits* GetDefaultTexture(uint32_t width, uint32_t height) {
    MipBits* images = s_mipBits;
    MipBits* level = images;

    MippedImgSet(PIXEL_ARGB8888, width, height, images);

    while (width > 1 || height > 1) {
        memset(level->m_mip[0], 0xFF, 4 * height * width);
        level++;
        width = width >> 1 ? width >> 1 : 1;
        height >>= 1;

        if (!height) {
            height = 1;
        }
    }

    return images;
}

// OFFSET: 0x00543D60
void AsyncTextureFailureCallback(CAsyncObject* object) {
    CTexture* texture = static_cast<CTexture*>(object->m_userArg);
    texture->m_asyncObject = NULL;
    s_asyncLoadBufferUsed -= object->m_size;
    void* buffer = object->m_buffer;
    object->m_userArg = NULL;
    AsyncFileReadDestroyObject(object);
    SMemFree(buffer, __FILE__, __LINE__, 0);
    FillInSolidTexture(*reinterpret_cast<const CImVector*>(&s_defaultColor), texture);
}

// OFFSET: 0x00540AB0
SFileImpl* TextureCreateOpenFile(const char* debugName, const char* fileName, int32_t flags) {
    SFileImpl* file = NULL;
    SErrSetLastError(0);

    if (SFile::OpenEx(NULL, fileName, flags != 0, &file)) {
        return file;
    }

    if (!SErrGetLastError()) {
        SErrSetLastError(STORM_ERROR_FILE_NOT_FOUND);
    }

    return NULL;
}

// OFFSET: 0x00540A40/0x140193520
static void TextureAppendExtension(char* extension, int32_t type, uint32_t size) {
    if (!extension) {
        return;
    }

    if (type == 0) {
        SStrCopy(extension, ".tga", size);
    } else if (type == 1) {
        SStrCopy(extension, ".blp", size);
    }
}

// OFFSET: 0x00540B00
static int32_t TextureSplitExtension(char* buffer, const char* fileName, char*& extension) {
    int32_t type = 1;
    SStrCopy(buffer, fileName, 260);
    uint32_t length = strlen(fileName);

    if (length >= 4 && buffer[length - 4] == '.') {
        length -= 4;
    }

    extension = &buffer[length];

    if (*extension == '.') {
        _strlwr(extension + 1);

        if (extension[3] == 'a') {
            if (extension[1] == 't' && extension[2] == 'g') {
                type = 0;
            }
        } else if (extension[3] == 'p' && extension[1] == 'b' && extension[2] == 'l') {
            *extension = '\0';
            return 1;
        }

        *extension = '\0';
    }

    return type;
}

// PROVEN: 55a942d1 IDENTICAL/IDENTICAL
// OFFSET: 0x00540BB0/0x140193630
EGxTexFilter TextureGetFilterMode() {
    return s_filterMode;
}

// PROVEN: 8deda106 IDENTICAL/IDENTICAL
// OFFSET: 0x00540BC0/0x140193640
uint32_t TextureGetMaxAnisotropy() {
    return s_maxAnisotropy;
}

// PROVEN: e3fbcd12 IDENTICAL/IDENTICAL
// OFFSET: 0x00540BD0/0x140193650
void TextureEnableAtlas(int32_t enable) {
    s_atlasEnable = enable;
}

// OFFSET: %0x00551040
CGxTexFlags TextureMakeTexFlags(CGxTexFlags texFlags, int32_t defaultFilter) {
    if (defaultFilter) {
        texFlags.m_filter = s_filterMode;
    }

    texFlags.m_maxAnisotropy = texFlags.m_filter == GxTex_Anisotropic ? s_maxAnisotropy : 1;
    texFlags.m_bit19 = defaultFilter;

    return texFlags;
}

// PROVEN: 53f3bc6c IDENTICAL/IDENTICAL
// OFFSET: 0x00540BE0/0x140193660
void TextureAddRef(CTexture* texture) {
    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    texture->m_refcount++;
}

// OFFSET: 0x00540C00/0x140193680
void TextureRelease(CTexture*& texture) {
    if (!--texture->m_refcount) {
        delete texture;
    }

    texture = NULL;
}

// OFFSET: 0x00540C20/0x1401936B0
int32_t TextureIsSame(CTexture* texture, const char* fileName) {
    char name[260];
    char* extension;
    TextureSplitExtension(name, fileName, extension);

    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return SStrCmpI(name, texture->m_filename, sizeof(texture->m_filename)) == 0;
}

// PROVEN: a0a0409b IDENTICAL/NONE
// OFFSET: 0x00540C80/0x140193790
void TextureSetUserData(CTexture* texture, GxTexUserFunc userFunc, void* userArg) {
    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    GxTexSetUserData(texture->m_gxTex, userFunc, userArg);
}

// PROVEN: 39642d4a IDENTICAL/IDENTICAL
// OFFSET: 0x00540CB0/0x1401937E0
int32_t TextureOnAtlas(CTexture* texture) {
    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return texture->m_atlas != NULL;
}

// OFFSET: 0x00540CE0/0x140193810
int32_t TextureGetAtlasOffsetAndScale(CTexture* texture, C2Vector& offset, float& scale) {
    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (!texture->m_atlas) {
        return 0;
    }

    scale = 0.125f;
    offset.x = 0.125f * (texture->m_atlasBlockIndex & 7);
    offset.y = ((texture->m_atlasBlockIndex >> 3) & 7) * scale;

    return 1;
}

// OFFSET: #0x004B54F0
int32_t TextureIsOpaque(CTexture* texture) {
    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return texture->m_flags & 1;
}

// OFFSET: %0x005511F0
int32_t TextureCalcMipCount(uint32_t width, uint32_t height) {
    int32_t count = 1;

    while (width > 1 || height > 1) {
        width >>= 1;
        count++;

        if (!width) {
            width = 1;
        }

        height >>= 1;

        if (!height) {
            height = 1;
        }
    }

    return count;
}

// OFFSET: %0x00551230
void TextureGenerateMips(uint32_t width, uint32_t height, uint32_t firstLevel, uint32_t levelCount, MipBits* images) {
    uint32_t sourceWidth = width;
    uint32_t sourceHeight = height;
    uint32_t sourceLevel = 0;

    if (firstLevel) {
        sourceWidth = width >> (firstLevel - 1);
        sourceHeight = height >> (firstLevel - 1);
        sourceLevel = firstLevel - 1;
    }

    for (uint32_t level = 1; level < levelCount; level++) {
        width = width >> 1 ? width >> 1 : 1;
        height = height >> 1 ? height >> 1 : 1;

        if (level >= firstLevel) {
            FullShrink(images->m_mip[level], width, height, images->m_mip[sourceLevel], sourceWidth, sourceHeight);
        }
    }
}

// PROVEN: 02708afe IDENTICAL/IDENTICAL
// OFFSET: 0x00540E20/0x140193980
void TextureFreeMippedImg(MipBits* bits) {
    if (bits) {
        SMemFree(bits, __FILE__, __LINE__, 0);
    }
}

// PROVEN: b2d930da IDENTICAL/DIFFERENT
// OFFSET: 0x00540E40/0x1401939B0
TEXFILETYPE TextureDiscoverFileType(const char* fileName) {
    const char* extension = OsPathFindExtensionWithDot(fileName);

    if (!extension || strlen(extension) != 4) {
        return TEXFILE_UNKNOWN;
    }

    if (!_stricmp(extension, ".TGA")) {
        return TEXFILE_TGA;
    }

    if (!_stricmp(extension, ".BLP")) {
        return TEXFILE_BLP;
    }

    return _stricmp(extension, ".PNG") ? TEXFILE_UNKNOWN : TEXFILE_PNG;
}

// OFFSET: 0x00540ED0/0x140193A50
TEXFILETYPE TexturePickAlternateFilename(const char* fileName, TEXFILETYPE type, char* buffer, uint32_t size) {
    if (fileName != buffer) {
        SStrCopy(buffer, fileName, size);
    }

    if (!type) {
        return TEXFILE_UNKNOWN;
    }

    char* extension = strrchr(buffer, '.');

    if (extension) {
        *extension = '\0';
    }

    if (type == TEXFILE_TGA || type == TEXFILE_PNG) {
        SStrPack(buffer, ".BLP", size);
        return TEXFILE_BLP;
    }

    if (type == TEXFILE_BLP) {
        SStrPack(buffer, ".TGA", size);
        return TEXFILE_TGA;
    }

    return type;
}

// PROVEN: 7f3e27ce IDENTICAL/IDENTICAL
// OFFSET: 0x00540FD0/0x140193BC0
const char* TextureGetFilename(CTexture* texture) {
    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    return texture->m_filename;
}

// OFFSET: 0x00540FF0/0x140193BF0
int32_t TextureIsLoaded(CTexture* texture) {
    return texture && !texture->m_asyncObject;
}

// OFFSET: %0x005514B0
void TextureSetTextureSubstitutions(const char* from, const char* to) {
    s_substituteFrom = const_cast<char*>(from);
    s_substituteTo = const_cast<char*>(to);
}

// PROVEN: 3833ca98 IDENTICAL/NONE
// OFFSET: 0x00541030
void TextureGetGxTexFlags(CTexture* texture, CGxTexFlags* texFlags) {
    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    *texFlags = texture->m_gxTexFlags;
}

// OFFSET: #0x004B5770
uint32_t TextureGetBytesPerBlock(PIXEL_FORMAT format) {
    return s_pixelFormatBytesPerBlock[format];
}

// OFFSET: %0x005514F0
int32_t TextureLoadBlob(const char* fileName) {
    return CTextureBlob::Load(fileName);
}

// OFFSET: 0x00541060/0x140193C50
int32_t TextureLoadBlob(const char* fileName, void* data, uint32_t size) {
    return CTextureBlob::Load(fileName, data, size);
}

// PROVEN: 4ddb0b42 IDENTICAL/IDENTICAL
// OFFSET: 0x00541070/0x140193C60
int32_t TextureUnloadBlob(const char* fileName) {
    return CTextureBlob::Unload(fileName);
}

// PROVEN: 2f536430 IDENTICAL/NONE
// OFFSET: 0x00541080/0x140193C70
const char* TextureGetObjectName(CTexture* texture) {
    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    return texture->GetObjectName();
}

// PROVEN: 73c03e4c IDENTICAL/DIFFERENT
// OFFSET: 0x005410A0/0x140193CA0
void TextureComputeLoadProgress(CTexture* texture, int64_t& loaded, int64_t& total) {
    char* extension = OsPathFindExtensionWithDot(texture->m_filename);
    strcpy(extension, ".blp");
    SFile::FileGetIsLocalAmount(texture->m_filename, loaded, total);
    total++;

    if (!texture->m_asyncObject) {
        loaded++;
    }

    *extension = '\0';
}

// PROVEN: af02a2c0 IDENTICAL/IDENTICAL
// OFFSET: 0x00541100/0x140193D30
int32_t TextureIsBlob(CTexture* texture) {
    return texture->m_asyncObject && texture->m_gxTex;
}

// PROVEN: 047493f0 IDENTICAL/DIFFERENT
// OFFSET: 0x00541120/0x140193D50
int32_t TextureIsStreaming(CTexture* texture) {
    return texture->m_asyncObject && texture->m_asyncObject->m_isStreamed;
}

// PROVEN: 56119f1f IDENTICAL/NONE
// OFFSET: 0x00541140/0x140193D70
int32_t TextureGetAsyncRenderStatus(CTexture* texture) {
    if (texture->m_asyncObject) {
        return texture->m_gxTex != NULL;
    }

    return 2;
}

// OFFSET: 0x005411C0/0x140193D90
void ResolveMipLevel(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    // STATIC: %0x00CD2E78
    static uint32_t s_emptyTexel;

    if (command == GxTex_3) {
        return;
    }

    CTextureAtlas* atlas = static_cast<CTextureAtlas*>(userArg);

    if (atlas->m_updateSlot == -1) {
        atlas->m_dirty |= 1;
        texelStrideInBytes = 0;
        texels = &s_emptyTexel;
        return;
    }

    CTexture* texture = atlas->m_textures[atlas->m_updateSlot];

    switch (command) {
        case GxTex_Lock:
            break;

        case GxTex_Latch:
            if (s_pumping) {
                texelStrideInBytes = GxCalcTexelStrideInBytes(texture->m_dataFormat, atlas->m_width);
                texels = s_mipBits[level].m_mip[0];
            }

            break;
    }
}

// OFFSET: %0x005516B0
uint32_t LoadPredrawnMips(CTgaFile* image, const char* format, int32_t flags, MipBits* images) {
    uint32_t level = 1;
    uint32_t count = TextureCalcMipCount(image->Width(), image->Height()) - 1;
    uint32_t width = image->Width() >> 1;
    uint32_t height = image->Height() >> 1;

    if (!count) {
        return level;
    }

    int32_t onDisk = flags != 0;

    do {
        char fileName[260];
        SStrPrintf(fileName, sizeof(fileName), format, level);

        if (!SFile::FileExistsEx(fileName, onDisk)) {
            break;
        }

        CTgaFile mip;

        if (!mip.Open(fileName, flags) || width != mip.Width() || height != mip.Height() || !mip.LoadImageData(2)) {
            return level;
        }

        if (!mip.AlphaBits()) {
            mip.AddAlphaChannel(NULL);
        }

        mip.SetTopDown(1);

        const TGA32Pixel* source = mip.ImageBGRA32Pixel();
        C4Pixel* dest = images->m_mip[level++];

        for (uint32_t pixels = width * height; pixels; pixels--) {
            dest->m_b = source->m_b;
            dest->m_g = source->m_g;
            dest->m_r = source->m_r;
            dest->m_a = source->m_a;
            dest++;
            source++;
        }

        if (width > 1) {
            width >>= 1;
        }

        if (height > 1) {
            height >>= 1;
        }
    } while (--count);

    return level;
}

// OFFSET: 0x005414C0
void RequestImageDimensions(uint32_t* width, uint32_t* height, uint32_t* bestMip) {
    CGxCaps caps = g_theGxDevicePtr->Caps();

    if (!caps.m_texMaxSize[0]) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    uint32_t maxWidth = caps.m_texMaxSize[0] >= s_maxTextureWidth ? s_maxTextureWidth : caps.m_texMaxSize[0];
    uint32_t maxHeight = caps.m_texMaxSize[0] < s_maxTextureHeight ? caps.m_texMaxSize[0] : s_maxTextureHeight;

    while (*width > maxWidth || *height > maxHeight) {
        *width >>= 1;
        *height >>= 1;
        (*bestMip)++;

        if (!*width) {
            *width = 1;
        }

        if (!*height) {
            *height = 1;
        }
    }
}

// OFFSET: 0x00541230
static void* TextureLoadFile(const char* fileName, int32_t flags, uint32_t* size) {
    SFileImpl* file = TextureCreateOpenFile(NULL, fileName, flags);

    if (!file) {
        return NULL;
    }

    uint32_t fileSize = SFileGetFileSize(file, NULL);
    void* data = SMemAlloc(fileSize, __FILE__, __LINE__, 0);

    if (!SFile::Read(file, data, fileSize, NULL)) {
        DebugLog("TextureLoadFile() failed read: %s.\n", fileName);
    }

    SFileCloseFile(file);
    *size = fileSize;

    return data;
}

// OFFSET: 0x00541930
int32_t TextureLoadBlpMips(void* data, uint32_t size, MipBits** images) {
    StaticImage image;

    if (!image.SetToBLPData(static_cast<uint8_t*>(data), size)) {
        return 0;
    }

    PIXEL_FORMAT pixelFormat;
    EGxTexFormat gxTexFormat;
    TextureGetTextureFormats(pixelFormat, gxTexFormat, image.m_pixelFormat, image.m_alphaFormat & 0x3F);

    uint32_t bestMip = 0;
    uint32_t width = image.m_width;
    uint32_t height = image.m_height;
    RequestImageDimensions(&width, &height, &bestMip);
    MippedImgSet(pixelFormat, width, height, *images);

    return image.Convert(pixelFormat, *images, bestMip);
}

// OFFSET: 0x00541550/0x140194080
void TextureGetTextureFormats(PIXEL_FORMAT& pixelFormat, EGxTexFormat& gxTexFormat, PIXEL_FORMAT preferredFormat, uint32_t alphaBits) {
    switch (preferredFormat) {
        case PIXEL_DXT1:
            if (g_theGxDevicePtr->Caps().m_texFmt[GxTex_Dxt1]) {
                gxTexFormat = GxTex_Dxt1;
                pixelFormat = PIXEL_DXT1;
            } else if (alphaBits) {
                gxTexFormat = GxTex_Argb1555;
                pixelFormat = PIXEL_ARGB1555;
            } else {
                gxTexFormat = GxTex_Rgb565;
                pixelFormat = PIXEL_RGB565;
            }

            break;

        case PIXEL_DXT3:
            if (g_theGxDevicePtr->Caps().m_texFmt[GxTex_Dxt3]) {
                gxTexFormat = GxTex_Dxt3;
                pixelFormat = PIXEL_DXT3;
            } else {
                gxTexFormat = GxTex_Argb4444;
                pixelFormat = PIXEL_ARGB4444;
            }

            break;

        case PIXEL_ARGB8888:
            gxTexFormat = GxTex_Argb8888;
            pixelFormat = PIXEL_ARGB8888;
            break;

        case PIXEL_ARGB1555:
            gxTexFormat = GxTex_Argb1555;
            pixelFormat = PIXEL_ARGB8888;
            break;

        case PIXEL_ARGB4444:
            gxTexFormat = GxTex_Argb4444;
            pixelFormat = PIXEL_ARGB8888;
            break;

        case PIXEL_RGB565:
            gxTexFormat = GxTex_Rgb565;
            pixelFormat = PIXEL_ARGB8888;
            break;

        case PIXEL_DXT5:
            if (g_theGxDevicePtr->Caps().m_texFmt[GxTex_Dxt5]) {
                gxTexFormat = GxTex_Dxt5;
                pixelFormat = PIXEL_DXT5;
            } else {
                gxTexFormat = GxTex_Argb4444;
                pixelFormat = PIXEL_ARGB4444;
            }

            break;

        case PIXEL_UNSPECIFIED:
            gxTexFormat = GxTex_Argb8888;
            pixelFormat = PIXEL_ARGB8888;
            break;

        default:
            break;
    }
}

// PROVEN: d6e405d8 DIFFERENT/IDENTICAL
// OFFSET: 0x005416B0/0x140194180
uint32_t TextureCacheGetMaxSize() {
    uint32_t size = 0x4000000;

    if (OsGetPhysicalMemory() <= 0x40000000) {
        size = 0x2000000;
    }

    if (g_theGxDevicePtr->DeviceApi() == GxApi_D3d9 || g_theGxDevicePtr->DeviceApi() == GxApi_D3d11) {
        return 0;
    }

    return size;
}

// OFFSET: 0x00541700/0x1401941D0
void TextureSetFilterMode(EGxTexFilter filter) {
    if (filter >= GxTex_LinearMipLinear && !g_theGxDevicePtr->Caps().m_texFilterTrilinear) {
        s_maxAnisotropy = 1;
        s_filterMode = GxTex_LinearMipNearest;
    } else if (filter >= GxTex_Anisotropic && !g_theGxDevicePtr->Caps().m_texFilterAnisotropic) {
        s_maxAnisotropy = 1;
        s_filterMode = GxTex_LinearMipLinear;
    } else {
        s_filterMode = filter;
    }
}

// OFFSET: 0x00541760/0x140194230
void TextureSetMaxAnisotropy(uint32_t maxAnisotropy) {
    if (s_filterMode != GxTex_Anisotropic) {
        s_maxAnisotropy = 1;
        return;
    }

    if (maxAnisotropy > g_theGxDevicePtr->Caps().m_maxTexAnisotropy) {
        maxAnisotropy = g_theGxDevicePtr->Caps().m_maxTexAnisotropy;
    }

    s_maxAnisotropy = maxAnisotropy;
}

// OFFSET: #0x004B6280
int32_t TextureGetRefCnt(CTexture* texture) {
    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return texture->m_refcount;
}

// OFFSET: 0x00541820/0x140194260
int32_t TextureIsReadyNow(CTexture* texture) {
    if (!texture || !texture->m_gxTex) {
        return 0;
    }

    return g_theGxDevicePtr->TexIsReadyNow(texture->m_gxTex);
}

// OFFSET: 0x00541850/0x140194280
int32_t TextureCanUpdate(CTexture* texture) {
    if (!texture || !texture->m_gxTex) {
        return 0;
    }

    return g_theGxDevicePtr->TexCanUpdate(texture->m_gxTex);
}

// OFFSET: 0x00541880/0x1401942A0
uint32_t TextureCalcGxTexSize(EGxTexFormat format, uint32_t width, uint32_t height, EGxTexFilter filter) {
    uint32_t baseMip = g_theGxDevicePtr->DeviceBaseMipLevel();
    width >>= baseMip;
    height >>= baseMip;

    if (!width) {
        width = 1;
    }

    if (!height) {
        height = 1;
    }

    uint32_t texelsPerBlock = s_texFormatTexelsPerBlock[format];
    uint32_t bytesPerTexel = s_texFormatBytesPerTexel[format];
    uint32_t size = (texelsPerBlock + width - 1) / texelsPerBlock * ((texelsPerBlock + height - 1) / texelsPerBlock) * bytesPerTexel;

    if (filter > GxTex_Linear) {
        while (width != 1 || height != 1) {
            if (width > 1) {
                width >>= 1;
            }

            if (height > 1) {
                height >>= 1;
            }

            size += (texelsPerBlock + width - 1) / texelsPerBlock * ((texelsPerBlock + height - 1) / texelsPerBlock) * bytesPerTexel;
        }
    }

    return size;
}

// OFFSET: %0x00551F40
void CTextureAtlas::Reload() {
    for (uint32_t i = 0; i < 64; i++) {
        CTexture* texture = this->m_textures[i];

        if (!texture) {
            continue;
        }

        s_pumping = 1;

        if (!TextureReloadMips(texture, &s_mipBits)) {
            GetGlobalStatusObj().Add(STATUS_ERROR, "Texture %s not loaded.\n", texture->m_filename);
        }

        this->Update(texture);
        s_pumping = 0;
    }

    this->m_dirty &= ~1;
}

// OFFSET: 0x005419C0/0x1401944C0
void AsyncTextureProcess(CAsyncObject* object, int32_t immediate) {
    object->m_link.Unlink();
    object->m_buffer = SMemAlloc(object->m_size, __FILE__, __LINE__, 0);
    s_asyncLoadBufferUsed += object->m_size;
    AsyncFileReadObject(object, immediate);
}

// INLINED
void AsyncTextureWait(CTexture* texture) {
    CAsyncObject* object = texture->m_asyncObject;

    if (object) {
        if (!object->m_buffer) {
            AsyncTextureProcess(object, 1);
        }

        AsyncFileReadWait(texture->m_asyncObject);
    }
}

// OFFSET: 0x00541A30/0x140194550
void TextureSetCacheSize(int32_t size) {
    uint32_t maxSize = 0x4000000;

    if (OsGetPhysicalMemory() <= 0x40000000) {
        maxSize = 0x2000000;
    }

    if (g_theGxDevicePtr->DeviceApi() == GxApi_D3d9 || g_theGxDevicePtr->DeviceApi() == GxApi_D3d11) {
        maxSize = 0;
    }

    if (size < 0) {
        s_gxTexCacheMaxSize = 0;
        return;
    }

    s_gxTexCacheMaxSize = maxSize;

    if (size <= static_cast<int32_t>(maxSize)) {
        s_gxTexCacheMaxSize = size;
    }
}

// OFFSET: 0x00543770/0x1401970B0
void TextureNotifyGxRestart() {
    for (CTextureAtlas* atlas = s_atlasList.Head(); atlas; atlas = atlas->m_link.Next()) {
        atlas->Reload();
    }
}

// OFFSET: 0x00541AE0/0x140194620
int32_t TextureGetDimensions(CTexture* texture, uint32_t* width, uint32_t* height, int32_t wait) {
    if (texture->m_asyncObject) {
        if (!wait) {
            return 0;
        }

        DebugLog("TextureGetDimensions() Wait: %s\n", texture->m_filename);
        AsyncTextureWait(texture);
    }

    if (width) {
        *width = texture->m_gxWidth;
    }

    if (height) {
        *height = texture->m_gxHeight;
    }

    return 1;
}

// OFFSET: 0x00543800/0x140197170
CGxTex* TextureForceLoad(CTexture* texture) {
    if (texture->m_flags & 4) {
        AsyncTextureWait(texture);

        CTextureAtlas* atlas = texture->m_atlas;

        if (atlas) {
            if (atlas->m_dirty & 1) {
                atlas->Reload();
            }

            return texture->m_atlas->m_gxTex;
        }
    }

    AsyncTextureWait(texture);

    return texture->m_gxTex;
}

// OFFSET: 0x00543880/0x140197200
CGxTex* TextureForceLoadForDiskOnly(CTexture* texture) {
    CAsyncObject* object = texture->m_asyncObject;

    if (!object->m_buffer) {
        int32_t local = SFile::FileIsLocal(object->m_file, 0, 0);

        if (!local || local == 2) {
            return NULL;
        }
    }

    if (texture->m_asyncObject->m_isStreamed) {
        return NULL;
    }

    return TextureForceLoad(texture);
}

// OFFSET: 0x00541C50/0x140194760
CTextureAtlas::CTextureAtlas() {
    this->m_slots = 64;
    this->m_count = 0;
    this->m_freeSlot = -1;
    this->m_updateSlot = -1;
    this->m_width = 0;
    this->m_height = 0;
    this->m_capacity = 0;
    this->m_used = 0;
    this->m_gxTex = NULL;
    this->m_dirty = 0;
    memset(this->m_textures, 0, sizeof(this->m_textures));
}

// OFFSET: 0x00541CD0/0x1401948A0
CGxTex* TextureAllocGxTex(EGxTexTarget target, uint32_t width, uint32_t height, uint32_t depth, EGxTexFormat format, CGxTexFlags flags, void* userArg, GxTexUserFunc userFunc, EGxTexFormat dataFormat, const char* name) {
    CGxTexParms parms;
    parms.m_target = target;
    parms.m_width = width;
    parms.m_height = height;
    parms.m_depth = depth;
    parms.m_format = format;
    parms.m_dataFormat = dataFormat;
    parms.m_flags = flags;
    parms.m_userArg = userArg;
    parms.m_userFunc = userFunc;

    if (GxTexReusable(parms) && width - 32 <= 480 && height - 32 <= 480) {
        uint32_t w = width >> 5;
        uint32_t h = height >> 5;
        uint32_t x = 0;
        uint32_t y = 0;

        if (!(width & 0x20)) {
            do {
                w >>= 1;
                x++;
            } while (!(w & 1));
        }

        if (!(height & 0x20)) {
            do {
                h >>= 1;
                y++;
            } while (!(h & 1));
        }

        CGxTexParms cached;
        TSExplicitList<CGxTexCache, offsetof(CGxTexCache, m_link)>& list = s_gxTexCache[x][y];

        for (CGxTexCache* entry = list.Head(); entry; entry = list.Next(entry)) {
            GxTexParameters(entry->m_gxTex, cached);

            if (parms.m_flags.m_filter == cached.m_flags.m_filter && cached.m_format == parms.m_format && cached.m_target == parms.m_target && cached.m_depth == parms.m_depth) {
                s_gxTexCacheSize -= entry->m_size;
                entry->m_link.Unlink();
                CGxTex* gxTex = entry->m_gxTex;
                entry->m_gxTex = NULL;
                s_gxTexCacheFree.LinkToHead(entry);
                GxTexSetDataFormat(gxTex, parms.m_dataFormat);
                GxTexSetUserData(gxTex, parms.m_userFunc, parms.m_userArg);
                GxTexSetFlags(gxTex, parms.m_flags);

                return gxTex;
            }
        }
    }

    CGxTex* gxTex = NULL;
    GxTexCreate(parms, gxTex);

    return gxTex;
}

// OFFSET: 0x00543C10/0x140197760
void FillInSolidTexture(const CImVector& color, CTexture* texture) {
    texture->m_flags = 0;

    if (texture->m_gxTex) {
        TextureFreeGxTex(texture->m_gxTex, texture->m_filename);
        texture->m_gxTex = NULL;
    }

    if (texture->m_atlas) {
        CTextureAtlas::Free(texture->m_atlas, texture);
        texture->m_atlas = NULL;
    }

    texture->m_gxTex = TextureAllocGxTex(GxTex_2d, 8, 8, 0, GxTex_Argb8888, CGxTexFlags(), reinterpret_cast<void*>(static_cast<uintptr_t>(*reinterpret_cast<const uint32_t*>(&color))), GxuUpdateSingleColorTexture, GxTex_Argb8888, texture->m_filename);

    if (color.a < 0xFE) {
        texture->m_flags &= ~1;
    } else {
        texture->m_flags |= 1;
    }

    texture->m_gxHeight = 8;
    texture->m_dataFormat = GxTex_Argb8888;
    texture->m_gxTexTarget = GxTex_2d;
    texture->m_gxWidth = 8;
    texture->m_gxTexFormat = GxTex_Argb8888;
    texture->m_gxTexFlags = CGxTexFlags();
    texture->m_colorEncoding = 3;
}

// OFFSET: 0x00541E60
void AsyncTextureHandler() {
    uint32_t available = 0x300000 - s_asyncLoadBufferUsed;

    for (CAsyncObject* object = s_asyncTextureList.Head(), * next; !s_asyncTextureList.IsEnd(object); object = next) {
        next = s_asyncTextureList.RawNext(object);

        if (available >= object->m_size) {
            AsyncTextureProcess(object, 0);
            available -= object->m_size;
        }
    }
}

// OFFSET: 0x00541F00/0x140194B70
void TextureCacheUpdate() {
    int32_t destroyed = 0;
    int32_t pressure;

    if (!s_gxTexCacheMaxSize) {
        pressure = 1;
    } else if (s_gxTexCacheSize > 2 * s_gxTexCacheMaxSize) {
        pressure = 2;
    } else if (s_gxTexCacheSize > s_gxTexCacheMaxSize) {
        pressure = 1;
    } else {
        return;
    }

    int32_t maxDestroyed = pressure > 1 ? 32 : 16;

    for (int32_t x = 0; x < 6 && s_gxTexCacheSize > s_gxTexCacheMaxSize; x++) {
        for (int32_t y = 0; y < 6 && s_gxTexCacheSize > s_gxTexCacheMaxSize; y++) {
            CGxTexCache* entry = s_gxTexCache[x][y].Head();

            while (entry && destroyed < maxDestroyed && s_gxTexCacheSize > s_gxTexCacheMaxSize) {
                s_gxTexCacheSize -= entry->m_size;
                GxTexDestroy(entry->m_gxTex);
                destroyed++;
                entry->m_gxTex = NULL;
                entry->m_size = 0;
                entry->m_time = 0xFFFFFFFF;

                CGxTexCache* next = entry->m_link.Next();
                entry->m_link.Unlink();
                s_gxTexCacheFree.LinkToHead(entry);
                entry = next;
            }
        }
    }
}

// OFFSET: 0x00541AA0/0x1401945D0
void TextureIncreasePriority(CTexture* texture) {
    CAsyncObject* object = texture->m_asyncObject;

    if (!object) {
        return;
    }

    if (object->m_buffer) {
        AsyncFileReadEnterQueueLock();
        AsyncFileReadLinkToPriority(texture->m_asyncObject, 1);
        AsyncFileReadLeaveQueueLock();
    } else {
        AsyncTextureProcess(object, 1);
    }
}

// OFFSET: 0x005437A0/0x1401970F0
CGxTex* TextureGetGxTex(CTexture* texture) {
    if (!texture) {
        DebugValidate("texture");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (!(texture->m_flags & 4)) {
        return texture->m_gxTex;
    }

    if (texture->m_asyncObject) {
        TextureIncreasePriority(texture);
        return NULL;
    }

    CTextureAtlas* atlas = texture->m_atlas;

    if (!atlas) {
        return texture->m_gxTex;
    }

    if (atlas->m_dirty & 1) {
        atlas->Reload();
    }

    return texture->m_atlas->m_gxTex;
}

// PROVEN: 983e494a IDENTICAL/UNPROVEN
// OFFSET: 0x005444E0/0x140198510
CGxTex* TextureGetGxTexBlocking(CTexture* texture) {
    CGxTex* gxTex = TextureGetGxTex(texture);

    if (gxTex) {
        return gxTex;
    }

    return TextureForceLoad(texture);
}

// OFFSET: 0x00542180/0x140194DC0
CTextureAtlas* CTextureAtlas::Alloc(uint32_t width, uint32_t height, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags) {
    CTextureAtlas* atlas = new (__FILE__, __LINE__) CTextureAtlas;
    atlas->m_height = height;
    atlas->m_width = width;
    atlas->m_used = 0;
    atlas->m_freeSlot = -1;
    atlas->m_capacity = 64;
    atlas->m_gxTex = TextureAllocGxTex(GxTex_2d, 8 * width, 8 * height, 0, format, flags, atlas, ResolveMipLevel, dataFormat, "Atlas");
    atlas->m_dataFormat = dataFormat;
    atlas->m_format = format;
    atlas->m_texFlags = flags;

    return atlas;
}

// OFFSET: 0x00542220
CTexture* TextureCacheGetTexture(const char* fileName) {
    uint32_t hashval = SStrHashHT(fileName);
    HASHKEY_TEXTUREFILE key;
    key.m_anyTexFlags = 1;
    key.m_filename = const_cast<char*>(fileName);
    CTexture* texture = s_textureCache.Ptr(hashval, key);

    if (texture) {
        texture->m_refcount++;
    }

    return texture;
}

// OFFSET: 0x00542270
CTexture* TextureCacheGetTexture(char* fileName, char* extension, CGxTexFlags texFlags) {
    if (extension) {
        *extension = '\0';
    }

    uint32_t hashval = SStrHashHT(fileName);
    HASHKEY_TEXTUREFILE key;
    key.m_anyTexFlags = 0;
    key.m_filename = fileName;
    key.m_texFlags = texFlags;
    CTexture* texture = s_textureCache.Ptr(hashval, key);

    if (extension) {
        *extension = '.';
    }

    if (texture) {
        texture->m_refcount++;
    }

    return texture;
}

// OFFSET: 0x005422D0/0x1401950F0
void TextureFreeGxTex(CGxTex* gxTex, const char* name) {
    CGxTexParms parms;
    GxTexParameters(gxTex, parms);

    if (GxTexReusable(gxTex) && parms.m_width - 32 <= 480 && parms.m_height - 32 <= 480) {
        uint32_t w = parms.m_width >> 5;
        uint32_t h = parms.m_height >> 5;
        uint32_t x = 0;
        uint32_t y = 0;

        if (!(parms.m_width & 0x20)) {
            do {
                w >>= 1;
                x++;
            } while (!(w & 1));
        }

        if (!(parms.m_height & 0x20)) {
            do {
                h >>= 1;
                y++;
            } while (!(h & 1));
        }

        CGxTexCache* entry = s_gxTexCacheFree.Head();

        if (!entry) {
            entry = s_gxTexCacheFree.NewNode(1, 0, 0);
        }

        entry->m_link.Unlink();
        entry->m_gxTex = gxTex;
        entry->m_time = s_currentTime;
        entry->m_size = TextureCalcGxTexSize(parms.m_format, parms.m_width, parms.m_height, static_cast<EGxTexFilter>(parms.m_flags.m_filter));
        s_gxTexCache[x][y].LinkToTail(entry);
        s_gxTexCacheSize += entry->m_size;
        GxTexSetUserData(entry->m_gxTex, GxTexNullUserFunc, NULL);
    } else {
        TextureCalcGxTexSize(parms.m_format, parms.m_width, parms.m_height, static_cast<EGxTexFilter>(parms.m_flags.m_filter));
        GxTexDestroy(gxTex);
    }
}

// PROVEN: f2601043 IDENTICAL/UNPROVEN
// OFFSET: 0x00542420/0x140195370
int32_t TextureUpdate(const void* data, void* param) {
    s_currentTime = OsGetAsyncTimeMs();
    TextureCacheUpdate();

    return 1;
}

// OFFSET: 0x00542440/0x140195390
MipBits* TextureAllocMippedImg(PIXEL_FORMAT format, uint32_t width, uint32_t height) {
    int32_t cache = s_pixelFormatToMipBitsCache[format];

    if (width < 8 || height < 8 || width > 256 || height > 256 || cache == -1) {
        return MippedImgAllocA(format, width, height, __FILE__, __LINE__);
    }

    uint32_t w = width >> 3;
    uint32_t h = height >> 3;
    uint32_t x = 0;
    uint32_t y = 0;

    if (!(width & 8)) {
        do {
            w >>= 1;
            x++;
        } while (!(w & 1));
    }

    if (!(height & 8)) {
        do {
            h >>= 1;
            y++;
        } while (!(h & 1));
    }

    CMipBitsCache* entry = s_mipBitsCache[x][y][cache].Head();

    if (!entry) {
        return MippedImgAllocA(format, width, height, __FILE__, __LINE__);
    }

    entry->m_link.Unlink();
    MipBits* bits = entry->m_bits;
    entry->m_bits = NULL;
    s_mipBitsCacheFree.LinkToTail(entry);

    return bits;
}

// OFFSET: 0x00542520/0x140195510
void TextureFreeMippedImg(MipBits* bits, PIXEL_FORMAT format, uint32_t width, uint32_t height) {
    int32_t cache = s_pixelFormatToMipBitsCache[format];

    if (width < 8 || height < 8 || width > 256 || height > 256 || cache == -1) {
        if (bits) {
            SMemFree(bits, __FILE__, __LINE__, 0);
        }

        return;
    }

    uint32_t w = width >> 3;
    uint32_t h = height >> 3;
    uint32_t x = 0;
    uint32_t y = 0;

    if (!(width & 8)) {
        do {
            w >>= 1;
            x++;
        } while (!(w & 1));
    }

    if (!(height & 8)) {
        do {
            h >>= 1;
            y++;
        } while (!(h & 1));
    }

    CMipBitsCache* entry = s_mipBitsCacheFree.Head();

    if (!entry) {
        entry = s_mipBitsCacheFree.NewNode(2, 0, 0);
        entry->m_link.Unlink();
    }

    entry->m_bits = bits;
    s_mipBitsCache[x][y][cache].LinkToTail(entry);
}

// OFFSET: 0x00542920/0x140195B00
CTextureAtlas::~CTextureAtlas() {
    if (this->m_gxTex) {
        TextureFreeGxTex(this->m_gxTex, "ATLAS");
    }

    this->m_link.Unlink();
}

// OFFSET: 0x005429B0
CTextureAtlas* CTextureAtlas::Get(CTexture* texture) {
    uint16_t width = texture->m_gxWidth;
    uint16_t height = texture->m_gxHeight;

    if (width != height || width > 64 || height > 64) {
        return NULL;
    }

    CGxTexFlags texFlags = texture->m_gxTexFlags;
    texFlags.m_bit15 = 1;
    texFlags.m_bit20 = 1;
    CTextureAtlas* best = NULL;

    for (CTextureAtlas* atlas = s_atlasList.Head(); atlas; atlas = atlas->m_link.Next()) {
        if (atlas->m_format == texture->m_gxTexFormat && atlas->m_texFlags == texFlags && atlas->m_dataFormat == texture->m_dataFormat && atlas->m_width == width && atlas->m_height == height) {
            if (static_cast<int32_t>(atlas->m_used) < static_cast<int32_t>(atlas->m_capacity) && (!best || static_cast<int32_t>(atlas->m_used) > static_cast<int32_t>(best->m_used))) {
                best = atlas;
            }
        }
    }

    CTextureAtlas* atlas = best;

    if (!atlas) {
        atlas = CTextureAtlas::Alloc(width, height, texture->m_gxTexFormat, texture->m_dataFormat, texFlags);
        s_atlasList.LinkToTail(atlas);
    }

    texture->m_atlasBlockIndex = -1;

    if (atlas->m_freeSlot == -1) {
        for (uint32_t i = 0; i < 64; i++) {
            if (!atlas->m_textures[i]) {
                texture->m_atlasBlockIndex = i;
                break;
            }
        }
    } else {
        texture->m_atlasBlockIndex = atlas->m_freeSlot;
        atlas->m_freeSlot = -1;
    }

    atlas->m_used++;
    atlas->m_count++;
    atlas->m_textures[texture->m_atlasBlockIndex] = texture;

    return atlas;
}

// OFFSET: 0x00542AE0/0x140195DC0
int32_t LoadTgaMips(void* data, uint32_t size, const char* fileName, int32_t flags, MipBits** images, StaticImage* image) {
    CTgaFile tga;

    if (!tga.SetToData(static_cast<uint8_t*>(data), size) || !tga.LoadImageData(3)) {
        return 0;
    }

    tga.SetTopDown(1);

    uint32_t height = tga.Height();
    uint32_t width = tga.Width();
    uint32_t mipCount = TextureCalcMipCount(width, height);

    if (*images) {
        MippedImgSet(PIXEL_ARGB8888, width, height, *images);
    } else {
        *images = TextureAllocMippedImg(PIXEL_ARGB8888, width, height);
    }

    const TGA32Pixel* source = tga.ImageBGRA32Pixel();
    C4Pixel* dest = (*images)->m_mip[0];

    for (uint32_t pixels = width * height; pixels; pixels--) {
        dest->m_b = source->m_b;
        dest->m_g = source->m_g;
        dest->m_r = source->m_r;
        dest->m_a = source->m_a;
        dest++;
        source++;
    }

    tga.Close();

    char mipName[260];
    TextureBuildMipFormat(mipName, fileName);
    uint32_t predrawn = LoadPredrawnMips(&tga, mipName, flags, *images);
    TextureGenerateMips(tga.Width(), tga.Height(), predrawn, mipCount, *images);

    if (image) {
        image->Set(*images, width, height, mipCount, COLOR_ARGB8888, PIXEL_ARGB8888, static_cast<ALPHA_FORMAT>(tga.AlphaBits()));
    }

    return 1;
}

// OFFSET: 0x00542C80
int32_t TextureReloadMips(CTexture* texture, MipBits** images) {
    char buffer[260];
    char* fileName = texture->m_filename;

    if (TextureBuildAliasPath(buffer, texture->m_filename)) {
        fileName = buffer;
    }

    char* extension = &fileName[strlen(fileName)];
    int32_t type = (texture->m_flags >> 3) & 7;
    int32_t flags = texture->m_flags & 2;
    int32_t result = 0;
    TextureAppendExtension(extension, type, fileName - extension + 260);

    uint32_t size;
    void* data = TextureLoadFile(fileName, flags, &size);

    if (data) {
        if (type == 0) {
            result = LoadTgaMips(data, size, fileName, flags, images, NULL);
        } else if (type == 1) {
            result = TextureLoadBlpMips(data, size, images);
        }

        SMemFree(data, __FILE__, __LINE__, 0);
    }

    *extension = '\0';

    return result;
}

// OFFSET: 0x00542DC0
int32_t LoadBlpMips(void* data, uint32_t size, MipBits** images, StaticImage* image, PIXEL_FORMAT format) {
    if (!image->SetToBLPData(static_cast<uint8_t*>(data), size)) {
        return 0;
    }

    uint32_t width = image->m_width;

    if (format == PIXEL_UNSPECIFIED) {
        format = image->m_pixelFormat;
    }

    if (*images) {
        MippedImgSet(format, width, image->m_height, *images);
    } else {
        *images = TextureAllocMippedImg(format, width, image->m_height);
    }

    return image->Convert(format, *images, 0);
}

// OFFSET: 0x00543170/0x1401968C0
void UpdateTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    CTexture* texture = static_cast<CTexture*>(userArg);

    switch (command) {
        case GxTex_Lock:
            if (!s_pumping && !TextureReloadMips(texture, &s_mipBits)) {
                GetDefaultTexture(width, height);
                GetGlobalStatusObj().Add(STATUS_ERROR, "Texture %s not loaded -- replaced with default.\n", texture->m_filename);
            }

            break;

        case GxTex_Latch: {
            texelStrideInBytes = GxCalcTexelStrideInBytes(texture->m_dataFormat, width);

            const uint8_t* bits = reinterpret_cast<const uint8_t*>(s_mipBits->m_mip[level]);
            texels = bits;

            if (texture->m_gxTexTarget == GxTex_CubeMap) {
                uint32_t order = face;

                switch (face) {
                    case 0:
                        order = 0;
                        break;

                    case 1:
                        order = 2;
                        break;

                    case 2:
                        order = 4;
                        break;

                    case 3:
                        order = 5;
                        break;

                    case 4:
                        order = 3;
                        break;

                    case 5:
                        order = 1;
                        break;
                }

                texels = bits + order * texelStrideInBytes;
                texelStrideInBytes *= 6;
            }

            break;
        }

        default:
            break;
    }
}

// OFFSET: 0x00543280/0x140196A00
static int32_t PumpTexture(StaticImage* image, CTexture* texture, CStatus* status) {
    if ((texture->m_flags & 4) && !(image->m_isAtlasable & 1)) {
        texture->m_flags &= ~4;
    }

    texture->m_colorEncoding = image->m_colorFormat;
    texture->m_alphaBits = image->m_alphaFormat;

    if (!texture->m_alphaBits) {
        texture->m_flags |= 1;
    }

    uint32_t bestMip = 0;
    uint32_t width = image->m_width;
    uint32_t height = image->m_height;
    RequestImageDimensions(&width, &height, &bestMip);
    texture->m_bestMip = bestMip;

    if (width < 8 || height < 8) {
        if (status) {
            status->Add(STATUS_FATAL, "Error loading file \"%s\": Texture size must be at least %dx%d\n", texture->m_filename, 8, 8);
        }

        return 0;
    }

    EGxTexTarget target = GxTex_2d;

    if (width == 6 * height) {
        target = GxTex_CubeMap;
        width /= 6;
    }

    if (((width - 1) & width) || ((height - 1) & height)) {
        if (!(texture->m_flags & 0x100)) {
            if (status) {
                status->Add(STATUS_FATAL, "Error loading file \"%s\": Texture size (%dx%d) must be powers of two\n", texture->m_filename, width, height);
            }

            return 0;
        }

        target = GxTex_NonPow2;
    }

    PIXEL_FORMAT pixelFormat;
    EGxTexFormat gxTexFormat;
    TextureGetTextureFormats(pixelFormat, gxTexFormat, image->m_pixelFormat, image->m_alphaFormat & 0x3F);
    s_pumping = 1;

    int32_t inPlace;

    if (image->m_colorFormat == COLOR_DXT) {
        inPlace = pixelFormat == PIXEL_DXT1 || pixelFormat == PIXEL_DXT3 || pixelFormat == PIXEL_DXT5;
    } else {
        inPlace = image->m_colorFormat == COLOR_ARGB8888 || image->m_colorFormat == COLOR_ARGB8888_2;
    }

    if (inPlace) {
        for (uint32_t i = bestMip, j = 0; i < image->m_mipCount; i++, j++) {
            s_mipBits->m_mip[j] = reinterpret_cast<C4Pixel*>(image->m_mips[i]);
        }
    } else if (!image->Convert(pixelFormat, s_mipBits, bestMip)) {
        s_pumping = 0;
        texture->m_status.Add(STATUS_FATAL, "Texture failure: \"%s\" conversion failed.\n", texture->m_filename);
        return 0;
    }

    texture->m_dataFormat = s_pixelFormatToGxTexFormat[pixelFormat];
    texture->m_gxTexFormat = gxTexFormat;
    texture->m_gxTexTarget = target;
    texture->m_gxWidth = width;
    texture->m_gxHeight = height;

    uint32_t filter = texture->m_gxTexFlags.m_filter;

    if (filter != GxTex_Nearest && filter != GxTex_Linear && image->m_mipCount == 1) {
        texture->m_gxTexFlags.m_filter = filter == GxTex_NearestMipNearest ? GxTex_Nearest : GxTex_Linear;
    }

    if (texture->m_flags & 4) {
        texture->m_atlas = CTextureAtlas::Get(texture);

        if (texture->m_atlas) {
            texture->m_atlas->Update(texture);
        } else {
            texture->m_flags &= ~4;
        }
    }

    if (!texture->m_atlas) {
        if (texture->m_gxTex) {
            TextureFreeGxTex(texture->m_gxTex, texture->m_filename);
            texture->m_gxTex = NULL;
        }

        texture->m_gxTex = TextureAllocGxTex(texture->m_gxTexTarget, texture->m_gxWidth, texture->m_gxHeight, 0, texture->m_gxTexFormat, texture->m_gxTexFlags, texture, UpdateTexture, texture->m_dataFormat, texture->m_filename);

        if (!texture->m_gxTex) {
            s_pumping = 0;
            texture->m_status.Add(STATUS_FATAL, "Texture failure: \"%s\" allocating %dx%d texture failed.\n", texture->m_filename, width, height);
            return 0;
        }

        GxTexUpdate(texture->m_gxTex, 0, 0, width, height, 1);
    }

    s_pumping = 0;

    return 1;
}

// OFFSET: 0x00543580/0x140196E30
static int32_t PumpTextureFromData(void* data, CTexture* texture, uint32_t size) {
    int32_t type = (texture->m_flags >> 3) & 7;

    if (type == 0) {
        MipBits* images = NULL;
        StaticImage image;

        if (!LoadTgaMips(data, size, texture->m_filename, texture->m_flags & 2, &images, &image)) {
            return 0;
        }

        int32_t result = PumpTexture(&image, texture, &texture->m_status);

        if (images) {
            SMemFree(images, __FILE__, __LINE__, 0);
        }

        return result;
    }

    if (type != 1) {
        return 0;
    }

    StaticImage image;

    if (image.SetToBLPData(static_cast<uint8_t*>(data), size)) {
        return PumpTexture(&image, texture, &texture->m_status);
    }

    const BLPHeader* header = static_cast<const BLPHeader*>(data);
    texture->m_status.Add(STATUS_FATAL, "BLP Texture failure: \"%s\" invalid file version (size = %d, magic = 0x%08X, version = %d, width = %d, height = %d)\n", texture->m_filename, size, header->m_magic, header->m_formatVersion, header->m_width, header->m_height);

    for (uint32_t i = 0; i < 16; i++) {
        texture->m_status.Add(STATUS_FATAL, "BLP Texture failure: (Mip Level %d - Offset %d, Size %d)\n", i, header->m_mipOffsets[i], header->m_mipSizes[i]);
    }

    return 0;
}

// OFFSET: 0x00543CD0/0x140197880
void AsyncTextureSuccessCallback(CAsyncObject* object) {
    CTexture* texture = static_cast<CTexture*>(object->m_userArg);

    if (!PumpTextureFromData(object->m_buffer, texture, object->m_size)) {
        if (!texture->m_status.IsEmpty()) {
            SysMsgAdd(&texture->m_status);
        }

        if (!(texture->m_flags & 0x80)) {
            FillInSolidTexture(*reinterpret_cast<const CImVector*>(&s_defaultColor), texture);
        }
    }

    s_asyncLoadBufferUsed -= object->m_size;
    SMemFree(object->m_buffer, __FILE__, __LINE__, 0);
    AsyncFileReadDestroyObject(object);
    texture->m_asyncObject = NULL;
}

// OFFSET: 0x00541EC0
int32_t AsyncTextureStatus() {
    int32_t count = 0;

    for (CAsyncObject* object = s_asyncTextureList.Head(); !s_asyncTextureList.IsEnd(object); object = s_asyncTextureList.RawNext(object)) {
        count++;
    }

    return count;
}

// OFFSET: 0x00542D50/0x1401962B0
void TextureGxCacheClear() {
    for (int32_t x = 0; x < 6; x++) {
        for (int32_t y = 0; y < 6; y++) {
            for (CGxTexCache* entry = s_gxTexCache[x][y].Head(); !s_gxTexCache[x][y].IsEnd(entry); entry = entry->m_link.RawNext()) {
                s_gxTexCacheSize -= entry->m_size;
            }

            s_gxTexCache[x][y].Clear();
        }
    }
}

// OFFSET: 0x00542DB0/0x140196350
void TextureGxCacheFlush() {
    TextureGxCacheClear();
    s_gxTexCacheFree.Clear();
}

// OFFSET: 0x00542E30
MipBits* TextureLoadImage(const char* fileName, uint32_t* width, uint32_t* height, PIXEL_FORMAT* dataFormat, int32_t* opaque, CStatus* status, uint32_t* alphaBits, int32_t flags) {
    if (!fileName) {
        DebugValidate("filename");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (!width) {
        DebugValidate("width");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (!height) {
        DebugValidate("height");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (!dataFormat) {
        DebugValidate("dataFormat");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    char name[260];
    StaticImage image;
    MipBits* images = NULL;
    SStrCopy(name, fileName, sizeof(name));
    char* extension = OsPathFindExtensionWithDot(name);
    *extension = '\0';

    int32_t type = 1;

    for (uint32_t tries = 0; ; ) {
        PIXEL_FORMAT format = *dataFormat;
        TextureAppendExtension(extension, type, name + sizeof(name) - extension);

        uint32_t size;
        void* data = TextureLoadFile(name, flags, &size);

        if (data) {
            if (type == 0) {
                LoadTgaMips(data, size, name, flags, &images, &image);
            } else if (type == 1) {
                LoadBlpMips(data, size, &images, &image, format);
            }

            SMemFree(data, __FILE__, __LINE__, 0);

            if (images) {
                break;
            }
        }

        type = (type + 1) % 2;

        if (++tries >= 2) {
            if (status) {
                status->Add(STATUS_FATAL, "Error loading texure file \"%s\": unsupported image format\n", fileName);
            }

            return images;
        }
    }

    *width = image.m_width;
    *height = image.m_height;

    if (opaque) {
        *opaque = image.m_alphaFormat == 0;
    }

    if (alphaBits) {
        *alphaBits = image.m_alphaFormat & 0x3F;
    }

    return images;
}

// OFFSET: 0x005436B0/0x140196FB0
void TextureDestroy() {
    CTextureBlob::UnloadAll();
    TextureGxCacheClear();
    s_gxTexCacheFree.Clear();
    SMemFree(s_mipBits, __FILE__, __LINE__, 0);
    s_mipBits = NULL;

    for (int32_t x = 0; x < 6; x++) {
        for (int32_t y = 0; y < 6; y++) {
            s_mipBitsCache[x][y][0].Clear();
            s_mipBitsCache[x][y][1].Clear();
        }
    }

    s_mipBitsCacheFree.Clear();

    for (CTexture* texture = s_textureList.Head(); !s_textureList.IsEnd(texture); texture = s_textureList.RawNext(texture)) {
        DebugLog("TextureDestroy() found unfreed texture: %s\n", texture->m_filename);
    }

    EventUnregisterEx(EVENT_ID_POLL, TextureUpdate, NULL, 0xFFFFFFFF);
}

// OFFSET: 0x00543B10/0x140197580
void CTextureAtlas::Free(CTextureAtlas* atlas, CTexture* texture) {
    atlas->m_textures[texture->m_atlasBlockIndex] = NULL;
    atlas->m_count--;
    atlas->m_used--;
    atlas->m_freeSlot = texture->m_atlasBlockIndex;

    if (static_cast<int16_t>(atlas->m_count) < 1) {
        atlas->~CTextureAtlas();
        SMemFree(atlas, "delete", SLOG_FUNCTION, 0);
    }

    texture->m_atlasBlockIndex = -1;
}

// OFFSET: 0x00543B60/0x140197600
CTexture::CTexture() {
    this->m_refcount = 1;
    this->m_flags = 0;
    this->m_bestMip = 0;
    this->m_alphaBits = 0;
    this->m_gxWidth = 0;
    this->m_asyncObject = NULL;
    this->m_gxTex = NULL;
    this->m_gxTexTarget = GxTex_2d;
    this->m_gxHeight = 0;
    this->m_gxTexFormat = GxTex_Unknown;
    this->m_dataFormat = GxTex_Unknown;
    this->m_atlas = NULL;
    this->m_atlasBlockIndex = 0;
    this->m_filename[0] = '\0';
    s_textureList.LinkToTail(this);
}

// PROVEN: 5bfcc2da IDENTICAL/IDENTICAL
// OFFSET: 0x00543C00/0x140197750
const char* CTexture::GetObjectName() {
    return this->m_filename;
}

// OFFSET: 0x005407D0/0x1401932E0
static void TextureCancelAsyncLoad(CTexture* texture) {
    CAsyncObject* object = texture->m_asyncObject;

    if (!object) {
        return;
    }

    if (texture->m_flags & 0x40) {
        object->m_userArg = NULL;
        texture->m_asyncObject->m_userPostUnloadCallback = AsyncTextureDestroyedCallback;
        texture->m_asyncObject->m_userPostloadCallback = texture->m_asyncObject->m_userPostUnloadCallback;
        texture->m_asyncObject = NULL;
    } else {
        uint32_t size = object->m_size;
        void* buffer = object->m_buffer;

        if (AsyncFileReadCancel(object, AsyncTextureDestroyedCallback) && buffer) {
            s_asyncLoadBufferUsed -= size;
            SMemFree(buffer, __FILE__, __LINE__, 0);
        }

        texture->m_asyncObject = NULL;
    }
}

// OFFSET: [0x00544860/0x140198A60, 0x00544AA0/0x140198E10]
CTexture::~CTexture() {
    this->m_flags = 0;

    if (this->m_gxTex) {
        TextureFreeGxTex(this->m_gxTex, this->m_filename);
        this->m_gxTex = NULL;
    }

    if (this->m_atlas) {
        CTextureAtlas::Free(this->m_atlas, this);
        this->m_atlas = NULL;
    }

    TextureCancelAsyncLoad(this);
    this->m_link.Unlink();
}

// OFFSET: 0x00543DB0
static int32_t CreateTextureSync(char* extension, CTexture* texture, const char* fileName, int32_t type, uint32_t size, uint32_t flags, CGxTexFlags texFlags) {
    TextureAppendExtension(extension, type, fileName + size - extension);

    int32_t localFlag = flags & 2;
    uint32_t fileSize;
    void* data = TextureLoadFile(fileName, localFlag, &fileSize);

    if (!data) {
        return 0;
    }

    texture->m_gxTexFlags = texFlags;
    texture->m_flags = type << 3;

    if (localFlag) {
        texture->m_flags = (type << 3) | 2;
    }

    if ((flags & 4) && s_atlasEnable) {
        texture->m_flags |= 4;
    }

    if (flags & 0x20) {
        texture->m_flags |= 0x100;
    }

    if (!PumpTextureFromData(data, texture, fileSize) && !(texture->m_flags & 0x80)) {
        FillInSolidTexture(*reinterpret_cast<const CImVector*>(&s_defaultColor), texture);
    }

    SMemFree(data, __FILE__, __LINE__, 0);

    return 1;
}

// OFFSET: 0x00543E70
static int32_t CreateTextureAsync(const char* fileName, char* extension, CTexture* texture, int32_t type, uint32_t size, uint32_t flags, CGxTexFlags texFlags, uint8_t priority) {
    TextureAppendExtension(extension, type, fileName + size - extension);

    SFileImpl* file = NULL;
    SErrSetLastError(0);

    if (!SFile::OpenEx(NULL, fileName, (flags >> 1) & 1, &file)) {
        if (!SErrGetLastError()) {
            SErrSetLastError(STORM_ERROR_FILE_NOT_FOUND);
        }

        return 0;
    }

    if (!file) {
        return 0;
    }

    if (extension) {
        *extension = '\0';
    }

    CTextureBlobTexture* blobTexture = NULL;

    if (!(flags & 4) && !(texture->m_flags & 0x80)) {
        blobTexture = CTextureBlob::GetTexture(fileName);
    }

    texture->m_gxTexFlags = texFlags;

    if (flags & 2) {
        texture->m_flags |= 2;
    }

    if ((flags & 4) && s_atlasEnable) {
        texture->m_flags |= 4;
    }

    if ((flags & 0x10) && SFile::IsStreamingMode()) {
        texture->m_flags |= 0x40;
    }

    if (flags & 0x20) {
        texture->m_flags |= 0x100;
    }

    texture->m_flags |= type << 3;

    if (blobTexture) {
        CTextureBlob::CreateTexture(texture, blobTexture);
    }

    if (texture->m_asyncObject) {
        TextureCancelAsyncLoad(texture);
    }

    texture->m_asyncObject = AsyncFileReadAllocObject();
    texture->m_asyncObject->m_userArg = texture;
    texture->m_asyncObject->m_userPostloadCallback = AsyncTextureSuccessCallback;
    texture->m_asyncObject->m_userPostUnloadCallback = AsyncTextureFailureCallback;
    texture->m_asyncObject->m_file = file;
    texture->m_asyncObject->m_size = SFileGetFileSize(file, NULL);
    texture->m_asyncObject->m_priority = priority;

    CAsyncObject* object = texture->m_asyncObject;

    if (object->m_priority == 2 || 0x300000 - s_asyncLoadBufferUsed >= object->m_size) {
        AsyncTextureProcess(object, 0);
    } else {
        s_asyncTextureList.LinkNode(object, 2, NULL);
    }

    return 1;
}

// OFFSET: 0x00544010
static int32_t CreateTextureOfType(char* extension, const char* fileName, int32_t type, CTexture* texture, uint32_t size, uint32_t flags, CGxTexFlags texFlags, uint8_t priority) {
    char buffer[260];

    if (TextureBuildAliasPath(buffer, fileName)) {
        fileName = buffer;
        extension = &buffer[strlen(buffer)];
    }

    if (s_createBlpAsync) {
        return CreateTextureAsync(fileName, extension, texture, type, size, flags, texFlags, priority);
    }

    return CreateTextureSync(extension, texture, fileName, type, size, flags, texFlags);
}

// OFFSET: 0x005440A0/0x140197DF0
static void TextureReload(CTexture* texture) {
    CGxTexFlags texFlags = texture->m_gxTexFlags;
    texture->m_flags |= 0x80;

    CStatus status;
    char name[260];
    SStrCopy(name, texture->m_filename, sizeof(name));
    char* extension = OsPathFindExtensionWithDot(name);

    if (extension) {
        *extension = '\0';
    }

    CGxTexFlags gxTexFlags;
    GxTexFlags(texture->m_gxTex, gxTexFlags);

    for (int32_t type = 0; type < 2; type++) {
        if (CreateTextureOfType(extension, name, type, texture, sizeof(name), 0, texFlags, 2)) {
            GxTexSetFlags(texture->m_gxTex, gxTexFlags);
            return;
        }
    }
}

// OFFSET: 0x005441A0/0x140197FD0
static void TextureFileChanged(const char* fileName) {
    DebugLog("Texture changed: %s\n", fileName);

    char name[260];
    SStrCopy(name, fileName, sizeof(name));
    char* extension = OsPathFindExtensionWithDot(name);

    if (extension) {
        *extension = '\0';
    }

    for (CTexture* texture = s_textureList.Head(); texture; ) {
        CTexture* next = s_textureList.Next(texture);
        const char* textureName = OsPathFindFilename(texture->m_filename);

        if (textureName && !_stricmp(textureName, name)) {
            TextureReload(texture);
        }

        texture = next;
    }
}

// OFFSET: 0x00544270/0x1401980C0
void TextureInitialize() {
    s_mipBits = static_cast<MipBits*>(SMemAlloc(MippedImgCalcSize(PIXEL_ARGB8888, 1024, 1024), __FILE__, __LINE__, 0));
    AsyncFileReadAddPollHandler(AsyncTextureHandler);
    AsyncFileReadAddStatusHandler(AsyncTextureStatus);
    s_currentTime = OsGetAsyncTimeMs();
    s_gxTexCacheSize = 0;
    s_gxTexCacheMaxSize = 0;
    EventRegisterEx(EVENT_ID_POLL, TextureUpdate, NULL, 0.0f);
    FileNotification::RegisterSingleFile("blp", TextureFileChanged);
    FileNotification::RegisterSingleFile("tga", TextureFileChanged);
    FileNotification::RegisterSingleFile("png", TextureFileChanged);
}

// OFFSET: 0x00544320/0x140198180
CTexture* TextureCreate(EGxTexTarget target, uint32_t width, uint32_t height, uint32_t depth, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags texFlags, void* userArg, GxTexUserFunc userFunc, const char* name, int32_t defaultFilter) {
    CTexture* texture = new (__FILE__, __LINE__) CTexture;
    SStrCopy(texture->m_filename, name ? name : "UniqueTexture", sizeof(texture->m_filename));
    texFlags = TextureMakeTexFlags(texFlags, defaultFilter);
    texture->m_gxTex = TextureAllocGxTex(target, width, height, depth, format, texFlags, userArg, userFunc, dataFormat, texture->m_filename);
    texture->m_dataFormat = dataFormat;
    texture->m_gxTexFormat = format;
    texture->m_gxHeight = height;
    texture->m_gxTexTarget = target;
    texture->m_gxWidth = width;
    texture->m_gxTexFlags = texFlags;
    texture->m_asyncObject = NULL;

    return texture;
}

// OFFSET: 0x005443F0/0x140198350
CTexture* TextureCreateBlob(char* fileName, CGxTexFlags texFlags, CStatus* status, int32_t defaultFilter) {
    if (!fileName) {
        DebugValidate("fileName");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (!fileName[0]) {
        DebugValidate("fileName[0]");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (!status) {
        DebugValidate("status");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    texFlags = TextureMakeTexFlags(texFlags, defaultFilter);
    char* extension = OsPathFindExtensionWithDot(fileName);

    if (extension) {
        *extension = '\0';
    }

    CTexture* texture = NULL;
    CTextureBlobTexture* blobTexture = CTextureBlob::GetTexture(fileName);

    if (blobTexture) {
        texture = new (__FILE__, __LINE__) CTexture;
        SStrCopy(texture->m_filename, fileName, sizeof(texture->m_filename));
        texture->m_gxTexFlags = texFlags;
        CTextureBlob::CreateTexture(texture, blobTexture);
    }

    if (extension) {
        *extension = '.';
    }

    return texture;
}

// PROVEN: e41d405d IDENTICAL/IDENTICAL
// OFFSET: 0x00544900/0x140198B40
CTexture* TextureCreate(uint32_t width, uint32_t height, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags texFlags, void* userArg, GxTexUserFunc userFunc, const char* name, int32_t defaultFilter) {
    return TextureCreate(GxTex_2d, width, height, 0, format, dataFormat, texFlags, userArg, userFunc, name, defaultFilter);
}

// OFFSET: 0x00544D80
void TextureCacheNewTexture(CTexture* texture) {
    uint32_t hashval = SStrHashHT(texture->m_filename);
    HASHKEY_TEXTUREFILE key;
    key.m_anyTexFlags = 0;
    key.m_texFlags = texture->m_gxTexFlags;
    key.m_filename = texture->m_filename;
    s_textureCache.Insert(texture, hashval, key);
}

// OFFSET: 0x00544B40
void TextureCacheNewTexture(CTexture* texture, CGxTexFlags texFlags) {
    uint32_t hashval = SStrHashHT(texture->m_filename);
    HASHKEY_TEXTUREFILE key;
    key.m_anyTexFlags = 0;
    key.m_filename = texture->m_filename;
    key.m_texFlags = texFlags;
    s_textureCache.Insert(texture, hashval, key);
}

// OFFSET: 0x00544DD0/0x1401993E0
CTexture* TextureCreateSolid(const CImVector& color) {
    char name[260];
    SStrPrintf(name, sizeof(name), "Color-%x%x%x%x", color.r, color.g, color.b, color.a);

    uint32_t hashval = SStrHashHT(name);
    HASHKEY_TEXTUREFILE key;
    key.m_anyTexFlags = 1;
    key.m_filename = name;
    CTexture* texture = s_textureCache.Ptr(hashval, key);

    if (texture) {
        texture->m_refcount++;
        return texture;
    }

    texture = new (__FILE__, __LINE__) CTexture;
    SStrCopy(texture->m_filename, name, sizeof(texture->m_filename));
    FillInSolidTexture(color, texture);
    TextureCacheNewTexture(texture);

    return texture;
}

// OFFSET: 0x00544B90/0x140199030
CTexture* TextureCreate(const char* fileName, CGxTexFlags texFlags, CStatus* status, uint32_t flags, const char* debugName, uint8_t priority) {
    if (!fileName) {
        DebugValidate("fileName");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (!fileName[0]) {
        DebugValidate("fileName[0]");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (!status) {
        DebugValidate("status");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    texFlags = TextureMakeTexFlags(texFlags, (flags & 1) == 0);

    int32_t tries = 2;
    int32_t type = 1;
    char name[260];
    SStrCopy(name, fileName, sizeof(name));
    char* extension = OsPathFindExtensionWithDot(name);

    if ((flags & 8) && extension) {
        tries = 1;

        if (!_stricmp(extension, ".blp")) {
            type = 1;
        } else if (!_stricmp(extension, ".tga")) {
            type = 0;
        }

        extension = NULL;
    }

    CTexture* texture = TextureCacheGetTexture(name, extension, texFlags);

    if (texture) {
        return texture;
    }

    texture = new (__FILE__, __LINE__) CTexture;

    if (extension) {
        *extension = '\0';
    }

    SStrCopy(texture->m_filename, name, sizeof(texture->m_filename));

    int32_t i;

    for (i = 0; i < tries; i++) {
        if (CreateTextureOfType(extension, name, type, texture, sizeof(name), flags, texFlags, priority)) {
            break;
        }

        type = (type + 1) % 2;
    }

    if (i >= tries) {
        TextureReportLoadError("texture", fileName, status);
        FillInSolidTexture(*reinterpret_cast<const CImVector*>(&s_defaultColor), texture);
    }

    TextureCacheNewTexture(texture, texFlags);

    return texture;
}
