#ifndef WOW_COMMON_COMPONENTCORE_CHARACTERCOMPONENT_H
#define WOW_COMMON_COMPONENTCORE_CHARACTERCOMPONENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BLPFile/Types.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "WoW/Common/ComponentCore/Types.h"
#include "WoW/Source/Glue/ComponentData.h"

class CCharacterComponent;
class CComponentMipBits;
class CComponentRequest;
class CM2Model;
class ChrRacesRec;
class CreatureDisplayInfoRec;
class CreatureModelDataRec;
class CTexture;
struct CVar;
class ItemDisplayInfoRec;
struct st_race;

struct CharacterBaseVariation {
    /* 0000 */ void* m_texture [3];

    ~CharacterBaseVariation();
};

struct CharacterSection {
    /* 0000 */ void* m_layerTex [8];
    /* 0020 */ uint32_t m_layerItemDisplayId [8];
    /* 0040 */ uint32_t m_layerMask;

    CharacterSection();
    ~CharacterSection();
};

struct CompSectionInfo {
    /* 0000 */ C2iVector m_pos;
    /* 0008 */ C2iVector m_size;
};

class CComponentMipBits {
    public:
    /* 0000 */ MipBits* m_mipBits;
    /* 0004 */ PIXEL_FORMAT m_format;
    /* 0008 */ uint32_t m_mipLevels;
    /* 000C */ TSLink<CComponentMipBits> m_link;
};

class CComponentRequest {
    public:
    /* 0000 */ uint8_t m_active;
    /* 0001 */ uint8_t m_ready;
    /* 0002 */ uint8_t m_mipLevels;
    /* 0004 */ EGxTexFormat m_format;
    /* 0008 */ CComponentMipBits* m_mipBits;
    /* 000C */ void* m_baseTextures [10];
    /* 0034 */ uint32_t m_layerCount;
    /* 0038 */ void* m_layerTextures [40];
    /* 00D8 */ uint8_t m_layerSections [40];
    /* 0100 */ TSLink<CComponentRequest> m_link;

    void AddLayer(uint8_t section, void* texture);
    void DestroyCachedTextures();
};

class CHelmPathSuffix {
    public:
    /* 0000 */ char m_buffer [260];
    /* 0104 */ size_t m_length;

    CHelmPathSuffix(const char* format, ...);
};

class CCharacterComponent {
    public:
    static CVar* g_componentCompressVar;
    static CVar* g_componentTextureLevelVar;
    static CVar* g_componentThreadVar;
    static CCharacterComponent* m_activePlayerComponent;
    static void (CCharacterComponent::*m_itemFunc[NUM_COMPONENT_SECTIONS])(ITEM_SLOT, const ItemDisplayInfoRec*, bool);
    static void (CCharacterComponent::*m_prepFunc[NUM_COMPONENT_SECTIONS])();
    static int32_t s_bComponentCompression;
    static int32_t s_bComponentThread;
    static int32_t s_bInRenderPrep;
    static uint32_t* s_characterFacialHairStylesList;
    static st_race* s_chrVarArray;
    static uint32_t s_chrVarArrayLength;
    static EGxTexFormat s_gxFormat;
    static EGxTexFormat s_gxFormatHigh;
    static uint32_t s_mipLevels;
    static void (CCharacterComponent::*s_requestFunc[NUM_COMPONENT_SECTIONS])();
    static CompSectionInfo s_sectionInfoRaw[NUM_COMPONENT_SECTIONS];
    static MipBits* s_textureBuffer;

