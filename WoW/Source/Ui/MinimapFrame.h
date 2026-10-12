#ifndef WOW_SOURCE_UI_MINIMAPFRAME_H
#define WOW_SOURCE_UI_MINIMAPFRAME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/Game/GameClient/Minimap.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/C44Matrix.h"

class CRenderBatch;
class CScriptRegion;
class CSimpleTexture;
class CStatus;
class linkObject;
struct lua_State;

class AreaPOIRec;
class CGMinimapFrame;
class CGPlayer_C;
class CMapEntity;
class CTexture;
class C44Matrix;

struct MINIMAPQUAD {
    /* 0000 */ CRect m_rect;
    /* 0010 */ C2Vector m_origin;
};

struct OBJINFO {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ C2Vector m_position;
    /* 0010 */ C2Vector m_rotated;
    /* 0018 */ uint8_t m_otherFloor;
    /* 0019 */ uint8_t m_tracked;
    /* 001A */ uint8_t m_questGiver;
    /* 001C */ int32_t m_icon;
    /* 0020 */ float m_facing;
    /* 0024 */ int32_t m_marker;
    /* 0028 */ float m_height;
    /* 002C */ uint32_t unk_002C;
};

struct MINIMAPENUMPARAM {
    /* 0000 */ CGPlayer_C* m_player;
    /* 0004 */ int32_t m_wmoId;
    /* 0008 */ int32_t m_wmoHash;
    /* 000C */ C3Vector m_position;
    /* 0018 */ float m_radius;
    /* 001C */ float m_scale;
    /* 0020 */ CGMinimapFrame* m_frame;
};

struct POIINFO {
    /* 0000 */ int32_t m_icon;
    /* 0004 */ uint8_t unk_0004[48];
    /* 0034 */ C2Vector m_position;
    /* 003C */ const char* m_name;
};

struct MARKEDBLIP {
    /* 0000 */ int32_t m_list;
    /* 0004 */ int32_t m_index;
};

struct MinimapBlobRing {
    /* 0000 */ float m_start;
    /* 0004 */ float m_end;
    /* 0008 */ uint8_t m_reverse;

    MinimapBlobRing();
};

struct MINIMAPBLOB {
    /* 0000 */ TSGrowableArray<BLOBPOI> m_points;
    /* 0010 */ CTexture* m_insideTexture;
    /* 0014 */ CTexture* m_outsideTexture;
    /* 0018 */ CTexture* m_ringTexture;
    /* 001C */ uint8_t m_insideAlpha;
    /* 001D */ uint8_t m_outsideAlpha;
    /* 001E */ uint8_t m_ringAlpha;
    /* 0020 */ TSGrowableArray<MinimapBlobRing> m_rings;

    MINIMAPBLOB();
    ~MINIMAPBLOB();
};

struct MINIMAPMD5NAME : public TSHashObject<MINIMAPMD5NAME, HASHKEY_STRI> {
    /* 0018 */ char m_md5Name[40];
};

struct TrackingTypeInfo {
    /* 0000 */ int32_t m_type;
    /* 0004 */ uint32_t m_npcFlags;
    /* 0008 */ const char* m_name;
    /* 000C */ const char* m_texture;
    /* 0010 */ uint32_t m_classMask;
    /* 0014 */ int32_t m_category;
};


class CGMinimapFrame : public CSimpleFrame {
    public:
    static void RefreshShapeshiftTrackingSpells();
    static void SaveShapeshiftTracking();
    static void LoadTrackingState();
    static void EnterWorld();
    static void ShutdownGame();
    static void LeaveWorld();
    static CDataAllocator s_minimapHeap;
    static void SetPingPosition(const WOWGUID& guid, const C2Vector& position);
    static void MinimapTextureCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
    static void UpdateTrackingSpell();
    static void AddShapeshiftTrackingSpell(int32_t spellId);
    static void RemoveShapeshiftTrackingSpell(int32_t spellId);
    static void LoadShapeshiftTracking();
    static bool IsTrackingObjectType(char type);
    static bool IsTrackingNPC(uint32_t npcFlags);
    static void SetTracking(uint32_t index, bool enable);
    static bool SetTrackingMask(int32_t mask);
    static int32_t s_metatable;
    static int32_t s_objectType;

    static bool IsTrackingTrivialQuests();
    static bool IsTrackingDynamicPOI();
    static const char* s_objectTypeName;
    static CGMinimapFrame* s_minimapFrame;
    static int32_t s_rotateMinimap;
    static float s_facing;
    static C2Vector s_pingPosition;
    static int32_t s_scaleChanged;
    static float s_blipSize1;
    static float s_blipHalfSize1;
    static float s_blipSize2;
    static float s_blipHalfSize2;
    static float s_blipSize3;
    static float s_blipHalfSize3;
    static float s_blipSize4;
    static C3Vector s_blipQuads[3][4];
    static uint32_t s_trackingMask;
    static const TrackingTypeInfo s_trackingTypes[19];
    static TSGrowableArray<int32_t> s_shapeshiftTrackingSpells;
    static int32_t CountAvailableTrackingTypes();
    static int32_t GetTrackingTypeByIndex(int32_t index);

