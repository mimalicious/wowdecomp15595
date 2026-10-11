#include <cstddef>
#include "Engine/Source/Services/AsyncFile.h"
#include <cstdint>
#include <cmath>
#include <cstring>
#include <emmintrin.h>
#include <new>
#include <typeinfo>
#if defined(_MSC_VER)
#include <malloc.h>
#else
#include <alloca.h>
#endif
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Base/Prop.h"
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Os/Processor.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Storm/Source/SThread.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Common/ComponentCore/ComponentUtils.h"
#include "WoW/Common/TextureCache.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CharHairGeosetsRec.h"
#include "WoW/Source/DB/Rec/CharSectionsRec.h"
#include "WoW/Source/DB/Rec/CharacterFacialHairStylesRec.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/HelmetGeosetVisDataRec.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/ItemVisualEffectsRec.h"
#include "WoW/Source/DB/Rec/ItemVisualsRec.h"
#include "WoW/Source/DB/Rec/ParticleColorRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"

// STATIC: 0x00BC28E0
static int32_t s_itemPriority[NUM_ITEM_SLOTS][NUM_COMPONENT_SECTIONS] = {
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 2, 2, -1, 2, 2, -1, -1, -1, -1, -1 },
    { 3, 3, -1, 3, 3, 1, 1, -1, -1, -1 },
    { -1, -1, -1, -1, 7, 2, -1, -1, -1, -1 },
    { -1, -1, -1, -1, -1, 0, 0, -1, -1, -1 },
    { -1, -1, -1, -1, -1, -1, 2, 0, -1, -1 },
    { -1, 4, -1, -1, -1, -1, -1, -1, -1, -1 },
    { -1, 5, 0, -1, -1, -1, -1, -1, -1, -1 },
    { -1, -1, -1, 6, 6, -1, -1, -1, -1, -1 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

// STATIC: 0x00BC32E4
static COMPONENT_SECTIONS s_tabardSections[2] = { SECTION_TORSO_UPPER, SECTION_TORSO_LOWER };

// STATIC: 0x00BC32EC
static COMPONENT_SECTIONS s_shirtSections[4] = { SECTION_TORSO_UPPER, SECTION_TORSO_LOWER, SECTION_ARM_UPPER, SECTION_ARM_LOWER };

// STATIC: 0x00CFFD44
static const char* s_componentSections[8] = { "ArmUpperTexture", "ArmLowerTexture", "HandTexture", "TorsoUpperTexture", "TorsoLowerTexture", "LegUpperTexture", "LegLowerTexture", "FootTexture" };

// STATIC: 0x00CFFD64
static const char* s_fileDecorations[3] = { "M", "F", "U" };

// STATIC: 0x00CFFDA0
static CStatus s_status;

// STATIC: 0x00CFFDB4
static TSExplicitList<CCharacterComponent, offsetof(CCharacterComponent, m_link)> s_queue;

// STATIC: 0x00CFFDC0
static TSExplicitList<CCharacterComponent, offsetof(CCharacterComponent, m_componentLink)> s_componentList;

// STATIC: 0x00CFFDCC
static TSExplicitList<CComponentRequest, offsetof(CComponentRequest, m_link)> s_requestList;

// STATIC: 0x00CFFDD8
static TSExplicitList<CComponentRequest, offsetof(CComponentRequest, m_link)> s_finishedList;

// STATIC: 0x00CFFDE4
static TSExplicitList<CComponentRequest, offsetof(CComponentRequest, m_link)> s_requestFreeList;

// STATIC: 0x00CFFDF0
static TSExplicitList<CComponentMipBits, offsetof(CComponentMipBits, m_link)> s_mipBitsFreeList;

// STATIC: 0x00CFFDFC
static TSExplicitList<CComponentMipBits, offsetof(CComponentMipBits, m_link)> s_mipBitsCompressedFreeList;

// STATIC: 0x00CFFE08
static const char* s_proceduralEmblemFormat = "Textures\\GuildEmblems\\emblem_%02i";

// STATIC: 0x00E4D858
static int32_t s_closestComponentSortVal;

// STATIC: 0x00E4D85C
static CCharacterComponent* s_closestComponent;

// STATIC: 0x00E4D860
void (CCharacterComponent::*CCharacterComponent::s_requestFunc[NUM_COMPONENT_SECTIONS])();

// STATIC: 0x00E4D888
static HPROPCONTEXT s_threadPropContext;

// STATIC: 0x00E4D88C
int32_t CCharacterComponent::s_bComponentCompression;

// STATIC: 0x00E4D890
int32_t CCharacterComponent::s_bComponentThread;

// STATIC: 0x00E4D894
static char* s_pathEnd2;

// STATIC: 0x00E4D898
static char* s_pathEnd;

// STATIC: 0x00E4D89C
static int32_t s_useCompression;

// STATIC: 0x00E4D8A0
static char s_path2[260];

// STATIC: 0x00E4D9A4
uint32_t CCharacterComponent::s_mipLevels;

// STATIC: 0x00E4D9A8
static char s_path[260];

// STATIC: 0x00E4DAAC
EGxTexFormat CCharacterComponent::s_gxFormatHigh;

// STATIC: 0x00E4DAB0
static char s_buffer[260];

// STATIC: 0x00E4DBB4
void (CCharacterComponent::*CCharacterComponent::m_itemFunc[NUM_COMPONENT_SECTIONS])(ITEM_SLOT, const ItemDisplayInfoRec*, bool);

// STATIC: 0x00E4DBDC
void (CCharacterComponent::*CCharacterComponent::m_prepFunc[NUM_COMPONENT_SECTIONS])();

// STATIC: 0x00E4DC04
EGxTexFormat CCharacterComponent::s_gxFormat;

// STATIC: 0x00E4DC08
uint32_t* CCharacterComponent::s_characterFacialHairStylesList;

// STATIC: 0x00E4DC0C
st_race* CCharacterComponent::s_chrVarArray;

// STATIC: 0x00E4DC10
static MipBits* s_threadTextureBuffer;

// STATIC: 0x00E4DC14
static MipBits* s_compressedTextureBuffer;

// STATIC: 0x00E4DC18
MipBits* CCharacterComponent::s_textureBuffer;

// STATIC: 0x00E4DC1C
uint32_t CCharacterComponent::s_chrVarArrayLength;

// STATIC: 0x00E4DC20
CCharacterComponent* CCharacterComponent::m_activePlayerComponent;

// STATIC: 0x00E4DC28
static TSObjectAllocPool<CCharacterComponent>* s_componentPool;

// STATIC: 0x00E4DC2C
static CVar* s_textureLevelVar;

// STATIC: 0x00E4DC30
int32_t CCharacterComponent::s_bInRenderPrep;

// STATIC: 0x00E4DC34
static SThread s_thread;

// STATIC: 0x00E4DC38
static CompSectionInfo s_sectionInfo[NUM_COMPONENT_SECTIONS];

// STATIC: 0x00E4DCD8
CompSectionInfo CCharacterComponent::s_sectionInfoRaw[NUM_COMPONENT_SECTIONS] = {
    { { 0, 0 }, { 256, 128 } },
    { { 0, 128 }, { 256, 128 } },
    { { 0, 256 }, { 256, 64 } },
    { { 256, 0 }, { 256, 128 } },
    { { 256, 128 }, { 256, 64 } },
    { { 256, 192 }, { 256, 128 } },
    { { 256, 320 }, { 256, 128 } },
    { { 256, 448 }, { 256, 64 } },
    { { 0, 320 }, { 256, 64 } },
    { { 0, 384 }, { 256, 128 } },
};

// STATIC: 0x00E4DD78
static SEvent s_threadBeginEvent(0, 0);

// STATIC: 0x00E4DD7C
static SCritSect s_requestListCritSect;

// STATIC: 0x00E4DDA4
static SCritSect s_finishedListCritSect;

// STATIC: 0x00E4DDCC
static SCritSect s_mipBitsCritSect;

// INLINED
void CComponentRequest::AddLayer(uint8_t section, void* texture) {
    if (this->m_layerCount < sizeof(this->m_layerTextures) / sizeof(this->m_layerTextures[0])) {
        this->m_layerTextures[this->m_layerCount] = TextureCacheCopyTexture(texture);
        this->m_layerSections[this->m_layerCount++] = section;
    }
}

// INLINED
inline CharacterSection::CharacterSection() {
    for (int32_t layer = 0; layer < 8; layer++) {
        this->m_layerTex[layer] = NULL;
        this->m_layerItemDisplayId[layer] = 0;
    }

    this->m_layerMask = 0;
}

// INLINED
inline CharacterSection::~CharacterSection() {
    for (int32_t layer = 0; layer < 8; layer++) {
        if (this->m_layerTex[layer]) {
            TextureCacheDestroyTexture(this->m_layerTex[layer]);
        }

        this->m_layerTex[layer] = NULL;
        this->m_layerItemDisplayId[layer] = 0;
    }
}

// INLINED
inline CharacterBaseVariation::~CharacterBaseVariation() {
    for (int32_t i = 0; i < 3; i++) {
        if (this->m_texture[i]) {
            TextureCacheDestroyTexture(this->m_texture[i]);
        }

        this->m_texture[i] = NULL;
    }
}

// INLINED
void CCharacterComponent::ResetClosestComponent() {
    s_closestComponent = NULL;
    s_closestComponentSortVal = 0x7FFFFFFF;
}

// OFFSET: 0x00401800/0x140003F70
bool CCharacterComponent::Initialize() {
    CVar* textureLevelVar = CVar::Lookup("componentTextureLevel");
    CVar* threadVar = CVar::Lookup("componentThread");
    CVar* compressVar = CVar::Lookup("componentCompress");
    uint32_t mipLevels = textureLevelVar->m_intValue;
    int32_t useThread = threadVar->m_intValue;
    int32_t useCompression = compressVar->m_intValue;

    if (OsGetNumberOfProcessors() < 2) {
        useThread = 0;
    }

    if (!useThread) {
        if (mipLevels > 8) {
            mipLevels = 8;
        }

        useCompression = 0;
    } else if (mipLevels > 9) {
        return CCharacterComponent::Initialize(GxTex_Rgb565, 9, useThread, useCompression);
    }

    if (mipLevels < 7) {
        mipLevels = 7;
    }

    return CCharacterComponent::Initialize(GxTex_Rgb565, mipLevels, useThread, useCompression);
}

// PROVEN: 3b7c062b IDENTICAL/NONE
// OFFSET: 0x006F8340
void CComponentRequest::DestroyCachedTextures() {
    for (uint32_t i = 0; i < NUM_COMPONENT_SECTIONS; i++) {
        if (this->m_baseTextures[i]) {
            TextureCacheDestroyTexture(this->m_baseTextures[i]);
            this->m_baseTextures[i] = NULL;
        }
    }

    for (uint32_t i = 0; i < this->m_layerCount; i++) {
        if (this->m_layerTextures[i]) {
            TextureCacheDestroyTexture(this->m_layerTextures[i]);
            this->m_layerTextures[i] = NULL;
        }

        this->m_layerSections[i] = 0xFF;
    }

    this->m_layerCount = 0;
}

// PROVEN: cb1a1897 IDENTICAL/NONE
// OFFSET: 0x006F83B0
void CCharacterComponent::ResetCounts() {
    TextureCacheResetLoadCount();
}

// OFFSET: 0x006F83F0/0x1403C1360
static void DestroyVariationArrays() {
    for (uint32_t i = 0; i < CCharacterComponent::s_chrVarArrayLength; i++) {
        for (int32_t variation = 0; variation < NUM_COMPONENT_VARIATIONS; variation++) {
            CharVariationList& list = CCharacterComponent::s_chrVarArray[i].m_variation[variation];

            for (int32_t j = 0; j < list.m_numVariations; j++) {
                delete[] list.m_variation[j].m_color;
            }

            delete[] list.m_variation;
        }
    }

    delete[] CCharacterComponent::s_chrVarArray;
    CCharacterComponent::s_chrVarArray = NULL;
    CCharacterComponent::s_chrVarArrayLength = 0;

    if (CCharacterComponent::s_characterFacialHairStylesList) {
        SMemFree(CCharacterComponent::s_characterFacialHairStylesList, __FILE__, __LINE__, 0);
    }
}

// PROVEN: 1c129337 IDENTICAL/NONE
// OFFSET: 0x006F84D0
void CCharacterComponent::TextureBufferInitialize() {
    uint32_t size = 1 << CCharacterComponent::s_mipLevels;
    CCharacterComponent::s_textureBuffer = TextureAllocMippedImg(PIXEL_ARGB8888, size, size);

    if (CCharacterComponent::s_bComponentCompression) {
        s_compressedTextureBuffer = TextureAllocMippedImg(PIXEL_DXT1, size, size);
    }

    if (CCharacterComponent::s_bComponentThread && CCharacterComponent::s_bComponentCompression) {
        s_threadTextureBuffer = TextureAllocMippedImg(PIXEL_ARGB8888, size, size);
    }
}

// PROVEN: 8df01310 IDENTICAL/NONE
// OFFSET: 0x006F8530
static void TextureBufferDestroy() {
    uint32_t size = 1 << CCharacterComponent::s_mipLevels;

    if (CCharacterComponent::s_textureBuffer) {
        TextureFreeMippedImg(CCharacterComponent::s_textureBuffer, PIXEL_ARGB8888, size, size);
        CCharacterComponent::s_textureBuffer = NULL;
    }

    if (s_compressedTextureBuffer) {
        TextureFreeMippedImg(s_compressedTextureBuffer, PIXEL_DXT1, size, size);
        s_compressedTextureBuffer = NULL;
    }

    if (s_threadTextureBuffer) {
        TextureFreeMippedImg(s_threadTextureBuffer, PIXEL_ARGB8888, size, size);
        s_threadTextureBuffer = NULL;
    }
}

// OFFSET: 0x006F85A0/0x1403C1530
static void ComputeSectionInfo(uint32_t mipLevels) {
    if (mipLevels > 9) {
        mipLevels = 9;
    } else if (mipLevels < 7) {
        mipLevels = 7;
    }

    if (!s_useCompression && mipLevels > 8) {
        mipLevels = 8;
    }

    CCharacterComponent::s_mipLevels = mipLevels;
    uint32_t shift = 9 - mipLevels;

    for (uint32_t i = 0; i < NUM_COMPONENT_SECTIONS; i++) {
        s_sectionInfo[i].m_pos.x = CCharacterComponent::s_sectionInfoRaw[i].m_pos.x >> shift;
        s_sectionInfo[i].m_pos.y = CCharacterComponent::s_sectionInfoRaw[i].m_pos.y >> shift;
        s_sectionInfo[i].m_size.x = CCharacterComponent::s_sectionInfoRaw[i].m_size.x >> shift;
        s_sectionInfo[i].m_size.y = CCharacterComponent::s_sectionInfoRaw[i].m_size.y >> shift;
    }
}

// PROVEN: bc8e1539 IDENTICAL/NONE
// OFFSET: 0x006F87F0
void* CCharacterComponent::LoadItemComponentTexture(const ItemDisplayInfoRec* displayRec, COMPONENT_SECTIONS section) {
    SStrPrintf(s_path, sizeof(s_path), "Item\\TextureComponents\\%s\\%s_%s.blp", s_componentSections[section], static_cast<const char*>(displayRec->m_texture[section]), s_fileDecorations[2]);

    if (!SFile::FileExists(s_path)) {
        s_path[strlen(s_path) - 5] = s_fileDecorations[this->m_data.m_preferences.m_sexID][0];
    }

    return TextureCacheCreateTexture(s_path);
}

// PROVEN: 171d13e4 IDENTICAL/NONE
// OFFSET: 0x006F8870/0x1403C17A0
void CCharacterComponent::ApplyExtraTextures() {
    if (this->m_extraSkinTexture) {
        this->m_data.m_model->ReplaceTexture(8, this->m_extraSkinTexture);
        TextureRelease(this->m_extraSkinTexture);
    }

    if (this->m_hairTexture) {
        this->m_data.m_model->ReplaceTexture(6, this->m_hairTexture);
        TextureRelease(this->m_hairTexture);
    }
}

// OFFSET: 0x006F88C0/0x1403C1810
static void CompressMipLevels(MipBits* src, MipBits* dst, uint32_t mipLevels) {
    uint32_t size = 1 << mipLevels;

    for (uint32_t level = 0; level < mipLevels; level++) {
        if (size >= 4) {
            int32_t blocks = size / 4;
            uint32_t stride = size * 4;
            const uint8_t* srcRow = reinterpret_cast<const uint8_t*>(src->m_mip[level]);
            uint8_t* out = reinterpret_cast<uint8_t*>(dst->m_mip[level]);

            for (int32_t y = 0; y < blocks; y++) {
                for (int32_t x = 0; x < blocks; x++) {
                    SIMD::s_functions.CompressDXT1(srcRow, stride, out);
                    out += 8;
                    srcRow += 16;
                }

                srcRow += 3 * stride;
            }
        } else {
            SIMD::s_functions.CompressDXT1(reinterpret_cast<const uint8_t*>(src->m_mip[mipLevels - 2]), 16, reinterpret_cast<uint8_t*>(dst->m_mip[level]));
        }

        size >>= 1;
    }
}

// PROVEN: 34201937 IDENTICAL/NONE
// OFFSET: 0x006F8990
int32_t CCharacterComponent::SetClosestComponent(CCharacterComponent* component, int32_t a2, float distance) {
    int32_t sortVal = AsyncFileReadComputePriority(3, 0, a2, distance);

    if (sortVal < s_closestComponentSortVal) {
        s_closestComponent = component;
        s_closestComponentSortVal = sortVal;
    }

    return sortVal;
}

// PROVEN: b112e3f9 IDENTICAL/NONE
// OFFSET: 0x006F89F0/0x1403C19C0
CCharacterComponent* CCharacterComponent::GetClosestComponent() {
    return s_closestComponent;
}

// PROVEN: 31427ac3 IDENTICAL/NONE
// OFFSET: 0x006F8A00/0x1403C19D0
int32_t CCharacterComponent::GetClosestComponentSortVal() {
    return s_closestComponentSortVal;
}

// PROVEN: d4707b22 IDENTICAL/NONE
// OFFSET: 0x006F8A10
void CCharacterComponent::DestroyRenderTextures() {
    for (int32_t section = 0; section < NUM_COMPONENT_SECTIONS; section++) {
        for (int32_t layer = 0; layer < 8; layer++) {
            if (this->m_section[section].m_layerItemDisplayId[layer] && this->m_section[section].m_layerTex[layer]) {
                TextureCacheDestroyTexture(this->m_section[section].m_layerTex[layer]);
                this->m_section[section].m_layerTex[layer] = NULL;
            }
        }
    }
}

// OFFSET: 0x006F8A80
CharSectionsRec* CCharacterComponent::GetSectionsRecord(COMPONENT_VARIATIONS variation, int32_t variationIndex, int32_t colorIndex, bool* found) {
    return ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, variation, variationIndex, colorIndex, found);
}

// OFFSET: 0x006F8AB0
bool CCharacterComponent::CheckForComponentChanges(ComponentData* data) {
    if (!data) {
        return false;
    }

    CharacterPreferences& prefs = data->m_preferences;

    if (!prefs.m_raceID && !prefs.m_sexID && !prefs.m_skinID && !prefs.m_faceId && !prefs.m_hairStyleId && !prefs.m_hairColorId && !prefs.m_facialHairId) {
        return true;
    }

    if (prefs.m_raceID != this->m_data.m_preferences.m_raceID || prefs.m_sexID != this->m_data.m_preferences.m_sexID || prefs.m_skinID != this->m_data.m_preferences.m_skinID || prefs.m_faceId != this->m_data.m_preferences.m_faceId || prefs.m_hairStyleId != this->m_data.m_preferences.m_hairStyleId || prefs.m_hairColorId != this->m_data.m_preferences.m_hairColorId || prefs.m_facialHairId != this->m_data.m_preferences.m_facialHairId) {
        return true;
    }

    if (prefs.m_classId != this->m_data.m_preferences.m_classId && (prefs.m_classId == 6 || this->m_data.m_preferences.m_classId == 6)) {
        return true;
    }

    return false;
}

// OFFSET: 0x006F8B40/0x1403C5380
void CCharacterComponent::ReplaceExtraSkinTexture(const char* debug) {
    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0, this->m_data.m_preferences.m_skinID)) {
        return;
    }

    const char* textureName = CCharacterComponent::s_chrVarArray[this->m_data.m_preferences.m_raceID * 2 + this->m_data.m_preferences.m_sexID].m_variation[VARIATION_SKIN].m_variation[0].m_color[this->m_data.m_preferences.m_skinID]->m_textureName[1];

    if (!*textureName) {
        return;
    }

    strcpy(s_pathEnd, textureName);

    if (this->m_extraSkinTexture) {
        TextureRelease(this->m_extraSkinTexture);
    }

    CGxTexFlags flags(GxTex_LinearMipNearest, 0, 0, 0, 0, 1);
    this->m_extraSkinTexture = TextureCreate(s_path, flags, &s_status, 0, NULL, 3);
}

// OFFSET: 0x006F8BF0/0x1403C5490
void CCharacterComponent::ReplaceHairTexture(int32_t hairStyleID, const char* debug) {
    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_HAIR, hairStyleID, this->m_data.m_preferences.m_hairColorId)) {
        return;
    }

    CharSectionsRec* rec = CCharacterComponent::s_chrVarArray[this->m_data.m_preferences.m_raceID * 2 + this->m_data.m_preferences.m_sexID].m_variation[VARIATION_HAIR].m_variation[hairStyleID].m_color[this->m_data.m_preferences.m_hairColorId];

    if (!rec || !*rec->m_textureName[0]) {
        return;
    }

    strcpy(s_pathEnd, rec->m_textureName[0]);

    if (this->m_hairTexture) {
        TextureRelease(this->m_hairTexture);
    }

    CGxTexFlags flags(GxTex_LinearMipNearest, 0, 0, 0, 0, 1);
    this->m_hairTexture = TextureCreate(s_path, flags, &s_status, 0, NULL, 3);
}

// OFFSET: 0x006F8CB0
uint32_t CCharacterComponent::GetHandItemAttachIDs(uint32_t* ids, uint32_t maxCount) const {
    uint32_t count = 0;

    if (!this->m_attachMask) {
        return count;
    }

    if ((this->m_attachMask & 0x1) && maxCount) {
        ids[0] = 6;
        count = 1;
    }

    if ((this->m_attachMask & 0x2) && count < maxCount) {
        ids[count++] = 5;
    }

    if ((this->m_attachMask & 0x4) && count < maxCount) {
        ids[count++] = ATTACH_HELM;
    }

    if ((this->m_attachMask & 0x8) && count < maxCount) {
        ids[count++] = ATTACH_HANDRIGHT;
    }

    if ((this->m_attachMask & 0x10) && count < maxCount) {
        ids[count++] = ATTACH_HANDLEFT;
    }

    if ((this->m_attachMask & 0x20) && count < maxCount) {
        ids[count++] = ATTACH_SHIELD;
    }

    return count;
}

// OFFSET: 0x006F8D40
void CCharacterComponent::ComponentCloseFingers(CM2Model* model, COMP_HAND_SLOT hand) {
    uint32_t first;
    uint32_t last;

    if (hand) {
        first = 13;
        last = 17;
    } else {
        first = 8;
        last = 12;
    }

    for (uint32_t bone = first; bone <= last; bone++) {
        model->SetBoneSequence(bone, 15, 0xFFFFFFFF, 0, 1.0f, 0, 1, 0, 0, 0);
    }
}

// OFFSET: 0x006F8D90
void CCharacterComponent::ComponentOpenFingers(CM2Model* model, COMP_HAND_SLOT hand) {
    uint32_t first;
    uint32_t last;

    if (hand) {
        first = 13;
        last = 17;
    } else {
        first = 8;
        last = 12;
    }

    for (uint32_t bone = first; bone <= last; bone++) {
        model->UnsetBoneSequence(bone, 0, 1);
    }
}

// OFFSET: 0x006F8DD0/0x1403C1C90
void CCharacterComponent::ApplyMonsterGeosets(CM2Model* model, const CreatureDisplayInfoRec* displayRec) {
    if (!model || !displayRec || !displayRec->m_creatureGeosetData) {
        return;
    }

    for (uint32_t group = 100, shift = 0; group < 900; group += 100, shift += 4) {
        uint32_t geoset = (displayRec->m_creatureGeosetData >> shift) & 0xF;

        if (geoset) {
            model->SetGeometryVisible(group, group + 99, 0);
            model->SetGeometryVisible(group + geoset, group + geoset, 1);
        }
    }

    model->OptimizeVisibleGeometry();
}

// OFFSET: 0x006F8E40
GEOCOMPONENTLINKS CCharacterComponent::GetSheatheLink(SHEATHE_TYPE sheatheType, bool mainHand) {
    switch (sheatheType) {
        case SHEATHE_MAINHAND:
            return static_cast<GEOCOMPONENTLINKS>(ATTACH_SHEATH_OFFHAND - (mainHand != 0));

        case SHEATHE_BACK:
            return static_cast<GEOCOMPONENTLINKS>(ATTACH_BACK_OFFHAND - (mainHand != 0));

        case SHEATHE_HIP:
            return static_cast<GEOCOMPONENTLINKS>(ATTACH_HIP_OFFHAND - (mainHand != 0));

        case SHEATHE_SHIELD:
            return ATTACH_SHEATH_SHIELD;

        default:
            return ATTACH_NONE;
    }
}

// OFFSET: 0x006F8EA0/0x1403C1D80
void CCharacterComponent::RemoveLinkpt(CM2Model* model, uint8_t* attachMask, GEOCOMPONENTLINKS link) {
    if (link == ATTACH_NONE) {
        return;
    }

    switch (link) {
        case ATTACH_SHIELD:
            for (uint32_t bone = 13; bone <= 17; bone++) {
                model->UnsetBoneSequence(bone, 0, 1);
            }

            *attachMask &= ~0x20;
            break;

        case ATTACH_HANDRIGHT:
            for (uint32_t bone = 8; bone <= 12; bone++) {
                model->UnsetBoneSequence(bone, 0, 1);
            }

            *attachMask &= ~0x8;
            break;

        case ATTACH_HANDLEFT:
            for (uint32_t bone = 13; bone <= 17; bone++) {
                model->UnsetBoneSequence(bone, 0, 1);
            }

            *attachMask &= ~0x10;
            break;

        case ATTACH_SHOULDERRIGHT:
            *attachMask &= ~0x2;
            break;

        case ATTACH_SHOULDERLEFT:
            *attachMask &= ~0x1;
            break;

        case ATTACH_HELM:
            *attachMask &= ~0x4;
            break;

        default:
            break;
    }

    if (model) {
        model->DetachAllChildrenById(link);
    }
}