    static CM2Model* AddLink(CM2Model* parent, uint8_t* attachMask, GEOCOMPONENTLINKS link, const char* modelPath, const char* texturePath, int32_t itemVisual, const ItemDisplayInfoRec* displayRec);
    static GEOCOMPONENTLINKS AddHandItem(CM2Model* model, uint8_t* attachMask, const ItemDisplayInfoRec* displayRec, INVENTORY_SLOTS invSlot, SHEATHE_TYPE sheatheType, bool sheathed, bool shield, bool mainHand, int32_t itemVisual);
    static CCharacterComponent* AllocComponent();
    static CComponentMipBits* AllocComponentMipBits(EGxTexFormat format);
    static CComponentRequest* AllocComponentRequest();
    static void ApplyMonsterGeosets(CM2Model* model, const CreatureDisplayInfoRec* displayRec);
    static void ComponentCloseFingers(CM2Model* model, COMP_HAND_SLOT hand);
    static void ComponentOpenFingers(CM2Model* model, COMP_HAND_SLOT hand);
    static void ComponentUtilAddItemVisual(CM2Model* model, int32_t itemVisual);
    static void CreateComponentFreeLists();
    static bool Destroy();
    static void DestroyComponentFreeLists();
    static void FreeComponent(CCharacterComponent* component);
    static CCharacterComponent* GetClosestComponent();
    static int32_t GetClosestComponentSortVal();
    static INVENTORY_SLOTS GetInvSlotForItemSlot(ITEM_SLOT itemSlot);
    static INVENTORY_SLOTS GetInvSlotForInvType(INVENTORY_TYPES invType);
    static ITEM_SLOT GetItemSlotForInvSlot(INVENTORY_SLOTS invSlot);
    static ITEM_SLOT GetItemSlotForInvType(INVENTORY_TYPES invType);
    static int32_t GetNthFacialFeatureIndex(int32_t raceID, int32_t sexID, int32_t classID, int32_t hairColorID, int32_t facialHairID, int32_t index, COMPONENT_SELECTION selection);
    static int32_t GetNumFacesForSkin(int32_t raceID, int32_t sexID, int32_t classID, int32_t skinID, COMPONENT_CONTEXT context);
    static int32_t GetNumFacialFeaturesForHairColor(int32_t raceID, int32_t sexID, int32_t classID, int32_t hairColorID, COMPONENT_CONTEXT context);
    static int32_t GetNumHairColorsForStyle(int32_t raceID, int32_t sexID, int32_t classID, int32_t hairStyleID, COMPONENT_CONTEXT context);
    static int32_t GetNumHairStylesForColor(int32_t raceID, int32_t sexID, int32_t classID, int32_t hairColorID, COMPONENT_CONTEXT context);
    static int32_t GetNumSkins(int32_t raceID, int32_t sexID, int32_t classID, COMPONENT_CONTEXT context);
    static int32_t GetProceduralEmblemFileName(int32_t emblem, char* buffer, int32_t size);
    static GEOCOMPONENTLINKS GetSheatheLink(SHEATHE_TYPE sheatheType, bool mainHand);
    static char* GetShirtBackgroundFileName(uint32_t flags, COMPONENT_SECTIONS section, int32_t background, char* buffer, int32_t size);
    static char* GetShirtBorderFileName(uint32_t flags, COMPONENT_SECTIONS section, int32_t border, int32_t color, char* buffer, int32_t size);
    static char* GetTabardBackgroundFileName(uint32_t flags, COMPONENT_SECTIONS section, int32_t background, char* buffer, int32_t size);
    static char* GetTabardBorderFileName(uint32_t flags, COMPONENT_SECTIONS section, int32_t border, int32_t color, char* buffer, int32_t size);
    static char* GetTabardEmblemFileName(COMPONENT_SECTIONS section, int32_t emblem, int32_t color, char* buffer, int32_t size);
    static void InitCVars();
    static void StreamingUpdate();
    static bool Initialize();
    static bool Initialize(EGxTexFormat format, uint32_t mipLevels, int32_t useThread, int32_t useCompression);
    static CM2Model* MakeAmmoModel(CM2Model* model, const ItemDisplayInfoRec* displayRec, uint32_t invType);
    static void Paste(void* texture, MipBits* dstMips, const C2iVector& dstPos, const C2iVector& srcPos, const C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, uint32_t mipLevels);
    static void PasteCrappyGreen(MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t dstMipOffset);
    static void PasteFromSkin(COMPONENT_SECTIONS section, void* texture, MipBits* dstMips, uint32_t mipLevels);
    static void PasteOpaque(void* texture, const BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t dstMipOffset);
    static void PasteOpaqueScale(void* texture, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo);
    static void PasteScale(void* texture, MipBits* dstMips, const C2iVector& dstPos, const C2iVector& srcPos, const C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t mipLevels);
    static void PasteToSection(COMPONENT_SECTIONS section, void* texture, MipBits* dstMips, uint32_t mipLevels);
    static void PasteTransparent1Bit(void* texture, const BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t dstMipOffset);
    static void PasteTransparent1BitScale(void* texture, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo);
    static void PasteTransparent4Bit(void* texture, const BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t dstMipOffset);
    static void PasteTransparent4BitScale(void* texture, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo);
    static void PasteTransparent8Bit(void* texture, const BlpPalPixel* pal, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo, uint32_t srcMipLevel, int32_t dstMipOffset);
    static void PasteTransparent8BitScale(void* texture, MipBits* dstMips, C2iVector& dstPos, uint32_t dstStride, C2iVector& srcPos, C2iVector& srcSize, TCTEXTUREINFO& srcInfo);
    static void ProcessComponentFinishedList(int32_t all);
    static void ProcessComponentRequest(CComponentRequest* request);
    static void RemoveHandItem(CM2Model* model, uint8_t* attachMask, INVENTORY_SLOTS invSlot, SHEATHE_TYPE sheatheType, bool shield);
    static void RemoveLinkpt(CM2Model* model, uint8_t* attachMask, GEOCOMPONENTLINKS link);
    static void RemoveVisuals(CM2Model* model);
    static void ReplaceMonsterSkin(CM2Model* model, const CreatureDisplayInfoRec* displayRec, const CreatureModelDataRec* modelRec);
    static void ResetClosestComponent();
    static void ResetCounts();
    static int32_t SetClosestComponent(CCharacterComponent* component, int32_t sortVal, float distance);
    static void SetMipLevels(uint32_t mipLevels);
    static void TextureBufferInitialize();
    static void ThreadDestroy();
    static int32_t ThreadInitialize();
    static uint32_t STORMAPI ThreadProc(void* param);
    static int32_t Update(const void* data, void* param);
    static void UpdateBaseTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
    static void ValidateComponentData(ComponentData* data, COMPONENT_CONTEXT context);
    static uint32_t ValidateComponentTextureLevel(uint32_t level);

