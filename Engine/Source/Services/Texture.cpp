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

// STATIC: %0x00BB9E5C
static TSExplicitList<CAsyncObject, offsetof(CAsyncObject, m_link)> s_asyncLoadList;

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

// PROVEN: c74642f9 IDENTICAL
// OFFSET: 0x00401000
void GxTexNullUserFunc(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
}

// PROVEN: 450314b2 IDENTICAL
// OFFSET: 0x00540720
EGxTexFormat TexturePixelFormatToGxTexFormat(PIXEL_FORMAT format) {
    return s_pixelFormatToGxTexFormat[format];
}

// OFFSET: %0x00550C60
void CTextureAtlas::Update(CTexture* texture) {
    this->m_updateSlot = texture->m_atlasBlockIndex;
    GxTexUpdate(this->m_gxTex, (texture->m_atlasBlockIndex & 7) * this->m_width, ((texture->m_atlasBlockIndex >> 3) & 7) * this->m_height, (texture->m_atlasBlockIndex & 7) * this->m_width + this->m_width, ((texture->m_atlasBlockIndex >> 3) & 7) * this->m_height + this->m_height, 1);
    this->m_updateSlot = -1;
}

// OFFSET: %0x00550CC0
void AsyncTextureDestroyedCallback(void* param, CAsyncObject* object) {
    s_asyncLoadBufferUsed -= object->m_size;
    void* buffer = object->m_buffer;
    AsyncFileReadDestroyObject(object);
    SMemFree(buffer, __FILE__, __LINE__, 0);
}

// OFFSET: %0x00550D00
void TextureReportLoadError(const char* type, const char* fileName, CStatus* status) {
    char buffer[256];
    SErrGetErrorStr(SErrGetLastError(), buffer, sizeof(buffer));
    status->Add(STATUS_FATAL, "Error loading %s file \"%s\": %s\n", type, fileName, buffer);
    SErrSetLastError(0);
}

// OFFSET: %0x00550D50
static void TextureBuildMipFormat(char* buffer, const char* fileName) {
    SStrCopy(buffer, fileName, 260);
    *OsPathFindExtensionWithDot(buffer) = '\0';
    SStrPack(buffer, "_mip%d.tga", 260);
}

// OFFSET: %0x00550DC0
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

// OFFSET: %0x00550E70
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

// OFFSET: %0x00554460
void AsyncTextureFailureCallback(CTexture* texture, CAsyncObject* object) {
    texture->m_asyncObject = NULL;
    s_asyncLoadBufferUsed -= object->m_size;
    void* buffer = object->m_buffer;
    AsyncFileReadDestroyObject(object);
    SMemFree(buffer, __FILE__, __LINE__, 0);
    FillInSolidTexture(*reinterpret_cast<const CImVector*>(&s_defaultColor), texture);
}