// PROVEN: 557dd2b4 IDENTICAL/NONE
// OFFSET: 0x006F8F80
INVENTORY_SLOTS CCharacterComponent::GetInvSlotForInvType(INVENTORY_TYPES invType) {
    switch (invType) {
        case INVTYPE_HEAD:
            return INVSLOT_HEAD;
        case INVTYPE_SHOULDERS:
            return INVSLOT_SHOULDER;
        case INVTYPE_BODY:
            return INVSLOT_BODY;
        case INVTYPE_CHEST:
        case INVTYPE_ROBE:
            return INVSLOT_CHEST;
        case INVTYPE_WAIST:
            return INVSLOT_WAIST;
        case INVTYPE_LEGS:
            return INVSLOT_LEGS;
        case INVTYPE_FEET:
            return INVSLOT_FEET;
        case INVTYPE_WRISTS:
            return INVSLOT_WRIST;
        case INVTYPE_HANDS:
            return INVSLOT_HAND;
        case INVTYPE_WEAPON:
        case INVTYPE_2HWEAPON:
        case INVTYPE_WEAPONMAINHAND:
            return INVSLOT_MAINHAND;
        case INVTYPE_SHIELD:
        case INVTYPE_WEAPONOFFHAND:
            return INVSLOT_OFFHAND;
        case INVTYPE_RANGED:
        case INVTYPE_HOLDABLE:
        case INVTYPE_RANGEDRIGHT:
            return INVSLOT_RANGED;
        case INVTYPE_CLOAK:
            return INVSLOT_BACK;
        case INVTYPE_TABARD:
            return INVSLOT_TABARD;
        default:
            return static_cast<INVENTORY_SLOTS>(86);
    }
}

// OFFSET: 0x006F9060
ITEM_SLOT CCharacterComponent::GetItemSlotForInvSlot(INVENTORY_SLOTS invSlot) {
    switch (invSlot) {
        case INVSLOT_HEAD:
            return ITEMSLOT_HEAD;

        case INVSLOT_SHOULDER:
            return ITEMSLOT_SHOULDER;

        case INVSLOT_BODY:
            return ITEMSLOT_SHIRT;

        case INVSLOT_CHEST:
            return ITEMSLOT_CHEST;

        case INVSLOT_WAIST:
            return ITEMSLOT_BELT;

        case INVSLOT_LEGS:
            return ITEMSLOT_PANTS;

        case INVSLOT_FEET:
            return ITEMSLOT_BOOTS;

        case INVSLOT_WRIST:
            return ITEMSLOT_BRACERS;

        case INVSLOT_HAND:
            return ITEMSLOT_GLOVES;

        case INVSLOT_BACK:
            return ITEMSLOT_CAPE;

        case INVSLOT_TABARD:
            return ITEMSLOT_TABARD;

        default:
            return NUM_ITEM_SLOTS;
    }
}

// OFFSET: 0x006F9110/0x1403C2040
int32_t CCharacterComponent::GetNumSkins(int32_t raceID, int32_t sexID, int32_t classID, COMPONENT_CONTEXT context) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, classID);
    int32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_SKIN, 0);

    if (numColors <= 0) {
        return 0;
    }

    int32_t count = 0;

    for (int32_t i = 0; i < numColors; i++) {
        CharSectionsRec* rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_SKIN, 0, i, NULL);

        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            count++;
        }
    }

    return count;
}

// OFFSET: 0x006F91A0/0x1403C2110
int32_t CCharacterComponent::GetNumHairStylesForColor(int32_t raceID, int32_t sexID, int32_t classID, int32_t hairColorID, COMPONENT_CONTEXT context) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, classID);
    int32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_HAIR);

    if (numVariations <= 0) {
        return 0;
    }

    int32_t count = 0;

    for (int32_t i = 0; i < numVariations; i++) {
        CharSectionsRec* rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_HAIR, i, hairColorID, NULL);

        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            count++;
        }
    }

    return count;
}

// OFFSET: 0x006F9240/0x1403C21E0
int32_t CCharacterComponent::GetNumHairColorsForStyle(int32_t raceID, int32_t sexID, int32_t classID, int32_t hairStyleID, COMPONENT_CONTEXT context) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, classID);
    int32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_HAIR, hairStyleID);

    if (numColors <= 0) {
        return 0;
    }

    int32_t count = 0;

    for (int32_t i = 0; i < numColors; i++) {
        CharSectionsRec* rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_HAIR, hairStyleID, i, NULL);

        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            count++;
        }
    }

    return count;
}

// OFFSET: 0x006F92E0/0x1403C22C0
int32_t CCharacterComponent::GetNumFacesForSkin(int32_t raceID, int32_t sexID, int32_t classID, int32_t skinID, COMPONENT_CONTEXT context) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, classID);
    int32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_FACE);

    if (numVariations <= 0) {
        return 0;
    }

    int32_t count = 0;

    for (int32_t i = 0; i < numVariations; i++) {
        CharSectionsRec* rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_FACE, i, skinID, NULL);

        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            count++;
        }
    }

    return count;
}

// PROVEN: ee105df0 IDENTICAL/NONE
// OFFSET: 0x006F9380/0x1403C2390
int32_t CCharacterComponent::GetNumFacialFeaturesForHairColor(int32_t raceID, int32_t sexID, int32_t classID, int32_t hairColorID, COMPONENT_CONTEXT context) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, classID);
    int32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_FACIAL_HAIR);

    if (!numVariations) {
        return CCharacterComponent::s_characterFacialHairStylesList[raceID * 2 + sexID];
    }

    int32_t count = 0;

    for (int32_t i = 0; i < numVariations; i++) {
        CharSectionsRec* rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_FACIAL_HAIR, i, hairColorID, NULL);

        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            count++;
        }
    }

    return count;
}

// OFFSET: 0x006F9420/0x1403C2480
static int32_t ComponentGetSkinColor(int32_t raceID, int32_t sexID, int32_t index, COMPONENT_SELECTION selection) {
    int32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_SKIN, 0);

    if (numColors <= 0) {
        return -1;
    }

    int32_t found = 0;

    for (int32_t i = 0; i < numColors; i++) {
        CharSectionsRec* rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_SKIN, 0, i, NULL);

        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            if (index == found) {
                return rec->m_colorIndex;
            }

            found++;
        }
    }

    return -1;
}

// OFFSET: 0x006F94B0/0x1403C2550
static int32_t ComponentGetHairVariation(int32_t raceID, int32_t sexID, int32_t hairColorID, int32_t index, COMPONENT_SELECTION selection) {
    int32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_HAIR);

    if (numVariations <= 0) {
        return -1;
    }

    int32_t found = 0;

    for (int32_t i = 0; i < numVariations; i++) {
        CharSectionsRec* rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_HAIR, i, hairColorID, NULL);

        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            if (index == found) {
                return rec->m_variationIndex;
            }

            found++;
        }
    }

    return -1;
}

// OFFSET: 0x006F9540/0x1403C2630
static int32_t ComponentGetHairColor(int32_t raceID, int32_t sexID, int32_t hairStyleID, int32_t index, COMPONENT_SELECTION selection) {
    int32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_HAIR, hairStyleID);

    if (numColors <= 0) {
        return -1;
    }

    int32_t found = 0;

    for (int32_t i = 0; i < numColors; i++) {
        CharSectionsRec* rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_HAIR, hairStyleID, i, NULL);

        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            if (index == found) {
                return rec->m_colorIndex;
            }

            found++;
        }
    }

    return -1;
}

// OFFSET: 0x006F95D0/0x1403C2710
static int32_t ComponentGetFaceVariation(int32_t raceID, int32_t sexID, int32_t skinID, int32_t index, COMPONENT_SELECTION selection) {
    int32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_FACE);

    if (numVariations <= 0) {
        return -1;
    }

    int32_t found = 0;

    for (int32_t i = 0; i < numVariations; i++) {
        CharSectionsRec* rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_FACE, i, skinID, NULL);

        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            if (index == found) {
                return rec->m_variationIndex;
            }

            found++;
        }
    }

    return -1;
}

// OFFSET: 0x006F9660/0x1403C27F0
int32_t CCharacterComponent::GetNthFacialFeatureIndex(int32_t raceID, int32_t sexID, int32_t classID, int32_t hairColorID, int32_t facialHairID, int32_t index, COMPONENT_SELECTION selection) {
    bool found;
    ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_FACIAL_HAIR, facialHairID, hairColorID, &found);

    if (!found) {
        if (index < static_cast<int32_t>(CCharacterComponent::s_characterFacialHairStylesList[raceID * 2 + sexID])) {
            return index;
        }

        return -1;
    }

    int32_t count = CCharacterComponent::GetNumFacialFeaturesForHairColor(raceID, sexID, classID, hairColorID, GetContextFromSelection(selection));

    if (count <= 0) {
        return -1;
    }

    int32_t matched = 0;

    for (int32_t i = 0; i < count; i++) {
        CharSectionsRec* rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, raceID, sexID, VARIATION_FACIAL_HAIR, i, hairColorID, NULL);

        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            if (index == matched) {
                return rec->m_variationIndex;
            }

            matched++;
        }
    }

    return -1;
}

// OFFSET: 0x006F9740/0x1403C2960
char* CCharacterComponent::GetTabardBackgroundFileName(uint32_t flags, COMPONENT_SECTIONS section, int32_t background, char* buffer, int32_t size) {
    *buffer = '\0';

    if (flags & 0x10) {
        if (section == SECTION_TORSO_UPPER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\EpicBG_%02d_TU_U", background);
        } else if (section == SECTION_TORSO_LOWER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\EpicBG_%02d_TL_U", background);
        }
    } else if (flags & 0x8) {
        if (section == SECTION_TORSO_UPPER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\RareBG_%02d_TU_U", background);
        } else if (section == SECTION_TORSO_LOWER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\RareBG_%02d_TL_U", background);
        }
    } else if (section == SECTION_TORSO_UPPER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Background_%02d_TU_U", background);
    } else if (section == SECTION_TORSO_LOWER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Background_%02d_TL_U", background);
    }

    return buffer;
}

// OFFSET: 0x006F9810/0x1403C2A40
char* CCharacterComponent::GetTabardEmblemFileName(COMPONENT_SECTIONS section, int32_t emblem, int32_t color, char* buffer, int32_t size) {
    *buffer = '\0';

    if (section == SECTION_TORSO_UPPER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Emblem_%02d_%02d_TU_U", emblem, color);
    } else if (section == SECTION_TORSO_LOWER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Emblem_%02d_%02d_TL_U", emblem, color);
    }

    return buffer;
}

// OFFSET: 0x006F9860/0x1403C2AA0
int32_t CCharacterComponent::GetProceduralEmblemFileName(int32_t emblem, char* buffer, int32_t size) {
    if (!buffer || static_cast<uint32_t>(emblem) >= 196) {
        return 0;
    }

    SStrPrintf(buffer, size, s_proceduralEmblemFormat, emblem);
    return 1;
}

// OFFSET: 0x006F98A0/0x1403C2AE0
char* CCharacterComponent::GetTabardBorderFileName(uint32_t flags, COMPONENT_SECTIONS section, int32_t border, int32_t color, char* buffer, int32_t size) {
    *buffer = '\0';

    if (flags & 0x10) {
        if (section == SECTION_TORSO_UPPER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\EpicBorder_%02d_TU_U", color);
        } else if (section == SECTION_TORSO_LOWER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\EpicBorder_%02d_TL_U", color);
        }
    } else if (flags & 0x8) {
        if (section == SECTION_TORSO_UPPER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\RareBorder_%02d_TU_U", color);
        } else if (section == SECTION_TORSO_LOWER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\RareBorder_%02d_TL_U", color);
        }
    } else if (section == SECTION_TORSO_UPPER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Border_%02d_%02d_TU_U", border, color);
    } else if (section == SECTION_TORSO_LOWER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Border_%02d_%02d_TL_U", border, color);
    }

    return buffer;
}

// OFFSET: 0x006F9980/0x1403C2BB0
char* CCharacterComponent::GetShirtBackgroundFileName(uint32_t flags, COMPONENT_SECTIONS section, int32_t background, char* buffer, int32_t size) {
    *buffer = '\0';

    if (flags & 0x10) {
        if (section == SECTION_TORSO_UPPER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_EpicBG_%02d_TU_U", background);
        } else if (section == SECTION_TORSO_LOWER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_EpicBG_%02d_TL_U", background);
        } else if (section == SECTION_ARM_UPPER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_EpicBG_%02d_AU_U", background);
        } else if (section == SECTION_ARM_LOWER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_EpicBG_%02d_AL_U", background);
        }
    } else if (flags & 0x8) {
        if (section == SECTION_TORSO_UPPER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_RareBG_%02d_TU_U", background);
        } else if (section == SECTION_TORSO_LOWER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_RareBG_%02d_TL_U", background);
        } else if (section == SECTION_ARM_UPPER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_RareBG_%02d_AU_U", background);
        } else if (section == SECTION_ARM_LOWER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_RareBG_%02d_AL_U", background);
        }
    } else if (section == SECTION_TORSO_UPPER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_CommonBG_%02d_TU_U", background);
    } else if (section == SECTION_TORSO_LOWER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_CommonBG_%02d_TL_U", background);
    } else if (section == SECTION_ARM_UPPER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_CommonBG_%02d_AU_U", background);
    } else if (section == SECTION_ARM_LOWER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_CommonBG_%02d_AL_U", background);
    }

    return buffer;
}

// OFFSET: 0x006F9B00/0x1403C2D60
char* CCharacterComponent::GetShirtBorderFileName(uint32_t flags, COMPONENT_SECTIONS section, int32_t border, int32_t color, char* buffer, int32_t size) {
    *buffer = '\0';

    if (flags & 0x10) {
        if (section == SECTION_TORSO_UPPER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_EpicBorder_%02d_TU_U", color);
        } else if (section == SECTION_TORSO_LOWER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_EpicBorder_%02d_TL_U", color);
        } else if (section == SECTION_ARM_UPPER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_EpicBorder_%02d_AU_U", color);
        } else if (section == SECTION_ARM_LOWER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_EpicBorder_%02d_AL_U", color);
        }
    } else if (flags & 0x8) {
        if (section == SECTION_TORSO_UPPER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_RareBorder_%02d_TU_U", color);
        } else if (section == SECTION_TORSO_LOWER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_RareBorder_%02d_TL_U", color);
        } else if (section == SECTION_ARM_UPPER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_RareBorder_%02d_AU_U", color);
        } else if (section == SECTION_ARM_LOWER) {
            SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_RareBorder_%02d_AL_U", color);
        }
    } else if (section == SECTION_TORSO_UPPER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_CommonBorder_%02d_TU_U", color);
    } else if (section == SECTION_TORSO_LOWER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_CommonBorder_%02d_TL_U", color);
    } else if (section == SECTION_ARM_UPPER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_CommonBorder_%02d_AU_U", color);
    } else if (section == SECTION_ARM_LOWER) {
        SStrPrintf(buffer, size, "Textures\\GuildEmblems\\Shirt01_CommonBorder_%02d_AL_U", color);
    }

    return buffer;
}

// OFFSET: 0x006F9D00
void CCharacterComponent::GetPreferences(CharacterPreferences* preferences) {
    if (preferences) {
        *preferences = this->m_data.m_preferences;
    }
}

// PROVEN: 634ec5a8 IDENTICAL/NONE
// OFFSET: 0x006F9D40
void CCharacterComponent::SetDirtySections(uint32_t mask) {
    this->m_dirtySections |= mask;

    if (this->m_request) {
        this->m_request->m_active = 0;
        this->m_request = NULL;
    }

    this->m_flags &= ~0x8;
}

// PROVEN: 36684900 IDENTICAL/NONE
// OFFSET: 0x006F9D70
void CCharacterComponent::SetTexturesDirty() {
    this->m_flags |= 0x1;

    if (this->m_request) {
        this->m_request->m_active = 0;
        this->m_request = NULL;
    }

    this->m_flags &= ~0x8;
}

// PROVEN: feb0fd8a IDENTICAL/NONE
// OFFSET: 0x006F9D90
void CCharacterComponent::ReleaseBaseTexture() {
    if (this->m_baseTexture) {
        GxTexSetCannotUpdate(TextureGetGxTex(this->m_baseTexture));
        TextureRelease(this->m_baseTexture);
        this->m_baseTexture = NULL;
    }
}

// OFFSET: 0x006F9DC0
bool CCharacterComponent::ItemDisplayHasGeosetGroup(uint32_t displayID, int32_t group, ItemDisplayInfoRec& rec) {
    if (!displayID) {
        return false;
    }

    return g_itemDisplayInfoDB.GetRecord(displayID, &rec) && rec.m_geosetGroup[group];
}

// OFFSET: 0x006F9E20
bool CCharacterComponent::ItemDisplayHasGeosetGroup(uint32_t displayID, int32_t group) {
    ItemDisplayInfoRec rec;

    return this->ItemDisplayHasGeosetGroup(displayID, group, rec);
}

// OFFSET: 0x006F9E40
void CCharacterComponent::GeosRenderPrep() {
    bool eyeGlow = false;

    if (this->m_data.m_preferences.m_classId == 6) {
        eyeGlow = true;
    } else {
        CharSectionsRec* faceRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_FACE, this->m_data.m_preferences.m_faceId, this->m_data.m_preferences.m_skinID, NULL);

        if (faceRec && (faceRec->m_flags & 0x4)) {
            eyeGlow = true;
        }
    }

    CM2Model* model = this->m_data.m_model;
    model->SetGeometryVisible(0, 2000, 0);
    model->SetGeometryVisible(0, 0, 1);

    for (int32_t i = 0; i < 19; i++) {
        if (i == 17 && eyeGlow) {
            this->m_data.m_model->SetGeometryVisible(1703, 1703, 1);
        } else {
            this->m_data.m_model->SetGeometryVisible(this->m_data.m_geosets[i], this->m_data.m_geosets[i], 1);
        }
    }

    ItemDisplayInfoRec rec;

    if (this->m_itemDisplayID[ITEMSLOT_GLOVES] && g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_GLOVES], &rec) && rec.m_geosetGroup[0]) {
        this->m_data.m_model->SetGeometryVisible(401, 499, 0);
        this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[0] + 401, rec.m_geosetGroup[0] + 401, 1);
    } else if (this->m_itemDisplayID[ITEMSLOT_CHEST] && g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_CHEST], &rec) && rec.m_geosetGroup[0]) {
        this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[0] + 801, rec.m_geosetGroup[0] + 801, 1);
    }

    int32_t layer;

    for (layer = 3; layer < 8; layer++) {
        if ((1 << layer) & this->m_section[SECTION_ARM_LOWER].m_layerMask) {
            break;
        }
    }

    if (layer >= 8 && this->m_itemDisplayID[ITEMSLOT_SHIRT] && g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_SHIRT], &rec) && rec.m_geosetGroup[0]) {
        this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[0] + 801, rec.m_geosetGroup[0] + 801, 1);
    }

    bool robe = false;
    bool dress = false;
    bool tabard = false;

    if (this->m_itemDisplayID[ITEMSLOT_CHEST] && g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_CHEST], &rec) && rec.m_geosetGroup[2]) {
        this->m_data.m_model->SetGeometryVisible(501, 599, 0);
        this->m_data.m_model->SetGeometryVisible(902, 999, 0);
        this->m_data.m_model->SetGeometryVisible(1100, 1199, 0);
        this->m_data.m_model->SetGeometryVisible(1300, 1399, 0);
        this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[2] + 1301, rec.m_geosetGroup[2] + 1301, 1);
        robe = true;
    } else if (this->m_itemDisplayID[ITEMSLOT_PANTS] && g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_PANTS], &rec) && rec.m_geosetGroup[2] && !(this->m_flags & 0x20)) {
        this->m_data.m_model->SetGeometryVisible(501, 599, 0);
        this->m_data.m_model->SetGeometryVisible(902, 999, 0);
        this->m_data.m_model->SetGeometryVisible(1100, 1199, 0);
        this->m_data.m_model->SetGeometryVisible(1300, 1399, 0);
        this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[2] + 1301, rec.m_geosetGroup[2] + 1301, 1);
        dress = true;
    } else {
        if (this->m_itemDisplayID[ITEMSLOT_BOOTS] && g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_BOOTS], &rec) && rec.m_geosetGroup[0]) {
            this->m_data.m_model->SetGeometryVisible(501, 599, 0);
            this->m_data.m_model->SetGeometryVisible(901, 901, 1);
            this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[0] + 501, rec.m_geosetGroup[0] + 501, 1);
        } else if (this->m_itemDisplayID[ITEMSLOT_PANTS] && g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_PANTS], &rec) && rec.m_geosetGroup[1] && !(this->m_flags & 0x20)) {
            this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[1] + 901, rec.m_geosetGroup[1] + 901, 1);
        } else {
            this->m_data.m_model->SetGeometryVisible(901, 901, 1);
        }

        if (this->m_itemDisplayID[ITEMSLOT_TABARD] && g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_TABARD], &rec) && rec.m_geosetGroup[0]) {
            this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[0] + 1201, rec.m_geosetGroup[0] + 1201, 1);
            tabard = true;
        }
    }

    if (this->m_flags & 0x40) {
        this->m_data.m_model->SetGeometryVisible(1201, 1201, 1);

        if (!robe && !dress) {
            this->m_data.m_model->SetGeometryVisible(1202, 1202, 1);
        }
    }

    if (!robe) {
        if (!tabard) {
            if (this->m_itemDisplayID[ITEMSLOT_CHEST] && g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_CHEST], &rec) && rec.m_geosetGroup[1]) {
                this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[1] + 1001, rec.m_geosetGroup[1] + 1001, 1);
            } else if (this->m_itemDisplayID[ITEMSLOT_SHIRT] && g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_SHIRT], &rec) && rec.m_geosetGroup[1]) {
                this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[1] + 1001, rec.m_geosetGroup[1] + 1001, 1);
            }
        }

        if (!(this->m_flags & 0x20) && this->m_itemDisplayID[ITEMSLOT_PANTS] && g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_PANTS], &rec) && rec.m_geosetGroup[0]) {
            if (rec.m_geosetGroup[0] > 2) {
                this->m_data.m_model->SetGeometryVisible(1300, 1399, 0);
                this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[0] + 1101, rec.m_geosetGroup[0] + 1101, 1);
            } else if (!tabard) {
                this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[0] + 1101, rec.m_geosetGroup[0] + 1101, 1);
            }
        }
    }

    if (this->m_itemDisplayID[ITEMSLOT_CAPE] && g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_CAPE], &rec) && rec.m_geosetGroup[0]) {
        this->m_data.m_model->SetGeometryVisible(1500, 1599, 0);
        this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[0] + 1501, rec.m_geosetGroup[0] + 1501, 1);
    }

    if (this->m_itemDisplayID[ITEMSLOT_BELT] && g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_BELT], &rec) && rec.m_geosetGroup[0]) {
        this->m_data.m_model->SetGeometryVisible(1800, 1899, 0);
        this->m_data.m_model->SetGeometryVisible(rec.m_geosetGroup[0] + 1801, rec.m_geosetGroup[0] + 1801, 1);
    }

    this->m_data.m_model->OptimizeVisibleGeometry();
    this->m_flags &= ~0x4;
}

// OFFSET: 0x006FA550
void CCharacterComponent::PrepSections() {
    uint32_t size = 1 << this->m_mipLevels;

    for (int32_t section = 0; section < NUM_COMPONENT_SECTIONS; section++) {
        if (!((1 << section) & this->m_dirtySections)) {
            continue;
        }

        (this->*CCharacterComponent::m_prepFunc[section])();

        if (!(this->m_flags & 0x1) && this->m_gxTexFormat != GxTex_Dxt1) {
            const CompSectionInfo& info = s_sectionInfo[section];
            GxTexUpdate(TextureGetGxTex(this->m_baseTexture), info.m_pos.x, info.m_pos.y, info.m_pos.x + info.m_size.x, info.m_pos.y + info.m_size.y, 1);
        }
    }

    if (!(this->m_flags & 0x1) && this->m_gxTexFormat != GxTex_Dxt1) {
        return;
    }

    if (this->m_gxTexFormat == GxTex_Dxt1) {
        CompressMipLevels(CCharacterComponent::s_textureBuffer, s_compressedTextureBuffer, this->m_mipLevels);
    }

    if (this->m_flags & 0x1) {
        GxTexUpdate(TextureGetGxTex(this->m_baseTexture), 0, 0, size, size, 1);
        return;
    }

    for (int32_t section = 0; section < NUM_COMPONENT_SECTIONS; section++) {
        if ((1 << section) & this->m_dirtySections) {
            const CompSectionInfo& info = s_sectionInfo[section];
            GxTexUpdate(TextureGetGxTex(this->m_baseTexture), info.m_pos.x, info.m_pos.y, info.m_pos.x + info.m_size.x, info.m_pos.y + info.m_size.y, 1);
        }
    }
}

// OFFSET: 0x006FA690
void CCharacterComponent::CreateRequestAU() {
    this->m_request->m_baseTextures[SECTION_ARM_UPPER] = TextureCacheCopyTexture(this->m_baseVariation[VARIATION_SKIN].m_texture[0]);

    for (int32_t layer = 0; layer < 4; layer++) {
        if ((1 << layer) & this->m_section[SECTION_ARM_UPPER].m_layerMask) {
            this->m_request->AddLayer(SECTION_ARM_UPPER, this->m_section[SECTION_ARM_UPPER].m_layerTex[layer]);
        }
    }
}

// OFFSET: 0x006FA720
void CCharacterComponent::CreateRequestAL() {
    this->m_request->m_baseTextures[SECTION_ARM_LOWER] = TextureCacheCopyTexture(this->m_baseVariation[VARIATION_SKIN].m_texture[0]);

    for (int32_t layer = 0; layer < 8; layer++) {
        if ((1 << layer) & this->m_section[SECTION_ARM_LOWER].m_layerMask) {
            this->m_request->AddLayer(SECTION_ARM_LOWER, this->m_section[SECTION_ARM_LOWER].m_layerTex[layer]);
        }
    }
}

// OFFSET: 0x006FA7B0
void CCharacterComponent::CreateRequestHA() {
    this->m_request->m_baseTextures[SECTION_HAND] = TextureCacheCopyTexture(this->m_baseVariation[VARIATION_SKIN].m_texture[0]);

    if (this->m_section[SECTION_HAND].m_layerMask & 0x1) {
        this->m_request->AddLayer(SECTION_HAND, this->m_section[SECTION_HAND].m_layerTex[0]);
    }
}

// OFFSET: 0x006FA810
void CCharacterComponent::CreateRequestHU() {
    CharSectionsRec* skinRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0, this->m_data.m_preferences.m_skinID, NULL);

    if (skinRec && (skinRec->m_flags & 0x8)) {
        this->m_request->m_baseTextures[SECTION_HEAD_UPPER] = TextureCacheCopyTexture(this->m_baseVariation[VARIATION_SKIN].m_texture[0]);
    }

    if (this->m_baseVariation[VARIATION_FACE].m_texture[1]) {
        this->m_request->AddLayer(SECTION_HEAD_UPPER, this->m_baseVariation[VARIATION_FACE].m_texture[1]);
    }

    if (this->m_baseVariation[VARIATION_FACIAL_HAIR].m_texture[1]) {
        this->m_request->AddLayer(SECTION_HEAD_UPPER, this->m_baseVariation[VARIATION_FACIAL_HAIR].m_texture[1]);
    }

    if (this->m_baseVariation[VARIATION_HAIR].m_texture[2]) {
        this->m_request->AddLayer(SECTION_HEAD_UPPER, this->m_baseVariation[VARIATION_HAIR].m_texture[2]);
    }
}