    static void operator delete(void* ptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void InitializeGame();
    static void RegisterScriptMethods(lua_State* L);
    static void RenderCallback(void* param);

    /* 01F0 */ int32_t m_blipFrameLevel;
    /* 01F4 */ CSimpleTexture* m_playerTexture;
    /* 01F8 */ float m_facing;
    /* 01FC */ CScriptRegion* m_compassTexture;
    /* 0200 */ uint32_t m_lastBlipUpdate;
    /* 0204 */ CTexture* m_questBlobInsideTexture;
    /* 0208 */ CTexture* m_questBlobOutsideTexture;
    /* 020C */ CTexture* m_questBlobRingTexture;
    /* 0210 */ uint8_t m_questBlobInsideAlpha;
    /* 0211 */ uint8_t m_questBlobOutsideAlpha;
    /* 0212 */ uint8_t m_questBlobRingAlpha;
    /* 0214 */ float m_questBlobRingSize;
    /* 0218 */ float m_questBlobRingScalar;
    /* 021C */ uint8_t m_questBlobVisible;
    /* 0220 */ CTexture* m_archBlobInsideTexture;
    /* 0224 */ CTexture* m_archBlobOutsideTexture;
    /* 0228 */ CTexture* m_archBlobRingTexture;
    /* 022C */ uint8_t m_archBlobInsideAlpha;
    /* 022D */ uint8_t m_archBlobOutsideAlpha;
    /* 022E */ uint8_t m_archBlobRingAlpha;
    /* 0230 */ float m_archBlobRingSize;
    /* 0234 */ float m_archBlobRingScalar;
    /* 0238 */ TSGrowableArray<C2Vector> m_blobPoints;

    ~CGMinimapFrame();
    int32_t GetScriptMetaTable() override;
    bool IsA(int32_t type) override;
    bool IsA(const char* typeName) override;
    const char* GetObjectTypeName() override;
    void PostLoadXML(linkObject* node, CStatus* status) override;
    void OnLayerUpdate(float elapsedSec) override;
    int32_t OnLayerTrackUpdate(const CMouseEvent& evt) override;
    void OnFrameRender(CRenderBatch* batch, uint32_t layer) override;
    void OnLayerCursorExit(int32_t a2, int32_t a3) override;
    bool UpdateScale(bool force) override;

    CGMinimapFrame(CSimpleFrame* parent);
    int32_t SetQuestBlobInsideTexture(const char* fileName);
    int32_t SetQuestBlobOutsideTexture(const char* fileName);
    int32_t SetQuestBlobRingTexture(const char* fileName);
    int32_t SetArchBlobInsideTexture(const char* fileName);
    int32_t SetArchBlobOutsideTexture(const char* fileName);
    int32_t SetArchBlobRingTexture(const char* fileName);
    void Render();
    void RenderInside(float scale, const C2Vector* center, const CImVector* color);
    static void RenderInsideTexture(const CImVector* color);
    static void RenderInsideSortQuads(MINIMAPTILE** head);
    static void RenderInsideQuad(MINIMAPTILE* tile, const CImVector* color);
    void SetCompassRotation(float rotation);
    void UpdateQuads(const C2Vector& center, float scale);
    void RenderPartyRaidBlips(const C44Matrix& rotation);
    void RenderVehicleBlips(const C44Matrix& rotation);
    static C2Vector WorldPosToMinimapFrameCoords(C3Vector center, float radius, float x, float y, float scale, float width);
    void RenderObjectBlips(const C44Matrix& rotation, bool otherFloor);
    void RenderPOIArrows(const C44Matrix& rotation);
    void RenderSuperTrackedQuestArrow(const C44Matrix& rotation, const C3Vector& position);
    void RenderPartyArrows(const C44Matrix& rotation);
    void RenderRaidArrows(const C44Matrix& rotation);
    void RenderPings(const C44Matrix& rotation);
    void RenderQuestCompletionNPC(const C44Matrix& rotation, C3Vector position);
    void RenderHeightArrow(const C2Vector& position, float height, const C44Matrix& rotation);
    void RenderMarkedBlips(const C44Matrix& rotation);
    void RenderQuestBlobs(const C44Matrix& rotation);
    void RenderArchBlobs(const C44Matrix& rotation);
    void RenderQuestBlob(const C44Matrix& rotation, uint32_t blobIndex);
    void RenderArchBlob(const C44Matrix& rotation, uint32_t siteIndex);
    void RenderBlobInside(const C44Matrix& rotation, MINIMAPBLOB* blob);
    void RenderBlobOutside(const C44Matrix& rotation, MINIMAPBLOB* blob);
    void RenderBlobRing(const C44Matrix& rotation, MINIMAPBLOB* blob);
    void AddBlobRingSegment(MINIMAPBLOB* blob, float from, float to);
    bool BuildBlobRings(MINIMAPBLOB* blob);
    float GetBlobRingScale();
    static int32_t ObjectEnumProc(WOWGUID guid, void* param);
    void UpdateRotation(float facing);
};

extern CTexture* s_staticPOIArrowTexture;
extern CTexture* s_corpseArrowTexture;
extern CTexture* s_poiArrowTexture;
extern CTexture* s_maskTexture;
extern CTexture* s_classBlipTexture;
extern CTexture* s_blipTexture;
extern CTexture* s_iconTexture;

void MinimapRegisterScriptFunctions();
void MinimapUnregisterScriptFunctions();


int32_t MinimapGetZoomLevels();


#include "WoW/Source/Ui/MinimapFrame.inl"

#endif