// OFFSET: %0x00550EF0
SFileImpl* TextureCreateOpenFile(const char* fileName, int32_t flags) {
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

// OFFSET: %0x00550F40
static TEXFILETYPE TextureSplitExtension(char* buffer, const char* fileName, char*& extension) {
    TEXFILETYPE type = TEXFILE_BLP;
    SStrCopy(buffer, fileName, 260);
    uint32_t length = strlen(fileName);

    if (length >= 4 && buffer[length - 4] == '.') {
        length -= 4;
    }

    extension = &buffer[length];

    if (*extension == '.') {
        SStrLower(extension + 1);

        if (extension[3] == 'a') {
            if (extension[1] == 't' && extension[2] == 'g') {
                type = TEXFILE_TGA;
            }
        } else if (extension[3] == 'g') {
            if (extension[1] == 'p' && extension[2] == 'n') {
                *extension = '\0';
                return TEXFILE_UNKNOWN;
            }
        } else if (extension[3] == 'p' && extension[1] == 'b' && extension[2] == 'l') {
            *extension = '\0';
            return TEXFILE_BLP;
        }

        *extension = '\0';
    }

    return type;
}

// PROVEN: 55a942d1 IDENTICAL
// OFFSET: 0x00540BB0
EGxTexFilter TextureGetFilterMode() {
    return s_filterMode;
}

// PROVEN: 8deda106 IDENTICAL
// OFFSET: 0x00540BC0
uint32_t TextureGetMaxAnisotropy() {
    return s_maxAnisotropy;
}

// PROVEN: e3fbcd12 IDENTICAL
// OFFSET: 0x00540BD0
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

// PROVEN: 53f3bc6c IDENTICAL
// OFFSET: 0x00540BE0
void TextureAddRef(CTexture* texture) {
    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    texture->m_refcount++;
}

// OFFSET: %0x005510B0
void TextureRelease(CTexture*& texture) {
    if (!--texture->m_refcount) {
        delete texture;
    }

    texture = NULL;
}

// OFFSET: %0x005510D0
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

// PROVEN: a0a0409b IDENTICAL
// OFFSET: 0x00540C80
void TextureSetUserData(CTexture* texture, GxTexUserFunc userFunc, void* userArg) {
    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    GxTexSetUserData(texture->m_gxTex, userFunc, userArg);
}

// PROVEN: 39642d4a IDENTICAL
// OFFSET: 0x00540CB0
int32_t TextureOnAtlas(CTexture* texture) {
    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return texture->m_atlas != NULL;
}

// OFFSET: %0x00551190
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

// PROVEN: 02708afe IDENTICAL
// OFFSET: 0x00540E20
void TextureFreeMippedImg(MipBits* bits) {
    if (bits) {
        SMemFree(bits, __FILE__, __LINE__, 0);
    }
}

// OFFSET: %0x005512E0
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

// OFFSET: %0x00551370
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

// PROVEN: 7f3e27ce IDENTICAL
// OFFSET: 0x00540FD0
const char* TextureGetFilename(CTexture* texture) {
    if (!texture) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    return texture->m_filename;
}

// OFFSET: %0x00551490
int32_t TextureIsLoaded(CTexture* texture) {
    return texture && !texture->m_asyncObject;
}

// OFFSET: %0x005514B0
void TextureSetTextureSubstitutions(const char* from, const char* to) {
    s_substituteFrom = const_cast<char*>(from);
    s_substituteTo = const_cast<char*>(to);
}

// PROVEN: 3833ca98 IDENTICAL
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

// OFFSET: %0x00551500
int32_t TextureLoadBlob(const char* fileName, void* data, uint32_t size) {
    return CTextureBlob::Load(fileName, data, size);
}

// PROVEN: 4ddb0b42 IDENTICAL
// OFFSET: 0x00541070
int32_t TextureUnloadBlob(const char* fileName) {
    return CTextureBlob::Unload(fileName);
}

// OFFSET: %0x00551540
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

// PROVEN: af02a2c0 IDENTICAL
// OFFSET: 0x00541100
int32_t TextureIsBlob(CTexture* texture) {
    return texture->m_asyncObject && texture->m_gxTex;
}

// OFFSET: %0x005515C0
int32_t TextureIsStreaming(CTexture* texture) {
    return texture->m_asyncObject && texture->m_asyncObject->m_isStreamed;
}

// OFFSET: %0x00551640
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

        const TGA32Pixel* source = mip.ImageTGA32Pixel();
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

// OFFSET: %0x00551880
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

// OFFSET: %0x00551910
int32_t TextureLoadBlpMips(char* fileName, int32_t flags, MipBits** images) {
    CBLPFile image;

    if (!image.Open(fileName, flags)) {
        return 0;
    }

    PIXEL_FORMAT pixelFormat = PIXEL_ARGB8888;

    if (image.m_header.m_colorEncoding == 2) {
        pixelFormat = static_cast<PIXEL_FORMAT>(image.m_header.m_preferredFormat);

        if (pixelFormat == PIXEL_DXT1) {
            if (!g_theGxDevicePtr->Caps().m_texFmt[GxTex_Dxt1]) {
                pixelFormat = (image.m_header.m_alphaSize & 0x3F) ? PIXEL_ARGB1555 : PIXEL_RGB565;
            }
        } else if (pixelFormat == PIXEL_DXT3) {
            if (!g_theGxDevicePtr->Caps().m_texFmt[GxTex_Dxt3]) {
                pixelFormat = PIXEL_ARGB4444;
            }
        } else if (pixelFormat == PIXEL_DXT5 && !g_theGxDevicePtr->Caps().m_texFmt[GxTex_Dxt5]) {
            pixelFormat = PIXEL_ARGB4444;
        }
    }

    uint32_t height = image.m_header.m_height;
    uint32_t bestMip = 0;
    uint32_t width = image.m_header.m_width;
    RequestImageDimensions(&width, &height, &bestMip);

    return image.LockChain(pixelFormat, *images, bestMip) != 0;
}

// OFFSET: %0x00551A70
int32_t LoadBlpWithAlias(char* fileName, int32_t flags, MipBits** images) {
    char buffer[260];
    char* extension = &fileName[strlen(fileName)];
    strcpy(extension, ".blp");

    if (TextureBuildAliasPath(buffer, fileName)) {
        fileName = buffer;
    }

    int32_t result = TextureLoadBlpMips(fileName, flags, images);
    *extension = '\0';

    return result;
}

// OFFSET: %0x00551AD0
void UpdateBlpTextureAsync(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    CTexture* texture = static_cast<CTexture*>(userArg);

    switch (command) {
        case GxTex_Lock:
            if (!s_pumping && !LoadBlpWithAlias(texture->m_filename, texture->m_flags & 2, &s_mipBits)) {
                GetDefaultTexture(width, height);
                GetGlobalStatusObj().Add(STATUS_ERROR, "Texture %s not loaded -- replaced with default.\n", texture->m_filename);
            }

            break;

        case GxTex_Latch: {
            texelStrideInBytes = GxCalcTexelStrideInBytes(texture->m_dataFormat, width);

            const uint8_t* bits = reinterpret_cast<const uint8_t*>(s_mipBits[level].m_mip[0]);
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

// OFFSET: %0x00551BE0
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

// OFFSET: %0x00551D40
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

// OFFSET: %0x00551D90
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

// OFFSET: %0x00551DF0
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

// OFFSET: %0x00551E30
int32_t TextureIsReadyNow(CTexture* texture) {
    if (!texture || !texture->m_gxTex) {
        return 0;
    }

    return g_theGxDevicePtr->TexIsReadyNow(texture->m_gxTex);
}

// OFFSET: %0x00551E60
int32_t TextureCanUpdate(CTexture* texture) {
    if (!texture || !texture->m_gxTex) {
        return 0;
    }

    return g_theGxDevicePtr->TexCanUpdate(texture->m_gxTex);
}

// OFFSET: %0x00551E90
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

        if (!LoadBlpWithAlias(texture->m_filename, texture->m_flags & 2, &s_mipBits)) {
            GetGlobalStatusObj().Add(STATUS_ERROR, "Texture %s not loaded.\n", texture->m_filename);
        }

        this->Update(texture);
        s_pumping = 0;
    }

    this->m_dirty &= ~1;
}

// OFFSET: %0x00552070
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

// OFFSET: %0x005520E0
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

// OFFSET: %0x00552150
void TextureNotifyGxRestart() {
    for (CTextureAtlas* atlas = s_atlasList.Head(); atlas; atlas = atlas->m_link.Next()) {
        atlas->Reload();
    }
}

// OFFSET: %0x00552220
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

// OFFSET: %0x00552290
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

// OFFSET: %0x00552310
CGxTex* TextureForceLoadForDiskOnly(CTexture* texture) {
    CAsyncObject* object = texture->m_asyncObject;

    if (!object->m_buffer) {
        int32_t local = SFile::FileIsLocal(object->m_file, 6);

        if (!local || local == 2) {
            return NULL;
        }
    }

    if (texture->m_asyncObject->m_isStreamed) {
        return NULL;
    }

    return TextureForceLoad(texture);
}

// OFFSET: %0x00552450
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

// OFFSET: %0x005524D0
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

// OFFSET: %0x00554320
void FillInSolidTexture(const CImVector& color, CTexture* texture) {
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

// OFFSET: %0x00552660
void AsyncTextureHandler() {
    uint32_t available = 0x300000 - s_asyncLoadBufferUsed;

    for (CAsyncObject* object = s_asyncLoadList.Head(); !s_asyncLoadList.IsEnd(object); ) {
        CAsyncObject* next = s_asyncLoadList.RawNext(object);

        if (available >= object->m_size) {
            AsyncTextureProcess(object, 0);
            available -= object->m_size;
        }

        object = next;
    }

    for (CAsyncObject* object = s_asyncTextureList.Head(); !s_asyncTextureList.IsEnd(object); ) {
        CAsyncObject* next = s_asyncTextureList.RawNext(object);

        if (available >= object->m_size) {
            AsyncTextureProcess(object, 0);
            available -= object->m_size;
        }

        object = next;
    }
}

// OFFSET: %0x00552760
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

// OFFSET: %0x00552180
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

// OFFSET: %0x005521C0
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

// OFFSET: %0x005528D0
CGxTex* TextureGetGxTexBlocking(CTexture* texture) {
    CGxTex* gxTex = TextureGetGxTex(texture);

    if (gxTex) {
        return gxTex;
    }

    return TextureForceLoad(texture);
}

// OFFSET: %0x00552A00
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

// OFFSET: %0x00552AA0
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

// OFFSET: %0x00552AF0
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

// OFFSET: %0x00552B50
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

// PROVEN: f2601043 IDENTICAL
// OFFSET: 0x00542420
int32_t TextureUpdate(const void* data, void* param) {
    s_currentTime = OsGetAsyncTimeMs();
    TextureCacheUpdate();

    return 1;
}

// OFFSET: %0x00552CC0
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

// OFFSET: %0x00552DA0
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

// OFFSET: %0x005531A0
CTextureAtlas::~CTextureAtlas() {
    if (this->m_gxTex) {
        TextureFreeGxTex(this->m_gxTex, "ATLAS");
    }

    this->m_link.Unlink();
}

// OFFSET: %0x00553230
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

// OFFSET: %0x00553360
int32_t LoadTgaMips(char* fileName, char* extension, int32_t flags, MipBits** images, uint32_t* width, uint32_t* height, EGxTexFormat* gxTexFormat, int32_t* opaque, uint32_t* alphaBits, PIXEL_FORMAT* pixelFormat) {
    if (extension) {
        strcpy(extension, ".tga");
    }

    CTgaFile image;

    if (!image.Open(fileName, flags)) {
        return 0;
    }

    if (opaque) {
        *opaque = image.AlphaBits() == 0;
    }

    if (!image.LoadImageData(3)) {
        return 0;
    }

    image.SetTopDown(1);

    uint32_t imageWidth = image.Width();
    uint32_t imageHeight = image.Height();
    uint32_t mipCount = TextureCalcMipCount(imageWidth, imageHeight);

    if (*images) {
        MippedImgSet(PIXEL_ARGB8888, imageWidth, imageHeight, *images);
    } else {
        *images = TextureAllocMippedImg(PIXEL_ARGB8888, imageWidth, imageHeight);
    }

    const TGA32Pixel* source = image.ImageTGA32Pixel();
    C4Pixel* dest = (*images)->m_mip[0];

    for (uint32_t pixels = imageWidth * imageHeight; pixels; pixels--) {
        dest->m_b = source->m_b;
        dest->m_g = source->m_g;
        dest->m_r = source->m_r;
        dest->m_a = source->m_a;
        dest++;
        source++;
    }

    image.Close();

    char mipName[260];
    TextureBuildMipFormat(mipName, fileName);
    uint32_t predrawn = LoadPredrawnMips(&image, mipName, flags, *images);
    TextureGenerateMips(image.Width(), image.Height(), predrawn, mipCount, *images);

    if (width) {
        *width = image.Width();
    }

    if (height) {
        *height = image.Height();
    }

    if (gxTexFormat) {
        *gxTexFormat = GxTex_Argb8888;
    }

    if (alphaBits) {
        *alphaBits = image.AlphaBits();
    }

    if (pixelFormat) {
        *pixelFormat = PIXEL_ARGB8888;
    }

    return 1;
}

// OFFSET: %0x00553550
static int32_t LoadTgaWithExtension(char* fileName, int32_t flags, MipBits** images) {
    char* extension = &fileName[strlen(fileName)];
    int32_t result = LoadTgaMips(fileName, extension, flags, images, NULL, NULL, NULL, NULL, NULL, NULL);
    *extension = '\0';

    return result;
}

// OFFSET: %0x005535A0
void UpdateTgaTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    CTexture* texture = static_cast<CTexture*>(userArg);

    switch (command) {
        case GxTex_Lock:
            if (!s_pumping && !LoadTgaWithExtension(texture->m_filename, texture->m_flags & 2, &s_mipBits)) {
                GetDefaultTexture(width, height);
                GetGlobalStatusObj().Add(STATUS_ERROR, "Texture %s not loaded -- replaced with default.\n", texture->m_filename);
            }

            break;

        case GxTex_Latch: {
            texelStrideInBytes = 4 * width;

            const uint8_t* bits = reinterpret_cast<const uint8_t*>(s_mipBits[level].m_mip[0]);
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

// OFFSET: %0x005536B0
int32_t PumpBlpTextureAsync(CTexture* texture, void* data) {
    CBLPFile image;

    if (!image.Source(data)) {
        texture->m_status.Add(STATUS_FATAL, "BLP Texture failure: \"%s\" invalid file version\n", texture->m_filename);
        return 0;
    }

    if ((texture->m_flags & 4) && !(image.m_header.m_hasMips & 0x10)) {
        texture->m_flags &= ~4;
    }

    texture->m_colorEncoding = image.m_header.m_colorEncoding;
    texture->m_alphaBits = image.m_header.m_alphaSize;

    if (!image.m_header.m_alphaSize) {
        texture->m_flags |= 1;
    }

    uint32_t bestMip = 0;
    uint32_t width = image.m_header.m_width;
    uint32_t height = image.m_header.m_height;
    RequestImageDimensions(&width, &height, &bestMip);
    texture->m_bestMip = bestMip;

    PIXEL_FORMAT pixelFormat;
    EGxTexFormat gxTexFormat;
    TextureGetTextureFormats(pixelFormat, gxTexFormat, static_cast<PIXEL_FORMAT>(image.m_header.m_preferredFormat), image.m_header.m_alphaSize & 0x3F);

    s_pumping = 1;

    if (!image.LockChain2(texture->m_filename, pixelFormat, s_mipBits, bestMip, 1)) {
        s_pumping = 0;
        texture->m_status.Add(STATUS_FATAL, "BLP Texture failure: \"%s\" decompression failed.\n", texture->m_filename);
        return 0;
    }

    uint32_t faceWidth = width;
    EGxTexTarget target = GxTex_2d;

    if (width == 6 * height) {
        target = GxTex_CubeMap;
        faceWidth = width / 6;
    }

    texture->m_gxHeight = height;
    texture->m_gxTexFormat = gxTexFormat;
    texture->m_gxTexTarget = target;
    texture->m_dataFormat = s_pixelFormatToGxTexFormat[pixelFormat];
    texture->m_gxWidth = faceWidth;

    uint32_t filter = texture->m_gxTexFlags.m_filter;

    if (filter != GxTex_Nearest && filter != GxTex_Linear && image.m_numLevels == 1) {
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

        texture->m_gxTex = TextureAllocGxTex(texture->m_gxTexTarget, texture->m_gxWidth, texture->m_gxHeight, 0, texture->m_gxTexFormat, texture->m_gxTexFlags, texture, UpdateBlpTextureAsync, texture->m_dataFormat, texture->m_filename);

        if (!texture->m_gxTex) {
            s_pumping = 0;
            texture->m_status.Add(STATUS_FATAL, "BLP Texture failure: \"%s\" allocating %dx%d texture failed.\n", texture->m_filename, faceWidth, height);
            return 0;
        }

        GxTexUpdate(texture->m_gxTex, 0, 0, faceWidth, height, 1);
    }

    s_pumping = 0;

    return 1;
}

// OFFSET: %0x005543D0
void AsyncTextureSuccessCallback(CTexture* texture, CAsyncObject* object) {
    if (!PumpBlpTextureAsync(texture, object->m_buffer)) {
        if (!texture->m_status.IsEmpty()) {
            SysMsgAdd(&texture->m_status);
        }

        FillInSolidTexture(*reinterpret_cast<const CImVector*>(&s_defaultColor), texture);
    }

    SFileCloseFile(object->m_file);
    void* buffer = object->m_buffer;
    s_asyncLoadBufferUsed -= object->m_size;
    object->m_file = NULL;
    SMemFree(buffer, __FILE__, __LINE__, 0);
    AsyncFileReadDestroyObject(object);
    texture->m_asyncObject = NULL;
}

// OFFSET: %0x00553960
int32_t AsyncTextureStatus() {
    int32_t count = 0;

    for (CAsyncObject* object = s_asyncLoadList.Head(); !s_asyncLoadList.IsEnd(object); object = s_asyncLoadList.RawNext(object)) {
        count++;
    }

    for (CAsyncObject* object = s_asyncTextureList.Head(); !s_asyncTextureList.IsEnd(object); object = s_asyncTextureList.RawNext(object)) {
        count++;
    }

    return count;
}

// OFFSET: %0x005539C0
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

// OFFSET: %0x00553A20
void TextureGxCacheFlush() {
    TextureGxCacheClear();
    s_gxTexCacheFree.Clear();
}

// OFFSET: %0x00553A30
int32_t LoadBlpMips(char* fileName, char* extension, int32_t flags, MipBits** images, uint32_t* width, uint32_t* height, int32_t* opaque, uint32_t* alphaBits, PIXEL_FORMAT* pixelFormat) {
    if (extension) {
        strcpy(extension, ".blp");
    }

    CBLPFile image;

    if (!image.Open(fileName, flags)) {
        return 0;
    }

    uint32_t alphaSize = image.m_header.m_alphaSize & 0x3F;
    uint32_t imageWidth = image.m_header.m_width;
    uint32_t imageHeight = image.m_header.m_height;
    PIXEL_FORMAT format;

    if (!pixelFormat || (format = *pixelFormat) == PIXEL_UNSPECIFIED) {
        if (image.m_header.m_alphaSize & 0x3F) {
            format = alphaSize != 1 ? PIXEL_ARGB4444 : PIXEL_ARGB1555;
        } else {
            format = PIXEL_RGB565;
        }
    }

    *images = TextureAllocMippedImg(format, image.m_header.m_width, image.m_header.m_height);

    if (!image.LockChain2(fileName, format, *images, 0, 0)) {
        return 0;
    }

    if (width) {
        *width = imageWidth;
    }

    if (height) {
        *height = imageHeight;
    }

    if (opaque) {
        *opaque = alphaSize == 0;
    }

    if (alphaBits) {
        *alphaBits = alphaSize;
    }

    if (pixelFormat) {
        *pixelFormat = format;
    }

    return 1;
}

// OFFSET: %0x00553B90
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

    MipBits* images = NULL;
    char name[260];
    SStrCopy(name, fileName, sizeof(name));
    char* extension = OsPathFindExtensionWithDot(name);
    *extension = '\0';

    int32_t type = TEXFILE_BLP;

    for (int32_t tries = 0; tries < 3; tries++) {
        if (type == TEXFILE_TGA) {
            LoadTgaMips(name, extension, flags, &images, width, height, NULL, opaque, alphaBits, dataFormat);
        } else if (type == TEXFILE_BLP) {
            LoadBlpMips(name, extension, flags, &images, width, height, opaque, alphaBits, dataFormat);
        }

        if (images) {
            return images;
        }

        type = (type + 1) % 3;
    }

    if (status) {
        status->Add(STATUS_FATAL, "Error loading texure file \"%s\": unsupported image format\n", fileName);
    }

    return images;
}

// OFFSET: %0x00553D90
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

// OFFSET: %0x00554090
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

// OFFSET: %0x005540E0
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

// PROVEN: 5bfcc2da IDENTICAL
// OFFSET: 0x00543C00
const char* CTexture::GetObjectName() {
    return this->m_filename;
}

// OFFSET: [%0x00555090, %0x00555320]
CTexture::~CTexture() {
    if (this->m_gxTex) {
        TextureFreeGxTex(this->m_gxTex, this->m_filename);
        this->m_gxTex = NULL;
    }

    if (this->m_atlas) {
        CTextureAtlas::Free(this->m_atlas, this);
        this->m_atlas = NULL;
    }

    if (this->m_asyncObject) {
        if (this->m_flags & 0x20) {
            this->m_asyncObject->m_userArg = NULL;
            this->m_asyncObject->m_userPostUnloadCallback = reinterpret_cast<void*>(AsyncTextureDestroyedCallback);
            this->m_asyncObject->m_userPostloadCallback = this->m_asyncObject->m_userPostUnloadCallback;
        } else {
            void* buffer = this->m_asyncObject->m_buffer;
            uint32_t size = this->m_asyncObject->m_size;

            if (AsyncFileReadCancel(this->m_asyncObject, AsyncTextureDestroyedCallback) && buffer) {
                s_asyncLoadBufferUsed -= size;
                SMemFree(buffer, __FILE__, __LINE__, 0);
            }
        }

        this->m_asyncObject = NULL;
    }

    this->m_link.Unlink();
}

// OFFSET: %0x005544B0
int32_t CreateBlpSync(CTexture* texture, const char* fileName, uint32_t flags, CGxTexFlags texFlags) {
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

    if (texture->m_gxTex) {
        TextureFreeGxTex(texture->m_gxTex, texture->m_filename);
        texture->m_gxTex = NULL;
    }

    if (texture->m_atlas) {
        CTextureAtlas::Free(texture->m_atlas, texture);
        texture->m_atlas = NULL;
    }

    texture->m_gxTexFlags = texFlags;

    if (flags & 2) {
        texture->m_flags |= 2;
    }

    if ((flags & 4) && s_atlasEnable) {
        texture->m_flags |= 4;
    }

    uint32_t size = SFileGetFileSize(file, NULL);
    void* data = SMemAlloc(size, __FILE__, __LINE__, 0);

    if (!SFileReadFile(file, data, size, NULL, NULL, 0)) {
        DebugLog("CreateBlpTexture() failed read: %s.\n", fileName);
    }

    if (!PumpBlpTextureAsync(texture, data)) {
        FillInSolidTexture(*reinterpret_cast<const CImVector*>(&s_defaultColor), texture);
    }

    SFileCloseFile(file);
    SMemFree(data, __FILE__, __LINE__, 0);

    return 1;
}

// OFFSET: %0x005545F0
int32_t CreateBlpAsync(CTexture* texture, const char* fileName, char* extension, uint32_t flags, CGxTexFlags texFlags) {
    CTextureBlobTexture* blobTexture = NULL;
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

    if (texture->m_gxTex) {
        TextureFreeGxTex(texture->m_gxTex, texture->m_filename);
        texture->m_gxTex = NULL;
    }

    if (texture->m_atlas) {
        CTextureAtlas::Free(texture->m_atlas, texture);
        texture->m_atlas = NULL;
    }

    if (extension) {
        *extension = '\0';
    }

    if (!(flags & 4)) {
        blobTexture = CTextureBlob::GetTexture(fileName);
    }

    texture->m_gxTexFlags = texFlags;

    if (flags & 2) {
        texture->m_flags |= 2;
    }

    if ((flags & 4) && s_atlasEnable) {
        texture->m_flags |= 4;
    }

    if ((flags & 0x20) && SFile::IsStreamingMode()) {
        texture->m_flags |= 0x20;
    }

    if (blobTexture) {
        CTextureBlob::CreateTexture(texture, blobTexture);
    }

    texture->m_asyncObject = AsyncFileReadAllocObject();
    texture->m_asyncObject->m_userArg = texture;
    texture->m_asyncObject->m_userPostloadCallback = reinterpret_cast<void*>(AsyncTextureSuccessCallback);
    texture->m_asyncObject->m_userPostUnloadCallback = reinterpret_cast<void*>(AsyncTextureFailureCallback);
    texture->m_asyncObject->m_file = file;
    texture->m_asyncObject->m_size = SFileGetFileSize(file, NULL);

    if (blobTexture) {
        texture->m_asyncObject->m_priority = 132;
    } else if (flags & 0x10) {
        texture->m_asyncObject->m_priority = 129;
    } else {
        texture->m_asyncObject->m_priority = 130;
    }

    CAsyncObject* object = texture->m_asyncObject;

    if (0x300000 - s_asyncLoadBufferUsed < object->m_size) {
        s_asyncTextureList.LinkNode(object, 2, NULL);
    } else {
        AsyncTextureProcess(object, 0);
    }

    return 1;
}

// OFFSET: %0x00554770
static int32_t CreateBlpTexture(CTexture* texture, char* fileName, char* extension, uint32_t flags, CGxTexFlags texFlags) {
    char buffer[260];

    if (extension) {
        strcpy(extension, ".blp");
    }

    if (TextureBuildAliasPath(buffer, fileName)) {
        fileName = buffer;
        extension = OsPathFindExtensionWithDot(buffer);
    }

    if (s_createBlpAsync) {
        return CreateBlpAsync(texture, fileName, extension, flags, texFlags);
    }

    return CreateBlpSync(texture, fileName, flags, texFlags);
}

// OFFSET: %0x00554800
static int32_t CreateTextureOfType(CTexture* texture, TEXFILETYPE type, char* fileName, char* extension, uint32_t flags, CGxTexFlags texFlags, CStatus* status) {
    switch (type) {
        case TEXFILE_TGA:
            return CreateTgaTexture(texture, fileName, extension, flags, texFlags, status);

        case TEXFILE_BLP:
            return CreateBlpTexture(texture, fileName, extension, flags, texFlags);

        default:
            return 0;
    }
}

// OFFSET: %0x00554850
static void TextureReload(CTexture* texture) {
    SFile::RebuildHash();

    CGxTexFlags texFlags = texture->m_gxTexFlags;
    CStatus status;
    char name[260];
    SStrCopy(name, texture->m_filename, sizeof(name));
    char* extension = OsPathFindExtensionWithDot(name);

    if (extension) {
        *extension = '\0';
    }

    for (int32_t type = TEXFILE_UNKNOWN; type < 3; type++) {
        int32_t result;

        if (type == TEXFILE_TGA) {
            result = CreateTgaTexture(texture, name, extension, 0, texFlags, &status);
        } else if (type == TEXFILE_BLP) {
            result = CreateBlpTexture(texture, name, extension, 0, texFlags);
        } else {
            continue;
        }

        if (result) {
            GxTexUpdate(texture->m_gxTex, 0, 0, 0, 0, 0);
            return;
        }
    }

    TextureReportLoadError("texture", name, &status);
    FillInSolidTexture(*reinterpret_cast<const CImVector*>(&s_defaultColor), texture);
}

// OFFSET: %0x005549A0
static void TextureFileChanged(const char* fileName) {
    DebugLog("Texture changed: %s\n", fileName);

    char name[260];
    SStrCopy(name, fileName, sizeof(name));
    char* extension = OsPathFindExtensionWithDot(name);

    if (extension) {
        *extension = '\0';
    }

    int32_t createBlpAsync = s_createBlpAsync;
    s_createBlpAsync = 0;

    for (CTexture* texture = s_textureList.Head(); texture; ) {
        CTexture* next = s_textureList.Next(texture);
        const char* textureName = OsPathFindFilename(texture->m_filename);

        if (textureName && !_stricmp(textureName, name)) {
            TextureReload(texture);
        }

        texture = next;
    }

    s_createBlpAsync = createBlpAsync;
}

// OFFSET: %0x00554A80
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

// OFFSET: %0x00554B30
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

// OFFSET: %0x00554C40
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

// PROVEN: e41d405d IDENTICAL
// OFFSET: 0x00544900
CTexture* TextureCreate(uint32_t width, uint32_t height, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags texFlags, void* userArg, GxTexUserFunc userFunc, const char* name, int32_t defaultFilter) {
    return TextureCreate(GxTex_2d, width, height, 0, format, dataFormat, texFlags, userArg, userFunc, name, defaultFilter);
}

// OFFSET: %0x00555610
void TextureCacheNewTexture(CTexture* texture) {
    uint32_t hashval = SStrHashHT(texture->m_filename);
    HASHKEY_TEXTUREFILE key;
    key.m_anyTexFlags = 0;
    key.m_texFlags = texture->m_gxTexFlags;
    key.m_filename = texture->m_filename;
    s_textureCache.Insert(texture, hashval, key);
}

// OFFSET: %0x005553C0
void TextureCacheNewTexture(CTexture* texture, CGxTexFlags texFlags) {
    uint32_t hashval = SStrHashHT(texture->m_filename);
    HASHKEY_TEXTUREFILE key;
    key.m_anyTexFlags = 0;
    key.m_filename = texture->m_filename;
    key.m_texFlags = texFlags;
    s_textureCache.Insert(texture, hashval, key);
}

// OFFSET: %0x00555660
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

// OFFSET: %0x00554190
int32_t CreateTgaTexture(CTexture* texture, const char* fileName, char* extension, int32_t flags, CGxTexFlags texFlags, CStatus* status) {
    if (extension) {
        strcpy(extension, ".tga");
    }

    CTgaFile image;

    if (!image.Open(fileName, flags)) {
        return 0;
    }

    if (texture->m_gxTex) {
        TextureFreeGxTex(texture->m_gxTex, texture->m_filename);
        texture->m_gxTex = NULL;
    }

    if (texture->m_atlas) {
        CTextureAtlas::Free(texture->m_atlas, texture);
        texture->m_atlas = NULL;
    }

    uint32_t height = image.Height();
    EGxTexTarget target = GxTex_2d;
    uint32_t width = image.Width();

    if (width == 6 * height) {
        target = GxTex_CubeMap;
        width /= 6;
    }

    if (width < 8 || height < 8) {
        status->Add(STATUS_FATAL, "Error loading file \"%s\": Texture size must be at least %dx%d\n", fileName, 8, 8);
        return 0;
    }

    if (((width - 1) & width) || ((height - 1) & height)) {
        status->Add(STATUS_FATAL, "Error loading file \"%s\": Texture size (%dx%d) must be powers of two\n", fileName, width, height);
        return 0;
    }

    texture->m_colorEncoding = 3;
    texture->m_alphaBits = image.AlphaBits();

    if (!texture->m_alphaBits) {
        texture->m_flags |= 1;
    }

    if (flags) {
        texture->m_flags |= 2;
    }

    texture->m_gxTex = TextureAllocGxTex(target, width, height, 0, GxTex_Argb8888, texFlags, texture, UpdateTgaTexture, GxTex_Argb8888, texture->m_filename);

    if (!texture->m_gxTex) {
        return 0;
    }

    texture->m_gxTexTarget = target;
    texture->m_gxWidth = width;
    texture->m_gxHeight = height;

    return 1;
}

// OFFSET: %0x00555410
CTexture* TextureCreate(const char* fileName, CGxTexFlags texFlags, CStatus* status, uint32_t flags) {
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

    int32_t tries = 3;
    int32_t type = TEXFILE_BLP;
    char name[260];
    SStrCopy(name, fileName, sizeof(name));
    char* extension = OsPathFindExtensionWithDot(name);

    if ((flags & 8) && extension) {
        tries = 1;

        if (_stricmp(extension, ".blp")) {
            if (!_stricmp(extension, ".tga")) {
                type = TEXFILE_TGA;
            } else if (!_stricmp(extension, ".png")) {
                type = TEXFILE_UNKNOWN;
            }
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
        if (CreateTextureOfType(texture, static_cast<TEXFILETYPE>(type), name, extension, flags, texFlags, status)) {
            break;
        }

        type = (type + 1) % 3;
    }

    if (i >= tries) {
        TextureReportLoadError("texture", fileName, status);
        FillInSolidTexture(*reinterpret_cast<const CImVector*>(&s_defaultColor), texture);
    }

    TextureCacheNewTexture(texture, texFlags);

    return texture;
}