// OFFSET: 0x006FA900
void CCharacterComponent::CreateRequestHL() {
    CharSectionsRec* skinRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0, this->m_data.m_preferences.m_skinID, NULL);

    if (skinRec && (skinRec->m_flags & 0x8)) {
        this->m_request->m_baseTextures[SECTION_HEAD_LOWER] = TextureCacheCopyTexture(this->m_baseVariation[VARIATION_SKIN].m_texture[0]);
    }

    if (this->m_baseVariation[VARIATION_FACE].m_texture[0]) {
        this->m_request->AddLayer(SECTION_HEAD_LOWER, this->m_baseVariation[VARIATION_FACE].m_texture[0]);
    }

    if (this->m_baseVariation[VARIATION_FACIAL_HAIR].m_texture[0]) {
        this->m_request->AddLayer(SECTION_HEAD_LOWER, this->m_baseVariation[VARIATION_FACIAL_HAIR].m_texture[0]);
    }

    if (this->m_baseVariation[VARIATION_HAIR].m_texture[1]) {
        this->m_request->AddLayer(SECTION_HEAD_LOWER, this->m_baseVariation[VARIATION_HAIR].m_texture[1]);
    }
}

// OFFSET: 0x006FA9F0
void CCharacterComponent::CreateRequestTU() {
    this->m_request->m_baseTextures[SECTION_TORSO_UPPER] = TextureCacheCopyTexture(this->m_baseVariation[VARIATION_SKIN].m_texture[0]);
    uint32_t mask = this->m_section[SECTION_TORSO_UPPER].m_layerMask & 0x7F;

    if (!mask && this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1]) {
        this->m_request->AddLayer(SECTION_TORSO_UPPER, this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1]);
    }

    for (int32_t layer = 0; layer < 7; layer++) {
        if ((1 << layer) & mask) {
            this->m_request->AddLayer(SECTION_TORSO_UPPER, this->m_section[SECTION_TORSO_UPPER].m_layerTex[layer]);
        }
    }
}

// OFFSET: 0x006FAAE0
void CCharacterComponent::CreateRequestTL() {
    this->m_request->m_baseTextures[SECTION_TORSO_LOWER] = TextureCacheCopyTexture(this->m_baseVariation[VARIATION_SKIN].m_texture[0]);

    for (int32_t layer = 0; layer < 8; layer++) {
        if ((1 << layer) & this->m_section[SECTION_TORSO_LOWER].m_layerMask) {
            this->m_request->AddLayer(SECTION_TORSO_LOWER, this->m_section[SECTION_TORSO_LOWER].m_layerTex[layer]);
        }
    }
}

// OFFSET: 0x006FAB70
void CCharacterComponent::CreateRequestLU() {
    this->m_request->m_baseTextures[SECTION_LEG_UPPER] = TextureCacheCopyTexture(this->m_baseVariation[VARIATION_SKIN].m_texture[0]);

    if (((this->m_flags & 0x20) || !(this->m_section[SECTION_LEG_UPPER].m_layerMask & 0x3)) && this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[0]) {
        this->m_request->AddLayer(SECTION_LEG_UPPER, this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[0]);
    }

    int32_t first = (this->m_flags & 0x20) != 0;
    int32_t last = 3 - first;

    for (int32_t layer = first; layer < last; layer++) {
        if ((1 << layer) & this->m_section[SECTION_LEG_UPPER].m_layerMask) {
            this->m_request->AddLayer(SECTION_LEG_UPPER, this->m_section[SECTION_LEG_UPPER].m_layerTex[layer]);
        }
    }
}

// OFFSET: 0x006FAC60
void CCharacterComponent::CreateRequestLL() {
    this->m_request->m_baseTextures[SECTION_LEG_LOWER] = TextureCacheCopyTexture(this->m_baseVariation[VARIATION_SKIN].m_texture[0]);

    int32_t first = (this->m_flags & 0x20) != 0;
    int32_t last = 2 * ((this->m_flags & 0x20) == 0) + 4;

    for (int32_t layer = first; layer < last; layer++) {
        if ((1 << layer) & this->m_section[SECTION_LEG_LOWER].m_layerMask) {
            this->m_request->AddLayer(SECTION_LEG_LOWER, this->m_section[SECTION_LEG_LOWER].m_layerTex[layer]);
        }
    }
}

// OFFSET: 0x006FAD10
void CCharacterComponent::CreateRequestFO() {
    this->m_request->m_baseTextures[SECTION_FOOT] = TextureCacheCopyTexture(this->m_baseVariation[VARIATION_SKIN].m_texture[0]);

    if (this->m_section[SECTION_FOOT].m_layerMask & 0x1) {
        this->m_request->AddLayer(SECTION_FOOT, this->m_section[SECTION_FOOT].m_layerTex[0]);
    }
}

// PROVEN: ddea1587 IDENTICAL/NONE
// OFFSET: 0x006FAD70
void CCharacterComponent::UpdateSections() {
    uint32_t size = 1 << this->m_mipLevels;

    if (this->m_flags & 0x1) {
        GxTexUpdate(TextureGetGxTex(this->m_baseTexture), 0, 0, size, size, 1);
        return;
    }

    for (int32_t section = 0; section < NUM_COMPONENT_SECTIONS; section++) {
        if ((1 << section) & this->m_dirtySections) {
            const CompSectionInfo& info = s_sectionInfo[section];
            GxTexUpdate(TextureGetGxTex(this->m_baseTexture), info.m_pos.x, info.m_pos.y, info.m_pos.x + info.m_size.x, info.m_pos.y + info.m_size.y, 1);
        }
    }
}

// OFFSET: 0x006FAE00/0x1403C4FA0
void CCharacterComponent::ValidateComponentData(ComponentData* data, COMPONENT_CONTEXT context) {
    CharacterPreferences& prefs = data->m_preferences;
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, prefs.m_classId);

    if (!ComponentValidateSkin(CCharacterComponent::s_chrVarArray, prefs.m_raceID, prefs.m_sexID, prefs.m_classId, prefs.m_skinID, context)) {
        int32_t numSkins = CCharacterComponent::GetNumSkins(prefs.m_raceID, prefs.m_sexID, prefs.m_classId, context);

        if (numSkins <= 0) {
            prefs.m_skinID = 0;
        } else {
            prefs.m_skinID = ComponentGetSkinColor(prefs.m_raceID, prefs.m_sexID, static_cast<int32_t>(prefs.m_skinID) % numSkins, selection);
        }
    }

    if (!ComponentValidateFace(CCharacterComponent::s_chrVarArray, prefs.m_raceID, prefs.m_sexID, prefs.m_classId, prefs.m_skinID, prefs.m_faceId, context)) {
        int32_t numFaces = CCharacterComponent::GetNumFacesForSkin(prefs.m_raceID, prefs.m_sexID, prefs.m_classId, prefs.m_skinID, context);

        if (numFaces <= 0) {
            prefs.m_faceId = 0;
        } else {
            prefs.m_faceId = ComponentGetFaceVariation(prefs.m_raceID, prefs.m_sexID, prefs.m_skinID, static_cast<int32_t>(prefs.m_faceId) % numFaces, selection);
        }
    }

    if (!ComponentValidateHair(CCharacterComponent::s_chrVarArray, prefs.m_raceID, prefs.m_sexID, prefs.m_classId, prefs.m_hairColorId, prefs.m_hairStyleId, context)) {
        int32_t numStyles = CCharacterComponent::GetNumHairStylesForColor(prefs.m_raceID, prefs.m_sexID, prefs.m_classId, prefs.m_hairColorId, context);

        if (numStyles > 0) {
            prefs.m_hairStyleId = ComponentGetHairVariation(prefs.m_raceID, prefs.m_sexID, prefs.m_hairColorId, static_cast<int32_t>(prefs.m_hairStyleId) % numStyles, selection);
        } else {
            int32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, prefs.m_raceID, prefs.m_sexID, VARIATION_HAIR);
            int32_t* numColors = static_cast<int32_t*>(alloca(sizeof(int32_t) * numVariations));
            int32_t numValid = 0;

            for (int32_t i = 0; i < numVariations; i++) {
                numColors[i] = CCharacterComponent::GetNumHairColorsForStyle(prefs.m_raceID, prefs.m_sexID, prefs.m_classId, i, context);

                if (numColors[i] > 0) {
                    numValid++;
                }
            }

            if (numValid <= 0) {
                prefs.m_hairColorId = 0;
                prefs.m_hairStyleId = 0;
            } else {
                for (int32_t i = 0; i < numVariations; i++) {
                    if (numColors[i] > 0 && static_cast<int32_t>(prefs.m_hairStyleId) % numValid == i) {
                        prefs.m_hairStyleId = i;
                        break;
                    }
                }

                prefs.m_hairColorId = ComponentGetHairColor(prefs.m_raceID, prefs.m_sexID, prefs.m_hairStyleId, static_cast<int32_t>(prefs.m_hairColorId) % numColors[prefs.m_hairStyleId], selection);
            }
        }
    }

    if (!ComponentValidateFacialFeature(CCharacterComponent::s_chrVarArray, CCharacterComponent::s_characterFacialHairStylesList, prefs.m_raceID, prefs.m_sexID, prefs.m_classId, prefs.m_hairColorId, prefs.m_facialHairId, context)) {
        int32_t numFeatures = CCharacterComponent::GetNumFacialFeaturesForHairColor(prefs.m_raceID, prefs.m_sexID, prefs.m_classId, prefs.m_hairColorId, context);

        if (numFeatures <= 0) {
            prefs.m_facialHairId = 0;
        } else {
            prefs.m_facialHairId = CCharacterComponent::GetNthFacialFeatureIndex(prefs.m_raceID, prefs.m_sexID, prefs.m_classId, prefs.m_hairColorId, prefs.m_facialHairId, static_cast<int32_t>(prefs.m_facialHairId) % numFeatures, selection);
        }
    }
}

// OFFSET: 0x006FB0B0/0x1403C52A0
static CharacterFacialHairStylesRec* GetConditionalFacialHairStyle(CharacterPreferences* prefs) {
    for (int32_t i = 0; i < g_characterFacialHairStylesDB.m_numRecords; i++) {
        CharacterFacialHairStylesRec* rec = g_characterFacialHairStylesDB.GetRecordByIndex(i);

        if (!rec) {
            return NULL;
        }

        if (prefs->m_raceID == rec->m_raceID && prefs->m_sexID == rec->m_sexID && prefs->m_facialHairId == rec->m_variationID) {
            return rec;
        }
    }

    return NULL;
}

// OFFSET: 0x006FB100/0x1403C5300
static int32_t GetConditionalGeoset(CharacterPreferences* prefs) {
    for (int32_t i = 0; i < g_charHairGeosetsDB.m_numRecords; i++) {
        CharHairGeosetsRec* rec = g_charHairGeosetsDB.GetRecordByIndex(i);

        if (!rec) {
            return 1;
        }

        if (prefs->m_raceID == rec->m_raceID && prefs->m_sexID == rec->m_sexID && prefs->m_hairStyleId == rec->m_variationID) {
            if (rec->m_geosetID > 0) {
                return rec->m_geosetID;
            }

            return 1;
        }
    }

    return 1;
}

// OFFSET: 0x006FB160/0x1403C55C0
void CCharacterComponent::LoadBaseVariation(COMPONENT_VARIATIONS variation, int32_t textureIndex, int32_t variationIndex, int32_t colorIndex, COMPONENT_SECTIONS section, const char* debug) {
    void*& texture = this->m_baseVariation[variation].m_texture[textureIndex];

    if (texture) {
        TextureCacheDestroyTexture(texture);
        texture = NULL;
    }

    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, variation, variationIndex, colorIndex)) {
        return;
    }

    CharSectionsRec* rec = CCharacterComponent::s_chrVarArray[this->m_data.m_preferences.m_raceID * 2 + this->m_data.m_preferences.m_sexID].m_variation[variation].m_variation[variationIndex].m_color[colorIndex];

    if (!rec) {
        return;
    }

    const char* textureName = rec->m_textureName[textureIndex];

    if (*textureName) {
        strcpy(s_pathEnd, textureName);
        texture = TextureCacheCreateTexture(s_path);

        if (!texture) {
            DebugLog("**** Unable to Load Texture %s\n", s_path);
        }
    }

    this->SetDirtySections(1 << section);
}

// OFFSET: 0x006FB260/0x1403C5740
void CCharacterComponent::SetHairColor(int32_t hairColorID, bool loadHair, const char* debug) {
    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_HAIR, this->m_data.m_preferences.m_hairStyleId, hairColorID)) {
        return;
    }

    this->m_data.m_preferences.m_hairColorId = hairColorID;
    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(this->m_data.m_preferences.m_raceID);

    if (!this->m_data.m_preferences.m_sexID && !this->m_data.m_preferences.m_hairStyleId && (raceRec->m_flags & 0x8)) {
        this->ReplaceHairTexture(1, debug);
    }

    this->ReplaceHairTexture(this->m_data.m_preferences.m_hairStyleId, debug);

    if (this->m_data.m_flags & 0x1) {
        return;
    }

    if (loadHair) {
        this->LoadBaseVariation(VARIATION_HAIR, 1, this->m_data.m_preferences.m_hairStyleId, this->m_data.m_preferences.m_hairColorId, SECTION_HEAD_LOWER, debug);
        this->LoadBaseVariation(VARIATION_HAIR, 2, this->m_data.m_preferences.m_hairStyleId, this->m_data.m_preferences.m_hairColorId, SECTION_HEAD_UPPER, debug);
    }

    this->LoadBaseVariation(VARIATION_FACIAL_HAIR, 0, this->m_data.m_preferences.m_facialHairId, this->m_data.m_preferences.m_hairColorId, SECTION_HEAD_LOWER, debug);
    this->LoadBaseVariation(VARIATION_FACIAL_HAIR, 1, this->m_data.m_preferences.m_facialHairId, this->m_data.m_preferences.m_hairColorId, SECTION_HEAD_UPPER, debug);
}

// OFFSET: 0x006FB350/0x1403C58B0
void CCharacterComponent::SetHairStyle(int32_t hairStyleID, const char* debug) {
    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_HAIR, hairStyleID, this->m_data.m_preferences.m_hairColorId)) {
        return;
    }

    this->m_data.m_preferences.m_hairStyleId = hairStyleID;
    this->m_data.m_geosets[0] = GetConditionalGeoset(&this->m_data.m_preferences);

    if (!(this->m_data.m_flags & 0x1)) {
        this->LoadBaseVariation(VARIATION_HAIR, 1, hairStyleID, this->m_data.m_preferences.m_hairColorId, SECTION_HEAD_LOWER, debug);
        this->LoadBaseVariation(VARIATION_HAIR, 2, this->m_data.m_preferences.m_hairStyleId, this->m_data.m_preferences.m_hairColorId, SECTION_HEAD_UPPER, debug);
    }

    this->SetHairColor(this->m_data.m_preferences.m_hairColorId, false, debug);
    this->m_flags |= 0x4;
    this->SetDirtySections(0x300);
}

// OFFSET: 0x006FB400/0x1403C59B0
void CCharacterComponent::SetFace(int32_t faceID, bool loadHair, const char* debug) {
    if (this->m_data.m_flags & 0x1) {
        return;
    }

    CharSectionsRec* skinRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0, this->m_data.m_preferences.m_skinID, NULL);

    if (skinRec && (skinRec->m_flags & 0x8)) {
        return;
    }

    if (!ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_FACE, faceID, this->m_data.m_preferences.m_skinID)) {
        return;
    }

    this->m_data.m_preferences.m_faceId = faceID;
    this->LoadBaseVariation(VARIATION_FACE, 0, faceID, this->m_data.m_preferences.m_skinID, SECTION_HEAD_LOWER, debug);
    this->LoadBaseVariation(VARIATION_FACE, 1, this->m_data.m_preferences.m_faceId, this->m_data.m_preferences.m_skinID, SECTION_HEAD_UPPER, debug);

    if (loadHair) {
        this->LoadBaseVariation(VARIATION_HAIR, 1, this->m_data.m_preferences.m_hairStyleId, this->m_data.m_preferences.m_hairColorId, SECTION_HEAD_LOWER, debug);
        this->LoadBaseVariation(VARIATION_HAIR, 2, this->m_data.m_preferences.m_hairStyleId, this->m_data.m_preferences.m_hairColorId, SECTION_HEAD_UPPER, debug);
    }

    this->m_flags |= 0x4;
    this->SetDirtySections(0x300);
}

// OFFSET: 0x006FB500/0x1403C5B30
void CCharacterComponent::SetBeardStyle(int32_t facialHairID, bool loadVariation, const char* debug) {
    if (facialHairID < 0 || facialHairID > static_cast<int32_t>(CCharacterComponent::s_characterFacialHairStylesList[this->m_data.m_preferences.m_raceID * 2 + this->m_data.m_preferences.m_sexID])) {
        return;
    }

    this->m_data.m_preferences.m_facialHairId = facialHairID;
    CharacterFacialHairStylesRec* styleRec = GetConditionalFacialHairStyle(&this->m_data.m_preferences);

    if (styleRec) {
        this->m_data.m_geosets[1] = styleRec->m_geoset[0] + 100;
        this->m_data.m_geosets[3] = styleRec->m_geoset[1] + 300;
        this->m_data.m_geosets[2] = styleRec->m_geoset[2] + 200;
        this->m_data.m_geosets[16] = styleRec->m_geoset[3] + 1600;
        this->m_flags |= 0x4;
        this->m_data.m_geosets[17] = styleRec->m_geoset[4] + 1700;

        if ((styleRec->m_geoset[0] || styleRec->m_geoset[1] || styleRec->m_geoset[2]) && (!this->m_baseVariation[VARIATION_HAIR].m_texture[1] || !this->m_baseVariation[VARIATION_HAIR].m_texture[2])) {
            this->ReplaceHairTexture(1, debug);
        }
    }

    if (this->m_data.m_flags & 0x1) {
        return;
    }

    if (loadVariation) {
        this->LoadBaseVariation(VARIATION_FACIAL_HAIR, 0, this->m_data.m_preferences.m_facialHairId, this->m_data.m_preferences.m_hairColorId, SECTION_HEAD_LOWER, debug);
        this->LoadBaseVariation(VARIATION_FACIAL_HAIR, 1, this->m_data.m_preferences.m_facialHairId, this->m_data.m_preferences.m_hairColorId, SECTION_HEAD_UPPER, debug);
    }

    this->SetDirtySections(0x300);
}

// OFFSET: 0x006FB620/0x1403C5CA0
void CCharacterComponent::SetSkinColor(int32_t skinID, bool loadHair, bool renderAll, const char* debug) {
    this->m_data.m_preferences.m_skinID = skinID;

    if ((this->m_data.m_flags & 0x1) || !ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0, skinID)) {
        return;
    }

    int32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0);
    CharSectionsRec* skinRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0, skinID, NULL);

    if (skinID < numColors && skinRec && !(skinRec->m_flags & 0x8)) {
        CharSectionsRec* underwearRec = this->GetSectionsRecord(VARIATION_UNDERWEAR, 0, skinID, NULL);

        if (this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[0]) {
            TextureCacheDestroyTexture(this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[0]);
            this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[0] = NULL;
        }

        if (this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1]) {
            TextureCacheDestroyTexture(this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1]);
            this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1] = NULL;
        }

        if (underwearRec) {
            if (*underwearRec->m_textureName[0]) {
                strcpy(s_pathEnd, underwearRec->m_textureName[0]);
                this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[0] = TextureCacheCreateTexture(s_path);

                if (!this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[0]) {
                    DebugLog("**** Unable to Load Texture %s\n", s_path);
                }
            }

            if (*underwearRec->m_textureName[1]) {
                strcpy(s_pathEnd, underwearRec->m_textureName[1]);
                this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1] = TextureCacheCreateTexture(s_path);

                if (!this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1]) {
                    DebugLog("**** Unable to Load Texture %s\n", s_path);
                }
            }
        }
    }

    this->ReplaceExtraSkinTexture(debug);
    this->LoadBaseVariation(VARIATION_SKIN, 0, 0, this->m_data.m_preferences.m_skinID, SECTION_TORSO_UPPER, debug);
    this->SetFace(this->m_data.m_preferences.m_faceId, loadHair, debug);

    if (renderAll) {
        this->SetTexturesDirty();
    }

    this->m_dirtySections = 0xFFFFFFFF;

    if (this->m_request) {
        this->m_request->m_active = 0;
        this->m_request = NULL;
    }

    this->m_flags &= ~0x8;
}

// OFFSET: 0x006FB7F0/0x1403C5F30
void CCharacterComponent::CopyChoicesToAlteredFormComponent(CCharacterComponent* altered, COMPONENT_CONTEXT context) {
    if (!altered) {
        return;
    }

    CharacterPreferences prefs = this->m_data.m_preferences;
    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(prefs.m_raceID);

    if (!raceRec || !raceRec->m_alteredFormRaceID) {
        return;
    }

    ComponentData data;
    data.SetPreferences(&prefs);
    data.m_preferences.m_raceID = raceRec->m_alteredFormRaceID;
    CCharacterComponent::ValidateComponentData(&data, context);
    altered->SetSkinColor(data.m_preferences.m_skinID, false, true, NULL);
    altered->SetHairColor(data.m_preferences.m_hairColorId, false, NULL);
    altered->SetHairStyle(data.m_preferences.m_hairStyleId, NULL);
    altered->SetFace(data.m_preferences.m_faceId, false, NULL);
    altered->SetBeardStyle(data.m_preferences.m_facialHairId, true, NULL);
}

// OFFSET: 0x006FB910
int32_t CCharacterComponent::UpdateTextureSlot(COMPONENT_SECTIONS section, const ItemDisplayInfoRec* displayRec, int32_t layer) {
    if (this->m_data.m_flags & 0x1) {
        return 0;
    }

    CharacterSection& sectionData = this->m_section[section];

    if (sectionData.m_layerItemDisplayId[layer]) {
        ItemDisplayInfoRec rec;

        if (g_itemDisplayInfoDB.GetRecord(sectionData.m_layerItemDisplayId[layer], &rec) && rec.m_texture[section] == displayRec->m_texture[section]) {
            return 0;
        }
    }

    sectionData.m_layerMask &= ~(1 << layer);

    if (sectionData.m_layerTex[layer]) {
        TextureCacheDestroyTexture(sectionData.m_layerTex[layer]);
        sectionData.m_layerTex[layer] = NULL;
    }

    sectionData.m_layerItemDisplayId[layer] = displayRec->m_id;

    return 1;
}

// OFFSET: 0x006FB9D0
void CCharacterComponent::FreeSectionTexture(COMPONENT_SECTIONS section, int32_t layer) {
    if (layer == -1) {
        return;
    }

    if (this->m_section[section].m_layerTex[layer]) {
        TextureCacheDestroyTexture(this->m_section[section].m_layerTex[layer]);
        this->m_section[section].m_layerTex[layer] = NULL;
    }

    this->m_section[section].m_layerItemDisplayId[layer] = 0;
    this->m_section[section].m_layerMask &= ~(1 << layer);
}

// OFFSET: 0x006FBA40
void CCharacterComponent::UpdateItem(ITEM_SLOT itemSlot, COMPONENT_SECTIONS section, const ItemDisplayInfoRec* displayRec, bool apply) {
    int32_t layer = s_itemPriority[itemSlot][section];

    switch (section) {
        case SECTION_ARM_LOWER:
            if (displayRec && displayRec->m_geosetGroup[0]) {
                switch (itemSlot) {
                    case ITEMSLOT_CHEST:
                        layer = 6;
                        break;

                    case ITEMSLOT_GLOVES:
                        layer = 7;
                        break;

                    default:
                        break;
                }
            }

            break;

        case SECTION_LEG_LOWER:
            switch (itemSlot) {
                case ITEMSLOT_CHEST:
                    if (displayRec && displayRec->m_geosetGroup[2]) {
                        layer = 4;
                        this->m_flags |= 0x4;
                    }

                    break;

                case ITEMSLOT_PANTS:
                    if (displayRec && displayRec->m_geosetGroup[2]) {
                        uint32_t chestID = this->m_itemDisplayID[ITEMSLOT_CHEST];

                        if (chestID && this->ItemDisplayHasGeosetGroup(chestID, 2)) {
                            layer = 3;
                        } else {
                            layer = 4;
                        }

                        this->m_flags |= 0x4;
                    }

                    break;

                case ITEMSLOT_BOOTS:
                    if (displayRec && displayRec->m_geosetGroup[0]) {
                        this->m_flags |= 0x4;
                        layer = 3;
                    }

                    break;

                default:
                    break;
            }

            break;

        case SECTION_FOOT:
            if (this->m_flags & 0x10) {
                apply = false;
                this->FreeSectionTexture(section, layer);
                goto updated;
            }

            break;

        default:
            break;
    }

    if (!apply) {
        this->FreeSectionTexture(section, layer);
    } else if (!this->UpdateTextureSlot(section, displayRec, layer)) {
        return;
    }

updated:
    if (section == SECTION_LEG_LOWER && !apply) {
        ItemDisplayInfoRec rec;

        switch (itemSlot) {
            case ITEMSLOT_CHEST:
                if (this->ItemDisplayHasGeosetGroup(this->m_itemDisplayID[ITEMSLOT_PANTS], 2, rec)) {
                    this->FreeSectionTexture(SECTION_LEG_LOWER, 3);
                    this->UpdateTextureSlot(SECTION_LEG_LOWER, &rec, 4);
                }

                if (this->ItemDisplayHasGeosetGroup(this->m_itemDisplayID[ITEMSLOT_BOOTS], 0, rec)) {
                    this->UpdateTextureSlot(SECTION_LEG_LOWER, &rec, 3);
                }

                break;

            case ITEMSLOT_PANTS:
                if (this->ItemDisplayHasGeosetGroup(this->m_itemDisplayID[ITEMSLOT_PANTS], 2, rec)) {
                    this->UpdateTextureSlot(SECTION_LEG_LOWER, &rec, 4);
                }

                break;

            default:
                break;
        }
    }

    if (layer != -1) {
        this->SetDirtySections(1 << section);
    }
}

// PROVEN: d545bcbf IDENTICAL/NONE
// OFFSET: 0x006FBC30
void CCharacterComponent::UpdateItemAU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_ARM_UPPER];

    if (!apply) {
        this->FreeSectionTexture(SECTION_ARM_UPPER, layer);
    } else if (!this->UpdateTextureSlot(SECTION_ARM_UPPER, displayRec, layer)) {
        return;
    }

    if (layer != -1) {
        this->SetDirtySections(1 << SECTION_ARM_UPPER);
    }
}