    /* 0000 */ uint32_t m_handle;
    /* 0004 */ TSLink<CCharacterComponent> m_link;
    /* 000C */ TSLink<CCharacterComponent> m_componentLink;
    /* 0014 */ uint32_t m_flags;
    /* 0018 */ uint8_t m_attachMask;
    /* 001C */ uint32_t m_dirtySections;
    /* 0020 */ EGxTexFormat m_gxTexFormat;
    /* 0024 */ uint32_t m_mipLevels;
    /* 0028 */ ComponentData m_data;
    /* 01A0 */ CTexture* m_baseTexture;
    /* 01A4 */ CTexture* m_extraSkinTexture;
    /* 01A8 */ CTexture* m_hairTexture;
    /* 01AC */ CharacterBaseVariation m_baseVariation [5];
    /* 01E8 */ CharacterSection m_section [10];
    /* 0490 */ uint32_t m_itemDisplayID [12];
    /* 04C0 */ uint32_t m_handItemDisplayID [3];
    /* 04CC */ CComponentRequest* m_request;

    CCharacterComponent();
    ~CCharacterComponent();
    void AddCape();
    GEOCOMPONENTLINKS AddHandItem(const ItemDisplayInfoRec* displayRec, INVENTORY_SLOTS invSlot, SHEATHE_TYPE sheatheType, bool sheathed, bool shield, bool mainHand, int32_t itemVisual);
    void AddHelm(int32_t itemVisual);
    void AddItem(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, int32_t itemVisual);
    void AddItem(ITEM_SLOT itemSlot, int32_t displayID, int32_t itemVisual);
    void AddItemBySlot(INVENTORY_SLOTS invSlot, int32_t displayID, int32_t itemVisual);
    void AddItemByType(INVENTORY_TYPES invType, int32_t displayID);
    void AddShoulders(int32_t itemVisual);
    void ApplyExtraTextures();
    void ApplyGuildColor(uint32_t flags, int32_t emblem, int32_t emblemColor, int32_t border, int32_t borderColor, int32_t background);
    void ApplyShirtGuildColor(uint32_t flags, int32_t emblem, int32_t emblemColor, int32_t border, int32_t borderColor, int32_t background);
    int32_t AreShoulderModelsCorrect();
    int32_t BuildShoulderItemPaths(ItemDisplayInfoRec& rec, const ItemDisplayInfoRec*& displayRec, char* leftModel, char* rightModel, char* leftTexture, char* rightTexture);
    bool CheckForComponentChanges(ComponentData* data);
    void ClearGuildShirt(int32_t count);
    void ClearGuildTabard(int32_t count);
    void ClearSectionLayer(COMPONENT_SECTIONS section, int32_t layer);
    int32_t ComposeHelmModelFilePath(const ChrRacesRec* raceRec, const ItemDisplayInfoRec* displayRec, char* buffer, uint32_t size);
    void ComputeLoadProgress(int64_t& local, int64_t& total);
    void CopyChoicesToAlteredFormComponent(CCharacterComponent* altered, COMPONENT_CONTEXT context);
    void CreateBaseTexture();
    void CreateComponentRequest();
    void CreateRequestAL();
    void CreateRequestAU();
    void CreateRequestFO();
    void CreateRequestHA();
    void CreateRequestHL();
    void CreateRequestHU();
    void CreateRequestLL();
    void CreateRequestLU();
    void CreateRequestTL();
    void CreateRequestTU();
    void DestroyRenderTextures();
    void FreeSectionTexture(COMPONENT_SECTIONS section, int32_t layer);
    void GeosRenderPrep();
    uint32_t GetHandItemAttachIDs(uint32_t* ids, uint32_t maxCount) const;
    int32_t GetHelmModelFilePath(ItemDisplayInfoRec& rec, const ItemDisplayInfoRec*& displayRec);
    void GetPreferences(CharacterPreferences* preferences);
    CharSectionsRec* GetSectionsRecord(COMPONENT_VARIATIONS variation, int32_t variationIndex, int32_t colorIndex, bool* found);
    int32_t GetShoulderDisplayInfo(ItemDisplayInfoRec& rec, const ItemDisplayInfoRec*& displayRec);
    bool Init(ComponentData* data, const char* debug);
    bool IsHelmModelCorrect();
    bool ItemDisplayHasGeosetGroup(uint32_t displayID, int32_t group, ItemDisplayInfoRec& rec);
    bool ItemDisplayHasGeosetGroup(uint32_t displayID, int32_t group);
    bool ItemsLoaded(int32_t wait);
    void LinkShoulderAnimations(CM2Model* left, CM2Model* right, const ItemDisplayInfoRec* displayRec);
    void LinkToAsyncHead();
    void LoadBaseVariation(COMPONENT_VARIATIONS variation, int32_t textureIndex, int32_t variationIndex, int32_t colorIndex, COMPONENT_SECTIONS section, const char* debug);
    void* LoadItemComponentTexture(const ItemDisplayInfoRec* displayRec, COMPONENT_SECTIONS section);
    void LoadSectionLayer(const char* fileName, COMPONENT_SECTIONS section, int32_t layer);
    void Prep();
    void PrepSections();
    void ProcessFinishedRequest();
    void RelinkShoulderAnimations(CM2Model* model);
    void ReleaseBaseTexture();
    void RemoveHandItem(INVENTORY_SLOTS invSlot, SHEATHE_TYPE sheatheType, bool shield);
    void RemoveItem(ITEM_SLOT itemSlot);
    void RemoveItemBySlot(INVENTORY_SLOTS invSlot);
    void RemoveItemByType(INVENTORY_TYPES invType);
    bool RenderPrep(int32_t wait);
    void RenderPrepAL();
    void RenderPrepAll();
    void RenderPrepAU();
    void RenderPrepFO();
    void RenderPrepHA();
    void RenderPrepHL();
    void RenderPrepHU();
    void RenderPrepLL();
    void RenderPrepLU();
    void RenderPrepSections();
    void RenderPrepTL();
    void RenderPrepTU();
    void ReplaceExtraSkinTexture(const char* debug);
    void ReplaceHairTexture(int32_t hairStyleID, const char* debug);
    void SetBeardStyle(int32_t facialHairID, bool loadVariation, const char* debug);
    void SetDirtySections(uint32_t mask);
    void SetFace(int32_t faceID, bool loadHair, const char* debug);
    void SetHairColor(int32_t hairColorID, bool loadHair, const char* debug);
    void SetHairStyle(int32_t hairStyleID, const char* debug);
    void SetHandItemDisplay(int32_t displayID, uint32_t hand, INVENTORY_SLOTS invSlot, bool shield);
    void SetMainHandItemDisplay(int32_t displayID);
    bool SetNextFace(COMPONENT_CONTEXT context, int32_t step, int32_t colorOffset);
    bool SetNextFacialFeature(COMPONENT_CONTEXT context, int32_t step);
    bool SetNextHairColor(COMPONENT_CONTEXT context, int32_t step);
    bool SetNextHairStyle(COMPONENT_CONTEXT context, int32_t step);
    bool SetNextSkin(COMPONENT_CONTEXT context, int32_t step);
    void SetOffHandItemDisplay(int32_t displayID);
    bool SetRandomFace(COMPONENT_CONTEXT context);
    bool SetRandomFacialFeature(COMPONENT_CONTEXT context);
    bool SetRandomHairColor(COMPONENT_CONTEXT context);
    bool SetRandomHairStyle(COMPONENT_CONTEXT context);
    bool SetRandomSkin(COMPONENT_CONTEXT context);
    void SetShieldItemDisplay(int32_t displayID);
    void SetSkinColor(int32_t skinID, bool loadHair, bool renderAll, const char* debug);
    void SetTexturesDirty();
    bool SkinNpc(const char* debug);
    void TurnOffGuildTabardPurchase();
    void UpdateItem(ITEM_SLOT itemSlot, COMPONENT_SECTIONS section, const ItemDisplayInfoRec* displayRec, bool apply);
    void UpdateItemAL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply);
    void UpdateItemAU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply);
    void UpdateItemFO(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply);
    void UpdateItemHA(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply);
    void UpdateItemHL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply);
    void UpdateItemHU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply);
    void UpdateItemLL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply);
    void UpdateItemLU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply);
    void UpdateItemTL(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply);
    void UpdateItemTU(ITEM_SLOT itemSlot, const ItemDisplayInfoRec* displayRec, bool apply);
    void UpdateSections();
    int32_t UpdateTextureSlot(COMPONENT_SECTIONS section, const ItemDisplayInfoRec* displayRec, int32_t layer);
    bool VariationsLoaded(int32_t wait);
};

bool ComponentVarHandler(CVar* var, const char* oldValue, const char* value, void* arg);

#endif