// PROVEN: d5766acf IDENTICAL/NONE
// OFFSET: 0x006FBC90
void CCharacterComponent::UpdateItemAL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_ARM_LOWER];

    if (displayRec && displayRec->m_geosetGroup[0]) {
        switch (itemSlot) {
            case ITEMSLOT_CHEST:
                layer = 6;
                break;

            case ITEMSLOT_GLOVES:
                layer = 7;
                break;

            default:
                break;
        }
    }

    if (!apply) {
        this->FreeSectionTexture(SECTION_ARM_LOWER, layer);
    } else if (!this->UpdateTextureSlot(SECTION_ARM_LOWER, displayRec, layer)) {
        return;
    }

    if (layer != -1) {
        this->SetDirtySections(1 << SECTION_ARM_LOWER);
    }
}

// PROVEN: f4af80ec IDENTICAL/NONE
// OFFSET: 0x006FBD10
void CCharacterComponent::UpdateItemHA(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_HAND];

    if (!apply) {
        this->FreeSectionTexture(SECTION_HAND, layer);
    } else if (!this->UpdateTextureSlot(SECTION_HAND, displayRec, layer)) {
        return;
    }

    if (layer != -1) {
        this->SetDirtySections(1 << SECTION_HAND);
    }
}

// PROVEN: 90aa2b97 IDENTICAL/NONE
// OFFSET: 0x006FBD70
void CCharacterComponent::UpdateItemTU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_TORSO_UPPER];

    if (!apply) {
        this->FreeSectionTexture(SECTION_TORSO_UPPER, layer);
    } else if (!this->UpdateTextureSlot(SECTION_TORSO_UPPER, displayRec, layer)) {
        return;
    }

    if (layer != -1) {
        this->SetDirtySections(1 << SECTION_TORSO_UPPER);
    }
}

// PROVEN: f35990fc IDENTICAL/NONE
// OFFSET: 0x006FBDD0
void CCharacterComponent::UpdateItemTL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_TORSO_LOWER];

    if (!apply) {
        this->FreeSectionTexture(SECTION_TORSO_LOWER, layer);
    } else if (!this->UpdateTextureSlot(SECTION_TORSO_LOWER, displayRec, layer)) {
        return;
    }

    if (layer != -1) {
        this->SetDirtySections(1 << SECTION_TORSO_LOWER);
    }
}

// PROVEN: 649ca295 IDENTICAL/NONE
// OFFSET: 0x006FBE30
void CCharacterComponent::UpdateItemLU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_LEG_UPPER];

    if (!apply) {
        this->FreeSectionTexture(SECTION_LEG_UPPER, layer);
    } else if (!this->UpdateTextureSlot(SECTION_LEG_UPPER, displayRec, layer)) {
        return;
    }

    if (layer != -1) {
        this->SetDirtySections(1 << SECTION_LEG_UPPER);
    }
}

// PROVEN: 428b3977 IDENTICAL/NONE
// OFFSET: 0x006FBEB0
void CCharacterComponent::UpdateItemFO(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply) {
    int32_t layer = s_itemPriority[itemSlot][SECTION_FOOT];

    if (this->m_flags & 0x10) {
        apply = false;
    }

    if (!apply) {
        this->FreeSectionTexture(SECTION_FOOT, layer);
    } else if (!this->UpdateTextureSlot(SECTION_FOOT, displayRec, layer)) {
        return;
    }

    if (layer != -1) {
        this->SetDirtySections(1 << SECTION_FOOT);
    }
}

// PROVEN: d2465183 IDENTICAL/NONE
// OFFSET: 0x006FBF20/0x1403C68C0
void CCharacterComponent::ComponentUtilAddItemVisual(CM2Model* model, int32_t itemVisual) {
    while (model->m_attachList) {
        model->m_attachList->DetachFromParent();
    }

    ItemVisualsRec* visualRec = g_itemVisualsDB.GetRecord(itemVisual);

    if (!visualRec) {
        return;
    }

    for (uint32_t i = 0; i < 5; i++) {
        ItemVisualEffectsRec* effectRec = g_itemVisualEffectsDB.GetRecord(visualRec->m_slot[i]);

        if (effectRec && *effectRec->m_model) {
            strcpy(s_pathEnd, effectRec->m_model);
            CM2Model* effectModel = model->m_scene->CreateModel(s_path, 0, 3);

            if (effectModel) {
                effectModel->ForceAdditiveParticleSort();
                effectModel->AttachToParent(model, i, NULL, 0);
                effectModel->Release();
            }
        }
    }
}

// OFFSET: 0x006FC010/0x1403C6A30
static void ReplaceParticleColor(int32_t particleColorID, CM2Model* model) {
    if (!model || !particleColorID) {
        return;
    }

    ParticleColorRec* colorRec = g_particleColorDB.GetRecord(particleColorID);

    if (colorRec) {
        for (uint32_t i = 0; i < 3; i++) {
            model->ReplaceParticleColor(i + 11, *reinterpret_cast<CImVector*>(&colorRec->m_start[i]), *reinterpret_cast<CImVector*>(&colorRec->m_mid[i]), *reinterpret_cast<CImVector*>(&colorRec->m_end[i]));
        }
    } else {
        CImVector green;
        *reinterpret_cast<uint32_t*>(&green) = 0xFF00FF00;

        for (uint32_t i = 0; i < 3; i++) {
            model->ReplaceParticleColor(i + 11, green, green, green);
        }
    }
}

// OFFSET: 0x006FC0A0
CM2Model* CCharacterComponent::AddLink(CM2Model* parent, uint8_t* attachMask, GEOCOMPONENTLINKS link, const char* modelPath, const char* texturePath, int32_t itemVisual, const ItemDisplayInfoRec* displayRec) {
    if (!parent) {
        return NULL;
    }

    CM2Model* model = parent->m_scene->CreateModel(modelPath, 0, 3);

    if (!model) {
        return NULL;
    }

    CGxTexFlags flags;
    flags.m_filter = GxTex_LinearMipLinear;
    CTexture* texture = TextureCreate(texturePath, flags, &s_status, 0, NULL, 3);

    if (!texture) {
        model->Release();
        return NULL;
    }

    model->ReplaceTexture(2, texture);
    TextureRelease(texture);

    if (itemVisual > 0) {
        CCharacterComponent::ComponentUtilAddItemVisual(model, itemVisual);
    }

    parent->DetachAllChildrenById(link);
    model->AttachToParent(parent, link, NULL, 0);
    uint8_t hide = (displayRec->m_flags >> 6) & 1;

    switch (link) {
        case ATTACH_SHIELD:
            *attachMask = (*attachMask & ~0x20) | (hide << 5);
            break;

        case ATTACH_HANDRIGHT:
            CCharacterComponent::ComponentCloseFingers(parent, COMP_HAND_RIGHT);
            *attachMask = (*attachMask & ~0x8) | (hide << 3);
            break;

        case ATTACH_HANDLEFT:
            CCharacterComponent::ComponentCloseFingers(parent, COMP_HAND_LEFT);
            *attachMask = (*attachMask & ~0x10) | (hide << 4);
            break;

        case ATTACH_SHOULDERRIGHT:
            *attachMask = (*attachMask & ~0x2) | (hide << 1);
            break;

        case ATTACH_SHOULDERLEFT:
            *attachMask = (*attachMask & ~0x1) | hide;
            break;

        case ATTACH_HELM:
            *attachMask = (*attachMask & ~0x4) | (hide << 2);
            break;

        default:
            break;
    }

    ReplaceParticleColor(displayRec->m_particleColorID, model);
    model->Release();

    return model;
}

// PROVEN: 1e9a0da9 IDENTICAL/NONE
// OFFSET: 0x006FC230/0x1403C6D00
int32_t CCharacterComponent::GetShoulderDisplayInfo(ItemDisplayInfoRec& rec, const ItemDisplayInfoRec*& displayRec) {
    if (!g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_SHOULDER], &rec)) {
        return 0;
    }

    displayRec = &rec;

    return 1;
}

// OFFSET: 0x006FC290/0x1403C6D90
int32_t CCharacterComponent::BuildShoulderItemPaths(ItemDisplayInfoRec& rec, const ItemDisplayInfoRec*& displayRec, char* leftModel, char* rightModel, char* leftTexture, char* rightTexture) {
    *leftModel = '\0';
    *rightModel = '\0';

    if (leftTexture) {
        *leftTexture = '\0';
    }

    if (rightTexture) {
        *rightTexture = '\0';
    }

    if (!this->m_itemDisplayID[ITEMSLOT_SHOULDER] || !this->GetShoulderDisplayInfo(rec, displayRec)) {
        return 0;
    }

    SStrCopy(s_buffer, "Item\\ObjectComponents\\Shoulder\\", sizeof(s_buffer));
    char* end = s_buffer + strlen(s_buffer);

    if (*displayRec->m_modelName[0]) {
        strcpy(end, static_cast<const char*>(displayRec->m_modelName[0]));
        strcpy(leftModel, s_buffer);

        if (leftTexture) {
            strcpy(end, displayRec->m_modelTexture[0]);
            strcat(end, ".blp");
            strcpy(leftTexture, s_buffer);
        }
    }

    if (*displayRec->m_modelName[1]) {
        strcpy(end, displayRec->m_modelName[1]);
        strcpy(rightModel, s_buffer);

        if (rightTexture) {
            strcpy(end, displayRec->m_modelTexture[1]);
            strcat(end, ".blp");
            strcpy(rightTexture, s_buffer);
        }
    }

    return 1;
}

// OFFSET: 0x006FC430
int32_t CCharacterComponent::AreShoulderModelsCorrect() {
    CM2Model* list = this->m_data.m_model->m_attachList;
    CM2Model* left = list;

    if (!left) {
        return 0;
    }

    while (left->m_attachmentId != ATTACH_SHOULDERLEFT) {
        left = left->m_attachNext;

        if (!left) {
            return 0;
        }
    }

    CM2Model* right = list;

    if (!right) {
        return 0;
    }

    while (right->m_attachmentId != ATTACH_SHOULDERRIGHT) {
        right = right->m_attachNext;

        if (!right) {
            return 0;
        }
    }

    ItemDisplayInfoRec rec;
    const ItemDisplayInfoRec* displayRec;

    if (!left->IsDrawable() || !right->IsDrawable() || (left->m_fFlags & 0x2000) || (right->m_fFlags & 0x2000) || !this->BuildShoulderItemPaths(rec, displayRec, s_path, s_path2, NULL, NULL)) {
        return 0;
    }

    OsPathReplaceExtension(s_path, sizeof(s_path), s_path, "m2");
    OsPathReplaceExtension(s_path2, sizeof(s_path2), s_path2, "m2");

    return !_stricmp(left->m_shared->m_fileName, s_path) && !_stricmp(right->m_shared->m_fileName, s_path2);
}

// PROVEN: a90fb84f IDENTICAL/NONE
// OFFSET: 0x006FC550
static void ShoulderSequenceStartCallback(CM2Model* model, uint32_t boneId, uint32_t sequenceId, uint32_t variation, uint32_t time, uint64_t param, int32_t primary) {
    if (!model || !primary || !model->m_attachParent) {
        return;
    }

    for (CM2Model* child = model->m_attachParent->m_attachList; child; child = child->m_attachNext) {
        if (child->m_attachmentId == ATTACH_SHOULDERLEFT) {
            if (child == reinterpret_cast<CM2Model*>(param)) {
                child->SetBoneSequence(boneId, sequenceId, variation, time, 1.0f, 1, 1, 0, 0, 0);
            }

            return;
        }
    }
}

// OFFSET: 0x006FC5B0
void CCharacterComponent::LinkShoulderAnimations(CM2Model* left, CM2Model* right, const ItemDisplayInfoRec* displayRec) {
    if (!right || !left || !(displayRec->m_flags & 0x80)) {
        return;
    }

    this->m_attachMask &= ~0x1;
    right->SetSequenceStartCallback(ShoulderSequenceStartCallback, reinterpret_cast<intptr_t>(left));
    right->SetBoneSequence(-1, 0, -1, 0, 1.0f, 1, 1, 0, 0, 0);
}

// OFFSET: 0x006FC610/0x1403C7340
CM2Model* CCharacterComponent::MakeAmmoModel(CM2Model* model, const ItemDisplayInfoRec* displayRec, uint32_t invType) {
    if (!displayRec) {
        return NULL;
    }

    const char* modelName = displayRec->m_modelName[1];

    if (!*modelName || !*displayRec->m_modelTexture[1]) {
        return NULL;
    }

    const char* folder;

    if (invType == INVTYPE_AMMO) {
        folder = "Item\\ObjectComponents\\Ammo\\";
    } else if (invType == INVTYPE_THROWN) {
        folder = "Item\\ObjectComponents\\Weapon\\";
    } else {
        return NULL;
    }

    char path[260];
    strcpy(path, folder);
    size_t length = strlen(path);
    strcpy(path + length, modelName);

    CM2Model* ammoModel = model->m_scene->CreateModel(path, 0, 3);

    if (!ammoModel) {
        return NULL;
    }

    strcpy(path + length, displayRec->m_modelTexture[1]);

    CGxTexFlags flags;
    flags.m_filter = GxTex_LinearMipLinear;
    CTexture* texture = TextureCreate(path, flags, &s_status, 0, NULL, 3);

    if (!texture) {
        ammoModel->Release();
        return NULL;
    }

    ammoModel->ReplaceTexture(2, texture);
    TextureRelease(texture);
    CCharacterComponent::ComponentUtilAddItemVisual(ammoModel, displayRec->m_itemVisual);

    return ammoModel;
}

// OFFSET: 0x006FC740
GEOCOMPONENTLINKS CCharacterComponent::AddHandItem(CM2Model* model, uint8_t* attachMask, const ItemDisplayInfoRec* displayRec, INVENTORY_SLOTS invSlot, SHEATHE_TYPE sheatheType, bool sheathed, bool shield, bool mainHand, int32_t itemVisual) {
    if (!model || !displayRec || static_cast<uint32_t>(invSlot) > INVSLOT_TABARD) {
        return ATTACH_NONE;
    }

    const char* folder = "Item\\ObjectComponents\\Weapon\\";
    GEOCOMPONENTLINKS hand;
    GEOCOMPONENTLINKS sheath;

    if (invSlot == INVSLOT_MAINHAND || (invSlot == INVSLOT_RANGED && mainHand)) {
        hand = ATTACH_HANDRIGHT;
        sheath = CCharacterComponent::GetSheatheLink(sheatheType, true);
    } else if (invSlot == INVSLOT_OFFHAND) {
        hand = ATTACH_HANDLEFT;
        sheath = CCharacterComponent::GetSheatheLink(sheatheType, false);

        if (shield) {
            folder = "Item\\ObjectComponents\\Shield\\";
            hand = ATTACH_SHIELD;
        }
    } else if (invSlot == INVSLOT_RANGED) {
        hand = ATTACH_HANDLEFT;
        sheath = CCharacterComponent::GetSheatheLink(sheatheType, false);
    } else {
        return ATTACH_NONE;
    }

    CCharacterComponent::RemoveLinkpt(model, attachMask, hand);
    CCharacterComponent::RemoveLinkpt(model, attachMask, sheath);

    GEOCOMPONENTLINKS link = sheathed ? sheath : hand;

    if (model->IsLoaded() && !model->HasAttachment(link)) {
        return ATTACH_NONE;
    }

    SStrPrintf(s_buffer, "%s%s", folder, static_cast<const char*>(displayRec->m_modelName[0]));
    strcpy(s_pathEnd, s_buffer);
    SStrPrintf(s_buffer, "%s%s.blp", folder, displayRec->m_modelTexture[0]);
    strcpy(s_pathEnd2, s_buffer);

    CCharacterComponent::AddLink(model, attachMask, link, s_path, s_path2, itemVisual ? itemVisual : displayRec->m_itemVisual, displayRec);

    return link;
}

// OFFSET: 0x006FC950/0x1403C7760
void CCharacterComponent::RemoveVisuals(CM2Model* model) {
    model->DetachAllChildrenById(15);
    model->DetachAllChildrenById(16);
    model->DetachAllChildrenById(17);
    model->DetachAllChildrenById(18);
    model->DetachAllChildrenById(29);
    model->DetachAllChildrenById(19);
    model->DetachAllChildrenById(20);
    model->DetachAllChildrenById(38);
    model->DetachAllChildrenById(21);
    model->DetachAllChildrenById(22);
    model->DetachAllChildrenById(23);
    model->DetachAllChildrenById(24);
    model->DetachAllChildrenById(25);
    model->DetachAllChildrenById(34);
    model->DetachAllChildrenById(35);
    model->DetachAllChildrenById(47);
    model->DetachAllChildrenById(48);
    model->DetachAllChildrenById(12);
    model->m_emissiveTintR = 0.0f;
    model->m_emissiveTintG = 0.0f;
    model->m_fadeAlpha = 1.0f;
    model->m_emissiveTintB = 0.0f;
    model->m_diffuseTintR = 1.0f;
    model->m_diffuseTintG = 1.0f;
    model->m_diffuseTintB = 1.0f;
}

// OFFSET: 0x006FCA70/0x1403C78D0
void CCharacterComponent::RemoveHandItem(CM2Model* model, uint8_t* attachMask, INVENTORY_SLOTS invSlot, SHEATHE_TYPE sheatheType, bool shield) {
    if (!model || static_cast<uint32_t>(invSlot - INVSLOT_BACK) > 4) {
        return;
    }

    GEOCOMPONENTLINKS hand = ATTACH_NONE;
    GEOCOMPONENTLINKS sheath = ATTACH_NONE;

    if (invSlot == INVSLOT_MAINHAND) {
        hand = ATTACH_HANDRIGHT;
        sheath = CCharacterComponent::GetSheatheLink(sheatheType, true);
    } else if (invSlot > INVSLOT_MAINHAND && invSlot <= INVSLOT_RANGED) {
        hand = ATTACH_HANDLEFT;
        sheath = CCharacterComponent::GetSheatheLink(sheatheType, false);

        if (shield) {
            hand = ATTACH_SHIELD;
        }
    }

    CCharacterComponent::RemoveLinkpt(model, attachMask, hand);
    CCharacterComponent::RemoveLinkpt(model, attachMask, sheath);
}

// OFFSET: 0x006FCB60
bool CCharacterComponent::SetNextSkin(COMPONENT_CONTEXT context, int32_t step) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, this->m_data.m_preferences.m_classId);
    int32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0);

    if (numColors <= 0) {
        return false;
    }

    int32_t skinID = this->m_data.m_preferences.m_skinID + step;

    if (skinID >= numColors) {
        skinID = 0;
    } else if (skinID < 0) {
        skinID = numColors - 1;
    }

    if (skinID == static_cast<int32_t>(this->m_data.m_preferences.m_skinID)) {
        return false;
    }

    CharSectionsRec* skinRec;

    while (true) {
        skinRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0, skinID, NULL);
        CharSectionsRec* faceRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_FACE, this->m_data.m_preferences.m_faceId, skinID, NULL);
        CharSectionsRec* underwearRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_UNDERWEAR, 0, skinID, NULL);

        if (skinRec && ComponentFlagsMatch(skinRec->m_flags, selection) && faceRec && ComponentFlagsMatch(faceRec->m_flags, selection) && (context == COMPONENT_CONTEXT_2 || (underwearRec && ComponentFlagsMatch(underwearRec->m_flags, selection)))) {
            break;
        }

        skinID += step;

        if (skinID >= numColors) {
            skinID = 0;
        } else if (skinID < 0) {
            skinID = numColors - 1;
        }

        if (skinID == static_cast<int32_t>(this->m_data.m_preferences.m_skinID)) {
            return false;
        }
    }

    this->SetSkinColor(skinRec->m_colorIndex, true, true, NULL);

    return true;
}

// OFFSET: 0x006FCCC0/0x1403C7B70
bool CCharacterComponent::SetRandomSkin(COMPONENT_CONTEXT context) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, this->m_data.m_preferences.m_classId);
    int32_t numSkins = CCharacterComponent::GetNumSkins(this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, this->m_data.m_preferences.m_classId, context);
    int32_t index = numSkins > 0 ? CRandom::dice_(numSkins, g_rndSeed) : 0;
    int32_t skinID = ComponentGetSkinColor(this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, index, selection);

    if (skinID < 0) {
        return false;
    }

    this->SetSkinColor(skinID, true, true, NULL);

    return true;
}

// OFFSET: 0x006FCD50/0x1403C7C20
bool CCharacterComponent::SetRandomHairStyle(COMPONENT_CONTEXT context) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, this->m_data.m_preferences.m_classId);
    int32_t numStyles = CCharacterComponent::GetNumHairStylesForColor(this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, this->m_data.m_preferences.m_classId, this->m_data.m_preferences.m_hairColorId, context);
    int32_t index = numStyles > 0 ? CRandom::dice_(numStyles, g_rndSeed) : 0;
    int32_t hairStyleID = ComponentGetHairVariation(this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, this->m_data.m_preferences.m_hairColorId, index, selection);

    if (hairStyleID < 0) {
        return false;
    }

    this->SetHairStyle(hairStyleID, NULL);

    return true;
}

// OFFSET: 0x006FCDE0
bool CCharacterComponent::SetNextHairColor(COMPONENT_CONTEXT context, int32_t step) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, this->m_data.m_preferences.m_classId);
    int32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_HAIR, this->m_data.m_preferences.m_hairStyleId);

    if (numColors <= 0) {
        return false;
    }

    int32_t hairColorID = this->m_data.m_preferences.m_hairColorId + step;

    if (hairColorID >= numColors) {
        hairColorID = 0;
    } else if (hairColorID < 0) {
        hairColorID = numColors - 1;
    }

    if (hairColorID == static_cast<int32_t>(this->m_data.m_preferences.m_hairColorId)) {
        return false;
    }

    CharSectionsRec* rec;

    while (true) {
        rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_HAIR, this->m_data.m_preferences.m_hairStyleId, hairColorID, NULL);

        if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
            break;
        }

        hairColorID += step;

        if (hairColorID >= numColors) {
            hairColorID = 0;
        } else if (hairColorID < 0) {
            hairColorID = numColors - 1;
        }

        if (hairColorID == static_cast<int32_t>(this->m_data.m_preferences.m_hairColorId)) {
            return false;
        }
    }

    this->SetHairColor(rec->m_colorIndex, true, NULL);

    return true;
}

// OFFSET: 0x006FCEC0/0x1403C7DE0
bool CCharacterComponent::SetRandomHairColor(COMPONENT_CONTEXT context) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, this->m_data.m_preferences.m_classId);
    int32_t numColors = CCharacterComponent::GetNumHairColorsForStyle(this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, this->m_data.m_preferences.m_classId, this->m_data.m_preferences.m_hairStyleId, context);
    int32_t index = numColors > 0 ? CRandom::dice_(numColors, g_rndSeed) : 0;
    int32_t hairColorID = ComponentGetHairColor(this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, this->m_data.m_preferences.m_hairStyleId, index, selection);

    if (hairColorID < 0) {
        return false;
    }

    this->SetHairColor(hairColorID, true, NULL);

    return true;
}

// OFFSET: 0x006FCF50
bool CCharacterComponent::SetNextFace(COMPONENT_CONTEXT context, int32_t step, int32_t colorOffset) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, this->m_data.m_preferences.m_classId);
    int32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_FACE);

    if (numVariations <= 0) {
        return false;
    }

    int32_t faceID = this->m_data.m_preferences.m_faceId + step;

    if (faceID >= numVariations) {
        faceID = 0;
    } else if (faceID < 0) {
        faceID = numVariations - 1;
    }

    if (faceID == static_cast<int32_t>(this->m_data.m_preferences.m_faceId)) {
        return false;
    }

    int32_t skinID;

    while (true) {
        int32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_FACE, faceID);
        bool matched = false;

        for (int32_t i = 0; i < numColors; i = skinID + 1) {
            skinID = (i + colorOffset) % numColors;
            CharSectionsRec* faceRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_FACE, faceID, skinID, NULL);
            CharSectionsRec* skinRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0, skinID, NULL);
            CharSectionsRec* underwearRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_UNDERWEAR, 0, skinID, NULL);

            if (faceRec && ComponentFlagsMatch(faceRec->m_flags, selection) && skinRec && ComponentFlagsMatch(skinRec->m_flags, selection) && (context == COMPONENT_CONTEXT_2 || (underwearRec && ComponentFlagsMatch(underwearRec->m_flags, selection)))) {
                matched = true;
                break;
            }
        }

        if (matched) {
            break;
        }

        faceID += step;

        if (faceID >= numVariations) {
            faceID = 0;
        } else if (faceID < 0) {
            faceID = numVariations - 1;
        }

        if (faceID == static_cast<int32_t>(this->m_data.m_preferences.m_faceId)) {
            return false;
        }
    }

    CharSectionsRec* faceRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_FACE, faceID, this->m_data.m_preferences.m_skinID, NULL);
    CharSectionsRec* skinRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0, this->m_data.m_preferences.m_skinID, NULL);
    CharSectionsRec* underwearRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_UNDERWEAR, 0, this->m_data.m_preferences.m_skinID, NULL);

    if (!faceRec || !ComponentFlagsMatch(faceRec->m_flags, selection) || !skinRec || !ComponentFlagsMatch(skinRec->m_flags, selection) || (context != COMPONENT_CONTEXT_2 && (!underwearRec || !ComponentFlagsMatch(underwearRec->m_flags, selection)))) {
        this->SetSkinColor(skinID, false, true, NULL);
    }

    this->SetFace(faceID, true, NULL);

    return true;
}

// OFFSET: 0x006FD1F0/0x1403C8160
bool CCharacterComponent::SetRandomFace(COMPONENT_CONTEXT context) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, this->m_data.m_preferences.m_classId);
    int32_t numFaces = CCharacterComponent::GetNumFacesForSkin(this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, this->m_data.m_preferences.m_classId, this->m_data.m_preferences.m_skinID, context);
    int32_t index = numFaces > 0 ? CRandom::dice_(numFaces, g_rndSeed) : 0;
    int32_t faceID = ComponentGetFaceVariation(this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, this->m_data.m_preferences.m_skinID, index, selection);

    if (faceID < 0) {
        return false;
    }

    this->SetFace(faceID, true, NULL);

    return true;
}

// OFFSET: 0x006FD280
bool CCharacterComponent::SetNextFacialFeature(COMPONENT_CONTEXT context, int32_t step) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, this->m_data.m_preferences.m_classId);
    bool found;
    ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_FACIAL_HAIR, this->m_data.m_preferences.m_facialHairId, this->m_data.m_preferences.m_hairColorId, &found);

    if (!found) {
        int32_t count = CCharacterComponent::s_characterFacialHairStylesList[this->m_data.m_preferences.m_raceID * 2 + this->m_data.m_preferences.m_sexID];
        int32_t facialHairID = step + this->m_data.m_preferences.m_facialHairId;

        if (facialHairID >= count) {
            facialHairID = 0;
        } else if (facialHairID < 0) {
            facialHairID = count - 1;
        }

        this->SetBeardStyle(facialHairID, true, NULL);

        return true;
    }

    int32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_FACIAL_HAIR);

    if (numVariations <= 0) {
        return false;
    }

    int32_t facialHairID = this->m_data.m_preferences.m_facialHairId + step;

    if (facialHairID >= numVariations) {
        facialHairID = 0;
    } else if (facialHairID < 0) {
        facialHairID = numVariations - 1;
    }

    if (facialHairID == static_cast<int32_t>(this->m_data.m_preferences.m_facialHairId)) {
        return false;
    }

    CharSectionsRec* rec = NULL;

    while (true) {
        int32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_FACIAL_HAIR, facialHairID);
        int32_t color;

        for (color = 0; color < numColors; color++) {
            rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_FACIAL_HAIR, facialHairID, color, NULL);

            if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
                break;
            }
        }

        if (color < numColors) {
            break;
        }

        facialHairID += step;

        if (facialHairID >= numVariations) {
            facialHairID = 0;
        } else if (facialHairID < 0) {
            facialHairID = numVariations - 1;
        }

        if (facialHairID == static_cast<int32_t>(this->m_data.m_preferences.m_facialHairId)) {
            return false;
        }
    }

    CharSectionsRec* keepRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_FACIAL_HAIR, facialHairID, this->m_data.m_preferences.m_hairColorId, NULL);

    if (keepRec && ComponentFlagsMatch(keepRec->m_flags, selection)) {
        this->SetBeardStyle(keepRec->m_variationIndex, true, NULL);
    } else {
        this->SetHairColor(rec->m_colorIndex, false, NULL);
        this->SetBeardStyle(rec->m_variationIndex, true, NULL);
    }

    return true;
}

// OFFSET: 0x006FD470/0x1403C8460
bool CCharacterComponent::SetRandomFacialFeature(COMPONENT_CONTEXT context) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, this->m_data.m_preferences.m_classId);
    int32_t numFeatures = CCharacterComponent::GetNumFacialFeaturesForHairColor(this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, this->m_data.m_preferences.m_classId, this->m_data.m_preferences.m_hairColorId, context);
    int32_t index = numFeatures > 0 ? CRandom::dice_(numFeatures, g_rndSeed) : 0;
    int32_t facialHairID = CCharacterComponent::GetNthFacialFeatureIndex(this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, this->m_data.m_preferences.m_classId, this->m_data.m_preferences.m_hairColorId, this->m_data.m_preferences.m_facialHairId, index, selection);

    if (facialHairID < 0) {
        return false;
    }

    this->SetBeardStyle(facialHairID, true, NULL);

    return true;
}

// OFFSET: 0x006FD500/0x1403C8520
void CCharacterComponent::ClearSectionLayer(COMPONENT_SECTIONS section, int32_t layer) {
    if (this->m_section[section].m_layerTex[layer]) {
        TextureCacheDestroyTexture(this->m_section[section].m_layerTex[layer]);
        this->m_section[section].m_layerTex[layer] = NULL;
    }

    this->m_section[section].m_layerMask &= ~(1 << layer);
    this->m_section[section].m_layerItemDisplayId[layer] = 0;
    this->SetDirtySections(1 << section);
}

// OFFSET: 0x006FD590
void CCharacterComponent::LoadSectionLayer(const char* fileName, COMPONENT_SECTIONS section, int32_t layer) {
    if (this->m_section[section].m_layerTex[layer]) {
        TextureCacheDestroyTexture(this->m_section[section].m_layerTex[layer]);
    }

    this->m_section[section].m_layerTex[layer] = TextureCacheCreateTexture(fileName);
    this->m_section[section].m_layerItemDisplayId[layer] = 0;
    this->m_section[section].m_layerMask &= ~(1 << layer);

    if (!this->m_section[section].m_layerTex[layer]) {
        DebugLog("**** Unable to Load Texture %s\n", fileName);
    }
}

// OFFSET: 0x006FD620
void CCharacterComponent::PasteCrappyGreen(MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t dstMipOffset) {
    uint32_t width = srcSize.x;
    uint32_t height = srcSize.y;

    for (uint32_t mip = srcMipLevel; mip < srcInfo.m_mipCount; mip++) {
        uint8_t* dst = reinterpret_cast<uint8_t*>(dstMips->m_mip[mip + dstMipOffset]) + dstStride * dstPos.y + 4 * dstPos.x;

        for (uint8_t* end = dst + dstStride * height; dst < end; dst += dstStride) {
            uint32_t* pixel = reinterpret_cast<uint32_t*>(dst);

            for (uint32_t x = 0; x < width; x++) {
                pixel[x] = 0xFF00FF00;
            }
        }

        width = (width >> 1) ? width >> 1 : 1;
        height = (height >> 1) ? height >> 1 : 1;
        dstPos.x >>= 1;
        dstPos.y >>= 1;
        dstStride >>= 1;
    }
}

// OFFSET: 0x006FD6F0
void CCharacterComponent::PasteOpaque(void* texture, const BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t dstMipOffset) {
    uint32_t srcWidth = srcInfo.m_width;
    uint32_t width = srcSize.x;
    uint32_t height = srcSize.y;

    if (srcMipLevel) {
        srcWidth >>= srcMipLevel;
    }

    for (uint32_t mip = srcMipLevel; mip < srcInfo.m_mipCount; mip++) {
        const uint8_t* src = static_cast<const uint8_t*>(TextureCacheGetMip(texture, mip)) + srcPos.x + srcWidth * srcPos.y;
        uint8_t* dst = reinterpret_cast<uint8_t*>(dstMips->m_mip[mip + dstMipOffset]) + dstStride * dstPos.y + 4 * dstPos.x;

        for (const uint8_t* end = src + srcWidth * height; src < end; src += srcWidth, dst += dstStride) {
            uint32_t* pixel = reinterpret_cast<uint32_t*>(dst);

            for (uint32_t x = 0; x < width; x++) {
                const BlpPalPixel& color = pal[src[x]];
                pixel[x] = color.m_b | ((color.m_g | ((color.m_r | 0xFFFFFF00) << 8)) << 8);
            }
        }

        width = (width >> 1) ? width >> 1 : 1;
        height = (height >> 1) ? height >> 1 : 1;
        dstPos.x >>= 1;
        dstPos.y >>= 1;
        srcPos.x >>= 1;
        srcPos.y >>= 1;
        srcWidth >>= 1;
        dstStride >>= 1;
    }
}

// OFFSET: 0x006FD830
void CCharacterComponent::PasteTransparent1Bit(void* texture, const BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t dstMipOffset) {
    uint32_t srcWidth = srcInfo.m_width;
    uint32_t width = srcSize.x;
    uint32_t height = srcSize.y;

    if (srcMipLevel) {
        srcWidth >>= srcMipLevel;
    }

    for (uint32_t mip = srcMipLevel; mip < srcInfo.m_mipCount; mip++) {
        const uint8_t* src = static_cast<const uint8_t*>(TextureCacheGetMip(texture, mip));
        uint8_t* dst = reinterpret_cast<uint8_t*>(dstMips->m_mip[mip + dstMipOffset]) + dstStride * dstPos.y + 4 * dstPos.x;
        const uint8_t* end = src + srcWidth * height;
        const uint8_t* alpha = end;

        for (; src < end; src += srcWidth, dst += dstStride, alpha += srcWidth >> 3) {
            uint32_t* pixel = reinterpret_cast<uint32_t*>(dst);

            for (uint32_t x = 0; x < width; x++) {
                if ((1 << (x & 7)) & alpha[x >> 3]) {
                    const BlpPalPixel& color = pal[src[x]];
                    pixel[x] = color.m_b | ((color.m_g | ((color.m_r | 0xFFFFFF00) << 8)) << 8);
                }
            }
        }

        width = (width >> 1) ? width >> 1 : 1;
        height = (height >> 1) ? height >> 1 : 1;
        dstPos.x >>= 1;
        dstPos.y >>= 1;
        srcPos.x >>= 1;
        srcPos.y >>= 1;
        srcWidth >>= 1;
        dstStride >>= 1;
    }
}

// OFFSET: 0x006FD990
void CCharacterComponent::PasteTransparent4Bit(void* texture, const BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t dstMipOffset) {
    uint32_t srcWidth = srcInfo.m_width;
    uint32_t width = srcSize.x;
    uint32_t height = srcSize.y;

    if (srcMipLevel) {
        srcWidth >>= srcMipLevel;
    }

    for (uint32_t mip = srcMipLevel; mip < srcInfo.m_mipCount; mip++) {
        const uint8_t* src = static_cast<const uint8_t*>(TextureCacheGetMip(texture, mip));
        const uint8_t* end = src + srcWidth * height;
        const uint8_t* alpha = end;
        uint8_t* dst = reinterpret_cast<uint8_t*>(dstMips->m_mip[mip + dstMipOffset]) + dstStride * dstPos.y + 4 * dstPos.x;

        for (; src < end; src += srcWidth, dst += dstStride, alpha += srcWidth >> 1) {
            uint8_t* pixel = dst;

            for (uint32_t x = 0; x < width; x++, pixel += 4) {
                const BlpPalPixel& color = pal[src[x]];
                uint32_t shift = 4 * (x & 1);
                uint32_t a = (alpha[x] & (0xF << shift)) >> shift;
                a |= a << 4;
                pixel[3] = 0xFF;
                pixel[2] = ((255 - a) * pixel[2] + a * color.m_r) >> 8;
                pixel[1] = ((255 - a) * pixel[1] + a * color.m_g) >> 8;
                pixel[0] = ((255 - a) * pixel[0] + a * color.m_b) >> 8;
            }
        }

        width = (width >> 1) ? width >> 1 : 1;
        height = (height >> 1) ? height >> 1 : 1;
        dstPos.x >>= 1;
        dstPos.y >>= 1;
        srcPos.x >>= 1;
        srcPos.y >>= 1;
        dstStride >>= 1;
        srcWidth >>= 1;
    }
}

// OFFSET: 0x006FDB70
void CCharacterComponent::PasteTransparent8Bit(void* texture, const BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t dstMipOffset) {
    uint32_t srcWidth = srcInfo.m_width;
    uint32_t width = srcSize.x;
    uint32_t height = srcSize.y;

    if (srcMipLevel) {
        srcWidth >>= srcMipLevel;
    }

    for (uint32_t mip = srcMipLevel; mip < srcInfo.m_mipCount; mip++) {
        const uint8_t* src = static_cast<const uint8_t*>(TextureCacheGetMip(texture, mip));
        const uint8_t* end = src + srcWidth * height;
        const uint8_t* alpha = end;
        uint8_t* dst = reinterpret_cast<uint8_t*>(dstMips->m_mip[mip + dstMipOffset]) + dstStride * dstPos.y + 4 * dstPos.x;

        for (; src < end; src += srcWidth, alpha += srcWidth, dst += dstStride) {
            uint8_t* pixel = dst;

            for (uint32_t x = 0; x < width; x++, pixel += 4) {
                const BlpPalPixel& color = pal[src[x]];
                uint32_t a = alpha[x];
                pixel[3] = 0xFF;
                pixel[2] = ((255 - a) * pixel[2] + a * color.m_r) >> 8;
                pixel[1] = ((255 - a) * pixel[1] + a * color.m_g) >> 8;
                pixel[0] = ((255 - a) * pixel[0] + a * color.m_b) >> 8;
            }
        }

        width = (width >> 1) ? width >> 1 : 1;
        height = (height >> 1) ? height >> 1 : 1;
        dstPos.x >>= 1;
        dstPos.y >>= 1;
        srcPos.x >>= 1;
        srcPos.y >>= 1;
        srcWidth >>= 1;
        dstStride >>= 1;
    }
}

// OFFSET: 0x006FDD10
void CCharacterComponent::PasteOpaqueScale(void* texture, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo) {
    uint32_t srcWidth = srcInfo.m_width;
    uint32_t width = srcSize.x;
    uint32_t height = srcSize.y;
    BlpPalPixel* pal = TextureCacheGetPal(texture);

    if (!pal) {
        return;
    }

    srcPos.x >>= 1;
    srcPos.y >>= 1;
    const uint8_t* src = static_cast<const uint8_t*>(TextureCacheGetMip(texture, 0)) + srcPos.x + srcWidth * srcPos.y;
    uint8_t* row = reinterpret_cast<uint8_t*>(dstMips->m_mip[0]) + dstStride * dstPos.y + 4 * dstPos.x;

    for (uint32_t y = 0; y < height; y++, row += dstStride) {
        uint32_t y0 = y >> 1;
        uint32_t y1 = y0 + 1;

        if (y1 >= (height >> 1) - 1) {
            y1 = (height >> 1) - 1;
        }

        uint8_t* pixel = row;

        if (y & 1) {
            const uint8_t* line0 = src + srcWidth * y0;
            const uint8_t* line1 = src + srcWidth * y1;

            for (uint32_t x = 0; x < width; x++, pixel += 4) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (width >> 1) - 1) {
                    x1 = (width >> 1) - 1;
                }

                const BlpPalPixel& c00 = pal[line0[x0]];

                if (x & 1) {
                    const BlpPalPixel& c01 = pal[line0[x1]];
                    const BlpPalPixel& c10 = pal[line1[x0]];
                    const BlpPalPixel& c11 = pal[line1[x1]];
                    pixel[3] = 0xFF;
                    pixel[2] = (c00.m_r + c01.m_r + c10.m_r + c11.m_r) >> 2;
                    pixel[1] = (c00.m_g + c01.m_g + c10.m_g + c11.m_g) >> 2;
                    pixel[0] = (c01.m_b + c10.m_b + c11.m_b + c00.m_b) >> 2;
                } else {
                    const BlpPalPixel& c10 = pal[line1[x0]];
                    pixel[3] = 0xFF;
                    pixel[2] = (c00.m_r + c10.m_r) >> 1;
                    pixel[1] = (c00.m_g + c10.m_g) >> 1;
                    pixel[0] = (c00.m_b + c10.m_b) >> 1;
                }
            }
        } else {
            const uint8_t* line0 = src + srcWidth * y0;

            for (uint32_t x = 0; x < width; x++, pixel += 4) {
                uint32_t x1 = (x >> 1) + 1;

                if (x1 >= (width >> 1) - 1) {
                    x1 = (width >> 1) - 1;
                }

                const BlpPalPixel& c00 = pal[line0[x >> 1]];

                if (x & 1) {
                    const BlpPalPixel& c01 = pal[line0[x1]];
                    pixel[3] = 0xFF;
                    pixel[2] = (c00.m_r + c01.m_r) >> 1;
                    pixel[1] = (c00.m_g + c01.m_g) >> 1;
                    pixel[0] = (c00.m_b + c01.m_b) >> 1;
                } else {
                    pixel[3] = 0xFF;
                    pixel[2] = c00.m_r;
                    pixel[1] = c00.m_g;
                    pixel[0] = c00.m_b;
                }
            }
        }
    }

    int32_t halfWidth = srcSize.x >> 1;
    srcSize.x = halfWidth < 1 ? 1 : halfWidth;
    int32_t halfHeight = srcSize.y >> 1;
    srcSize.y = halfHeight < 1 ? 1 : halfHeight;
    dstPos.x >>= 1;
    dstPos.y >>= 1;
    CCharacterComponent::PasteOpaque(texture, pal, dstMips, dstPos, dstStride >> 1, srcPos, srcSize, srcInfo, 0, 1);
}

// OFFSET: 0x006FE050
void CCharacterComponent::PasteTransparent1BitScale(void* texture, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo) {
    uint32_t srcRowPitch = srcInfo.m_width;
    uint32_t copyWidth = srcSize.x;
    uint32_t copyHeight = srcSize.y;

    BlpPalPixel* pal = TextureCacheGetPal(texture);

    if (!pal) {
        return;
    }

    srcPos.x >>= 1;
    srcPos.y >>= 1;

    const uint8_t* srcBase = static_cast<const uint8_t*>(TextureCacheGetMip(texture, 0));
    const uint8_t* alphaBase = srcBase + srcRowPitch * srcInfo.m_height;

    uint8_t* dstRow = reinterpret_cast<uint8_t*>(dstMips->m_mip[0]) + dstStride * dstPos.y + 4 * dstPos.x;

    for (uint32_t y = 0; y < copyHeight; y++) {
        uint32_t y0 = y >> 1;
        uint32_t y1 = y0 + 1;

        if (y1 >= (copyHeight >> 1) - 1) {
            y1 = (copyHeight >> 1) - 1;
        }

        uint32_t rowOff0 = srcRowPitch * y0;
        uint32_t rowOff1 = srcRowPitch * y1;

        C4Pixel* dst = reinterpret_cast<C4Pixel*>(dstRow);

        if (y & 1) {
            const uint8_t* row0 = srcBase + rowOff0;
            const uint8_t* row1 = srcBase + rowOff1;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                const BlpPalPixel& p00 = pal[row0[x0]];
                const BlpPalPixel& p10 = pal[row1[x0]];

                uint32_t a00 = (alphaBase[(rowOff0 + x0) >> 3] & (1 << (x0 & 7))) ? 255 : 0;
                uint32_t a10 = (alphaBase[(rowOff1 + x0) >> 3] & (1 << (x0 & 7))) ? 255 : 0;

                uint32_t a;
                uint32_t srcB;
                uint32_t srcG;
                uint32_t srcR;

                if (x & 1) {
                    const BlpPalPixel& p01 = pal[row0[x1]];
                    const BlpPalPixel& p11 = pal[row1[x1]];

                    uint32_t a01 = (alphaBase[(rowOff0 + x1) >> 3] & (1 << (x1 & 7))) ? 255 : 0;
                    uint32_t a11 = (alphaBase[(rowOff1 + x1) >> 3] & (1 << (x1 & 7))) ? 255 : 0;

                    a = (a00 + a10 + a01 + a11) >> 2;
                    srcB = (p00.m_b + p01.m_b + p10.m_b + p11.m_b) >> 2;
                    srcG = (p00.m_g + p01.m_g + p10.m_g + p11.m_g) >> 2;
                    srcR = (p00.m_r + p01.m_r + p10.m_r + p11.m_r) >> 2;
                } else {
                    a = (a00 + a10) >> 1;
                    srcB = (p00.m_b + p10.m_b) >> 1;
                    srcG = (p00.m_g + p10.m_g) >> 1;
                    srcR = (p00.m_r + p10.m_r) >> 1;
                }

                dst[x].m_b = ((255 - a) * dst[x].m_b + a * srcB) >> 8;
                dst[x].m_g = ((255 - a) * dst[x].m_g + a * srcG) >> 8;
                dst[x].m_r = ((255 - a) * dst[x].m_r + a * srcR) >> 8;
                dst[x].m_a = 0xFF;
            }
        } else {
            const uint8_t* row0 = srcBase + rowOff0;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                const BlpPalPixel& p00 = pal[row0[x0]];

                uint32_t a00 = (alphaBase[(rowOff0 + x0) >> 3] & (1 << (x0 & 7))) ? 255 : 0;

                uint32_t a;
                uint32_t srcB;
                uint32_t srcG;
                uint32_t srcR;

                if (x & 1) {
                    const BlpPalPixel& p01 = pal[row0[x1]];

                    uint32_t a01 = (alphaBase[(rowOff0 + x1) >> 3] & (1 << (x1 & 7))) ? 255 : 0;

                    a = (a00 + a01) >> 1;
                    srcB = (p00.m_b + p01.m_b) >> 1;
                    srcG = (p00.m_g + p01.m_g) >> 1;
                    srcR = (p00.m_r + p01.m_r) >> 1;
                } else {
                    a = a00;
                    srcB = p00.m_b;
                    srcG = p00.m_g;
                    srcR = p00.m_r;
                }

                dst[x].m_b = ((255 - a) * dst[x].m_b + a * srcB) >> 8;
                dst[x].m_g = ((255 - a) * dst[x].m_g + a * srcG) >> 8;
                dst[x].m_r = ((255 - a) * dst[x].m_r + a * srcR) >> 8;
                dst[x].m_a = 0xFF;
            }
        }

        dstRow += dstStride;
    }

    srcSize.x = (srcSize.x >> 1) < 1 ? 1 : (srcSize.x >> 1);
    srcSize.y = (srcSize.y >> 1) < 1 ? 1 : (srcSize.y >> 1);

    dstPos.x >>= 1;
    dstPos.y >>= 1;

    CCharacterComponent::PasteTransparent1Bit(texture, pal, dstMips, dstPos, dstStride >> 1, srcPos, srcSize, srcInfo, 0, 1);
}

// OFFSET: 0x006FE5D0
void CCharacterComponent::PasteTransparent4BitScale(void* texture, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo) {
    uint32_t srcRowPitch = srcInfo.m_width;
    uint32_t copyWidth = srcSize.x;
    uint32_t copyHeight = srcSize.y;

    BlpPalPixel* pal = TextureCacheGetPal(texture);

    if (!pal) {
        return;
    }

    srcPos.x >>= 1;
    srcPos.y >>= 1;

    const uint8_t* srcBase = static_cast<const uint8_t*>(TextureCacheGetMip(texture, 0));
    const uint8_t* alphaBase = srcBase + srcRowPitch * srcInfo.m_height;

    uint8_t* dstRow = reinterpret_cast<uint8_t*>(dstMips->m_mip[0]) + dstStride * dstPos.y + 4 * dstPos.x;

    for (uint32_t y = 0; y < copyHeight; y++) {
        uint32_t y0 = y >> 1;
        uint32_t y1 = y0 + 1;

        if (y1 >= (copyHeight >> 1) - 1) {
            y1 = (copyHeight >> 1) - 1;
        }

        uint32_t rowOff0 = srcRowPitch * y0;
        uint32_t rowOff1 = srcRowPitch * y1;

        C4Pixel* dst = reinterpret_cast<C4Pixel*>(dstRow);

        if (y & 1) {
            const uint8_t* row0 = srcBase + rowOff0;
            const uint8_t* row1 = srcBase + rowOff1;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                uint32_t shift0 = 4 * (x0 & 1);

                const BlpPalPixel& p00 = pal[row0[x0]];
                const BlpPalPixel& p10 = pal[row1[x0]];

                uint32_t n00 = (alphaBase[(rowOff0 + x0) >> 1] & (15 << shift0)) >> shift0;
                uint32_t n10 = (alphaBase[(rowOff1 + x0) >> 1] & (15 << shift0)) >> shift0;

                uint32_t a00 = (n00 << 4) | n00;
                uint32_t a10 = (n10 << 4) | n10;

                uint32_t a;
                uint32_t srcB;
                uint32_t srcG;
                uint32_t srcR;

                if (x & 1) {
                    uint32_t shift1 = 4 * (x1 & 1);

                    const BlpPalPixel& p01 = pal[row0[x1]];
                    const BlpPalPixel& p11 = pal[row1[x1]];

                    uint32_t n01 = (alphaBase[(rowOff0 + x1) >> 1] & (15 << shift1)) >> shift1;
                    uint32_t n11 = (alphaBase[(rowOff1 + x1) >> 1] & (15 << shift1)) >> shift1;

                    uint32_t a01 = (n01 << 4) | n01;
                    uint32_t a11 = (n11 << 4) | n11;

                    a = (a00 + a01 + a10 + a11) >> 2;
                    srcB = (p00.m_b + p01.m_b + p10.m_b + p11.m_b) >> 2;
                    srcG = (p00.m_g + p01.m_g + p10.m_g + p11.m_g) >> 2;
                    srcR = (p00.m_r + p01.m_r + p10.m_r + p11.m_r) >> 2;
                } else {
                    a = (a00 + a10) >> 1;
                    srcB = (p00.m_b + p10.m_b) >> 1;
                    srcG = (p00.m_g + p10.m_g) >> 1;
                    srcR = (p00.m_r + p10.m_r) >> 1;
                }

                dst[x].m_b = ((255 - a) * dst[x].m_b + a * srcB) >> 8;
                dst[x].m_g = ((255 - a) * dst[x].m_g + a * srcG) >> 8;
                dst[x].m_r = ((255 - a) * dst[x].m_r + a * srcR) >> 8;
                dst[x].m_a = 0xFF;
            }
        } else {
            const uint8_t* row0 = srcBase + rowOff0;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                uint32_t shift0 = 4 * (x0 & 1);

                const BlpPalPixel& p00 = pal[row0[x0]];

                uint32_t n00 = (alphaBase[(rowOff0 + x0) >> 1] & (15 << shift0)) >> shift0;
                uint32_t a00 = (n00 << 4) | n00;

                uint32_t a;
                uint32_t srcB;
                uint32_t srcG;
                uint32_t srcR;

                if (x & 1) {
                    uint32_t shift1 = 4 * (x1 & 1);

                    const BlpPalPixel& p01 = pal[row0[x1]];

                    uint32_t n01 = (alphaBase[(rowOff0 + x1) >> 1] & (15 << shift1)) >> shift1;
                    uint32_t a01 = (n01 << 4) | n01;

                    a = (a00 + a01) >> 1;
                    srcB = (p00.m_b + p01.m_b) >> 1;
                    srcG = (p00.m_g + p01.m_g) >> 1;
                    srcR = (p00.m_r + p01.m_r) >> 1;
                } else {
                    a = a00;
                    srcB = p00.m_b;
                    srcG = p00.m_g;
                    srcR = p00.m_r;
                }

                dst[x].m_b = ((255 - a) * dst[x].m_b + a * srcB) >> 8;
                dst[x].m_g = ((255 - a) * dst[x].m_g + a * srcG) >> 8;
                dst[x].m_r = ((255 - a) * dst[x].m_r + a * srcR) >> 8;
                dst[x].m_a = 0xFF;
            }
        }

        dstRow += dstStride;
    }

    srcSize.x = (srcSize.x >> 1) < 1 ? 1 : (srcSize.x >> 1);
    srcSize.y = (srcSize.y >> 1) < 1 ? 1 : (srcSize.y >> 1);

    dstPos.x >>= 1;
    dstPos.y >>= 1;

    CCharacterComponent::PasteTransparent4Bit(texture, pal, dstMips, dstPos, dstStride >> 1, srcPos, srcSize, srcInfo, 0, 1);
}

// OFFSET: 0x006FEBA0
void CCharacterComponent::PasteTransparent8BitScale(void* texture, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo) {
    uint32_t srcRowPitch = srcInfo.m_width;
    uint32_t copyWidth = srcSize.x;
    uint32_t copyHeight = srcSize.y;

    BlpPalPixel* pal = TextureCacheGetPal(texture);

    if (!pal) {
        return;
    }

    srcPos.x >>= 1;
    srcPos.y >>= 1;

    const uint8_t* srcBase = static_cast<const uint8_t*>(TextureCacheGetMip(texture, 0));
    const uint8_t* alphaBase = srcBase + srcRowPitch * srcInfo.m_height;

    uint8_t* dstRow = reinterpret_cast<uint8_t*>(dstMips->m_mip[0]) + dstStride * dstPos.y + 4 * dstPos.x;

    for (uint32_t y = 0; y < copyHeight; y++) {
        uint32_t y0 = y >> 1;
        uint32_t y1 = y0 + 1;

        if (y1 >= (copyHeight >> 1) - 1) {
            y1 = (copyHeight >> 1) - 1;
        }

        uint32_t rowOff0 = srcRowPitch * y0;
        uint32_t rowOff1 = srcRowPitch * y1;

        C4Pixel* dst = reinterpret_cast<C4Pixel*>(dstRow);

        if (y & 1) {
            const uint8_t* row0 = srcBase + rowOff0;
            const uint8_t* row1 = srcBase + rowOff1;

            const uint8_t* alphaRow0 = alphaBase + rowOff0;
            const uint8_t* alphaRow1 = alphaBase + rowOff1;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                const BlpPalPixel& p00 = pal[row0[x0]];
                const BlpPalPixel& p10 = pal[row1[x0]];

                uint32_t a;
                uint32_t srcB;
                uint32_t srcG;
                uint32_t srcR;

                if (x & 1) {
                    const BlpPalPixel& p01 = pal[row0[x1]];
                    const BlpPalPixel& p11 = pal[row1[x1]];

                    a = (alphaRow1[x0] + alphaRow1[x1] + alphaRow0[x0] + alphaRow0[x1]) >> 2;
                    srcB = (p00.m_b + p01.m_b + p10.m_b + p11.m_b) >> 2;
                    srcG = (p00.m_g + p01.m_g + p10.m_g + p11.m_g) >> 2;
                    srcR = (p00.m_r + p01.m_r + p10.m_r + p11.m_r) >> 2;
                } else {
                    a = (alphaRow1[x0] + alphaRow0[x0]) >> 1;
                    srcB = (p00.m_b + p10.m_b) >> 1;
                    srcG = (p00.m_g + p10.m_g) >> 1;
                    srcR = (p00.m_r + p10.m_r) >> 1;
                }

                dst[x].m_b = ((255 - a) * dst[x].m_b + a * srcB) >> 8;
                dst[x].m_g = ((255 - a) * dst[x].m_g + a * srcG) >> 8;
                dst[x].m_r = ((255 - a) * dst[x].m_r + a * srcR) >> 8;
                dst[x].m_a = 0xFF;
            }
        } else {
            const uint8_t* row0 = srcBase + rowOff0;

            const uint8_t* alphaRow0 = alphaBase + rowOff0;

            for (uint32_t x = 0; x < copyWidth; x++) {
                uint32_t x0 = x >> 1;
                uint32_t x1 = x0 + 1;

                if (x1 >= (copyWidth >> 1) - 1) {
                    x1 = (copyWidth >> 1) - 1;
                }

                const BlpPalPixel& p00 = pal[row0[x0]];

                uint32_t a;
                uint32_t srcB;
                uint32_t srcG;
                uint32_t srcR;

                if (x & 1) {
                    const BlpPalPixel& p01 = pal[row0[x1]];

                    a = (alphaRow0[x0] + alphaRow0[x1]) >> 1;
                    srcB = (p00.m_b + p01.m_b) >> 1;
                    srcG = (p00.m_g + p01.m_g) >> 1;
                    srcR = (p00.m_r + p01.m_r) >> 1;
                } else {
                    a = alphaRow0[x0];
                    srcB = p00.m_b;
                    srcG = p00.m_g;
                    srcR = p00.m_r;
                }

                dst[x].m_b = ((255 - a) * dst[x].m_b + a * srcB) >> 8;
                dst[x].m_g = ((255 - a) * dst[x].m_g + a * srcG) >> 8;
                dst[x].m_r = ((255 - a) * dst[x].m_r + a * srcR) >> 8;
                dst[x].m_a = 0xFF;
            }
        }

        dstRow += dstStride;
    }

    srcSize.x = (srcSize.x >> 1) < 1 ? 1 : (srcSize.x >> 1);
    srcSize.y = (srcSize.y >> 1) < 1 ? 1 : (srcSize.y >> 1);

    dstPos.x >>= 1;
    dstPos.y >>= 1;

    CCharacterComponent::PasteTransparent8Bit(texture, pal, dstMips, dstPos, dstStride >> 1, srcPos, srcSize, srcInfo, 0, 1);
}

// PROVEN: d21b0506 IDENTICAL/NONE
// OFFSET: 0x006FF010/0x1403C99C0
CHelmPathSuffix::CHelmPathSuffix(const char* format, ...) {
    va_list args;
    va_start(args, format);
    this->m_length = SStrVPrintf(this->m_buffer, sizeof(this->m_buffer), format, args);
}

// PROVEN: 831e559e IDENTICAL/NONE
// OFFSET: 0x006FF040/0x1403C9A00
bool CCharacterComponent::VariationsLoaded(int32_t wait) {
    TCTEXTUREINFO info;
    info.m_width = 0;
    info.m_height = 0;
    info.m_mipCount = 0;
    info.m_alphaSize = 0;
    info.m_opaque = 1;

    for (int32_t variation = 0; variation < NUM_COMPONENT_VARIATIONS; variation++) {
        for (int32_t i = 0; i < 3; i++) {
            if (this->m_baseVariation[variation].m_texture[i] && !TextureCacheGetInfo(this->m_baseVariation[variation].m_texture[i], info, wait)) {
                return false;
            }
        }
    }

    this->m_flags &= ~0x2;

    return true;
}

// OFFSET: 0x006FF0D0
bool CCharacterComponent::ItemsLoaded(int32_t wait) {
    bool loaded = true;
    TCTEXTUREINFO info;
    info.m_width = 0;
    info.m_height = 0;
    info.m_mipCount = 0;
    info.m_alphaSize = 0;
    info.m_opaque = 1;

    for (int32_t section = 0; section < NUM_COMPONENT_SECTIONS; section++) {
        if (!((1 << section) & this->m_dirtySections)) {
            continue;
        }

        CharacterSection& sectionData = this->m_section[section];

        for (int32_t layer = 0; layer < 8; layer++) {
            if (sectionData.m_layerItemDisplayId[layer]) {
                if (!sectionData.m_layerTex[layer]) {
                    ItemDisplayInfoRec rec;

                    if (g_itemDisplayInfoDB.GetRecord(sectionData.m_layerItemDisplayId[layer], &rec)) {
                        sectionData.m_layerTex[layer] = this->LoadItemComponentTexture(&rec, static_cast<COMPONENT_SECTIONS>(section));
                    }
                }
            } else if (!sectionData.m_layerTex[layer]) {
                continue;
            }

            if (wait) {
                if (!sectionData.m_layerTex[layer]) {
                    continue;
                }

                TextureCacheGetInfo(sectionData.m_layerTex[layer], info, 1);
            } else if (!TextureCacheGetInfo(sectionData.m_layerTex[layer], info, 0)) {
                loaded = false;
                continue;
            }

            if (TextureCacheHasMips(sectionData.m_layerTex[layer])) {
                sectionData.m_layerMask |= 1 << layer;
            } else {
                sectionData.m_layerMask &= ~(1 << layer);
            }
        }
    }

    return loaded;
}

// OFFSET: 0x006FF2F0
void CCharacterComponent::LinkToAsyncHead() {
    TCTEXTUREINFO info;
    info.m_width = 0;
    info.m_height = 0;
    info.m_mipCount = 0;
    info.m_alphaSize = 0;
    info.m_opaque = 1;

    for (int32_t variation = 0; variation < NUM_COMPONENT_VARIATIONS; variation++) {
        for (int32_t i = 0; i < 3; i++) {
            void* texture = this->m_baseVariation[variation].m_texture[i];

            if (texture && !TextureCacheGetInfo(texture, info, 0)) {
                TextureCacheIncreasePriority(texture);
            }
        }
    }

    for (int32_t section = 0; section < NUM_COMPONENT_SECTIONS; section++) {
        for (int32_t layer = 0; layer < 8; layer++) {
            void* texture = this->m_section[section].m_layerTex[layer];

            if (texture && !TextureCacheGetInfo(texture, info, 0)) {
                TextureCacheIncreasePriority(texture);
            }
        }
    }
}

// OFFSET: 0x006FF3B0/0x1403C9F50
CComponentMipBits* CCharacterComponent::AllocComponentMipBits(EGxTexFormat format) {
    s_mipBitsCritSect.Enter();

    CComponentMipBits* mipBits = format == GxTex_Dxt1 ? s_mipBitsCompressedFreeList.Head() : s_mipBitsFreeList.Head();

    if (mipBits) {
        mipBits->m_link.Unlink();
    }

    s_mipBitsCritSect.Leave();

    return mipBits;
}

// OFFSET: 0x006FF450
void CCharacterComponent::RenderPrepAll() {
    this->SetTexturesDirty();
    this->m_dirtySections = 0xFFFFFFFF;

    if (this->m_request) {
        this->m_request->m_active = 0;
        this->m_request = NULL;
    }

    this->m_flags &= ~0x8;
    this->VariationsLoaded(1);
    this->ItemsLoaded(1);

    for (int32_t section = 0; section < NUM_COMPONENT_SECTIONS; section++) {
        (this->*CCharacterComponent::m_prepFunc[section])();
    }

    if (this->m_gxTexFormat == GxTex_Dxt1) {
        CompressMipLevels(CCharacterComponent::s_textureBuffer, s_compressedTextureBuffer, this->m_mipLevels);
    }

    this->m_flags &= ~0x1;
    this->m_dirtySections = 0;
    this->DestroyRenderTextures();
    this->m_link.Unlink();
}

// OFFSET: 0x006FF530/0x1403CA120
int32_t CCharacterComponent::ComposeHelmModelFilePath(const ChrRacesRec* raceRec, const ItemDisplayInfoRec* displayRec, char* buffer, uint32_t size) {
    char path[260];
    SStrPrintf(path, "Item\\ObjectComponents\\Head\\%s", static_cast<const char*>(displayRec->m_modelName[0]));
    OsPathReplaceExtension(path, sizeof(path), path, "");
    CHelmPathSuffix suffix("_%s%s.m2", static_cast<const char*>(raceRec->m_clientPrefix), s_fileDecorations[this->m_data.m_preferences.m_sexID]);
    SStrPack(path, suffix.m_buffer, sizeof(path));
    strcpy(buffer, path);

    return 1;
}

// PROVEN: 4801c2c0 IDENTICAL/NONE
// OFFSET: 0x006FF600
int32_t CCharacterComponent::GetHelmModelFilePath(ItemDisplayInfoRec& rec, const ItemDisplayInfoRec*& displayRec) {
    if (!this->m_itemDisplayID[ITEMSLOT_HEAD]) {
        return 0;
    }

    if (!g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_HEAD], &rec) || !rec.m_modelName[0] || !*rec.m_modelName[0]) {
        return 0;
    }

    displayRec = &rec;
    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(this->m_data.m_preferences.m_raceID);

    if (!raceRec) {
        return 0;
    }

    return this->ComposeHelmModelFilePath(raceRec, &rec, s_pathEnd, 260);
}

// OFFSET: 0x006FF6A0
bool CCharacterComponent::IsHelmModelCorrect() {
    CM2Model* attached = this->m_data.m_model->m_attachList;

    if (!attached) {
        return false;
    }

    while (attached->m_attachmentId != ATTACH_HELM) {
        attached = attached->m_attachNext;

        if (!attached) {
            return false;
        }
    }

    ItemDisplayInfoRec rec;
    const ItemDisplayInfoRec* displayRec;

    if (!attached->IsDrawable() || (attached->m_fFlags & 0x2000) || !this->GetHelmModelFilePath(rec, displayRec)) {
        return false;
    }

    bool hideHair = false;
    HelmetGeosetVisDataRec* visRec = g_helmetGeosetVisDataDB.GetRecord(displayRec->m_helmetGeosetVisID[this->m_data.m_preferences.m_sexID]);

    if (visRec) {
        hideHair = ((1 << this->m_data.m_preferences.m_raceID) & visRec->m_hideGeoset[0]) != 0;
    }

    if (hideHair != (this->m_data.m_geosets[0] == 1)) {
        return false;
    }

    OsPathReplaceExtension(s_path, sizeof(s_path), s_path, "m2");

    return _stricmp(attached->m_shared->m_fileName, s_path) == 0;
}

// OFFSET: 0x006FF770/0x1403CA470
void CCharacterComponent::AddHelm(int32_t itemVisual) {
    ItemDisplayInfoRec rec;
    const ItemDisplayInfoRec* displayRec;

    if (!this->GetHelmModelFilePath(rec, displayRec)) {
        return;
    }

    SStrPrintf(s_buffer, "Item\\ObjectComponents\\Head\\%s.blp", displayRec->m_modelTexture[0]);
    strcpy(s_pathEnd2, s_buffer);

    CCharacterComponent::AddLink(this->m_data.m_model, &this->m_attachMask, ATTACH_HELM, s_path, s_path2, itemVisual, displayRec);

    HelmetGeosetVisDataRec* visRec = g_helmetGeosetVisDataDB.GetRecord(displayRec->m_helmetGeosetVisID[this->m_data.m_preferences.m_sexID]);

    if (!visRec) {
        return;
    }

    uint32_t raceMask = 1 << this->m_data.m_preferences.m_raceID;

    if (raceMask & visRec->m_hideGeoset[0]) {
        this->m_data.m_geosets[0] = 1;
    }

    if (raceMask & visRec->m_hideGeoset[1]) {
        this->m_data.m_geosets[1] = 101;
    }

    if (raceMask & visRec->m_hideGeoset[2]) {
        this->m_data.m_geosets[2] = 201;
    }

    if (raceMask & visRec->m_hideGeoset[3]) {
        this->m_data.m_geosets[3] = 301;
    }

    if (raceMask & visRec->m_hideGeoset[4]) {
        this->m_data.m_geosets[7] = 701;
    }

    if (raceMask & visRec->m_hideGeoset[5]) {
        this->m_data.m_geosets[16] = 1601;
    }

    if (raceMask & visRec->m_hideGeoset[6]) {
        this->m_data.m_geosets[17] = 1701;
    }
}

// OFFSET: 0x006FF940
void CCharacterComponent::AddShoulders(int32_t itemVisual) {
    ItemDisplayInfoRec rec;
    const ItemDisplayInfoRec* displayRec;
    char leftModel[260];
    char rightModel[260];
    char leftTexture[260];
    char rightTexture[260];

    if (!this->BuildShoulderItemPaths(rec, displayRec, leftModel, rightModel, leftTexture, rightTexture)) {
        return;
    }

    CM2Model* left = NULL;
    CM2Model* right = NULL;

    if (rightModel[0]) {
        right = CCharacterComponent::AddLink(this->m_data.m_model, &this->m_attachMask, ATTACH_SHOULDERRIGHT, rightModel, rightTexture, itemVisual, displayRec);
    }

    if (leftModel[0]) {
        left = CCharacterComponent::AddLink(this->m_data.m_model, &this->m_attachMask, ATTACH_SHOULDERLEFT, leftModel, leftTexture, itemVisual, displayRec);
    }

    this->LinkShoulderAnimations(left, right, displayRec);
}

// OFFSET: 0x006FFAB0
void CCharacterComponent::RelinkShoulderAnimations(CM2Model* model) {
    ItemDisplayInfoRec rec;
    const ItemDisplayInfoRec* displayRec = NULL;

    if (!this->GetShoulderDisplayInfo(rec, displayRec)) {
        return;
    }

    CM2Model* left = model->m_attachList;

    while (left && left->m_attachmentId != ATTACH_SHOULDERLEFT) {
        left = left->m_attachNext;
    }

    CM2Model* right = model->m_attachList;

    while (right && right->m_attachmentId != ATTACH_SHOULDERRIGHT) {
        right = right->m_attachNext;
    }

    this->LinkShoulderAnimations(left, right, displayRec);
}

// OFFSET: 0x006FFB30
void CCharacterComponent::SetHandItemDisplay(int32_t displayID, uint32_t hand, INVENTORY_SLOTS invSlot, bool shield) {
    ItemDisplayInfoRec rec;

    if (g_itemDisplayInfoDB.GetRecord(displayID, &rec)) {
        this->m_handItemDisplayID[hand] = displayID;
        CCharacterComponent::AddHandItem(this->m_data.m_model, &this->m_attachMask, &rec, invSlot, SHEATHE_NONE, false, shield, false, 0);
    }
}

// PROVEN: fdc670b6 IDENTICAL/NONE
// OFFSET: 0x006FFBB0
void CCharacterComponent::SetMainHandItemDisplay(int32_t displayID) {
    this->SetHandItemDisplay(displayID, 0, INVSLOT_MAINHAND, false);
}

// PROVEN: 22db7984 IDENTICAL/NONE
// OFFSET: 0x006FFBD0
void CCharacterComponent::SetOffHandItemDisplay(int32_t displayID) {
    this->SetHandItemDisplay(displayID, 1, INVSLOT_OFFHAND, false);
}

// PROVEN: aae38eaa IDENTICAL/NONE
// OFFSET: 0x006FFBF0
void CCharacterComponent::SetShieldItemDisplay(int32_t displayID) {
    this->SetHandItemDisplay(displayID, 1, INVSLOT_OFFHAND, true);
}

// PROVEN: adb9674e IDENTICAL/NONE
// OFFSET: 0x006FFC10
GEOCOMPONENTLINKS CCharacterComponent::AddHandItem(const ItemDisplayInfoRec* displayRec, INVENTORY_SLOTS invSlot, SHEATHE_TYPE sheatheType, bool sheathed, bool shield, bool mainHand, int32_t itemVisual) {
    return CCharacterComponent::AddHandItem(this->m_data.m_model, &this->m_attachMask, displayRec, invSlot, sheatheType, sheathed, shield, mainHand, itemVisual);
}

// PROVEN: 7d9cd055 IDENTICAL/NONE
// OFFSET: 0x006FFC50/0x1403CAD80
void CCharacterComponent::RemoveHandItem(INVENTORY_SLOTS invSlot, SHEATHE_TYPE sheatheType, bool shield) {
    CCharacterComponent::RemoveHandItem(this->m_data.m_model, &this->m_attachMask, invSlot, sheatheType, shield);
}

// PROVEN: 7b1c32bf IDENTICAL/NONE
// OFFSET: 0x006FFC80
void CCharacterComponent::ClearGuildTabard(int32_t count) {
    for (int32_t layer = count; layer > 3; layer--) {
        this->ClearSectionLayer(SECTION_TORSO_UPPER, layer);
    }

    for (int32_t layer = count; layer > 3; layer--) {
        this->ClearSectionLayer(SECTION_TORSO_LOWER, layer);
    }
}

// PROVEN: 002c784d IDENTICAL/NONE
// OFFSET: 0x006FFCC0
void CCharacterComponent::ClearGuildShirt(int32_t count) {
    for (int32_t layer = count; layer >= 0; layer--) {
        this->ClearSectionLayer(SECTION_TORSO_UPPER, layer);
    }

    for (int32_t layer = count; layer >= 0; layer--) {
        this->ClearSectionLayer(SECTION_TORSO_LOWER, layer);
    }

    for (int32_t layer = count; layer >= 0; layer--) {
        this->ClearSectionLayer(SECTION_ARM_UPPER, layer);
    }

    for (int32_t layer = count; layer >= 0; layer--) {
        this->ClearSectionLayer(SECTION_ARM_LOWER, layer);
    }
}

// OFFSET: 0x006FFD30
void CCharacterComponent::ApplyGuildColor(uint32_t flags, int32_t emblem, int32_t emblemColor, int32_t border, int32_t borderColor, int32_t background) {
    this->ClearGuildTabard(6);

    for (int32_t i = 0; i < 2; i++) {
        COMPONENT_SECTIONS section = s_tabardSections[i];
        CCharacterComponent::GetTabardBackgroundFileName(flags, section, background, s_buffer, sizeof(s_buffer));
        SStrPack(s_buffer, ".blp", sizeof(s_buffer));
        this->LoadSectionLayer(s_buffer, section, 4);
    }

    for (int32_t i = 0; i < 2; i++) {
        COMPONENT_SECTIONS section = s_tabardSections[i];
        CCharacterComponent::GetTabardEmblemFileName(section, emblem, emblemColor, s_buffer, sizeof(s_buffer));
        SStrPack(s_buffer, ".blp", sizeof(s_buffer));
        this->LoadSectionLayer(s_buffer, section, 5);
    }

    for (int32_t i = 0; i < 2; i++) {
        COMPONENT_SECTIONS section = s_tabardSections[i];
        CCharacterComponent::GetTabardBorderFileName(flags, section, border, borderColor, s_buffer, sizeof(s_buffer));
        SStrPack(s_buffer, ".blp", sizeof(s_buffer));
        this->LoadSectionLayer(s_buffer, section, 6);
    }

    this->SetDirtySections((1 << SECTION_TORSO_UPPER) | (1 << SECTION_TORSO_LOWER));
}

// OFFSET: 0x006FFF10
void CCharacterComponent::ApplyShirtGuildColor(uint32_t flags, int32_t emblem, int32_t emblemColor, int32_t border, int32_t borderColor, int32_t background) {
    this->ClearGuildShirt(2);

    for (int32_t i = 0; i < 4; i++) {
        COMPONENT_SECTIONS section = s_shirtSections[i];
        CCharacterComponent::GetShirtBackgroundFileName(flags, section, background, s_buffer, sizeof(s_buffer));
        SStrPack(s_buffer, ".blp", sizeof(s_buffer));
        this->LoadSectionLayer(s_buffer, section, 0);
    }

    for (int32_t i = 0; i < 4; i++) {
        COMPONENT_SECTIONS section = s_shirtSections[i];
        CCharacterComponent::GetTabardEmblemFileName(section, emblem, emblemColor, s_buffer, sizeof(s_buffer));
        SStrPack(s_buffer, ".blp", sizeof(s_buffer));
        this->LoadSectionLayer(s_buffer, section, 1);
    }

    for (int32_t i = 0; i < 4; i++) {
        COMPONENT_SECTIONS section = s_shirtSections[i];
        CCharacterComponent::GetShirtBorderFileName(flags, section, border, borderColor, s_buffer, sizeof(s_buffer));
        SStrPack(s_buffer, ".blp", sizeof(s_buffer));
        this->LoadSectionLayer(s_buffer, section, 2);
    }

    this->SetDirtySections((1 << SECTION_ARM_UPPER) | (1 << SECTION_ARM_LOWER) | (1 << SECTION_TORSO_UPPER) | (1 << SECTION_TORSO_LOWER));
}

// PROVEN: 7b1d2e33 IDENTICAL/NONE
// OFFSET: 0x007000F0/0x1403CB770
void CCharacterComponent::TurnOffGuildTabardPurchase() {
    if (!(this->m_flags & 0x40)) {
        return;
    }

    this->m_flags = (this->m_flags & ~0x44) | 0x4;
    this->ClearGuildTabard(6);
    this->SetDirtySections((1 << SECTION_TORSO_UPPER) | (1 << SECTION_TORSO_LOWER));
}

// OFFSET: 0x00700160/0x1403C85E0
void CCharacterComponent::Paste(void* texture, MipBits* dstMips, const C2iVector& dstPos, const C2iVector& srcPos, const C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, uint32_t mipLevels) {
    C2iVector pos = dstPos;
    C2iVector src = srcPos;
    C2iVector size = srcSize;
    uint32_t stride = 4 * (1 << mipLevels);
    BlpPalPixel* pal = TextureCacheGetPal(texture);

    if (!pal) {
        CCharacterComponent::PasteCrappyGreen(dstMips, pos, stride, size, srcInfo, srcMipLevel, -static_cast<int32_t>(srcMipLevel));
        return;
    }

    switch (srcInfo.m_alphaSize) {
        case 0:
            CCharacterComponent::PasteOpaque(texture, pal, dstMips, pos, stride, src, size, srcInfo, srcMipLevel, -static_cast<int32_t>(srcMipLevel));
            break;

        case 1:
            CCharacterComponent::PasteTransparent1Bit(texture, pal, dstMips, pos, stride, src, size, srcInfo, srcMipLevel, -static_cast<int32_t>(srcMipLevel));
            break;

        case 4:
            CCharacterComponent::PasteTransparent4Bit(texture, pal, dstMips, pos, stride, src, size, srcInfo, srcMipLevel, -static_cast<int32_t>(srcMipLevel));
            break;

        case 8:
            CCharacterComponent::PasteTransparent8Bit(texture, pal, dstMips, pos, stride, src, size, srcInfo, srcMipLevel, -static_cast<int32_t>(srcMipLevel));
            break;
    }
}

// OFFSET: 0x007002B0/0x1403CB810
void CCharacterComponent::PasteScale(void* texture, MipBits* dstMips, const C2iVector& dstPos, const C2iVector& srcPos, const C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t mipLevels) {
    C2iVector pos = dstPos;
    C2iVector src = srcPos;
    C2iVector size = srcSize;
    uint32_t stride = 4 * (1 << mipLevels);

    if (!TextureCacheGetPal(texture)) {
        CCharacterComponent::PasteCrappyGreen(dstMips, pos, stride, size, srcInfo, 0, 0);
        return;
    }

    switch (srcInfo.m_alphaSize) {
        case 0:
            CCharacterComponent::PasteOpaqueScale(texture, dstMips, pos, stride, src, size, srcInfo);
            break;

        case 1:
            CCharacterComponent::PasteTransparent1BitScale(texture, dstMips, pos, stride, src, size, srcInfo);
            break;

        case 4:
            CCharacterComponent::PasteTransparent4BitScale(texture, dstMips, pos, stride, src, size, srcInfo);
            break;

        case 8:
            CCharacterComponent::PasteTransparent8BitScale(texture, dstMips, pos, stride, src, size, srcInfo);
            break;
    }
}

// OFFSET: 0x007003C0
void CCharacterComponent::ComputeLoadProgress(int64_t& local, int64_t& total) {
    for (int32_t variation = 0; variation < NUM_COMPONENT_VARIATIONS; variation++) {
        for (int32_t i = 0; i < 3; i++) {
            if (this->m_baseVariation[variation].m_texture[i]) {
                TextureCacheComputeLoadProgress(this->m_baseVariation[variation].m_texture[i], local, total);
            }
        }
    }

    for (int32_t section = 0; section < NUM_COMPONENT_SECTIONS; section++) {
        for (int32_t layer = 0; layer < 8; layer++) {
            if (this->m_section[section].m_layerTex[layer]) {
                TextureCacheComputeLoadProgress(this->m_section[section].m_layerTex[layer], local, total);
            }
        }
    }

    if (local == total && !(this->m_flags & 0x1)) {
        return;
    }

    total += 0x2000;

    if (!this->m_link.m_next && this->m_baseTexture && !(this->m_dirtySections | -static_cast<int32_t>(this->m_flags & 0x1))) {
        local += 0x2000;
    }
}

// OFFSET: 0x007004B0
CCharacterComponent::CCharacterComponent() {
    this->m_attachMask = 0;
    memset(this->m_baseVariation, 0, sizeof(this->m_baseVariation));
    this->m_flags |= 0x7;
    this->m_dirtySections = 0xFFFFFFFF;
    this->m_request = NULL;
    this->m_baseTexture = NULL;
    this->m_extraSkinTexture = NULL;
    this->m_hairTexture = NULL;
    this->m_gxTexFormat = CCharacterComponent::s_gxFormat;
    this->m_mipLevels = CCharacterComponent::s_mipLevels;
    memset(this->m_itemDisplayID, 0, sizeof(this->m_itemDisplayID));
    this->m_handItemDisplayID[0] = 0;
    this->m_handItemDisplayID[1] = 0;
    this->m_handItemDisplayID[2] = 0;
    this->m_flags &= ~0x20;
    this->m_data.m_model = NULL;
    s_componentList.LinkToTail(this);
}

// OFFSET: 0x007005A0
CCharacterComponent::~CCharacterComponent() {
    if (this->m_request) {
        this->m_request->m_active = 0;
        this->m_request = NULL;
    }

    this->ReleaseBaseTexture();

    if (this->m_extraSkinTexture) {
        TextureRelease(this->m_extraSkinTexture);
    }

    if (this->m_hairTexture) {
        TextureRelease(this->m_hairTexture);
    }

    if (this->m_data.m_model) {
        this->m_data.m_model->Release();
        this->m_data.m_model = NULL;
    }

    this->m_link.Unlink();
    this->m_componentLink.Unlink();

    if (s_closestComponent == this) {
        CCharacterComponent::ResetClosestComponent();
    }
}

// OFFSET: 0x00700790/0x1403CBF60
void CCharacterComponent::UpdateBaseTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    CCharacterComponent* component = static_cast<CCharacterComponent*>(userArg);

    switch (command) {
        case GxTex_Lock:
            if (!CCharacterComponent::s_bInRenderPrep) {
                component->RenderPrepAll();
            }

            break;

        case GxTex_Latch:
            if (component->m_gxTexFormat == GxTex_Dxt1) {
                texelStrideInBytes = GxCalcTexelStrideInBytes(GxTex_Dxt1, width);
            } else {
                texelStrideInBytes = 4 * width;
            }

            if (component->m_request) {
                texels = component->m_request->m_mipBits->m_mipBits->m_mip[level];
            } else {
                MipBits* mipBits = s_compressedTextureBuffer;

                if (component->m_gxTexFormat != GxTex_Dxt1) {
                    mipBits = CCharacterComponent::s_textureBuffer;
                }

                texels = mipBits->m_mip[level];
            }

            break;

        case GxTex_3:
            if (!(component->m_data.m_flags & 0x2)) {
                component->SetTexturesDirty();
                component->SetDirtySections(0xFFFFFFFF);
            }

            break;

        default:
            break;
    }
}

// OFFSET: 0x00700850
void CCharacterComponent::Prep() {
    if (this->m_data.m_flags & 0x1) {
        if (this->m_flags & 0x4) {
            this->GeosRenderPrep();
        }

        return;
    }

    if (this->m_dirtySections || (this->m_flags & 0x1)) {
        if (!this->m_link.m_next) {
            s_queue.LinkToTail(this);
        }
    } else if (this->m_flags & 0x4) {
        this->GeosRenderPrep();
    }
}

// PROVEN: 12bb3fe5 IDENTICAL/NONE
// OFFSET: 0x00700890/0x1403CC240
void CCharacterComponent::CreateBaseTexture() {
    EGxTexFormat dataFormat = GxTex_Argb8888;

    if (this->m_gxTexFormat == GxTex_Dxt1) {
        dataFormat = GxTex_Dxt1;
    }

    CGxTexFlags flags;
    flags.m_filter = GxTex_LinearMipLinear;

    if (g_theGxDevicePtr->DeviceApi() == GxApi_GLL) {
        flags.m_bit14 = 1;
    }

    this->m_baseTexture = TextureCreate(1 << this->m_mipLevels, 1 << this->m_mipLevels, this->m_gxTexFormat, dataFormat, flags, this, &CCharacterComponent::UpdateBaseTexture, "CharacterBaseSkin", 1);
    this->m_data.m_model->ReplaceTexture(1, this->m_baseTexture);
}

// OFFSET: 0x00700920/0x1403CC300
void CCharacterComponent::RemoveItem(ITEM_SLOT itemSlot) {
    if (!this->m_itemDisplayID[itemSlot]) {
        return;
    }

    this->m_flags |= 0x4;

    switch (itemSlot) {
        case ITEMSLOT_HEAD: {
            this->m_data.m_geosets[0] = GetConditionalGeoset(&this->m_data.m_preferences);
            CCharacterComponent::RemoveLinkpt(this->m_data.m_model, &this->m_attachMask, ATTACH_HELM);
            CharacterFacialHairStylesRec* styleRec = GetConditionalFacialHairStyle(&this->m_data.m_preferences);

            if (styleRec) {
                this->m_data.m_geosets[1] = styleRec->m_geoset[0] + 100;
                this->m_data.m_geosets[3] = styleRec->m_geoset[1] + 300;
                this->m_data.m_geosets[2] = styleRec->m_geoset[2] + 200;
                this->m_data.m_geosets[7] = 702;
                this->m_data.m_geosets[16] = styleRec->m_geoset[3] + 1600;
                this->m_data.m_geosets[17] = styleRec->m_geoset[4] + 1700;
            }

            this->m_itemDisplayID[itemSlot] = 0;
            return;
        }

        case ITEMSLOT_SHOULDER:
            CCharacterComponent::RemoveLinkpt(this->m_data.m_model, &this->m_attachMask, ATTACH_SHOULDERLEFT);
            CCharacterComponent::RemoveLinkpt(this->m_data.m_model, &this->m_attachMask, ATTACH_SHOULDERRIGHT);
            this->m_itemDisplayID[itemSlot] = 0;
            return;

        case ITEMSLOT_SHIRT:
            this->ClearGuildShirt(2);
            break;

        case ITEMSLOT_CHEST:
            this->m_flags &= ~0x20;
            break;

        case ITEMSLOT_TABARD:
            if (this->m_flags & 0x40) {
                this->m_itemDisplayID[itemSlot] = 0;
                return;
            }

            this->m_data.m_geosets[15] = 1501;
            this->ClearGuildTabard(6);
            break;

        case ITEMSLOT_CAPE:
            this->m_data.m_geosets[15] = 1501;
            this->m_itemDisplayID[itemSlot] = 0;
            return;

        case ITEMSLOT_11:
            return;

        default:
            break;
    }

    const ItemDisplayInfoRec* displayRec = NULL;
    ItemDisplayInfoRec rec;

    if (g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[itemSlot], &rec)) {
        displayRec = &rec;
    }

    this->m_itemDisplayID[itemSlot] = 0;

    for (int32_t section = 0; section < 8; section++) {
        (this->*CCharacterComponent::m_itemFunc[section])(itemSlot, displayRec, false);
    }
}

// OFFSET: 0x00700B90/0x1403CC610
void CCharacterComponent::RemoveItemBySlot(INVENTORY_SLOTS invSlot) {
    ITEM_SLOT itemSlot = CCharacterComponent::GetItemSlotForInvSlot(invSlot);

    if (itemSlot != NUM_ITEM_SLOTS) {
        this->RemoveItem(itemSlot);
    }
}

// OFFSET: 0x00701070
void CCharacterComponent::RemoveItemByType(INVENTORY_TYPES invType) {
    ITEM_SLOT itemSlot = CCharacterComponent::GetItemSlotForInvType(invType);

    if (itemSlot != NUM_ITEM_SLOTS) {
        this->RemoveItem(itemSlot);
    }
}

// OFFSET: 0x007015E0/0x1403C79C0
bool CCharacterComponent::SetNextHairStyle(COMPONENT_CONTEXT context, int32_t step) {
    COMPONENT_SELECTION selection = GetSelectionFromContext(context, this->m_data.m_preferences.m_classId);
    int32_t numVariations = ComponentGetNumVariations(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_HAIR);

    if (numVariations <= 0) {
        return false;
    }

    int32_t hairStyleID = this->m_data.m_preferences.m_hairStyleId + step;

    if (hairStyleID >= numVariations) {
        hairStyleID = 0;
    } else if (hairStyleID < 0) {
        hairStyleID = numVariations - 1;
    }

    if (hairStyleID == static_cast<int32_t>(this->m_data.m_preferences.m_hairStyleId)) {
        return false;
    }

    CharSectionsRec* rec = NULL;

    while (true) {
        int32_t numColors = ComponentGetNumColors(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_HAIR, hairStyleID);
        int32_t color;

        for (color = 0; color < numColors; color++) {
            rec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_HAIR, hairStyleID, color, NULL);

            if (rec && ComponentFlagsMatch(rec->m_flags, selection)) {
                break;
            }
        }

        if (color < numColors) {
            break;
        }

        hairStyleID += step;

        if (hairStyleID >= numVariations) {
            hairStyleID = 0;
        } else if (hairStyleID < 0) {
            hairStyleID = numVariations - 1;
        }

        if (hairStyleID == static_cast<int32_t>(this->m_data.m_preferences.m_hairStyleId)) {
            return false;
        }
    }

    CharSectionsRec* keepRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_HAIR, hairStyleID, this->m_data.m_preferences.m_hairColorId, NULL);

    if (keepRec && ComponentFlagsMatch(keepRec->m_flags, selection)) {
        this->SetHairStyle(keepRec->m_variationIndex, NULL);
        return true;
    }

    this->SetHairColor(rec->m_colorIndex, true, NULL);
    this->SetHairStyle(rec->m_variationIndex, NULL);

    int32_t facialHairID = CCharacterComponent::GetNthFacialFeatureIndex(this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, this->m_data.m_preferences.m_classId, this->m_data.m_preferences.m_hairColorId, this->m_data.m_preferences.m_facialHairId, 0, GetSelectionFromContext(context, this->m_data.m_preferences.m_classId));

    if (facialHairID >= 0) {
        this->SetBeardStyle(facialHairID, true, NULL);
    }

    return true;
}

// OFFSET: 0x007017A0/0x1403CD5A0
void CCharacterComponent::PasteFromSkin(COMPONENT_SECTIONS section, void* texture, MipBits* dstMips, uint32_t mipLevels) {
    if (!TextureCacheHasMips(texture)) {
        return;
    }

    TCTEXTUREINFO info;
    info.m_width = 0;
    info.m_height = 0;
    info.m_mipCount = 0;
    info.m_alphaSize = 0;
    info.m_opaque = 1;
    TextureCacheGetInfo(texture, info, 1);
    uint32_t width = info.m_width;
    info.m_alphaSize = 0;
    uint32_t size = 1 << mipLevels;

    if (info.m_width < size && info.m_height < size) {
        CCharacterComponent::PasteScale(texture, dstMips, s_sectionInfo[section].m_pos, s_sectionInfo[section].m_pos, s_sectionInfo[section].m_size, info, mipLevels);
        return;
    }

    uint32_t mipLevel = 0;

    while (width > size) {
        width >>= 1;
        mipLevel++;
    }

    CCharacterComponent::Paste(texture, dstMips, s_sectionInfo[section].m_pos, s_sectionInfo[section].m_pos, s_sectionInfo[section].m_size, info, mipLevel, mipLevels);
}

// OFFSET: 0x00701880/0x1403CD6C0
void CCharacterComponent::PasteToSection(COMPONENT_SECTIONS section, void* texture, MipBits* dstMips, uint32_t mipLevels) {
    if (!TextureCacheHasMips(texture)) {
        return;
    }

    TCTEXTUREINFO info;
    info.m_width = 0;
    info.m_height = 0;
    info.m_mipCount = 0;
    info.m_alphaSize = 0;
    info.m_opaque = 1;
    TextureCacheGetInfo(texture, info, 1);
    uint32_t width = info.m_width;
    uint32_t sectionWidth = s_sectionInfo[section].m_size.x;
    C2iVector srcPos;

    if (info.m_width < sectionWidth || info.m_height < static_cast<uint32_t>(s_sectionInfo[section].m_size.y)) {
        srcPos.x = 0;
        srcPos.y = 0;
        CCharacterComponent::PasteScale(texture, dstMips, s_sectionInfo[section].m_pos, srcPos, s_sectionInfo[section].m_size, info, mipLevels);
        return;
    }

    uint32_t mipLevel = 0;

    while (width > sectionWidth) {
        width >>= 1;
        mipLevel++;
    }

    srcPos.x = 0;
    srcPos.y = 0;
    CCharacterComponent::Paste(texture, dstMips, s_sectionInfo[section].m_pos, srcPos, s_sectionInfo[section].m_size, info, mipLevel, mipLevels);
}

// PROVEN: 203b11e5 IDENTICAL/NONE
// OFFSET: 0x007019E0
void CCharacterComponent::RenderPrepAU() {
    CCharacterComponent::PasteFromSkin(SECTION_ARM_UPPER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);

    for (int32_t layer = 0; layer < 4; layer++) {
        if ((1 << layer) & this->m_section[SECTION_ARM_UPPER].m_layerMask) {
            CCharacterComponent::PasteToSection(SECTION_ARM_UPPER, this->m_section[SECTION_ARM_UPPER].m_layerTex[layer], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
        }
    }
}

// PROVEN: ea1d907f IDENTICAL/NONE
// OFFSET: 0x00701A50
void CCharacterComponent::RenderPrepAL() {
    CCharacterComponent::PasteFromSkin(SECTION_ARM_LOWER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);

    for (int32_t layer = 0; layer < 8; layer++) {
        if ((1 << layer) & this->m_section[SECTION_ARM_LOWER].m_layerMask) {
            CCharacterComponent::PasteToSection(SECTION_ARM_LOWER, this->m_section[SECTION_ARM_LOWER].m_layerTex[layer], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
        }
    }
}

// PROVEN: f5c86075 IDENTICAL/NONE
// OFFSET: 0x00701AC0
void CCharacterComponent::RenderPrepHA() {
    CCharacterComponent::PasteFromSkin(SECTION_HAND, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);

    if (this->m_section[SECTION_HAND].m_layerMask & 0x1) {
        CCharacterComponent::PasteToSection(SECTION_HAND, this->m_section[SECTION_HAND].m_layerTex[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
    }
}

// PROVEN: e88e774c IDENTICAL/NONE
// OFFSET: 0x00701B10
void CCharacterComponent::RenderPrepHU() {
    CharSectionsRec* skinRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0, this->m_data.m_preferences.m_skinID, NULL);

    if (skinRec && (skinRec->m_flags & 0x8)) {
        CCharacterComponent::PasteFromSkin(SECTION_HEAD_UPPER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
    }

    if (this->m_baseVariation[VARIATION_FACE].m_texture[1]) {
        CCharacterComponent::PasteToSection(SECTION_HEAD_UPPER, this->m_baseVariation[VARIATION_FACE].m_texture[1], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
    }

    if (this->m_baseVariation[VARIATION_FACIAL_HAIR].m_texture[1]) {
        CCharacterComponent::PasteToSection(SECTION_HEAD_UPPER, this->m_baseVariation[VARIATION_FACIAL_HAIR].m_texture[1], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
    }

    if (this->m_baseVariation[VARIATION_HAIR].m_texture[2]) {
        CCharacterComponent::PasteToSection(SECTION_HEAD_UPPER, this->m_baseVariation[VARIATION_HAIR].m_texture[2], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
    }
}

// PROVEN: e39df29c IDENTICAL/NONE
// OFFSET: 0x00701BC0
void CCharacterComponent::RenderPrepHL() {
    CharSectionsRec* skinRec = ComponentGetSectionsRecord(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0, this->m_data.m_preferences.m_skinID, NULL);

    if (skinRec && (skinRec->m_flags & 0x8)) {
        CCharacterComponent::PasteFromSkin(SECTION_HEAD_LOWER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
    }

    if (this->m_baseVariation[VARIATION_FACE].m_texture[0]) {
        CCharacterComponent::PasteToSection(SECTION_HEAD_LOWER, this->m_baseVariation[VARIATION_FACE].m_texture[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
    }

    if (this->m_baseVariation[VARIATION_FACIAL_HAIR].m_texture[0]) {
        CCharacterComponent::PasteToSection(SECTION_HEAD_LOWER, this->m_baseVariation[VARIATION_FACIAL_HAIR].m_texture[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
    }

    if (this->m_baseVariation[VARIATION_HAIR].m_texture[1]) {
        CCharacterComponent::PasteToSection(SECTION_HEAD_LOWER, this->m_baseVariation[VARIATION_HAIR].m_texture[1], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
    }
}

// OFFSET: 0x00701C70
void CCharacterComponent::RenderPrepTU() {
    CCharacterComponent::PasteFromSkin(SECTION_TORSO_UPPER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);

    if (!(this->m_section[SECTION_TORSO_UPPER].m_layerMask & 0x7) && this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1]) {
        CCharacterComponent::PasteToSection(SECTION_TORSO_UPPER, this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[1], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
    }

    for (int32_t layer = 0; layer < 7; layer++) {
        if ((1 << layer) & this->m_section[SECTION_TORSO_UPPER].m_layerMask) {
            CCharacterComponent::PasteToSection(SECTION_TORSO_UPPER, this->m_section[SECTION_TORSO_UPPER].m_layerTex[layer], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
        }
    }
}

// PROVEN: b4d7d56a IDENTICAL/NONE
// OFFSET: 0x00701D10
void CCharacterComponent::RenderPrepTL() {
    CCharacterComponent::PasteFromSkin(SECTION_TORSO_LOWER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);

    for (int32_t layer = 0; layer < 8; layer++) {
        if ((1 << layer) & this->m_section[SECTION_TORSO_LOWER].m_layerMask) {
            CCharacterComponent::PasteToSection(SECTION_TORSO_LOWER, this->m_section[SECTION_TORSO_LOWER].m_layerTex[layer], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
        }
    }
}

// OFFSET: 0x00701D80
void CCharacterComponent::RenderPrepLU() {
    CCharacterComponent::PasteFromSkin(SECTION_LEG_UPPER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);

    if (((this->m_flags & 0x20) || !(this->m_section[SECTION_LEG_UPPER].m_layerMask & 0x3)) && this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[0]) {
        CCharacterComponent::PasteToSection(SECTION_LEG_UPPER, this->m_baseVariation[VARIATION_UNDERWEAR].m_texture[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
    }

    int32_t last = 3 - ((this->m_flags & 0x20) != 0);

    for (int32_t layer = (this->m_flags & 0x20) != 0; layer < last; layer++) {
        if ((1 << layer) & this->m_section[SECTION_LEG_UPPER].m_layerMask) {
            CCharacterComponent::PasteToSection(SECTION_LEG_UPPER, this->m_section[SECTION_LEG_UPPER].m_layerTex[layer], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
        }
    }
}

// OFFSET: 0x00701E40
void CCharacterComponent::RenderPrepLL() {
    CCharacterComponent::PasteFromSkin(SECTION_LEG_LOWER, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);

    int32_t last = 2 * ((this->m_flags & 0x20) == 0) + 4;

    for (int32_t layer = (this->m_flags & 0x20) != 0; layer < last; layer++) {
        if ((1 << layer) & this->m_section[SECTION_LEG_LOWER].m_layerMask) {
            CCharacterComponent::PasteToSection(SECTION_LEG_LOWER, this->m_section[SECTION_LEG_LOWER].m_layerTex[layer], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
        }
    }
}

// PROVEN: ad27985f IDENTICAL/NONE
// OFFSET: 0x00701ED0
void CCharacterComponent::RenderPrepFO() {
    CCharacterComponent::PasteFromSkin(SECTION_FOOT, this->m_baseVariation[VARIATION_SKIN].m_texture[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);

    if (this->m_section[SECTION_FOOT].m_layerMask & 0x1) {
        CCharacterComponent::PasteToSection(SECTION_FOOT, this->m_section[SECTION_FOOT].m_layerTex[0], CCharacterComponent::s_textureBuffer, this->m_mipLevels);
    }
}

// OFFSET: 0x00701F20
void CCharacterComponent::ProcessFinishedRequest() {
    CCharacterComponent::s_bInRenderPrep = 1;

    if (this->m_flags & 0x4) {
        this->GeosRenderPrep();
    }

    if (!this->m_baseTexture) {
        this->CreateBaseTexture();
    }

    this->UpdateSections();
    this->ApplyExtraTextures();
    this->m_request->m_active = 0;
    this->m_flags &= ~0x1;
    this->m_request = NULL;
    this->m_dirtySections = 0;
    this->DestroyRenderTextures();
    CCharacterComponent::s_bInRenderPrep = 0;
}

// OFFSET: 0x00701F90/0x1403CDF00
void CCharacterComponent::ProcessComponentRequest(CComponentRequest* request) {
    MipBits* dstMips = request->m_mipBits->m_mipBits;

    if (request->m_format == GxTex_Dxt1) {
        dstMips = s_threadTextureBuffer;
    }

    for (int32_t section = 0; section < NUM_COMPONENT_SECTIONS; section++) {
        if (request->m_baseTextures[section]) {
            CCharacterComponent::PasteFromSkin(static_cast<COMPONENT_SECTIONS>(section), request->m_baseTextures[section], dstMips, request->m_mipLevels);
        }
    }

    for (uint32_t i = 0; i < request->m_layerCount; i++) {
        CCharacterComponent::PasteToSection(static_cast<COMPONENT_SECTIONS>(request->m_layerSections[i]), request->m_layerTextures[i], dstMips, request->m_mipLevels);
    }

    if (request->m_format == GxTex_Dxt1) {
        CompressMipLevels(dstMips, request->m_mipBits->m_mipBits, request->m_mipLevels);
    }
}

// OFFSET: 0x00702030/0x1403CDFE0
void CCharacterComponent::CreateComponentFreeLists() {
    int32_t count = 10;
    int32_t compressedCount = 0;

    if (CCharacterComponent::s_gxFormat == GxTex_Dxt1) {
        count = 1;
        compressedCount = 9;
    }

    s_mipBitsCritSect.Enter();
    uint32_t size = 1 << CCharacterComponent::s_mipLevels;

    for (int32_t i = 0; i < count; i++) {
        CComponentMipBits* mipBits = s_mipBitsFreeList.NewNode(2, 0, 0);
        mipBits->m_format = PIXEL_ARGB8888;
        mipBits->m_mipLevels = CCharacterComponent::s_mipLevels;
        mipBits->m_mipBits = TextureAllocMippedImg(PIXEL_ARGB8888, size, size);
    }

    for (int32_t i = 0; i < compressedCount; i++) {
        CComponentMipBits* mipBits = s_mipBitsCompressedFreeList.NewNode(2, 0, 0);
        mipBits->m_format = PIXEL_DXT1;
        mipBits->m_mipLevels = CCharacterComponent::s_mipLevels;
        mipBits->m_mipBits = TextureAllocMippedImg(PIXEL_DXT1, size, size);
    }

    s_mipBitsCritSect.Leave();
}

// OFFSET: 0x007021F0/0x1403CE210
CComponentRequest* CCharacterComponent::AllocComponentRequest() {
    CComponentRequest* request = s_requestFreeList.Head();

    if (!request) {
        request = s_requestFreeList.NewNode(1, 0, 0);
    }

    request->m_link.Unlink();
    request->m_ready = 0;
    request->m_mipLevels = 0;
    request->m_mipBits = NULL;
    request->m_layerCount = 0;
    request->m_active = 1;

    return request;
}

// OFFSET: 0x00702290/0x1403CE430
void CCharacterComponent::ProcessComponentFinishedList(int32_t all) {
    s_finishedListCritSect.Enter();

    for (CComponentRequest* request = s_finishedList.Head(); request;) {
        CComponentRequest* next = s_finishedList.Next(request);

        if (all || !request->m_active) {
            request->m_link.Unlink();
            request->DestroyCachedTextures();

            if (request->m_mipBits) {
                s_mipBitsCritSect.Enter();

                if (request->m_mipBits->m_format) {
                    s_mipBitsFreeList.LinkToTail(request->m_mipBits);
                } else {
                    s_mipBitsCompressedFreeList.LinkToTail(request->m_mipBits);
                }

                s_mipBitsCritSect.Leave();
                request->m_mipBits = NULL;
            }

            s_requestFreeList.LinkToTail(request);
        }

        request = next;
    }

    s_finishedListCritSect.Leave();
}

// OFFSET: 0x007023C0/0x1403CE530
uint32_t STORMAPI CCharacterComponent::ThreadProc(void* param) {
    PropSelectContext(s_threadPropContext);
    s_threadBeginEvent.Wait(0xFFFFFFFF);

    while (CCharacterComponent::s_bComponentThread) {
        while (true) {
            s_requestListCritSect.Enter();
            CComponentRequest* request = s_requestList.Head();

            if (request) {
                request->m_link.Unlink();
            }

            s_requestListCritSect.Leave();

            if (!request) {
                break;
            }

            if (!request->m_active) {
                s_finishedListCritSect.Enter();
                s_finishedList.LinkToTail(request);
                s_finishedListCritSect.Leave();
                request->m_ready = 1;
                continue;
            }

            CComponentMipBits* mipBits = CCharacterComponent::AllocComponentMipBits(request->m_format);

            if (!mipBits) {
                s_requestListCritSect.Enter();
                s_requestList.LinkToHead(request);
                s_requestListCritSect.Leave();
                OsSleep(10);
                continue;
            }

            request->m_mipBits = mipBits;
            CCharacterComponent::ProcessComponentRequest(request);
            s_finishedListCritSect.Enter();
            s_finishedList.LinkToTail(request);
            s_finishedListCritSect.Leave();
            request->m_ready = 1;
            OsSleep(1);
        }

        OsSleep(10);
        s_threadBeginEvent.Wait(0xFFFFFFFF);
    }

    return 0;
}

// OFFSET: 0x007025B0
void CCharacterComponent::RenderPrepSections() {
    CCharacterComponent::s_bInRenderPrep = 1;

    if (this->m_flags & 0x4) {
        this->GeosRenderPrep();
    }

    if (!this->m_baseTexture) {
        this->CreateBaseTexture();
    }

    this->PrepSections();
    this->ApplyExtraTextures();
    this->m_flags &= ~0x1;
    this->m_dirtySections = 0;
    this->DestroyRenderTextures();
    this->m_link.Unlink();
    CCharacterComponent::s_bInRenderPrep = 0;
}

// OFFSET: 0x00702630/0x1403CE940
bool CCharacterComponent::RenderPrep(int32_t wait) {
    if (this->m_data.m_flags & 0x1) {
        if (this->m_flags & 0x4) {
            this->GeosRenderPrep();
        }

        return true;
    }

    if (!this->m_dirtySections && !(this->m_flags & 0x1)) {
        if (this->m_flags & 0x4) {
            this->GeosRenderPrep();
        }

        return true;
    }

    if (!(this->m_flags & 0x1)) {
        if (!this->m_link.m_next) {
            s_queue.LinkToTail(this);
        }

        return true;
    }

    if (!wait) {
        if (!this->m_link.m_next) {
            s_queue.LinkToTail(this);
        }

        return false;
    }

    if (this->m_request) {
        this->m_request->m_active = 0;
        this->m_request = NULL;
    }

    this->VariationsLoaded(1);
    this->ItemsLoaded(1);
    this->m_flags |= 0x8;
    this->RenderPrepSections();
    this->m_link.Unlink();

    return true;
}

// PROVEN: 97286b15 IDENTICAL/NONE
// OFFSET: 0x007027E0/0x1403CEC70
CCharacterComponent* CCharacterComponent::AllocComponent() {
    return s_componentPool->New("", 0);
}

// OFFSET: 0x00702800/0x1403CEC80
int32_t CCharacterComponent::ThreadInitialize() {
    CCharacterComponent::s_requestFunc[SECTION_ARM_UPPER] = &CCharacterComponent::CreateRequestAU;
    CCharacterComponent::s_requestFunc[SECTION_ARM_LOWER] = &CCharacterComponent::CreateRequestAL;
    CCharacterComponent::s_requestFunc[SECTION_HAND] = &CCharacterComponent::CreateRequestHA;
    CCharacterComponent::s_requestFunc[SECTION_HEAD_UPPER] = &CCharacterComponent::CreateRequestHU;
    CCharacterComponent::s_requestFunc[SECTION_HEAD_LOWER] = &CCharacterComponent::CreateRequestHL;
    CCharacterComponent::s_requestFunc[SECTION_TORSO_UPPER] = &CCharacterComponent::CreateRequestTU;
    CCharacterComponent::s_requestFunc[SECTION_TORSO_LOWER] = &CCharacterComponent::CreateRequestTL;
    CCharacterComponent::s_requestFunc[SECTION_LEG_UPPER] = &CCharacterComponent::CreateRequestLU;
    CCharacterComponent::s_requestFunc[SECTION_LEG_LOWER] = &CCharacterComponent::CreateRequestLL;
    CCharacterComponent::s_requestFunc[SECTION_FOOT] = &CCharacterComponent::CreateRequestFO;
    CCharacterComponent::CreateComponentFreeLists();
    s_threadBeginEvent.Reset();
    s_threadPropContext = PropGetSelectedContext();

    return SThread::Create(&CCharacterComponent::ThreadProc, NULL, s_thread, const_cast<char*>("Component"), 0);
}

// OFFSET: 0x007028A0/0x1403CED60
void CCharacterComponent::CreateComponentRequest() {
    this->m_request = CCharacterComponent::AllocComponentRequest();
    this->m_request->m_format = this->m_gxTexFormat;
    this->m_request->m_mipLevels = this->m_mipLevels;

    for (int32_t section = 0; section < NUM_COMPONENT_SECTIONS; section++) {
        if ((1 << section) & this->m_dirtySections) {
            (this->*CCharacterComponent::s_requestFunc[section])();
        }
    }

    s_requestListCritSect.Enter();

    if (this->m_data.m_flags & 0x2) {
        s_requestList.LinkToHead(this->m_request);
    } else {
        s_requestList.LinkToTail(this->m_request);
    }

    s_requestListCritSect.Leave();
    s_threadBeginEvent.Set();
}

// OFFSET: 0x00702990
void CCharacterComponent::DestroyComponentFreeLists() {
    s_mipBitsCritSect.Enter();

    for (CComponentMipBits* mipBits = s_mipBitsFreeList.Head(); mipBits; mipBits = s_mipBitsFreeList.Next(mipBits)) {
        TextureFreeMippedImg(mipBits->m_mipBits, mipBits->m_format, 1 << mipBits->m_mipLevels, 1 << mipBits->m_mipLevels);
    }

    for (CComponentMipBits* mipBits = s_mipBitsCompressedFreeList.Head(); mipBits; mipBits = s_mipBitsCompressedFreeList.Next(mipBits)) {
        TextureFreeMippedImg(mipBits->m_mipBits, mipBits->m_format, 1 << mipBits->m_mipLevels, 1 << mipBits->m_mipLevels);
    }

    s_mipBitsFreeList.Clear();
    s_mipBitsCompressedFreeList.Clear();
    s_mipBitsCritSect.Leave();
}

// PROVEN: 33bd4cb8 IDENTICAL/NONE
// OFFSET: 0x00702A40/0x1403CF010
void CCharacterComponent::FreeComponent(CCharacterComponent* component) {
    s_componentPool->Delete(component);
}

// OFFSET: 0x00702A80
void CCharacterComponent::ThreadDestroy() {
    CCharacterComponent::s_bComponentThread = 0;
    s_threadBeginEvent.Set();
    s_thread.Wait(0xFFFFFFFF);

    for (CComponentRequest* request = s_requestList.Head(); request;) {
        CComponentRequest* next = s_requestList.Next(request);
        request->m_active = 0;
        s_finishedList.LinkToTail(request);
        request = next;
    }

    CCharacterComponent::ProcessComponentFinishedList(1);
    CCharacterComponent::DestroyComponentFreeLists();
    s_requestFreeList.Clear();

    for (int32_t section = 0; section < NUM_COMPONENT_SECTIONS; section++) {
        CCharacterComponent::s_requestFunc[section] = NULL;
    }
}

// OFFSET: 0x00702B80/0x1403CF260
void CCharacterComponent::SetMipLevels(uint32_t mipLevels) {
    if (mipLevels == CCharacterComponent::s_mipLevels) {
        return;
    }

    for (CCharacterComponent* component = s_componentList.Head(); component; component = s_componentList.Next(component)) {
        if (!(component->m_data.m_flags & 0x1)) {
            component->SetTexturesDirty();
            component->SetDirtySections(0xFFFFFFFF);
        }
    }

    int32_t useThread = CCharacterComponent::s_bComponentThread;

    if (useThread) {
        CCharacterComponent::ThreadDestroy();
    }

    TextureBufferDestroy();
    ComputeSectionInfo(mipLevels);
    CCharacterComponent::s_bComponentThread = useThread;

    if (useThread) {
        CCharacterComponent::ThreadInitialize();
    }

    CCharacterComponent::TextureBufferInitialize();

    for (CCharacterComponent* component = s_componentList.Head(); component; component = s_componentList.Next(component)) {
        if (component->m_data.m_flags & 0x1) {
            continue;
        }

        if (component->m_baseTexture) {
            component->ReleaseBaseTexture();
            component->m_mipLevels = CCharacterComponent::s_mipLevels;
            component->CreateBaseTexture();
        } else {
            component->m_mipLevels = CCharacterComponent::s_mipLevels;
        }
    }
}

// OFFSET: 0x00702CB0/0x1403CF9C0
int32_t CCharacterComponent::Update(const void* data, void* param) {
    int32_t hasContext = g_theGxDevicePtr ? g_theGxDevicePtr->CapsHasContext(-1) : 0;

    if (s_textureLevelVar || (s_textureLevelVar = CVar::Lookup("componentTextureLevel"))) {
        uint32_t mipLevels = s_textureLevelVar->m_intValue;

        if (mipLevels != CCharacterComponent::s_mipLevels) {
            CCharacterComponent::SetMipLevels(mipLevels);
        }
    }

    for (CCharacterComponent* component = s_queue.Head(); component;) {
        CCharacterComponent* next = s_queue.Next(component);

        if (component->m_request) {
            if (hasContext && component->m_request->m_ready) {
                component->ProcessFinishedRequest();
                component->m_link.Unlink();
            }
        } else {
            bool variationsLoaded = component->VariationsLoaded(0);
            bool itemsLoaded = component->ItemsLoaded(0);

            if (variationsLoaded && itemsLoaded) {
                component->m_flags |= 0x8;

                if (CCharacterComponent::s_bComponentThread) {
                    component->CreateComponentRequest();
                } else if (hasContext) {
                    component->RenderPrepSections();
                    component->m_link.Unlink();
                }
            }
        }

        component = next;
    }

    if (CCharacterComponent::s_bComponentThread) {
        CCharacterComponent::ProcessComponentFinishedList(0);
    }

    return 1;
}

// OFFSET: 0x00702E30/0x1403CFB80
bool CCharacterComponent::Initialize(EGxTexFormat format, uint32_t mipLevels, int32_t useThread, int32_t useCompression) {
    s_useCompression = useCompression;
    void* pool = SMemAlloc(sizeof(TSObjectAllocPool<CCharacterComponent>), __FILE__, __LINE__, 0);
    s_componentPool = pool ? new (pool) TSObjectAllocPool<CCharacterComponent>("CCharacterComponent", 32) : NULL;

    uint32_t count = 2 * g_chrRacesDB.m_maxID + 2;
    CCharacterComponent::s_chrVarArrayLength = count;
    BuildComponentArray(count, &CCharacterComponent::s_chrVarArray);
    CountFacialFeatures(count, &CCharacterComponent::s_characterFacialHairStylesList);

    s_pathEnd = s_path;
    s_pathEnd2 = s_path2;

    CCharacterComponent::m_prepFunc[SECTION_ARM_UPPER] = &CCharacterComponent::RenderPrepAU;
    CCharacterComponent::m_prepFunc[SECTION_ARM_LOWER] = &CCharacterComponent::RenderPrepAL;
    CCharacterComponent::m_prepFunc[SECTION_HAND] = &CCharacterComponent::RenderPrepHA;
    CCharacterComponent::m_prepFunc[SECTION_HEAD_UPPER] = &CCharacterComponent::RenderPrepHU;
    CCharacterComponent::m_prepFunc[SECTION_HEAD_LOWER] = &CCharacterComponent::RenderPrepHL;
    CCharacterComponent::m_prepFunc[SECTION_TORSO_UPPER] = &CCharacterComponent::RenderPrepTU;
    CCharacterComponent::m_prepFunc[SECTION_TORSO_LOWER] = &CCharacterComponent::RenderPrepTL;
    CCharacterComponent::m_prepFunc[SECTION_LEG_UPPER] = &CCharacterComponent::RenderPrepLU;
    CCharacterComponent::m_prepFunc[SECTION_LEG_LOWER] = &CCharacterComponent::RenderPrepLL;
    CCharacterComponent::m_prepFunc[SECTION_FOOT] = &CCharacterComponent::RenderPrepFO;

    CCharacterComponent::m_itemFunc[SECTION_ARM_UPPER] = &CCharacterComponent::UpdateItemAU;
    CCharacterComponent::m_itemFunc[SECTION_ARM_LOWER] = &CCharacterComponent::UpdateItemAL;
    CCharacterComponent::m_itemFunc[SECTION_HAND] = &CCharacterComponent::UpdateItemHA;
    CCharacterComponent::m_itemFunc[SECTION_HEAD_UPPER] = &CCharacterComponent::UpdateItemHU;
    CCharacterComponent::m_itemFunc[SECTION_HEAD_LOWER] = &CCharacterComponent::UpdateItemHL;
    CCharacterComponent::m_itemFunc[SECTION_TORSO_UPPER] = &CCharacterComponent::UpdateItemTU;
    CCharacterComponent::m_itemFunc[SECTION_TORSO_LOWER] = &CCharacterComponent::UpdateItemTL;
    CCharacterComponent::m_itemFunc[SECTION_LEG_UPPER] = &CCharacterComponent::UpdateItemLU;
    CCharacterComponent::m_itemFunc[SECTION_LEG_LOWER] = &CCharacterComponent::UpdateItemLL;
    CCharacterComponent::m_itemFunc[SECTION_FOOT] = &CCharacterComponent::UpdateItemFO;

    ComputeSectionInfo(mipLevels);
    TextureCacheResetLoadCount();

    CCharacterComponent::s_bComponentThread = useThread;
    CCharacterComponent::s_bComponentCompression = 0;
    CCharacterComponent::s_gxFormat = format;

    if (useThread) {
        if (s_useCompression) {
            CCharacterComponent::s_bComponentCompression = 1;
            CCharacterComponent::s_gxFormat = GxTex_Dxt1;
        }

        CCharacterComponent::ThreadInitialize();
    } else {
        for (int32_t section = 0; section < NUM_COMPONENT_SECTIONS; section++) {
            CCharacterComponent::s_requestFunc[section] = NULL;
        }
    }

    CCharacterComponent::s_gxFormatHigh = GxTex_Argb8888;
    CCharacterComponent::s_textureBuffer = NULL;
    s_compressedTextureBuffer = NULL;
    s_threadTextureBuffer = NULL;
    CCharacterComponent::TextureBufferInitialize();

    EventRegisterEx(EVENT_ID_POLL, &CCharacterComponent::Update, NULL, 0.0f);
    CCharacterComponent::ResetClosestComponent();

    return true;
}

// OFFSET: 0x007030D0
bool CCharacterComponent::Destroy() {
    if (CCharacterComponent::m_activePlayerComponent) {
        CCharacterComponent::FreeComponent(CCharacterComponent::m_activePlayerComponent);
        CCharacterComponent::m_activePlayerComponent = NULL;
    }

    if (CCharacterComponent::s_bComponentThread) {
        CCharacterComponent::ThreadDestroy();
    }

    TextureBufferDestroy();
    DestroyVariationArrays();
    TextureCacheDestroy();
    delete s_componentPool;
    s_componentPool = NULL;
    CGPlayer_C::s_displayId = 0;
    EventUnregisterEx(EVENT_ID_POLL, &CCharacterComponent::Update, NULL, 0xFFFFFFFF);

    return true;
}

// OFFSET: 0x007031B0
bool CCharacterComponent::SkinNpc(const char* debug) {
    this->m_data.m_flags &= ~0x1;

    if (!this->m_data.m_npcSkinTexture[0]) {
        return false;
    }

    CStatus status;
    strcpy(s_pathEnd, this->m_data.m_npcSkinTexture);
    CGxTexFlags flags;
    flags.m_filter = GxTex_LinearMipLinear;
    CTexture* texture = TextureCreate(s_path, flags, &status, 0, NULL, 3);

    if (!texture) {
        return false;
    }

    this->m_data.m_model->ReplaceTexture(1, texture);
    TextureRelease(texture);
    this->ReplaceExtraSkinTexture(debug);
    this->m_data.m_flags |= 0x1;
    this->m_flags |= 0x5;
    this->SetDirtySections(0);

    return true;
}

// OFFSET: 0x007032A0/0x1403CF570
void CCharacterComponent::ReplaceMonsterSkin(CM2Model* model, const CreatureDisplayInfoRec* displayRec, const CreatureModelDataRec* modelRec) {
    if (!model || !displayRec || !modelRec) {
        return;
    }

    char path[260];
    SStrCopy(path, modelRec->m_modelName, sizeof(path));

    CGxTexFlags flags;
    flags.m_filter = GxTex_LinearMipLinear;
    flags.m_wrapU = 1;
    flags.m_wrapV = 1;

    for (uint32_t i = 0; i < 3; i++) {
        if (*displayRec->m_textureVariation[i]) {
            *OsPathFindFilename(path) = '\0';
            SStrPack(path, displayRec->m_textureVariation[i], sizeof(path));
            CStatus status;
            CTexture* texture = TextureCreate(path, flags, &status, 0, NULL, 3);

            if (texture) {
                model->ReplaceTexture(i + 11, texture);
                TextureRelease(texture);
            }
        }
    }

    ReplaceParticleColor(displayRec->m_particleColorID, model);
}

// OFFSET: 0x00703410/0x1403CF790
void CCharacterComponent::AddCape() {
    if (!this->m_itemDisplayID[ITEMSLOT_CAPE]) {
        return;
    }

    ItemDisplayInfoRec rec;

    if (!g_itemDisplayInfoDB.GetRecord(this->m_itemDisplayID[ITEMSLOT_CAPE], &rec) || !*rec.m_modelTexture[0]) {
        return;
    }

    SStrCopy(s_buffer, "Item\\ObjectComponents\\Cape\\", sizeof(s_buffer));
    SStrPack(s_buffer, rec.m_modelTexture[0], sizeof(s_buffer));
    SStrPack(s_buffer, ".blp", sizeof(s_buffer));
    strcpy(s_pathEnd, s_buffer);

    CGxTexFlags flags;
    flags.m_filter = GxTex_LinearMipLinear;
    CStatus status;
    CTexture* texture = TextureCreate(s_path, flags, &status, 0, NULL, 3);

    if (texture) {
        this->m_data.m_model->ReplaceTexture(2, texture);
        TextureRelease(texture);
    }
}

// OFFSET: 0x007035C0/0x1403CFFE0
bool CCharacterComponent::Init(ComponentData* data, const char* debug) {
    if (this->m_data.m_model) {
        this->m_data.m_model->Release();
    }

    memcpy(&this->m_data, data, sizeof(this->m_data));
    memset(this->m_baseVariation, 0, sizeof(this->m_baseVariation));
    memset(this->m_section, 0, sizeof(this->m_section));
    this->m_handItemDisplayID[0] = 0;
    this->m_handItemDisplayID[1] = 0;
    this->m_handItemDisplayID[2] = 0;

    if (static_cast<int32_t>(data->m_preferences.m_raceID) < 0 || static_cast<int32_t>(data->m_preferences.m_raceID) > g_chrRacesDB.m_maxID || !ComponentValidateBase(CCharacterComponent::s_chrVarArray, this->m_data.m_preferences.m_raceID, this->m_data.m_preferences.m_sexID, VARIATION_SKIN, 0, this->m_data.m_preferences.m_skinID)) {
        return false;
    }

    this->m_attachMask = 0;
    this->m_flags = (this->m_flags & ~0x45) | 0x5;
    this->m_dirtySections = 0xFFFFFFFF;
    this->m_request = NULL;

    if (!(this->m_data.m_flags & 0x1) && this->m_baseTexture) {
        this->ReleaseBaseTexture();
    }

    uint32_t dataFlags = this->m_data.m_flags;

    if (dataFlags & 0x2) {
        this->m_gxTexFormat = CCharacterComponent::s_gxFormatHigh;
    }

    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(this->m_data.m_preferences.m_raceID);
    this->m_flags = (this->m_flags & ~0x10) | (((raceRec->m_flags & 0x2) != 0) << 4);

    if (dataFlags & 0x1) {
        this->SkinNpc(debug);
    }

    this->SetSkinColor(this->m_data.m_preferences.m_skinID, false, true, debug);
    this->SetHairStyle(this->m_data.m_preferences.m_hairStyleId, debug);
    this->SetBeardStyle(this->m_data.m_preferences.m_facialHairId, false, debug);

    if (this->m_data.m_flags & 0x1) {
        this->ApplyExtraTextures();
    }

    return true;
}

// OFFSET: 0x00703740/0x1403D0190
void CCharacterComponent::AddItem(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, int32_t itemVisual) {
    this->m_flags |= 0x4;
    this->m_itemDisplayID[itemSlot] = displayRec->m_id;

    switch (itemSlot) {
        case ITEMSLOT_HEAD:
            if (!this->IsHelmModelCorrect()) {
                if (this->m_itemDisplayID[ITEMSLOT_HEAD]) {
                    CCharacterComponent::RemoveLinkpt(this->m_data.m_model, &this->m_attachMask, ATTACH_HELM);
                }

                this->AddHelm(itemVisual);
            }

            break;

        case ITEMSLOT_SHOULDER:
            if (!this->AreShoulderModelsCorrect()) {
                if (this->m_itemDisplayID[ITEMSLOT_SHOULDER]) {
                    CCharacterComponent::RemoveLinkpt(this->m_data.m_model, &this->m_attachMask, ATTACH_SHOULDERLEFT);
                    CCharacterComponent::RemoveLinkpt(this->m_data.m_model, &this->m_attachMask, ATTACH_SHOULDERRIGHT);
                }

                this->AddShoulders(itemVisual);
            }

            break;

        case ITEMSLOT_CAPE:
            this->AddCape();
            break;

        case ITEMSLOT_11:
            return;

        default:
            if (itemSlot == ITEMSLOT_CHEST) {
                this->m_flags = (this->m_flags & ~0x20) | (32 * ((displayRec->m_flags >> 2) & 0x1));
            }

            if (this->m_data.m_flags & 0x1) {
                break;
            }

            for (int32_t section = 0; section < 8; section++) {
                if (*displayRec->m_texture[section] && s_itemPriority[itemSlot][section] != -1) {
                    (this->*CCharacterComponent::m_itemFunc[section])(itemSlot, displayRec, true);
                }
            }

            break;
    }
}

// OFFSET: 0x00703890/0x1403D0310
void CCharacterComponent::AddItem(ITEM_SLOT itemSlot, int32_t displayID, int32_t itemVisual) {
    if (displayID <= 0) {
        return;
    }

    ItemDisplayInfoRec rec;

    if (g_itemDisplayInfoDB.GetRecord(displayID, &rec)) {
        this->AddItem(itemSlot, &rec, itemVisual);
    }
}

// OFFSET: 0x00703900/0x1403D03A0
void CCharacterComponent::AddItemBySlot(INVENTORY_SLOTS invSlot, int32_t displayID, int32_t itemVisual) {
    switch (invSlot) {
        case INVSLOT_HEAD:
            this->AddItem(ITEMSLOT_HEAD, displayID, itemVisual);
            break;

        case INVSLOT_SHOULDER:
            this->AddItem(ITEMSLOT_SHOULDER, displayID, itemVisual);
            break;

        case INVSLOT_BODY:
            this->AddItem(ITEMSLOT_SHIRT, displayID, itemVisual);
            break;

        case INVSLOT_CHEST:
            this->AddItem(ITEMSLOT_CHEST, displayID, itemVisual);
            break;

        case INVSLOT_WAIST:
            this->AddItem(ITEMSLOT_BELT, displayID, itemVisual);
            break;

        case INVSLOT_LEGS:
            this->AddItem(ITEMSLOT_PANTS, displayID, itemVisual);
            break;

        case INVSLOT_FEET:
            this->AddItem(ITEMSLOT_BOOTS, displayID, itemVisual);
            break;

        case INVSLOT_WRIST:
            this->AddItem(ITEMSLOT_BRACERS, displayID, itemVisual);
            break;

        case INVSLOT_HAND:
            this->AddItem(ITEMSLOT_GLOVES, displayID, itemVisual);
            break;

        case INVSLOT_BACK:
            this->AddItem(ITEMSLOT_CAPE, displayID, itemVisual);
            break;

        case INVSLOT_TABARD:
            this->AddItem(ITEMSLOT_TABARD, displayID, itemVisual);
            break;

        default:
            break;
    }
}

// OFFSET: 0x00703A40
void CCharacterComponent::AddItemByType(INVENTORY_TYPES invType, int32_t displayID) {
    this->RemoveItemByType(invType);

    if (displayID <= 0) {
        return;
    }

    switch (invType) {
        case INVTYPE_HEAD:
            this->AddItem(ITEMSLOT_HEAD, displayID, 0);
            break;

        case INVTYPE_SHOULDERS:
            this->AddItem(ITEMSLOT_SHOULDER, displayID, 0);
            break;

        case INVTYPE_BODY:
            this->AddItem(ITEMSLOT_SHIRT, displayID, 0);
            break;

        case INVTYPE_CHEST:
        case INVTYPE_ROBE:
            this->AddItem(ITEMSLOT_CHEST, displayID, 0);
            break;

        case INVTYPE_WAIST:
            this->AddItem(ITEMSLOT_BELT, displayID, 0);
            break;

        case INVTYPE_LEGS:
            this->AddItem(ITEMSLOT_PANTS, displayID, 0);
            break;

        case INVTYPE_FEET:
            this->AddItem(ITEMSLOT_BOOTS, displayID, 0);
            break;

        case INVTYPE_WRISTS:
            this->AddItem(ITEMSLOT_BRACERS, displayID, 0);
            break;

        case INVTYPE_HANDS:
            this->AddItem(ITEMSLOT_GLOVES, displayID, 0);
            break;

        case INVTYPE_WEAPON:
        case INVTYPE_2HWEAPON:
        case INVTYPE_WEAPONMAINHAND:
            this->SetMainHandItemDisplay(displayID);
            break;

        case INVTYPE_SHIELD:
            this->SetShieldItemDisplay(displayID);
            break;

        case INVTYPE_CLOAK:
            this->AddItem(ITEMSLOT_CAPE, displayID, 0);
            break;

        case INVTYPE_TABARD:
            this->AddItem(ITEMSLOT_TABARD, displayID, 0);
            break;

        case INVTYPE_WEAPONOFFHAND:
            this->SetOffHandItemDisplay(displayID);
            break;

        default:
            break;
    }
}

// OFFSET: #0x004E7A30
ITEM_SLOT CCharacterComponent::GetItemSlotForInvType(INVENTORY_TYPES invType) {
    switch (invType) {
        case INVTYPE_HEAD:
            return ITEMSLOT_HEAD;

        case INVTYPE_SHOULDERS:
            return ITEMSLOT_SHOULDER;

        case INVTYPE_BODY:
            return ITEMSLOT_SHIRT;

        case INVTYPE_CHEST:
        case INVTYPE_ROBE:
            return ITEMSLOT_CHEST;

        case INVTYPE_WAIST:
            return ITEMSLOT_BELT;

        case INVTYPE_LEGS:
            return ITEMSLOT_PANTS;

        case INVTYPE_FEET:
            return ITEMSLOT_BOOTS;

        case INVTYPE_WRISTS:
            return ITEMSLOT_BRACERS;

        case INVTYPE_HANDS:
            return ITEMSLOT_GLOVES;

        case INVTYPE_CLOAK:
            return ITEMSLOT_CAPE;

        case INVTYPE_TABARD:
            return ITEMSLOT_TABARD;

        default:
            return NUM_ITEM_SLOTS;
    }
}

// PROVEN: 48c37d5b IDENTICAL/DIFFERENT
// OFFSET: 0x00627990/0x140693B10
void CCharacterComponent::UpdateItemHU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply) {
}

// PROVEN: 00cdc5bc IDENTICAL/DIFFERENT
// OFFSET: 0x00627990/0x140693B10
void CCharacterComponent::UpdateItemHL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply) {
}

// PROVEN: f28fa65e IDENTICAL/NONE
// OFFSET: 0x006FBE90/0x1403C6820
void CCharacterComponent::UpdateItemLL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply) {
    this->UpdateItem(itemSlot, SECTION_LEG_LOWER, displayRec, apply);
}

// OFFSET: 0x00B3C200/0x140909110
static void StreamingCallback(void* param) {
    CCharacterComponent* component = CCharacterComponent::GetClosestComponent();

    if (component) {
        component->LinkToAsyncHead();
    }

    CCharacterComponent::ResetClosestComponent();
}

// OFFSET: 0x00B3C220/0x140909130
void CCharacterComponent::StreamingUpdate() {
    if (CCharacterComponent::GetClosestComponent()) {
        AsyncFileReadAddStreamingObject(StreamingCallback, NULL, CCharacterComponent::GetClosestComponentSortVal());
    }
}
