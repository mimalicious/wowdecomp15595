#include "MinimapFrame.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/Ui/ResearchFrame.h"
#include "WoW/Source/Ui/BlobFrame.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Ui/CGQuestObjectiveCache.h"
#include "WoW/Source/UIUtil/CameraManager.h"
#include "WoW/Source/Game/GameClient/Minimap.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/Rec/AreaPOIRec.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include <cmath>
#include <cfloat>
#include <cstring>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/Gx/Gx.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/Ui/WorldMap.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/WMOAreaTableRec.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include <ctime>
#include <malloc.h>
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Gx/CGxMatrixStack.h"
#include "Engine/Source/Event/CMouseEvent.h"

// STATIC: 0x00D162EC
int32_t CGMinimapFrame::s_scaleChanged = 1;

// STATIC: 0x00D16364
const char* CGMinimapFrame::s_objectTypeName = "Minimap";

// STATIC: 0x00D1637C
int32_t CGMinimapFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00F611A4
float CGMinimapFrame::s_blipSize4;

// STATIC: 0x00F611A8
float CGMinimapFrame::s_blipHalfSize3;

// STATIC: 0x00F611AC
float CGMinimapFrame::s_blipSize3;

// STATIC: 0x00F611B0
float CGMinimapFrame::s_blipHalfSize2;

// STATIC: 0x00F611B4
float CGMinimapFrame::s_blipSize2;

// STATIC: 0x00F611B8
float CGMinimapFrame::s_blipHalfSize1;

// STATIC: 0x00F611BC
float CGMinimapFrame::s_blipSize1;

// STATIC: 0x00F611C4
int32_t CGMinimapFrame::s_objectType;

// STATIC: 0x00F611D0
int32_t CGMinimapFrame::s_rotateMinimap;

// STATIC: 0x00F611D4
float CGMinimapFrame::s_facing;

// STATIC: 0x00F611F8
C2Vector CGMinimapFrame::s_pingPosition;

// STATIC: 0x00F611F0
CGMinimapFrame* CGMinimapFrame::s_minimapFrame;

// STATIC: 0x00F6EF60
CDataAllocator CGMinimapFrame::s_minimapHeap(sizeof(CGMinimapFrame), 1);

// STATIC: 0x00F6DC88
C3Vector CGMinimapFrame::s_blipQuads[3][4];

// STATIC: 0x00F6114C
static CTexture* s_vehicleTextures[8];

// STATIC: 0x00F61180
static CTexture* s_groupArrowTexture;

// STATIC: 0x00F61184
CTexture* s_staticPOIArrowTexture;

// STATIC: 0x00F61188
CTexture* s_corpseArrowTexture;

// STATIC: 0x00F6118C
CTexture* s_poiArrowTexture;

// STATIC: 0x00F61190
CTexture* s_maskTexture;

// STATIC: 0x00F61194
CTexture* s_classBlipTexture;

// STATIC: 0x00F61198
CTexture* s_blipTexture;

// STATIC: 0x00F6119C
CTexture* s_iconTexture;

// STATIC: 0x00F6B808
static C2Vector s_iconTexCoords[196][4];

// STATIC: 0x00F6D888
static C2Vector s_classBlipTexCoords[32][4];

// STATIC: 0x00F6D088
static C2Vector s_blipTexCoords[64][4];

// STATIC: 0x00F610A8
static uint32_t s_hoverObjects[41];

// STATIC: 0x00F611D8
static uint32_t s_hoverPOIs;

// STATIC: 0x00F611DC
static uint32_t s_hoverPOIDirections;

// STATIC: 0x00F611E0
static uint32_t s_hoverParty;

// STATIC: 0x00F611E8
static uint64_t s_hoverRaid;

// STATIC: 0x00C08690
const TrackingTypeInfo CGMinimapFrame::s_trackingTypes[19] = {
    { 1, 0x200000, "MINIMAP_TRACKING_AUCTIONEER", "Auctioneer", 0x0, 2 },
    { 1, 0x20000, "MINIMAP_TRACKING_BANKER", "Banker", 0x0, 2 },
    { 1, 0x100000, "MINIMAP_TRACKING_BATTLEMASTER", "BattleMaster", 0x0, 2 },
    { 1, 0x20, "MINIMAP_TRACKING_TRAINER_CLASS", "Class", 0x0, 2 },
    { 1, 0x2000, "MINIMAP_TRACKING_FLIGHTMASTER", "FlightMaster", 0x0, -1 },
    { 1, 0x200, "MINIMAP_TRACKING_VENDOR_FOOD", "Food", 0x0, 2 },
    { 1, 0x10000, "MINIMAP_TRACKING_INNKEEPER", "Innkeeper", 0x0, 2 },
    { 2, 0x13, "MINIMAP_TRACKING_MAILBOX", "Mailbox", 0x0, 2 },
    { 1, 0x400, "MINIMAP_TRACKING_VENDOR_POISON", "Poisons", 0x10, 2 },
    { 1, 0x40, "MINIMAP_TRACKING_TRAINER_PROFESSION", "Profession", 0x0, 2 },
    { 1, 0x800, "MINIMAP_TRACKING_VENDOR_REAGENT", "Reagents", 0x0, 2 },
    { 1, 0x1000, "MINIMAP_TRACKING_REPAIR", "Repair", 0x0, 2 },
    { 3, 0x0, "MINIMAP_TRACKING_TRIVIAL_QUESTS", "TrivialQuests", 0x0, -1 },
    { 1, 0x400000, "MINIMAP_TRACKING_STABLEMASTER", "StableMaster", 0x8, 2 },
    { 4, 0x1, "MINIMAP_TRACKING_POI", "POIArrow", 0x0, -1 },
    { 5, 0x0, "MINIMAP_TRACKING_TARGET", "Target", 0x0, -1 },
    { 6, 0x0, "MINIMAP_TRACKING_FOCUS", "Focus", 0x0, -1 },
    { 7, 0x0, "MINIMAP_TRACKING_QUEST_POIS", "QuestBlob", 0x0, -1 },
    { 8, 0x0, "MINIMAP_TRACKING_DIGSITES", "ArchBlob", 0x0, -1 },
};

// STATIC: 0x00F61218
static MINIMAPQUAD s_quads[4] = {
    { { 0.0f, 0.0f, 0.5f, 0.5f }, C2Vector(0.0f, 0.0f) },
    { { 0.5f, 0.0f, 1.0f, 0.5f }, C2Vector(1.0f, 0.0f) },
    { { 0.5f, 0.5f, 1.0f, 1.0f }, C2Vector(1.0f, 1.0f) },
    { { 0.0f, 0.5f, 0.5f, 1.0f }, C2Vector(0.0f, 1.0f) },
};

// STATIC: 0x00F61278
static MINIMAPTILE s_tiles[256];

// STATIC: 0x00D164A0
static MINIMAPINDOOR s_indoor;

// STATIC: 0x00F6B678
static TSGrowableArray<POIINFO> s_poiInfo;

// STATIC: 0x00F6DD18
static TSGrowableArray<C3Vector> s_blipVertices;

// STATIC: 0x00F6DD28
static TSGrowableArray<C2Vector> s_blipTexCoords2;

// STATIC: 0x00F6DD38
static TSGrowableArray<uint16_t> s_blipIndices;

// STATIC: 0x00F6DD58
static TSGrowableArray<OBJINFO> s_objInfo[41];

// STATIC: 0x00F6B688
static C2Vector s_poiArrowPositions[3];

// STATIC: 0x00F6B6A0
static C2Vector s_partyArrowPositions[5];

// STATIC: 0x00F6B6C8
static C2Vector s_raidArrowPositions[40];

// STATIC: 0x00F6DD48
static TSGrowableArray<POIDIRECTIONDATA> s_poiDirections;

// STATIC: 0x00C08930
static uint16_t s_arrowIndices[4] = { 0, 2, 1, 3 };

// STATIC: 0x00D16498
static float s_arrowRotation = 2.3561945f;

// STATIC: 0x00F6DFE8
static MEMBERINFO s_partyMembers[5];

// STATIC: 0x00F6E1A0
static MEMBERINFO s_raidMembers[40];

// STATIC: 0x00D165A4
static uint16_t s_quadIndices[4] = { 0, 1, 2, 3 };

// STATIC: 0x00D165AC
static uint16_t s_insideQuadIndices[4] = { 0, 1, 3, 2 };

// STATIC: 0x00D165B4
static uint16_t s_insideIndices[4] = { 0, 1, 3, 2 };

// STATIC: 0x00D165BC
static float s_indoorTexScale = 0.33333334f;

// STATIC: 0x00F611A0
static int32_t s_enteredWorld;

// STATIC: 0x00F611CC
uint32_t CGMinimapFrame::s_trackingMask;

// STATIC: 0x00F611F4
static int32_t s_iconTexCoordsReady;

// STATIC: 0x00F61208
TSGrowableArray<int32_t> CGMinimapFrame::s_shapeshiftTrackingSpells;

// STATIC: 0x00F6116C
static CTexture* s_positionArrowsTexture;

// STATIC: 0x00F61170
static CTexture* s_pingExpandTexture;

// STATIC: 0x00F61174
static CTexture* s_pingRotateTexture;

// STATIC: 0x00F61178
static CTexture* s_pingCenterTexture;

// STATIC: 0x00F6117C
static CTexture* s_questArrowTexture;

// STATIC: 0x00D162D8
static uint8_t s_defaultBlobAlpha = 0x80;

// STATIC: 0x00D162DC
static float s_defaultBlobRingScalar = 1.0f;

// STATIC: 0x00D162E4
static float s_heightArrowThreshold = 10.0f;

// STATIC: 0x00D162F0
static int32_t s_objectBlipOrder[29] = { 21, 24, 26, 23, 18, 32, 16, 17, 22, 27, 20, 25, 19, 2, 3, 4, 5, 6, 7, 8, 14, 15, 11, 12, 28, 29, 9, 10, 13 };

// STATIC: 0x00F61090
static MARKEDBLIP s_markedBlips[2];

// STATIC: 0x00F610A0
static uint8_t s_superTrackedArrowShown;

// STATIC: 0x00F611C0
static uint32_t s_pingLastTime;

// STATIC: 0x00F611C8
static uint32_t s_pingTime;

// STATIC: 0x00F61200
static C2Vector s_superTrackedArrowPosition;

// STATIC: 0x00D162D9
static uint8_t s_blobsEnabled = 1;

// STATIC: 0x00D162DA
static uint8_t s_questBlobInsideEnabled = 1;

// STATIC: 0x00D162DB
static uint8_t s_questBlobOutsideEnabled = 1;

// STATIC: 0x00D162E0
static uint32_t s_blobSplinePoints = 32;

// STATIC: 0x00D162E8
static float s_blobRingRadiusScale = 0.9f;

// STATIC: 0x00D16368
static uint8_t s_mergeQuestBlobs = 1;

// STATIC: 0x00D1636C
static float s_quadrantAngles[4] = { 0.78539819f, 2.3561945f, 3.9269907f, 5.4977875f };

// STATIC: 0x00D1659C
static const char* s_heightArrowDownText = "|TInterface\\Minimap\\Minimap-PositionArrows:0:0:0:0:16:32:0:16:16:32|t";

// STATIC: 0x00D165A0
static const char* s_heightArrowUpText = "|TInterface\\Minimap\\Minimap-PositionArrows:0:0:0:0:16:32:0:16:0:16|t";

// STATIC: 0x00F611E4
static uint8_t s_hoverQuestArrow;

// INLINED
void CGMinimapFrame::operator delete(void* ptr) {
    CGMinimapFrame::s_minimapHeap.PutData(ptr, NULL, 0);
}

// INLINED
void CGMinimapFrame::UpdateRotation(float facing) {
    this->m_facing = facing;
    CGMinimapFrame::s_facing = facing;

    if (this->m_playerTexture) {
        if (g_rotateMinimapCVar->m_intValue) {
            facing = 0.0f;
        }

        this->m_playerTexture->SetRotation(facing, 0.5f, 0.5f);
    }
}

// INLINED
static uint32_t GetAngleQuadrant(float angle) {
    while (angle < 0.00000011920929f) {
        angle += 6.2831855f;
    }

    while (angle >= 6.2831855f) {
        angle -= 6.2831855f;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (s_quadrantAngles[i] > angle) {
            return i;
        }
    }

    return 0;
}

// INLINED
C2Vector CGMinimapFrame::WorldPosToMinimapFrameCoords(C3Vector center, float radius, float x, float y, float scale, float width) {
    float inv = 1.0f / radius;
    float half = scale * width * 0.5f;
    C2Vector coords;
    coords.x = half - (y - center.y) * half * inv;
    coords.y = (x - center.x) * half * inv + half;
    return coords;
}

// OFFSET: [0x00619630/0x1402A6B50]
int32_t MinimapGetZoomLevels() {
    return 6;
}

// OFFSET: 0x008C6410
CSimpleFrame* CGMinimapFrame::Create(CSimpleFrame* parent) {
    void* data = CGMinimapFrame::s_minimapHeap.GetData(0, typeid(CGMinimapFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGMinimapFrame(parent) : NULL;
}

// OFFSET: 0x009CE480
static void ComputeIconTexCoords() {
    uint32_t width = 256;
    uint32_t height = 256;
    TextureGetDimensions(s_iconTexture, &width, &height, 1);

    uint32_t columns = CMath::sqrt_(196);
    float texel = 1.0f / width;
    float size = texel * 18.0f;

    for (uint32_t i = 0; i < 196; i++) {
        float left = (i % columns) * size + texel;
        float right = left + size - texel;
        float top = (i / columns) * size + texel;
        float bottom = top + size - texel;

        s_iconTexCoords[i][0].x = left;
        s_iconTexCoords[i][0].y = bottom;
        s_iconTexCoords[i][1].x = right;
        s_iconTexCoords[i][1].y = bottom;
        s_iconTexCoords[i][2].x = left;
        s_iconTexCoords[i][2].y = top;
        s_iconTexCoords[i][3].x = right;
        s_iconTexCoords[i][3].y = top;
    }
}

// OFFSET: 0x009CE580
bool CGMinimapFrame::UpdateScale(bool force) {
    if (CSimpleFrame::UpdateScale(force)) {
        CGMinimapFrame::s_scaleChanged = 1;
        return true;
    }

    return false;
}

// OFFSET: 0x009CE5B0
void MINIMAPTILE::BuildQuad(const CRect& rect, uint32_t index, const C2Vector* center, float scale, const CRect& bounds, float layoutScale, float width) {
    float size = scale * 2.0f;
    float inv = 1.0f / size;
    this->m_maskCoords[0].x = (rect.minX - bounds.minX) * inv;
    this->m_maskCoords[0].y = (rect.maxY - bounds.minY) * inv;
    this->m_maskCoords[1].x = (rect.maxX - bounds.minX) * inv;
    this->m_maskCoords[1].y = (rect.maxY - bounds.minY) * inv;
    this->m_maskCoords[2].x = (rect.minX - bounds.minX) * inv;
    this->m_maskCoords[2].y = (rect.minY - bounds.minY) * inv;
    this->m_maskCoords[3].x = (rect.maxX - bounds.minX) * inv;
    this->m_maskCoords[3].y = (rect.minY - bounds.minY) * inv;

    const CRect& quad = s_quads[index].m_rect;
    this->m_texCoords[0].x = (rect.minX - quad.minY) * 2.0f;
    this->m_texCoords[0].y = (rect.maxY - quad.minX) * 2.0f;
    this->m_texCoords[1].x = (rect.maxX - quad.minY) * 2.0f;
    this->m_texCoords[1].y = (rect.maxY - quad.minX) * 2.0f;
    this->m_texCoords[2].x = (rect.minX - quad.minY) * 2.0f;
    this->m_texCoords[2].y = (rect.minY - quad.minX) * 2.0f;
    this->m_texCoords[3].x = (rect.maxX - quad.minY) * 2.0f;
    this->m_texCoords[3].y = (rect.minY - quad.minX) * 2.0f;

    float vertexInv = 1.0f / size;
    float extent = layoutScale * width;
    float left = (rect.minX - bounds.minX) * vertexInv * extent;
    float right = vertexInv * (rect.maxX - bounds.minX) * extent;
    float top = extent - vertexInv * (rect.maxY - bounds.minY) * extent;
    float bottom = extent - (rect.minY - bounds.minY) * vertexInv * extent;
    this->m_vertices[0].x = left;
    this->m_vertices[0].y = top;
    this->m_vertices[0].z = 0.0f;
    this->m_vertices[1].x = right;
    this->m_vertices[1].y = top;
    this->m_vertices[1].z = 0.0f;
    this->m_vertices[2].x = left;
    this->m_vertices[2].y = bottom;
    this->m_vertices[2].z = 0.0f;
    this->m_vertices[3].x = right;
    this->m_vertices[3].y = bottom;
    this->m_vertices[3].z = 0.0f;
}

// OFFSET: 0x009CE8A0
static void GetSquareDirection(float angle, float* sine, float* cosine) {
    CMath::fsincos_(angle, sine, cosine);
    float s = *sine;
    float absSine = std::fabs(s);

    if (absSine < 0.00000023841858f) {
        return;
    }

    float c = *cosine;
    float absCosine = std::fabs(c);

    if (absCosine < 0.00000023841858f) {
        return;
    }

    if (absSine > absCosine) {
        float sign = c > 0.0f ? 1.0f : -1.0f;
        *cosine = std::fabs(c / s) * sign;
        *sine = *sine > 0.0f ? 1.0f : -1.0f;
    } else {
        float sign = s > 0.0f ? 1.0f : -1.0f;
        *sine = std::fabs(s / c) * sign;
        *cosine = *cosine > 0.0f ? 1.0f : -1.0f;
    }
}

// OFFSET: 0x009CE990
static float GetDirectionAngle(const C3Vector& direction) {
    C2Vector normalized(direction.x, direction.y);
    float lengthSqr = normalized.x * normalized.x + normalized.y * normalized.y;

    if (lengthSqr > 0.00000023841858f) {
        float inverse = 1.0f / std::sqrt(lengthSqr);
        normalized.x *= inverse;
        normalized.y *= inverse;
    }

    float angle = normalized.Angle(1.0f);

    if (angle < 0.0f) {
        angle += 6.2831855f;
    }

    return angle;
}

// OFFSET: 0x009CEA20
float CGMinimapFrame::GetBlobRingScale() {
    if (MinimapIsIndoors()) {
        float radius = MinimapGetIndoorZoomRadius(MinimapGetIndoorZoom());
        return radius / MinimapGetIndoorZoomRadius(MinimapGetZoomLevels() - 1) * this->m_questBlobRingSize;
    }

    float tiles = MinimapGetOutdoorZoomTiles(MinimapGetOutdoorZoom());
    return tiles / MinimapGetOutdoorZoomTiles(MinimapGetZoomLevels() - 1) * this->m_questBlobRingSize;
}

// OFFSET: 0x009CEA90
void CGMinimapFrame::RenderInsideSortQuads(MINIMAPTILE** head) {
    for (uint32_t i = 0; i < 256; i++) {
        MINIMAPTILE* tile = &s_tiles[i];

        if (!(tile->m_flags & 2) || !tile->m_texture) {
            continue;
        }

        MINIMAPTILE** link = head;

        for (MINIMAPTILE* current = *head; current; current = current->m_nextSorted) {
            if (current->m_depth >= tile->m_depth) {
                break;
            }

            link = &current->m_nextSorted;
        }

        tile->m_nextSorted = *link;
        *link = tile;
    }
}

// OFFSET: 0x009CEB00
void CGMinimapFrame::ShutdownGame() {
    if (s_maskTexture) {
        TextureRelease(s_maskTexture);
    }

    s_maskTexture = NULL;

    if (s_iconTexture) {
        TextureRelease(s_iconTexture);
    }

    s_iconTexture = NULL;

    if (s_blipTexture) {
        TextureRelease(s_blipTexture);
    }

    s_blipTexture = NULL;

    if (s_classBlipTexture) {
        TextureRelease(s_classBlipTexture);
    }

    s_classBlipTexture = NULL;

    for (uint32_t i = 0; i < 8; i++) {
        TextureRelease(s_vehicleTextures[i]);
        s_vehicleTextures[i] = NULL;
    }

    if (s_staticPOIArrowTexture) {
        TextureRelease(s_staticPOIArrowTexture);
    }

    s_staticPOIArrowTexture = NULL;

    if (s_poiArrowTexture) {
        TextureRelease(s_poiArrowTexture);
    }

    s_poiArrowTexture = NULL;

    if (s_corpseArrowTexture) {
        TextureRelease(s_corpseArrowTexture);
    }

    s_corpseArrowTexture = NULL;

    if (s_groupArrowTexture) {
        TextureRelease(s_groupArrowTexture);
    }

    s_groupArrowTexture = NULL;

    if (s_questArrowTexture) {
        TextureRelease(s_questArrowTexture);
    }

    s_questArrowTexture = NULL;

    if (s_pingCenterTexture) {
        TextureRelease(s_pingCenterTexture);
    }

    s_pingCenterTexture = NULL;

    if (s_pingRotateTexture) {
        TextureRelease(s_pingRotateTexture);
    }

    s_pingRotateTexture = NULL;

    if (s_pingExpandTexture) {
        TextureRelease(s_pingExpandTexture);
    }

    s_pingExpandTexture = NULL;

    if (s_positionArrowsTexture) {
        TextureRelease(s_positionArrowsTexture);
    }

    s_positionArrowsTexture = NULL;
}

// OFFSET: 0x009CEC90
void CGMinimapFrame::UpdateTrackingSpell() {
    Script_SendUniqueSignal(SCRIPT_EVENT_MINIMAP_UPDATE_TRACKING);
}

// OFFSET: 0x009CECA0
static bool IsTrackingType(int32_t type) {
    for (uint32_t i = 0; i < 19; i++) {
        if (CGMinimapFrame::s_trackingTypes[i].m_type == type && ((1 << i) & CGMinimapFrame::s_trackingMask)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009CECE0
bool CGMinimapFrame::IsTrackingObjectType(char type) {
    for (uint32_t i = 0; i < 19; i++) {
        if (((1 << i) & CGMinimapFrame::s_trackingMask) && CGMinimapFrame::s_trackingTypes[i].m_type == 2 && CGMinimapFrame::s_trackingTypes[i].m_npcFlags == static_cast<uint32_t>(type)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009CED30
bool CGMinimapFrame::IsTrackingNPC(uint32_t npcFlags) {
    for (uint32_t i = 0; i < 19; i++) {
        if (((1 << i) & CGMinimapFrame::s_trackingMask) && CGMinimapFrame::s_trackingTypes[i].m_type == 1 && (npcFlags & CGMinimapFrame::s_trackingTypes[i].m_npcFlags)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009CF120
static uint32_t ColorToLuminance(CImVector color) {
    return (77 * color.r + 151 * color.g + 28 * color.b) >> 8;
}

// OFFSET: 0x009CF160
static bool IsCreatureInBlipList(int32_t list, int32_t entry) {
    for (uint32_t i = 0; i < s_objInfo[list].m_count; i++) {
        uint32_t high = static_cast<uint32_t>(s_objInfo[list].m_data[i].m_guid.m_guid >> 32);
        uint32_t type = high & 0xF0F00000;

        if (type == 0xF0300000 || type == 0xF0500000) {
            if ((high & 0xFFFFF) == static_cast<uint32_t>(entry)) {
                return true;
            }
        } else if (type == 0xF0100000) {
            if ((high & 0xFFFFF) == (static_cast<uint32_t>(entry) & 0x7FFFFFFF)) {
                return true;
            }
        }
    }

    return false;
}

// OFFSET: 0x009CF200
void MINIMAPTILE::UpdateQuad(uint32_t index, C2Vector center, float scale, float layoutScale, float width) {
    this->m_flags &= ~1u;

    if (index >= 4) {
        return;
    }

    CRect bounds;
    bounds.minY = center.y - scale;
    bounds.minX = center.x - scale;
    bounds.maxY = center.y + scale;
    bounds.maxX = center.x + scale;

    float minY = bounds.minY;
    float minX = bounds.minX;
    float maxY = bounds.maxY;
    float maxX = bounds.maxX;
    const CRect& quad = s_quads[index].m_rect;

    if (maxX >= quad.maxY) {
        maxX = quad.maxY;
    }

    if (quad.maxX <= maxY) {
        maxY = quad.maxX;
    }

    if (quad.minY >= minX) {
        minX = quad.minY;
    }

    if (quad.minX >= minY) {
        minY = quad.minX;
    }

    CRect rect;
    rect.minY = minY;
    rect.minX = minX;
    rect.maxY = maxY;
    rect.maxX = maxX;

    if (quad.minX < maxY && quad.maxX > minY && maxX > quad.minY && quad.maxY > minX) {
        this->BuildQuad(rect, index, &center, scale, bounds, layoutScale, width);
        this->m_flags |= 1;
    }
}

// OFFSET: 0x009CF300/0x14075FFC0
int32_t CGMinimapFrame::OnLayerTrackUpdate(const CMouseEvent& evt) {
    if (!CSimpleFrame::OnLayerTrackUpdate(evt)) {
        bool anyObject = false;

        for (uint32_t i = 0; i < 41; i++) {
            if (s_hoverObjects[i]) {
                anyObject = true;
                break;
            }
        }

        if (s_hoverPOIs || s_hoverPOIDirections || s_hoverParty || s_hoverRaid || anyObject || s_hoverQuestArrow) {
            s_hoverPOIs = 0;
            memset(s_hoverObjects, 0, sizeof(s_hoverObjects));
            s_hoverPOIDirections = 0;
            s_hoverParty = 0;
            s_hoverRaid = 0;
            s_hoverQuestArrow = 0;
            CGGameUI::m_gameTooltip->Hide();
        }

        return 0;
    }

    float mouseX = evt.m_data.m_mousePosition.x;
    float mouseY = evt.m_data.m_mousePosition.y;
    CRect rect;
    this->GetRect(&rect);
    float half = CGMinimapFrame::s_blipHalfSize1;
    uint32_t pois = 0;

    for (uint32_t i = 0; i < s_poiInfo.Count(); i++) {
        float x = s_poiInfo[i].m_position.x + rect.minX;
        float y = s_poiInfo[i].m_position.y + rect.minY;

        if (x - half <= mouseX && x + half >= mouseX && half + y >= mouseY && y - half <= mouseY) {
            pois |= 1 << i;
        }
    }

    uint32_t objects[41];
    memset(objects, 0, sizeof(objects));

    for (uint32_t i = 0; i < 41; i++) {
        for (uint32_t j = 0; j < s_objInfo[i].Count(); j++) {
            OBJINFO& info = s_objInfo[i][j];
            float x = info.m_position.x + rect.minX;
            float y = info.m_position.y + rect.minY;

            if (x - half > mouseX || x + half < mouseX || half + y < mouseY || y - half > mouseY) {
                continue;
            }

            const char* name;
            CGObject_C* object = ClntObjMgrObjectPtr(info.m_guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (object) {
                name = object->GetObjectName();
            } else {
                uint32_t high = static_cast<uint32_t>(info.m_guid.m_guid >> 32);
                name = NULL;

                if (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(info.m_guid.m_guid))) {
                    WOWGUID requester = WOWGUID();
                    NameCache* record = g_nameDBCache.GetRecord(info.m_guid.m_guid, requester, NULL, NULL, false);
                    name = record ? record->m_name : NULL;
                }
            }

            if (name) {
                objects[i] |= 1 << j;
            }
        }
    }

    float tolerance = this->m_layoutScale * 0.005f;
    uint32_t directions = 0;

    for (uint32_t i = 0; i < s_poiDirections.Count(); i++) {
        if ((!s_indoor.m_active || s_poiDirections[i].m_alwaysShow) && std::fabs(mouseX - (s_poiArrowPositions[i].x + rect.minX)) < tolerance && std::fabs(mouseY - (s_poiArrowPositions[i].y + rect.minY)) < tolerance) {
            directions |= 1 << i;
        }
    }

    uint32_t party = 0;

    for (uint32_t i = 0; i < 5; i++) {
        if (s_partyMembers[i].m_offMap && std::fabs(mouseX - (s_partyArrowPositions[i].x + rect.minX)) < tolerance && std::fabs(mouseY - (s_partyArrowPositions[i].y + rect.minY)) < tolerance) {
            party |= 1 << i;
        }
    }

    uint64_t raid = 0;

    for (uint32_t i = 0; i < 40; i++) {
        if (s_raidMembers[i].m_offMap && std::fabs(mouseX - (s_raidArrowPositions[i].x + rect.minX)) < tolerance && std::fabs(mouseY - (s_raidArrowPositions[i].y + rect.minY)) < tolerance) {
            raid |= 1ull << i;
        }
    }

    uint8_t questArrow = 0;

    if (s_superTrackedArrowShown && std::fabs(mouseX - (rect.minX + s_superTrackedArrowPosition.x)) < tolerance && std::fabs(mouseY - (rect.minY + s_superTrackedArrowPosition.y)) < tolerance) {
        questArrow = 1;
    }

    bool objectsChanged = false;

    for (uint32_t i = 0; i < 41; i++) {
        if (s_hoverObjects[i] != objects[i]) {
            objectsChanged = true;
            break;
        }
    }

    if (s_hoverPOIs == pois && !objectsChanged && s_hoverPOIDirections == directions && s_hoverParty == party && s_hoverRaid == raid && s_hoverQuestArrow == questArrow) {
        return 1;
    }

    s_hoverPOIs = pois;
    s_hoverPOIDirections = directions;
    s_hoverParty = party;
    s_hoverRaid = raid;
    s_hoverQuestArrow = questArrow;
    bool anyObject = false;

    for (uint32_t i = 0; i < 41; i++) {
        s_hoverObjects[i] = objects[i];

        if (objects[i]) {
            anyObject = true;
        }
    }

    int32_t colorblind = 0;
    CVar* colorblindVar = CVar::Lookup("colorblindMode");

    if (colorblindVar && colorblindVar->m_intValue) {
        colorblind = 1;
    }

    CGTooltip* tooltip = CGGameUI::m_gameTooltip;

    if (!tooltip) {
        return 1;
    }

    if (!s_hoverPOIs && !anyObject && !s_hoverPOIDirections && !s_hoverParty && !s_hoverRaid && !s_hoverQuestArrow) {
        tooltip->Hide();
        return 1;
    }

    tooltip->SetOwner(this, ANCHOR_CURSOR, 0.0f, 0.0f);
    bool first = true;
    char text[4096];
    text[0] = 0;

    if (s_hoverPOIDirections) {
        for (uint32_t i = 0; i < 3; i++) {
            if (s_hoverPOIDirections & (1 << i)) {
                if (!first) {
                    SStrPack(text, "\n", sizeof(text));
                }

                first = false;
                SStrPack(text, s_poiDirections.m_data[i].m_name, sizeof(text));
            }
        }
    }

    if (s_hoverParty) {
        for (uint32_t i = 0; i < 5; i++) {
            if (s_hoverParty & (1 << i)) {
                if (!first) {
                    SStrPack(text, "\n", sizeof(text));
                }

                first = false;
                SStrPack(text, s_partyMembers[i].m_name, sizeof(text));
            }
        }
    }

    if (s_hoverRaid) {
        for (uint32_t i = 0; i < 40; i++) {
            if (s_hoverRaid & (1ull << i)) {
                if (!first) {
                    SStrPack(text, "\n", sizeof(text));
                }

                first = false;
                SStrPack(text, s_raidMembers[i].m_name, sizeof(text));
            }
        }
    }

    if (s_hoverPOIs) {
        for (uint32_t i = 0; i < s_poiInfo.Count(); i++) {
            if (s_hoverPOIs & (1 << i)) {
                if (!first) {
                    SStrPack(text, "\n", sizeof(text));
                }

                first = false;
                SStrPack(text, s_poiInfo[i].m_name, sizeof(text));
            }
        }
    }

    for (uint32_t i = 0; i < 41; i++) {
        for (uint32_t j = 0; j < s_objInfo[i].Count(); j++) {
            if (!(s_hoverObjects[i] & (1 << j))) {
                continue;
            }

            const char* name;
            CGObject_C* object = ClntObjMgrObjectPtr(s_objInfo[i][j].m_guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (object) {
                name = object->GetObjectName();
            } else {
                WOWGUID requester = WOWGUID();
                NameCache* record = g_nameDBCache.GetRecord(s_objInfo[i][j].m_guid.m_guid, requester, NULL, NULL, false);
                name = record ? record->m_name : NULL;
            }

            if (!name) {
                continue;
            }

            if (!first) {
                SStrPack(text, "\n", sizeof(text));
            }

            first = false;

            if (s_objInfo[i][j].m_height > s_heightArrowThreshold) {
                SStrPack(text, s_heightArrowUpText, sizeof(text));
            } else if (-s_heightArrowThreshold > s_objInfo[i][j].m_height) {
                SStrPack(text, s_heightArrowDownText, sizeof(text));
            }

            if (s_objInfo[i][j].m_otherFloor) {
                SStrPack(text, "|cffb0b0b0", sizeof(text));
            }

            SStrPack(text, name, sizeof(text));

            if (s_objInfo[i][j].m_questGiver && colorblind) {
                uint32_t length = SStrLen(text);
                SStrPrintf(&text[length], sizeof(text) - length, "\n <%s>", FrameScript_GetText("MINIMAP_TRACKING_FLIGHTMASTER", -1, GENDER_NOT_APPLICABLE));
            }

            if (s_objInfo[i][j].m_tracked && object) {
                const char* title = static_cast<CGUnit_C*>(object)->GetUnitTitle();

                if (title) {
                    uint32_t length = SStrLen(text);
                    SStrPrintf(&text[length], sizeof(text) - length, "\n <%s>", title);
                }
            }

            if (s_objInfo[i][j].m_otherFloor) {
                SStrPack(text, "|r", sizeof(text));
            }
        }
    }

    if (s_hoverQuestArrow) {
        if (!first) {
            SStrPack(text, "\n", sizeof(text));
        }

        const char* questName = CGQuestLog::GetQuestNameByID(CGQuestLog::m_superTrackedQuest);

        if (questName) {
            SStrPack(text, questName, sizeof(text));
        }
    }

    tooltip->AddLine(text, NULL, 0);
    tooltip->Show();
    return 1;
}

// PROVEN: 0371142e IDENTICAL/NONE
// OFFSET: 0x009D05D0
void CGMinimapFrame::OnLayerCursorExit(int32_t a2, int32_t a3) {
    CGGameUI::m_gameTooltip->Hide();
    CSimpleFrame::OnLayerCursorExit(a2, a3);
}

// OFFSET: 0x009D0600
void CGMinimapFrame::SetCompassRotation(float rotation) {
    CSimpleTexture* compass = static_cast<CSimpleTexture*>(this->m_compassTexture);

    if (compass && (compass->m_visible)) {
        compass->SetRotation(rotation, 0.5f, 0.5f);
    }
}

// OFFSET: 0x009D0640
void CGMinimapFrame::UpdateQuads(const C2Vector& center, float scale) {
    for (uint32_t i = 0; i < 256; i++) {
        s_tiles[i].UpdateQuad(i, center, scale, this->m_layoutScale, this->GetWidth());
    }
}

// OFFSET: 0x009D06B0
void CGMinimapFrame::RenderPOIArrows(const C44Matrix& rotation) {
    C2Vector texCoords[4];
    C3Vector vertices[4];
    static C3Vector s_normal(0.0f, 0.0f, 1.0f);

    MinimapBuildPOIDirectionData(&s_poiDirections);

    for (uint32_t i = 0; i < s_poiDirections.m_count; i++) {
        POIDIRECTIONDATA* direction = &s_poiDirections.m_data[i];

        if (s_indoor.m_active && !direction->m_alwaysShow) {
            continue;
        }

        CTexture* texture;

        if (direction->m_dynamicIndex == 2) {
            texture = s_corpseArrowTexture;
        } else if (direction->m_dynamicIndex == -2) {
            texture = s_staticPOIArrowTexture;
        } else {
            texture = s_poiArrowTexture;
        }

        if (!texture) {
            continue;
        }

        CGxTex* gxTex = TextureGetGxTex(texture);

        if (!gxTex) {
            continue;
        }

        g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
        CImVector color(0xFF, 0xFF, 0xFF, 0xFF);
        float facing = direction->m_facing;
        float cosine;
        float sine;
        CMath::fsincos_(facing, &sine, &cosine);
        float width = this->GetWidth();
        float height = this->GetHeight();
        C3Vector point;
        point.x = width * this->m_layoutScale * 0.5f + CGMinimapFrame::s_blipSize4 * sine * -0.75f * this->m_layoutScale;
        point.y = height * this->m_layoutScale * 0.5f + CGMinimapFrame::s_blipSize4 * cosine * 0.75f * this->m_layoutScale;
        point.z = 0.0f;

        if (g_rotateMinimapCVar->m_intValue) {
            facing = facing - this->m_facing;
            point *= rotation;
        }

        facing = facing - s_arrowRotation;
        CMath::fsincos_(facing, &sine, &cosine);
        texCoords[0].x = sine + 0.5f;
        texCoords[0].y = 0.5f - cosine;
        texCoords[1].x = 0.5f - cosine;
        texCoords[1].y = 0.5f - sine;
        texCoords[2].x = cosine + 0.5f;
        texCoords[2].y = sine + 0.5f;
        texCoords[3].x = 0.5f - sine;
        texCoords[3].y = cosine + 0.5f;
        s_poiArrowPositions[i].x = point.x;
        s_poiArrowPositions[i].y = point.y;

        for (uint32_t j = 0; j < 4; j++) {
            vertices[j].x = CGMinimapFrame::s_blipQuads[2][j].x * this->m_layoutScale + point.x;
            vertices[j].y = CGMinimapFrame::s_blipQuads[2][j].y * this->m_layoutScale + point.y;
            vertices[j].z = CGMinimapFrame::s_blipQuads[2][j].z * this->m_layoutScale + point.z;
        }

        GxPrimLockVertexPtrs(4, vertices, sizeof(C3Vector), &s_normal, 0, &color, 0, NULL, 0, texCoords, sizeof(C2Vector), NULL, 0);
        DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
        GxDrawLockedElements(GxPrim_TriangleStrip, 4, s_arrowIndices);
        GxPrimUnlockVertexPtrs();
    }
}

// OFFSET: 0x009D0C10
void CGMinimapFrame::RenderSuperTrackedQuestArrow(const C44Matrix& rotation, const C3Vector& position) {
    float radius = static_cast<float>(MinimapGetViewRadius());
    s_superTrackedArrowShown = 0;

    if (!s_questArrowTexture) {
        return;
    }

    CGxTex* gxTex = TextureGetGxTex(s_questArrowTexture);

    if (!gxTex) {
        return;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    int32_t questId = CGQuestLog::m_superTrackedQuest;

    if (!questId) {
        return;
    }

    bool complete;
    int32_t mapId;
    int32_t floor;
    int32_t x;
    int32_t y;
    int32_t objectiveIndex;

    if (!CGQuestLog::GetQuestPOILoc(questId, complete, mapId, floor, x, y, objectiveIndex)) {
        return;
    }

    C2Vector loc(0.0f, 0.0f);
    float distSqr = CGQuestLog::GetQuestDistSqr(questId, loc);

    if (distSqr == FLT_MAX || (complete && radius * radius > distSqr * 1.3f)) {
        return;
    }

    radius = static_cast<float>(MinimapGetViewRadius());

    if (!complete && radius * radius > distSqr) {
        return;
    }

    C3Vector target(loc.x, loc.y, position.z);
    float facing = CalculateFacingTo(&position, &target);
    float cosine;
    float sine;
    CMath::fsincos_(facing, &sine, &cosine);
    C3Vector point;
    point.x = this->m_layoutScale * (CGMinimapFrame::s_blipSize4 * sine * -0.75f) + this->GetWidth() * this->m_layoutScale * 0.5f;
    point.y = this->m_layoutScale * (CGMinimapFrame::s_blipSize4 * cosine * 0.75f) + this->GetHeight() * this->m_layoutScale * 0.5f;
    point.z = 0.0f;

    if (g_rotateMinimapCVar->m_intValue) {
        facing = facing - this->m_facing;
        point *= rotation;
    }

    facing = facing - s_arrowRotation;
    CMath::fsincos_(facing, &sine, &cosine);
    s_superTrackedArrowPosition.x = point.x;
    s_superTrackedArrowPosition.y = point.y;
    C2Vector texCoords[4];
    texCoords[0].x = sine + 0.5f;
    texCoords[0].y = 0.5f - cosine;
    texCoords[1].x = 0.5f - cosine;
    texCoords[1].y = 0.5f - sine;
    texCoords[2].x = cosine + 0.5f;
    texCoords[2].y = sine + 0.5f;
    texCoords[3].x = 0.5f - sine;
    texCoords[3].y = cosine + 0.5f;
    C3Vector vertices[4];

    for (uint32_t j = 0; j < 4; j++) {
        vertices[j].x = this->m_layoutScale * CGMinimapFrame::s_blipQuads[1][j].x + point.x;
        vertices[j].y = this->m_layoutScale * CGMinimapFrame::s_blipQuads[1][j].y + point.y;
        vertices[j].z = this->m_layoutScale * CGMinimapFrame::s_blipQuads[1][j].z + point.z;
    }

    CImVector color(0xFF, 0xFF, 0xFF, 0xFF);
    static C3Vector s_normal(0.0f, 0.0f, 1.0f);
    GxPrimLockVertexPtrs(4, vertices, sizeof(C3Vector), &s_normal, 0, &color, 0, NULL, 0, texCoords, sizeof(C2Vector), NULL, 0);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    GxDrawLockedElements(GxPrim_TriangleStrip, 4, s_arrowIndices);
    GxPrimUnlockVertexPtrs();
    s_superTrackedArrowShown = 1;
}

// OFFSET: 0x009D1110/0x1407623F0
static int32_t UpdateObjectInteractIcon(WOWGUID guid, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    uint32_t npcFlags = reinterpret_cast<uint32_t>(param);

    if (unit && (!npcFlags || (npcFlags & unit->m_unit->m_unitNpcFlags))) {
        unit->UpdateInteractIcon();
    }

    return 1;
}

// OFFSET: 0x009D1160
bool CGMinimapFrame::IsTrackingTrivialQuests() {
    return IsTrackingType(3);
}

// OFFSET: 0x009D1170
bool CGMinimapFrame::IsTrackingDynamicPOI() {
    return IsTrackingType(4);
}

// OFFSET: 0x009D1A70
void MINIMAPTILE::Render(uint32_t index, const CImVector* color) {
    if (!(this->m_flags & 1) || !this->m_texture) {
        return;
    }

    CGxTex* gxTex = TextureGetGxTex(this->m_texture);

    if (!gxTex) {
        return;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    GxRsSet(GxRs_ColorOp0, 0);
    GxRsSet(GxRs_AlphaOp0, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTexBlocking(s_maskTexture));
    GxRsSet(GxRs_ColorOp1, 0);
    GxRsSet(GxRs_AlphaOp1, 0);
    C3Vector normal(0.0f, 0.0f, 1.0f);
    GxPrimLockVertexPtrs(4, this->m_vertices, sizeof(C3Vector), &normal, 0, color, 0, NULL, 0, this->m_texCoords, sizeof(C2Vector), this->m_maskCoords, sizeof(C2Vector));
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    GxDrawLockedElements(GxPrim_TriangleStrip, 4, s_quadIndices);
    GxPrimUnlockVertexPtrs();
    GxRsSet(GxRs_Texture1, static_cast<void*>(NULL));
}

// OFFSET: 0x009D1B70
void CGMinimapFrame::OnLayerUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerUpdate(elapsedSec);

    CGCamera* camera = CGWorldFrame::GetActiveCamera();
    CGObject_C* target = ClntObjMgrObjectPtr(camera->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (target) {
        this->UpdateRotation(target->GetFacing());
    } else {
        this->UpdateRotation(StaticSingleton<CCameraManager>::Instance()->GetYaw());
    }
}

// OFFSET: 0x009D1C60
void CGMinimapFrame::RenderInsideQuad(MINIMAPTILE* tile, const CImVector* color) {
    C3Vector vertices[4];
    vertices[0] = C3Vector(tile->m_bounds.b.x, tile->m_bounds.b.y, 0.0f);
    vertices[1] = C3Vector(tile->m_bounds.t.x, tile->m_bounds.b.y, 0.0f);
    vertices[2] = C3Vector(tile->m_bounds.t.x, tile->m_bounds.t.y, 0.0f);
    vertices[3] = C3Vector(tile->m_bounds.b.x, tile->m_bounds.t.y, 0.0f);
    CGxTex* gxTex = TextureGetGxTex(tile->m_texture);

    if (!gxTex) {
        return;
    }

    CGxTexParms parms;
    GxTexParameters(gxTex, parms);
    float halfX = 0.5f / parms.m_width;
    float halfY = 0.5f / parms.m_height;
    C2Vector texCoords[4];
    texCoords[0] = C2Vector(halfX, 1.0f - halfY);
    texCoords[1] = C2Vector(1.0f - halfX, 1.0f - halfY);
    texCoords[2] = C2Vector(1.0f - halfX, halfY);
    texCoords[3] = C2Vector(halfX, halfY);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    GxRsSet(GxRs_DepthTest, 0);
    GxPrimLockVertexPtrs(4, vertices, sizeof(C3Vector), NULL, 0, color, 0, NULL, 0, texCoords, sizeof(C2Vector), NULL, 0);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    GxDrawLockedElements(GxPrim_TriangleStrip, 4, s_insideQuadIndices);
    GxPrimUnlockVertexPtrs();
}

// OFFSET: 0x009D1E00/0x140763290
int32_t CGMinimapFrame::CountAvailableTrackingTypes() {
    int32_t count = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t classMask = player ? 1 << player->m_unit->m_unitFieldBytes0[1] : 0;

    for (uint32_t i = 0; i < sizeof(CGMinimapFrame::s_trackingTypes) / sizeof(CGMinimapFrame::s_trackingTypes[0]); i++) {
        uint32_t mask = CGMinimapFrame::s_trackingTypes[i].m_classMask;

        if (!mask || (mask & classMask)) {
            count++;
        }
    }

    return count;
}

// OFFSET: 0x009D1E60
static int32_t UpdateQuestGiverStatus(WOWGUID guid, void* param) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object && object->m_questStatus) {
            CGPlayer_C::QueryQuestGiverStatus(guid);
        }
    }

    return 1;
}

// OFFSET: 0x009D1ED0
void CGMinimapFrame::SetTracking(uint32_t index, bool enable) {
    if (index <= 18) {
        if (enable) {
            CGMinimapFrame::s_trackingMask |= 1 << index;
        } else {
            CGMinimapFrame::s_trackingMask &= ~(1 << index);
        }
    }

    if (CGMinimapFrame::s_trackingTypes[index].m_type == 3) {
        ClntObjMgrEnumVisibleObjects(UpdateQuestGiverStatus, NULL);
    }

    if (CGMinimapFrame::s_trackingTypes[index].m_type == 1) {
        ClntObjMgrEnumVisibleObjects(UpdateObjectInteractIcon, reinterpret_cast<void*>(CGMinimapFrame::s_trackingTypes[index].m_npcFlags));
    }

    if (CGMinimapFrame::s_trackingTypes[index].m_type == 4) {
        UpdatePointsOfInterest();
    }

    Script_SendUniqueSignal(SCRIPT_EVENT_MINIMAP_UPDATE_TRACKING);
}

// OFFSET: 0x009D1F60/0x140763410
bool CGMinimapFrame::SetTrackingMask(int32_t mask) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t classMask = player ? 1 << player->m_unit->m_unitFieldBytes0[1] : 0;
    CGMinimapFrame::s_trackingMask = 0;

    for (uint32_t i = 0; i < 19; i++) {
        if ((1 << i) & mask) {
            uint32_t typeMask = CGMinimapFrame::s_trackingTypes[i].m_classMask;

            if (!typeMask || (typeMask & classMask)) {
                CGMinimapFrame::s_trackingMask |= 1 << i;
            }
        }
    }

    return false;
}

// OFFSET: 0x009D1FE0/0x1407634A0
int32_t CGMinimapFrame::GetTrackingTypeByIndex(int32_t index) {
    int32_t available = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t classMask = player ? 1 << player->m_unit->m_unitFieldBytes0[1] : 0;

    for (uint32_t i = 0; i < 19; i++) {
        uint32_t mask = CGMinimapFrame::s_trackingTypes[i].m_classMask;

        if (!mask || (mask & classMask)) {
            if (available == index) {
                return i;
            }

            available++;
        }
    }

    return -1;
}

// OFFSET: 0x009D2050
void CGMinimapFrame::RefreshShapeshiftTrackingSpells() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t form = player->GetShapeshiftForm();

    for (uint32_t i = 0; i < CGMinimapFrame::s_shapeshiftTrackingSpells.Count(); i++) {
        SpellRec* spell = g_spellDB.GetRecord(CGMinimapFrame::s_shapeshiftTrackingSpells.m_data[i]);

        if (spell && Spell_C_SpellHasShapeshiftMask(spell, form - 1) && !player->HasAura(spell->m_id, WOWGUID())) {
            Spell_C_CastSpell(CGMinimapFrame::s_shapeshiftTrackingSpells.m_data[i], NULL, WOWGUID(), false);
        }
    }
}

// OFFSET: 0x009D2110
void CGMinimapFrame::SaveShapeshiftTracking() {
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("minimapShapeshiftTracking");
    list.m_pos = 2;
    list.m_state = 0;

    for (uint32_t i = 0; i < CGMinimapFrame::s_shapeshiftTrackingSpells.Count(); i++) {
        list.AddNextValue(CGMinimapFrame::s_shapeshiftTrackingSpells.m_data[i]);
    }

    if (list.m_state == 1) {
        return;
    }

    list.m_buffer[0] = 'v';
    list.m_buffer[1] = 1;
    list.m_buffer[list.m_pos] = 0;
    list.m_cvar->Set(list.m_buffer, true, false, false, true);
}

// OFFSET: 0x009D21B0/0x140763630
void CGMinimapFrame::SetPingPosition(const WOWGUID& guid, const C2Vector& position) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGMinimapFrame::s_pingPosition = position;
    char unit[32] = "player";

    if (guid != player->m_obj->m_guid) {
        bool found = false;

        for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
            if (guid == CGGameUI::GetPartyMember(i)) {
                SStrPrintf(unit, sizeof(unit), "party%d", i + 1);
                found = true;
                break;
            }
        }

        if (!found) {
            for (uint32_t j = 0; j < CGRaidInfo::m_numMembers; j++) {
                if (guid == GetRaidMemberGUID(j)) {
                    SStrPrintf(unit, sizeof(unit), "raid%d", j + 1);
                    break;
                }
            }
        }
    }

    C2Vector playerPosition(player->GetPosition());
    C2Vector offset;
    offset.x = position.x - playerPosition.x;
    offset.y = position.y - playerPosition.y;

    if (g_rotateMinimapCVar->m_intValue) {
        C44Matrix rotation = C44Matrix::Rotation(-CGMinimapFrame::s_facing, C3Vector(0.0f, 0.0f, 1.0f), true);
        C2Vector rotated;
        rotated.x = rotation.c0 * 0.0f + rotation.a0 * offset.x + rotation.b0 * offset.y;
        rotated.y = offset.y * rotation.b1 + offset.x * rotation.a1 + 0.0f * rotation.c1;
        offset = rotated;
    }

    float radius = MinimapGetViewRadius();
    FrameScript_SignalEvent(SCRIPT_EVENT_MINIMAP_PING, "%s%f%f", unit, -(1.0 / (radius + radius) * offset.y), offset.x * (1.0 / (radius + radius)));
    s_pingTime = 5000;
    s_pingLastTime = FrameTime::GetCurTimeMs();
}

// OFFSET: 0x009D3040
MINIMAPBLOB::~MINIMAPBLOB() {
    this->m_points.Clear();
    this->m_rings.Clear();
}

// OFFSET: 0x009D30D0
int32_t CGMinimapFrame::ObjectEnumProc(WOWGUID guid, void* param) {
    if (guid == ClntObjMgrGetActivePlayer()) {
        return 1;
    }

    MINIMAPENUMPARAM* data = static_cast<MINIMAPENUMPARAM*>(param);
    CGPlayer_C* player = data->m_player;
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 1;
    }

    C3Vector position = object->GetPosition();
    float centerX = data->m_position.x;
    float centerY = data->m_position.y;
    float radius = data->m_radius * 0.8f;

    if ((position.x - centerX) * (position.x - centerX) + (position.y - centerY) * (position.y - centerY) > radius * radius) {
        return 1;
    }

    uint8_t tracked = 0;
    uint8_t questGiver = 0;
    int32_t marker = -1;
    uint32_t type;
    int32_t objectType = object->m_obj->m_type;

    if (objectType == (TYPEMASK_GAMEOBJECT | TYPEMASK_OBJECT)) {
        CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(object);
        uint32_t status = gameObject->m_questStatus;

        if (status == 10) {
            type = 10;
        } else if (status == 8 || (status == 2 && IsTrackingType(3))) {
            type = 9;
        } else if (status == 7 || (status == 4 && IsTrackingType(3))) {
            type = 11;
        } else if (CGQuestObjectiveCache::ObjectTrackedInQuests(gameObject, true)) {
            type = 29;
        } else if (gameObject->IsTracked() && gameObject->m_gameObject->m_typeID == 19 && static_cast<CGUnit_C*>(player)->CanInteract(gameObject)) {
            type = 23;
        } else if (gameObject->IsTracked() && static_cast<CGUnit_C*>(player)->CanInteract(gameObject)) {
            type = 8;
        } else {
            if (!player->CanTrackObject(gameObject)) {
                return 1;
            }

            type = 8;
        }
    } else {
        if (objectType == (TYPEMASK_PLAYER | TYPEMASK_UNIT | TYPEMASK_OBJECT)) {
            if (CGGameUI::IsInParty(guid) || CGGameUI::IsPartyMember(guid)) {
                return 1;
            }
        } else if (objectType != (TYPEMASK_UNIT | TYPEMASK_OBJECT)) {
            return 1;
        }

        CGUnit_C* unit = static_cast<CGUnit_C*>(object);
        const WOWGUID& controller = unit->m_unit->m_unitFieldCharmedby ? unit->m_unit->m_unitFieldCharmedby : unit->m_unit->m_unitFieldSummonedby;

        if (controller == ClntObjMgrGetActivePlayer()) {
            type = 15;
        } else {
            if (unit->m_unit->m_unitFieldHealth <= 0 && !unit->CanInteractWhileDead()) {
                return 1;
            }

            uint32_t status = unit->m_questStatus;

            if (status == 10) {
                type = 10;
            } else if (status == 8 || (status == 2 && IsTrackingType(3))) {
                type = 9;
            } else if (status == 7 || (status == 4 && IsTrackingType(3))) {
                type = 11;
            } else if (CGQuestObjectiveCache::CreatureTrackedInQuests(unit, true)) {
                type = 29;
            } else if (unit->IsTracked() && player->CanInteract(unit)) {
                uint32_t npcFlags = unit->m_unit->m_unitNpcFlags;

                if (npcFlags & 0x200000) {
                    type = 16;
                } else if (npcFlags & 0x10000) {
                    type = 22;
                } else if (npcFlags & 0x4000000) {
                    type = 23;
                } else if (npcFlags & 0x20000) {
                    type = 17;
                } else if (npcFlags & 0x100000) {
                    type = 18;
                } else if (npcFlags & 0x20) {
                    tracked = 1;
                    type = 19;
                } else if (npcFlags & 0x2000) {
                    type = 20;
                } else if (npcFlags & 0x200) {
                    type = 21;
                } else if (npcFlags & 0x400) {
                    type = 24;
                } else if (npcFlags & 0x40) {
                    tracked = 1;
                    type = 25;
                } else if (npcFlags & 0x800) {
                    type = 26;
                } else if (npcFlags & 0x1000) {
                    type = 27;
                } else {
                    type = (npcFlags & 0x400000) ? 32 : 8;
                }
            } else if (unit->m_interactIconType == 2) {
                type = 13;
                questGiver = 1;
            } else {
                WOWGUID focus = CGGameUI::m_focusTarget;

                if (IsTrackingType(6) && focus && focus == unit->m_obj->m_guid) {
                    if (CGRaidInfo::CanBeRaidTarget(focus)) {
                        marker = 1;
                    }
                } else if (IsTrackingType(5) && CGGameUI::m_lockedTarget && CGGameUI::m_lockedTarget == unit->m_obj->m_guid && CGRaidInfo::CanBeRaidTarget(CGGameUI::m_lockedTarget)) {
                    marker = 0;
                }

                if (!player->CanTrackUnit(unit) && marker == -1) {
                    return 1;
                }

                if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
                    if (player->CanAttack(unit)) {
                        type = 2;
                    } else {
                        int32_t faction = static_cast<CGPlayer_C*>(unit)->GetPVPFactionIndex();
                        type = 4 - (faction != player->GetPVPFactionIndex());
                    }
                } else {
                    int32_t reaction = unit->UnitReaction(player);

                    if (reaction < 4) {
                        type = (reaction > 1) + 5;
                    } else {
                        type = 7;
                    }
                }
            }
        }
    }

    uint32_t wmoId = 0;
    uint32_t wmoHash = 0;
    uint32_t groupNum;

    if (World::QueryObjectInside(object->m_worldObject, 0)) {
        World::QueryMapObjIDs(object->m_worldObject, wmoId, wmoHash, groupNum);
    }

    if (!data->m_wmoId || (wmoId == data->m_wmoId && wmoHash == data->m_wmoHash)) {
        OBJINFO* info = s_objInfo[type].New();
        C3Vector objectPosition = object->GetPosition();
        float width = data->m_frame->GetWidth();
        C2Vector coords = CGMinimapFrame::WorldPosToMinimapFrameCoords(data->m_position, data->m_radius, objectPosition.x, objectPosition.y, data->m_scale, width);
        info->m_guid = guid;
        info->m_rotated.x = coords.x;
        info->m_rotated.y = coords.y;
        info->m_position.x = coords.x;
        info->m_position.y = coords.y;
        info->m_otherFloor = wmoId != data->m_wmoId;
        info->m_tracked = tracked;
        info->m_questGiver = questGiver;
        info->m_marker = marker;
        info->m_height = object->GetPosition().z - data->m_position.z;
    }

    return 1;
}

// OFFSET: 0x009D3730/0x140764FB0
void CGMinimapFrame::LeaveWorld() {
    CVar* var = CVar::Lookup("minimapTrackedInfov2");

    if (var) {
        var->Set(static_cast<int32_t>(CGMinimapFrame::s_trackingMask), true, false, false, true);
    }

    MinimapShutdown();

    for (uint32_t i = 0; i < 256; i++) {
        if (s_tiles[i].m_texture) {
            TextureRelease(s_tiles[i].m_texture);
            s_tiles[i].m_texture = NULL;
        }

        s_tiles[i].m_flags = 0;
        s_tiles[i].m_x = -1;
        s_tiles[i].m_y = -1;
    }

    if (s_indoor.m_texture) {
        TextureRelease(s_indoor.m_texture);
    }

    s_indoor.m_texture = NULL;

    if (s_indoor.m_frameBuffer) {
        g_theGxDevicePtr->FrameBufferDelete(s_indoor.m_frameBuffer);
    }

    s_poiInfo.Clear();

    for (uint32_t i = 0; i < 41; i++) {
        s_objInfo[i].Clear();
    }

    s_enteredWorld = 0;
}

// OFFSET: 0x009D3850
CGMinimapFrame::CGMinimapFrame(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_facing = -10000.0f;
    this->m_lastBlipUpdate = 0;
    CGMinimapFrame::s_rotateMinimap = g_rotateMinimapCVar->m_intValue;
    this->m_playerTexture = NULL;

    if (!CGMinimapFrame::s_minimapFrame) {
        CGMinimapFrame::s_minimapFrame = this;
    }

    this->EnableEvent(SIMPLE_EVENT_MOUSE, -1);

    float scale = NDCToDDCHeight(1.0f) * 1.6666666f;
    CGMinimapFrame::s_blipSize1 = 0.0125f * scale;
    CGMinimapFrame::s_blipSize4 = scale * 0.055f;
    CGMinimapFrame::s_blipSize3 = scale * 0.045f;
    CGMinimapFrame::s_blipSize2 = scale * 0.0375f;
    CGMinimapFrame::s_blipHalfSize1 = CGMinimapFrame::s_blipSize1 * 0.5f;
    CGMinimapFrame::s_blipHalfSize2 = CGMinimapFrame::s_blipSize2 * 0.5f;
    CGMinimapFrame::s_blipHalfSize3 = CGMinimapFrame::s_blipSize3 * 0.5f;

    float halfSizes[3] = { CGMinimapFrame::s_blipHalfSize1, CGMinimapFrame::s_blipHalfSize2, CGMinimapFrame::s_blipHalfSize3 };

    for (uint32_t i = 0; i < 3; i++) {
        float half = halfSizes[i];
        CGMinimapFrame::s_blipQuads[i][0] = C3Vector(-half, -half, 0.0f);
        CGMinimapFrame::s_blipQuads[i][1] = C3Vector(half, -half, 0.0f);
        CGMinimapFrame::s_blipQuads[i][2] = C3Vector(-half, half, 0.0f);
        CGMinimapFrame::s_blipQuads[i][3] = C3Vector(half, half, 0.0f);
    }

    this->m_blipFrameLevel = this->m_level + 3;

    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    CSimpleTexture* texture = data ? new (data) CSimpleTexture(this, 2, 0, true) : NULL;
    this->m_playerTexture = texture;
    texture->SetPoint(FRAMEPOINT_CENTER, this, FRAMEPOINT_CENTER, 0.001f, 0.001f, 1);
    this->m_questBlobInsideAlpha = s_defaultBlobAlpha;
    this->m_questBlobOutsideAlpha = s_defaultBlobAlpha;
    this->m_questBlobRingAlpha = s_defaultBlobAlpha;
    this->m_archBlobInsideAlpha = s_defaultBlobAlpha;
    this->m_archBlobOutsideAlpha = s_defaultBlobAlpha;
    this->m_archBlobRingAlpha = s_defaultBlobAlpha;
    this->m_compassTexture = NULL;
    this->m_questBlobInsideTexture = NULL;
    this->m_questBlobOutsideTexture = NULL;
    this->m_questBlobRingTexture = NULL;
    this->m_questBlobVisible = 0;
    this->m_archBlobInsideTexture = NULL;
    this->m_archBlobOutsideTexture = NULL;
    this->m_archBlobRingTexture = NULL;
    this->m_questBlobRingSize = 1250.0f;
    this->m_questBlobRingScalar = s_defaultBlobRingScalar;
    this->m_archBlobRingSize = 1250.0f;
    this->m_archBlobRingScalar = s_defaultBlobRingScalar;
}

// OFFSET: 0x009D3C70
const char* CGMinimapFrame::GetObjectTypeName() {
    return CGMinimapFrame::s_objectTypeName;
}

// PROVEN: 59736c8c IDENTICAL/NONE
// OFFSET: 0x009D3C80/0x140765610
int32_t CGMinimapFrame::GetScriptMetaTable() {
    return CGMinimapFrame::s_metatable;
}

// OFFSET: 0x009D3CA0
bool CGMinimapFrame::IsA(int32_t type) {
    return type == CGMinimapFrame::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: 0x009D3CE0
bool CGMinimapFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CGMinimapFrame::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: [0x009D3D20, 0x009D9170]
CGMinimapFrame::~CGMinimapFrame() {
    if (CGMinimapFrame::s_minimapFrame == this) {
        CGMinimapFrame::s_minimapFrame = NULL;
    }

    if (this->m_questBlobInsideTexture) {
        TextureRelease(this->m_questBlobInsideTexture);
        this->m_questBlobInsideTexture = NULL;
    }

    if (this->m_questBlobOutsideTexture) {
        TextureRelease(this->m_questBlobOutsideTexture);
        this->m_questBlobOutsideTexture = NULL;
    }

    if (this->m_questBlobRingTexture) {
        TextureRelease(this->m_questBlobRingTexture);
        this->m_questBlobRingTexture = NULL;
    }

    if (this->m_archBlobInsideTexture) {
        TextureRelease(this->m_archBlobInsideTexture);
        this->m_archBlobInsideTexture = NULL;
    }

    if (this->m_archBlobOutsideTexture) {
        TextureRelease(this->m_archBlobOutsideTexture);
        this->m_archBlobOutsideTexture = NULL;
    }

    if (this->m_archBlobRingTexture) {
        TextureRelease(this->m_archBlobRingTexture);
        this->m_archBlobRingTexture = NULL;
    }
}

// OFFSET: 0x009D3EF0
void CGMinimapFrame::RenderBlobInside(const C44Matrix& rotation, MINIMAPBLOB* blob) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || blob->m_points.m_count < 3) {
        return;
    }

    CGxTex* gxTex = TextureGetGxTex(blob->m_insideTexture);

    if (!gxTex) {
        return;
    }

    s_blipVertices.SetCount(blob->m_points.m_count + 1);
    s_blipTexCoords2.SetCount(blob->m_points.m_count + 1);
    this->m_blobPoints.SetCount(blob->m_points.m_count + 1);
    s_blipIndices.SetCount(3 * blob->m_points.m_count);
    C3Vector center(0.0f, 0.0f, 0.0f);

    for (uint32_t i = 0; i < blob->m_points.m_count; i++) {
        s_blipVertices.m_data[i].y = static_cast<float>(blob->m_points.m_data[i].m_y);
        s_blipVertices.m_data[i].x = static_cast<float>(blob->m_points.m_data[i].m_x);
        s_blipVertices.m_data[i].z = 0.0f;
        s_blipTexCoords2.m_data[i].x = 1.0f;
        s_blipTexCoords2.m_data[i].y = 1.0f;
        s_blipIndices.m_data[3 * i] = static_cast<uint16_t>(i);
        s_blipIndices.m_data[3 * i + 1] = static_cast<uint16_t>((i + 1) % blob->m_points.m_count);
        s_blipIndices.m_data[3 * i + 2] = static_cast<uint16_t>(blob->m_points.m_count);
        center.x = s_blipVertices.m_data[i].x + center.x;
        center.y = s_blipVertices.m_data[i].y + center.y;
        center.z = s_blipVertices.m_data[i].z + center.z;
    }

    float inverseCount = static_cast<float>(1.0 / static_cast<double>(blob->m_points.m_count));
    center.x *= inverseCount;
    center.y *= inverseCount;
    center.z = inverseCount * center.z;
    s_blipVertices.m_data[blob->m_points.m_count] = center;
    s_blipTexCoords2.m_data[blob->m_points.m_count].x = 0.0f;
    s_blipTexCoords2.m_data[blob->m_points.m_count].y = 0.0f;

    for (uint32_t i = 0; i < s_blipVertices.m_count; i++) {
        C3Vector* vertex = &s_blipVertices.m_data[i];
        C3Vector origin = player->GetPosition();
        float radius = static_cast<float>(MinimapGetViewRadius());
        float half = this->m_layoutScale * this->GetWidth() * 0.5f;
        C3Vector point;
        point.x = half - (1.0f / radius) * (half * (vertex->y - origin.y));
        point.y = half + (1.0f / radius) * (half * (vertex->x - origin.x));
        point.z = 0.0f;
        this->m_blobPoints.m_data[i].x = point.x;
        this->m_blobPoints.m_data[i].y = point.y;
        C2Vector* maskCoord = &this->m_blobPoints.m_data[i];
        float size = this->GetWidth() * this->m_layoutScale;
        maskCoord->y = (1.0f / size) * maskCoord->y;
        maskCoord->x = (1.0f / size) * maskCoord->x;

        if (g_rotateMinimapCVar->m_intValue) {
            point *= rotation;
        }

        s_blipVertices.m_data[i].x = point.x;
        s_blipVertices.m_data[i].y = point.y;
        s_blipVertices.m_data[i].z = 0.0f;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    GxRsSet(GxRs_ColorOp0, 0);
    GxRsSet(GxRs_AlphaOp0, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTexBlocking(s_maskTexture));
    GxRsSet(GxRs_ColorOp1, 0);
    GxRsSet(GxRs_AlphaOp1, 0);
    CImVector color(0xFF, 0xFF, 0xFF, blob->m_insideAlpha);
    C3Vector normal(0.0f, 0.0f, 1.0f);
    GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &normal, 0, &color, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), this->m_blobPoints.m_data, sizeof(C2Vector));
    GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    GxDrawLockedElements();
    GxPrimUnlockVertexPtrs();
    GxPrimUnlockIndexPtr();
    GxRsSet(GxRs_Texture1, static_cast<void*>(NULL));
    s_blipVertices.SetCount(0);
    s_blipTexCoords2.SetCount(0);
    s_blipIndices.m_count = 0;
    this->m_blobPoints.SetCount(0);
}

// OFFSET: 0x009D44A0
void CGMinimapFrame::RenderBlobOutside(const C44Matrix& rotation, MINIMAPBLOB* blob) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || blob->m_points.m_count < 3) {
        return;
    }

    CGxTex* gxTex = TextureGetGxTex(blob->m_outsideTexture);

    if (!gxTex) {
        return;
    }

    s_blipVertices.SetCount(2 * blob->m_points.m_count);
    s_blipTexCoords2.SetCount(2 * blob->m_points.m_count);
    this->m_blobPoints.SetCount(2 * blob->m_points.m_count);
    s_blipIndices.SetCount(2 * blob->m_points.m_count + 2);
    TSGrowableArray<C3Vector> edges[2];
    edges[0].SetCount(blob->m_points.m_count);
    edges[1].SetCount(blob->m_points.m_count);
    uint32_t count = blob->m_points.m_count;
    C3Vector center(0.0f, 0.0f, 0.0f);

    for (uint32_t i = 0; i < count; i++) {
        center.x = static_cast<float>(blob->m_points.m_data[i].m_x) + center.x;
        center.y = static_cast<float>(blob->m_points.m_data[i].m_y) + center.y;
    }

    float inverseCount = static_cast<float>(1.0 / static_cast<double>(count));
    center.x *= inverseCount;
    center.y *= inverseCount;
    center.z = inverseCount * center.z;

    for (uint32_t i = 0; i < blob->m_points.m_count; i++) {
        C3Vector* vertex = &s_blipVertices.m_data[2 * i];
        vertex->y = static_cast<float>(blob->m_points.m_data[i].m_y);
        vertex->z = 0.0f;
        vertex->x = static_cast<float>(blob->m_points.m_data[i].m_x);
        s_blipTexCoords2.m_data[2 * i].x = 0.0f;
        s_blipTexCoords2.m_data[2 * i].y = 0.0f;
        s_blipTexCoords2.m_data[2 * i + 1].x = 1.0f;
        s_blipTexCoords2.m_data[2 * i + 1].y = 1.0f;
        s_blipIndices.m_data[2 * i] = static_cast<uint16_t>(2 * i);
        s_blipIndices.m_data[2 * i + 1] = static_cast<uint16_t>((2 * i + 1) % s_blipVertices.m_count);
        uint32_t next = (i + 1) % blob->m_points.m_count;
        float currentY = static_cast<float>(blob->m_points.m_data[i].m_y);
        float currentX = static_cast<float>(blob->m_points.m_data[i].m_x);
        float nx = -(static_cast<float>(blob->m_points.m_data[next].m_y) - currentY);
        float ny = static_cast<float>(blob->m_points.m_data[next].m_x) - currentX;
        float nz = 0.0f;
        float length = ny * ny + nx * nx;

        if (length > 0.00000023841858f) {
            float inverse = 1.0f / std::sqrt(length);
            nx *= inverse;
            ny *= inverse;
            nz = inverse * 0.0f;
        }

        float border = this->GetBlobRingScale() * 0.0099999998f;
        nx = border * nx;
        ny = border * ny;
        nz = border * nz;

        if ((center.y - currentY) * ny + center.z * nz + (center.x - currentX) * nx > 0.0f) {
            nx *= -1.0f;
            ny *= -1.0f;
            nz *= -1.0f;
        }

        edges[0].m_data[i] = C3Vector(currentX + nx, currentY + ny, nz);
        edges[1].m_data[i] = C3Vector(static_cast<float>(blob->m_points.m_data[next].m_x) + nx, static_cast<float>(blob->m_points.m_data[next].m_y) + ny, nz);
    }

    s_blipIndices.m_data[2 * blob->m_points.m_count] = 0;
    s_blipIndices.m_data[2 * blob->m_points.m_count + 1] = 1;

    for (uint32_t i = 0; i < blob->m_points.m_count; i++) {
        uint32_t next = (i + 1) % blob->m_points.m_count;
        C3Vector hit(0.0f, 0.0f, 0.0f);

        if (IntersectLines(edges[0].m_data[i], edges[1].m_data[i], edges[0].m_data[next], edges[1].m_data[next], &hit)) {
            s_blipVertices.m_data[2 * i + 1] = C3Vector(hit.x, hit.y, 0.0f);
        } else {
            s_blipVertices.m_data[2 * i + 1] = edges[0].m_data[i];
        }
    }

    for (uint32_t i = 0; i < s_blipVertices.m_count; i++) {
        C3Vector* vertex = &s_blipVertices.m_data[i];
        C3Vector origin = player->GetPosition();
        float radius = static_cast<float>(MinimapGetViewRadius());
        float half = this->m_layoutScale * this->GetWidth() * 0.5f;
        C3Vector point;
        point.x = half - (1.0f / radius) * (half * (vertex->y - origin.y));
        point.y = half + (1.0f / radius) * (half * (vertex->x - origin.x));
        point.z = 0.0f;
        this->m_blobPoints.m_data[i].x = point.x;
        this->m_blobPoints.m_data[i].y = point.y;
        C2Vector* maskCoord = &this->m_blobPoints.m_data[i];
        float size = this->GetWidth() * this->m_layoutScale;
        maskCoord->x = maskCoord->x * (1.0f / size);
        maskCoord->y = (1.0f / size) * maskCoord->y;

        if (g_rotateMinimapCVar->m_intValue) {
            point *= rotation;
        }

        s_blipVertices.m_data[i].x = point.x;
        s_blipVertices.m_data[i].y = point.y;
        s_blipVertices.m_data[i].z = 0.0f;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    GxRsSet(GxRs_ColorOp0, 0);
    GxRsSet(GxRs_AlphaOp0, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTexBlocking(s_maskTexture));
    GxRsSet(GxRs_ColorOp1, 0);
    GxRsSet(GxRs_AlphaOp1, 0);
    CImVector color(0xFF, 0xFF, 0xFF, blob->m_outsideAlpha);
    C3Vector normal(0.0f, 0.0f, 1.0f);
    GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &normal, 0, &color, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), this->m_blobPoints.m_data, sizeof(C2Vector));
    GxPrimLockIndexPtr(GxPrim_TriangleStrip, s_blipIndices.m_count, s_blipIndices.m_data);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    GxDrawLockedElements();
    GxPrimUnlockVertexPtrs();
    GxPrimUnlockIndexPtr();
    GxRsSet(GxRs_Texture1, static_cast<void*>(NULL));
    s_blipVertices.SetCount(0);
    s_blipTexCoords2.SetCount(0);
    s_blipIndices.m_count = 0;
    this->m_blobPoints.SetCount(0);
}

// OFFSET: 0x009D4CF0
void CGMinimapFrame::AddBlobRingSegment(MINIMAPBLOB* blob, float from, float to) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    C3Vector position = player->GetPosition();
    float radius = static_cast<float>(MinimapGetViewRadius() * this->m_questBlobRingScalar);
    uint32_t vertexBase = s_blipVertices.m_count;
    uint16_t indexBase = static_cast<uint16_t>(s_blipIndices.m_count);
    uint32_t texCoordBase = s_blipTexCoords2.m_count;
    s_blipVertices.SetCount(s_blipVertices.m_count + 3);
    s_blipVertices.m_data[vertexBase].y = position.y;
    s_blipVertices.m_data[vertexBase].x = position.x;
    s_blipVertices.m_data[vertexBase].z = 0.0f;
    s_blipTexCoords2.SetCount(s_blipTexCoords2.m_count + 3);
    s_blipTexCoords2.m_data[texCoordBase].x = 0.5f;
    s_blipTexCoords2.m_data[texCoordBase].y = 0.5f;
    float sine;
    float cosine;
    GetSquareDirection(from, &sine, &cosine);
    s_blipTexCoords2.m_data[texCoordBase + 1].y = sine * 0.5f + 0.5f;
    s_blipTexCoords2.m_data[texCoordBase + 1].x = cosine * 0.5f + 0.5f;
    s_blipVertices.m_data[vertexBase + 1].x = cosine * radius + position.x;
    s_blipVertices.m_data[vertexBase + 1].y = sine * radius + position.y;
    s_blipVertices.m_data[vertexBase + 1].z = 0.0f;
    GetSquareDirection(to, &sine, &cosine);
    s_blipTexCoords2.m_data[texCoordBase + 2].y = sine * 0.5f + 0.5f;
    s_blipTexCoords2.m_data[texCoordBase + 2].x = cosine * 0.5f + 0.5f;
    s_blipVertices.m_data[vertexBase + 2].x = cosine * radius + position.x;
    s_blipVertices.m_data[vertexBase + 2].y = sine * radius + position.y;
    s_blipVertices.m_data[vertexBase + 2].z = 0.0f;
    s_blipIndices.SetCount(indexBase + 3);
    s_blipIndices.m_data[indexBase] = static_cast<uint16_t>(indexBase);
    s_blipIndices.m_data[indexBase + 1] = static_cast<uint16_t>(indexBase + 1);
    s_blipIndices.m_data[indexBase + 2] = static_cast<uint16_t>(indexBase + 2);
}

// OFFSET: 0x009D4F10/0x140766F40
bool CGMinimapFrame::BuildBlobRings(MINIMAPBLOB* blob) {
    blob->m_rings.SetCount(0);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    C3Vector position = player->GetPosition();
    position.z = 0.0f;
    C3Vector first(0.0f, 0.0f, 0.0f);
    float radius = static_cast<float>(MinimapGetViewRadius() * s_blobRingRadiusScale);
    uint32_t count = blob->m_points.m_count;
    bool allOutside = count != 0;
    bool allInside = count != 0;
    bool reverseSet = false;
    bool complete = false;
    MinimapBlobRing* ring = NULL;
    uint32_t start = 0;
    uint32_t quadrants = 0;

    for (uint32_t i = 0; i < count; i++) {
        BLOBPOI* nextPoint = &blob->m_points.m_data[(i + 1) % count];
        float nextX = static_cast<float>(nextPoint->m_x) - position.x;
        float nextY = static_cast<float>(nextPoint->m_y) - position.y;
        float currentX = static_cast<float>(blob->m_points.m_data[i].m_x) - position.x;
        float currentY = static_cast<float>(blob->m_points.m_data[i].m_y) - position.y;
        float nextDistance = std::sqrt(nextX * nextX + nextY * nextY);
        bool currentInside = radius > std::sqrt(currentX * currentX + currentY * currentY);
        bool nextInside = radius > nextDistance;

        if (currentInside || nextInside) {
            allOutside = false;
        }

        if (!currentInside || !nextInside) {
            allInside = false;
        }

        if (currentInside && !nextInside) {
            start = i;
            break;
        }
    }

    if (allInside) {
        return false;
    }

    if (allOutside) {
        TSGrowableArray<C3Vector> vertices;
        vertices.SetCount(count);

        for (uint32_t i = 0; i < blob->m_points.m_count; i++) {
            vertices.m_data[i].x = static_cast<float>(blob->m_points.m_data[i].m_x);
            vertices.m_data[i].y = static_cast<float>(blob->m_points.m_data[i].m_y);
            vertices.m_data[i].z = 0.0f;
        }

        if (NTempest::Intersect(position, vertices.m_data, vertices.m_count, C3Vector::AXIS_Z)) {
            blob->m_rings.New();
        }

        return blob->m_rings.m_count != 0;
    }

    for (uint32_t i = start; i < start + blob->m_points.m_count; i++) {
        BLOBPOI* currentPoint = &blob->m_points.m_data[i % blob->m_points.m_count];
        C3Vector current(static_cast<float>(currentPoint->m_x) - position.x, static_cast<float>(currentPoint->m_y) - position.y, -position.z);
        float currentDistance = std::sqrt(current.x * current.x + current.y * current.y);
        BLOBPOI* nextPoint = &blob->m_points.m_data[(i + 1) % blob->m_points.m_count];
        C3Vector next(static_cast<float>(nextPoint->m_x) - position.x, static_cast<float>(nextPoint->m_y) - position.y, -position.z);
        float nextDistance = std::sqrt(next.x * next.x + next.y * next.y);
        bool currentInside = radius > currentDistance;
        bool nextInside = radius > nextDistance;

        if (ring && !reverseSet) {
            ring->m_reverse = next.x * first.x + next.z * first.z + next.y * first.y > 0.0f;
            reverseSet = true;
        }

        if (currentInside != nextInside) {
            float t = (radius - currentDistance) / (nextDistance - currentDistance);
            C3Vector cross;
            cross.z = current.z + (next.z - current.z) * t;
            cross.x = (next.x - current.x) * t + current.x;
            cross.y = current.y + (next.y - current.y) * t;
            bool closing = false;
            C2Vector direction(cross.x, cross.y);
            float lengthSqr = cross.x * cross.x + cross.y * cross.y;

            if (lengthSqr > 0.00000023841858f) {
                float inverse = 1.0f / std::sqrt(lengthSqr);
                direction.x = cross.x * inverse;
                direction.y = inverse * cross.y;
            }

            float angle = direction.Angle(1.0f);

            if (angle < 0.0f) {
                angle += 6.2831855f;
            }

            if (ring) {
                closing = true;
            } else {
                ring = blob->m_rings.New();
                reverseSet = false;
                first = cross;
                complete = false;
                quadrants = 1 << GetAngleQuadrant(angle);
            }

            if (nextInside) {
                ring->m_start = angle;
            } else {
                ring->m_end = angle;
            }

            if (closing) {
                ring = NULL;
            }
        } else if (ring && !currentInside && !nextInside && !complete) {
            quadrants |= 1 << GetAngleQuadrant(GetDirectionAngle(current));

            if (quadrants == 15) {
                complete = true;
            }
        }
    }

    if (complete && blob->m_rings.m_count == 1) {
        uint32_t startQuadrant = GetAngleQuadrant(blob->m_rings.m_data[0].m_start);

        if (startQuadrant == GetAngleQuadrant(blob->m_rings.m_data[0].m_end)) {
            float middle = s_quadrantAngles[(static_cast<int32_t>(startQuadrant + 2) % 4 + 3) % 4];
            MinimapBlobRing* second = blob->m_rings.New();
            second->m_reverse = blob->m_rings.m_data[0].m_reverse;
            second->m_start = middle;
            second->m_end = blob->m_rings.m_data[0].m_end;
            blob->m_rings.m_data[0].m_end = middle;
        }
    }

    return blob->m_rings.m_count != 0;
}

// OFFSET: 0x009D55E0
static bool IsPlayerInQuestBlob(int32_t questId, uint32_t blobIndex) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    C3Vector position = player->GetPosition();
    BLOB* blob = CGQuestLog::GetQuestPOIBlob(questId, blobIndex);

    if (!blob || blob->m_points.m_count < 3) {
        return false;
    }

    TSGrowableArray<C3Vector> vertices;
    vertices.SetCount(blob->m_points.m_count);

    for (uint32_t i = 0; i < blob->m_points.m_count; i++) {
        vertices.m_data[i].x = static_cast<float>(blob->m_points.m_data[i].m_x);
        vertices.m_data[i].y = static_cast<float>(blob->m_points.m_data[i].m_y);
        vertices.m_data[i].z = position.z;
    }

    return NTempest::Intersect(position, vertices.m_data, vertices.m_count, C3Vector::AXIS_Z);
}

// OFFSET: 0x009D56F0
void CGMinimapFrame::RenderPartyRaidBlips(const C44Matrix& rotation) {
    if (!s_classBlipTexture) {
        return;
    }

    for (uint32_t list = 0; list < 2; list++) {
        for (uint32_t i = 0; i < s_objInfo[list].m_count; i++) {
            OBJINFO* info = &s_objInfo[list].m_data[i];

            if (info->m_otherFloor) {
                continue;
            }

            if (info->m_marker != -1) {
                s_markedBlips[info->m_marker].m_list = list;
                s_markedBlips[info->m_marker].m_index = i;
                continue;
            }

            C3Vector point(info->m_position.x, info->m_position.y, 0.0f);

            if (g_rotateMinimapCVar->m_intValue) {
                point = C3Vector(info->m_rotated.x, info->m_rotated.y, 0.0f);
                point *= rotation;
                info->m_position.x = point.x;
                info->m_position.y = point.y;
            }

            uint16_t base = static_cast<uint16_t>(s_blipVertices.m_count);
            C3Vector vertices[4];

            for (uint32_t j = 0; j < 4; j++) {
                vertices[j].x = this->m_layoutScale * CGMinimapFrame::s_blipQuads[0][j].x + point.x;
                vertices[j].y = this->m_layoutScale * CGMinimapFrame::s_blipQuads[0][j].y + point.y;
                vertices[j].z = 0.0f;
            }

            s_blipVertices.Add(4, vertices);
            uint16_t indices[6];
            indices[2] = base + 2;
            indices[3] = base + 2;
            indices[0] = base;
            indices[1] = base + 1;
            indices[4] = base + 1;
            indices[5] = base + 3;
            s_blipIndices.Add(6, indices);

            int32_t icon;

            if (s_objInfo[list].m_data[i].m_icon) {
                icon = s_objInfo[list].m_data[i].m_icon - 1;

                if (!list) {
                    icon += 16;
                }
            } else {
                icon = 30;
            }

            C2Vector texCoords[4];

            for (uint32_t j = 0; j < 4; j++) {
                texCoords[j] = s_classBlipTexCoords[icon][j];
            }

            s_blipTexCoords2.Add(4, texCoords);
        }
    }

    if (!s_blipVertices.m_count || !s_blipIndices.m_count) {
        return;
    }

    CGxTex* gxTex = TextureGetGxTex(s_classBlipTexture);

    if (!gxTex) {
        return;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    static CImVector s_color(0xFF, 0xFF, 0xFF, 0xFF);
    static C3Vector s_normal(0.0f, 0.0f, 1.0f);
    GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &s_normal, 0, &s_color, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), NULL, 0);
    GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    GxDrawLockedElements();
    GxPrimUnlockVertexPtrs();
    GxPrimUnlockIndexPtr();
    s_blipVertices.SetCount(0);
    s_blipTexCoords2.SetCount(0);
    s_blipIndices.SetCount(0);
}

// OFFSET: 0x009D5BA0
void CGMinimapFrame::RenderObjectBlips(const C44Matrix& rotation, bool otherFloor) {
    if (!s_blipTexture) {
        return;
    }

    for (uint32_t order = 0; order < sizeof(s_objectBlipOrder) / sizeof(s_objectBlipOrder[0]); order++) {
        int32_t list = s_objectBlipOrder[order];

        for (uint32_t i = 0; i < s_objInfo[list].m_count; i++) {
            OBJINFO* info = &s_objInfo[list].m_data[i];

            if (info->m_otherFloor != static_cast<uint8_t>(otherFloor)) {
                continue;
            }

            if (info->m_marker != -1) {
                s_markedBlips[info->m_marker].m_list = list;
                s_markedBlips[info->m_marker].m_index = i;
                continue;
            }

            C3Vector point(info->m_position.x, info->m_position.y, 0.0f);

            if (g_rotateMinimapCVar->m_intValue) {
                point = C3Vector(info->m_rotated.x, info->m_rotated.y, 0.0f);
                point *= rotation;
                info->m_position.x = point.x;
                info->m_position.y = point.y;
            }

            uint16_t base = static_cast<uint16_t>(s_blipVertices.m_count);
            C3Vector vertices[4];

            for (uint32_t j = 0; j < 4; j++) {
                vertices[j].x = CGMinimapFrame::s_blipQuads[0][j].x * this->m_layoutScale + point.x;
                vertices[j].y = this->m_layoutScale * CGMinimapFrame::s_blipQuads[0][j].y + point.y;
                vertices[j].z = 0.0f;
            }

            s_blipVertices.Add(4, vertices);
            uint16_t indices[6];
            indices[2] = base + 2;
            indices[3] = base + 2;
            indices[0] = base;
            indices[1] = base + 1;
            indices[4] = base + 1;
            indices[5] = base + 3;
            s_blipIndices.Add(6, indices);

            C2Vector texCoords[4];

            for (uint32_t j = 0; j < 4; j++) {
                texCoords[j] = s_blipTexCoords[list][j];
            }

            s_blipTexCoords2.Add(4, texCoords);
        }
    }

    if (!s_blipVertices.m_count) {
        return;
    }

    CGxTex* gxTex = TextureGetGxTex(s_blipTexture);

    if (!gxTex) {
        return;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    static CImVector s_color(0xFF, 0xFF, 0xFF, 0xFF);
    static CImVector s_otherFloorColor(0xB0, 0xB0, 0xB0, 0xFF);
    static C3Vector s_normal(0.0f, 0.0f, 1.0f);
    GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &s_normal, 0, otherFloor ? &s_otherFloorColor : &s_color, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), NULL, 0);
    GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    GxDrawLockedElements();
    GxPrimUnlockVertexPtrs();
    GxPrimUnlockIndexPtr();
    s_blipVertices.SetCount(0);
    s_blipTexCoords2.SetCount(0);
    s_blipIndices.m_count = 0;
}

// OFFSET: 0x009D6000
void CGMinimapFrame::RenderVehicleBlips(const C44Matrix& rotation) {
    for (uint32_t list = 33; list < 41; list++) {
        CTexture* texture = s_vehicleTextures[list - 33];

        if (!s_objInfo[list].m_count || !texture) {
            continue;
        }

        for (uint32_t i = 0; i < s_objInfo[list].m_count; i++) {
            OBJINFO* info = &s_objInfo[list].m_data[i];

            if (info->m_otherFloor) {
                continue;
            }

            C3Vector point(info->m_position.x, info->m_position.y, 0.0f);

            if (g_rotateMinimapCVar->m_intValue) {
                point = C3Vector(info->m_rotated.x, info->m_rotated.y, 0.0f);
                point *= rotation;
                info->m_position.x = point.x;
                info->m_position.y = point.y;
            }

            uint16_t base = static_cast<uint16_t>(s_blipVertices.m_count);
            C3Vector vertices[4];

            for (uint32_t j = 0; j < 4; j++) {
                vertices[j].x = CGMinimapFrame::s_blipQuads[1][j].x * this->m_layoutScale + point.x;
                vertices[j].y = this->m_layoutScale * CGMinimapFrame::s_blipQuads[1][j].y + point.y;
                vertices[j].z = 0.0f;
            }

            s_blipVertices.Add(4, vertices);
            uint16_t indices[6];
            indices[2] = base + 2;
            indices[3] = base + 2;
            indices[0] = base;
            indices[1] = base + 1;
            indices[4] = base + 1;
            indices[5] = base + 3;
            s_blipIndices.Add(6, indices);

            float facing = s_objInfo[list].m_data[i].m_facing - s_arrowRotation;
            float cosine;
            float sine;
            CMath::fsincos_(facing, &sine, &cosine);
            C2Vector texCoords[4];
            texCoords[0].x = sine + 0.5f;
            texCoords[0].y = 0.5f - cosine;
            texCoords[1].x = 0.5f - cosine;
            texCoords[1].y = 0.5f - sine;
            texCoords[2].x = cosine + 0.5f;
            texCoords[2].y = sine + 0.5f;
            texCoords[3].x = 0.5f - sine;
            texCoords[3].y = cosine + 0.5f;
            s_blipTexCoords2.Add(4, texCoords);
        }

        if (!s_blipVertices.m_count) {
            return;
        }

        CGxTex* gxTex = TextureGetGxTex(texture);

        if (!gxTex) {
            return;
        }

        g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
        static CImVector s_color(0xFF, 0xFF, 0xFF, 0xFF);
        static C3Vector s_normal(0.0f, 0.0f, 1.0f);
        GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &s_normal, 0, &s_color, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), NULL, 0);
        GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
        DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
        GxDrawLockedElements();
        GxPrimUnlockVertexPtrs();
        GxPrimUnlockIndexPtr();
        s_blipVertices.SetCount(0);
        s_blipTexCoords2.SetCount(0);
        s_blipIndices.m_count = 0;
    }
}

// OFFSET: 0x009D6420
void CGMinimapFrame::RenderPartyArrows(const C44Matrix& rotation) {
    static C3Vector s_normal(0.0f, 0.0f, 1.0f);

    if (!s_groupArrowTexture) {
        return;
    }

    MinimapGetPartyMembers(s_partyMembers);

    for (uint32_t i = 0; i < 5; i++) {
        if (!s_partyMembers[i].m_offMap) {
            continue;
        }

        float facing = s_partyMembers[i].m_facing;
        float cosine;
        float sine;
        CMath::fsincos_(facing, &sine, &cosine);
        C3Vector point;
        point.x = this->GetWidth() * this->m_layoutScale * 0.5f + this->m_layoutScale * CGMinimapFrame::s_blipSize4 * sine * -0.75f;
        point.y = this->GetHeight() * this->m_layoutScale * 0.5f + this->m_layoutScale * CGMinimapFrame::s_blipSize4 * cosine * 0.75f;
        point.z = 0.0f;

        if (g_rotateMinimapCVar->m_intValue) {
            facing = facing - this->m_facing;
            point *= rotation;
        }

        s_partyArrowPositions[i].x = point.x;
        s_partyArrowPositions[i].y = point.y;
        uint16_t base = static_cast<uint16_t>(s_blipVertices.m_count);
        C3Vector vertices[4];

        for (uint32_t j = 0; j < 4; j++) {
            vertices[j].x = CGMinimapFrame::s_blipQuads[2][j].x * this->m_layoutScale + point.x;
            vertices[j].y = CGMinimapFrame::s_blipQuads[2][j].y * this->m_layoutScale + point.y;
            vertices[j].z = CGMinimapFrame::s_blipQuads[2][j].z * this->m_layoutScale + point.z;
        }

        s_blipVertices.Add(4, vertices);
        uint16_t indices[6];
        indices[2] = base + 2;
        indices[3] = base + 2;
        indices[0] = base;
        indices[1] = base + 1;
        indices[4] = base + 1;
        indices[5] = base + 3;
        s_blipIndices.Add(6, indices);

        facing = facing - s_arrowRotation;
        CMath::fsincos_(facing, &sine, &cosine);
        C2Vector texCoords[4];
        texCoords[0].x = sine + 0.5f;
        texCoords[0].y = 0.5f - cosine;
        texCoords[1].x = 0.5f - cosine;
        texCoords[1].y = 0.5f - sine;
        texCoords[2].x = cosine + 0.5f;
        texCoords[2].y = sine + 0.5f;
        texCoords[3].x = 0.5f - sine;
        texCoords[3].y = cosine + 0.5f;
        s_blipTexCoords2.Add(4, texCoords);
    }

    if (!s_blipVertices.m_count) {
        return;
    }

    CGxTex* gxTex = TextureGetGxTex(s_groupArrowTexture);

    if (!gxTex) {
        return;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    static CImVector s_color(0xFF, 0xFF, 0xFF, 0xFF);
    GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &s_normal, 0, &s_color, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), NULL, 0);
    GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    GxDrawLockedElements();
    GxPrimUnlockVertexPtrs();
    GxPrimUnlockIndexPtr();
    s_blipVertices.SetCount(0);
    s_blipTexCoords2.SetCount(0);
    s_blipIndices.m_count = 0;
}

// OFFSET: 0x009D68E0
void CGMinimapFrame::RenderRaidArrows(const C44Matrix& rotation) {
    if (!s_groupArrowTexture) {
        return;
    }

    MinimapGetRaidMembers(s_raidMembers);

    for (uint32_t i = 0; i < 40; i++) {
        if (!s_raidMembers[i].m_offMap) {
            continue;
        }

        float facing = s_raidMembers[i].m_facing;
        float cosine;
        float sine;
        CMath::fsincos_(facing, &sine, &cosine);
        C3Vector point;
        point.x = this->GetWidth() * this->m_layoutScale * 0.5f + this->m_layoutScale * CGMinimapFrame::s_blipSize4 * sine * -0.75f;
        point.y = this->GetHeight() * this->m_layoutScale * 0.5f + this->m_layoutScale * CGMinimapFrame::s_blipSize4 * cosine * 0.75f;
        point.z = 0.0f;

        if (g_rotateMinimapCVar->m_intValue) {
            facing = facing - this->m_facing;
            point *= rotation;
        }

        s_raidArrowPositions[i].x = point.x;
        s_raidArrowPositions[i].y = point.y;
        uint16_t base = static_cast<uint16_t>(s_blipVertices.m_count);
        C3Vector vertices[4];

        for (uint32_t j = 0; j < 4; j++) {
            vertices[j].x = CGMinimapFrame::s_blipQuads[2][j].x * this->m_layoutScale + point.x;
            vertices[j].y = CGMinimapFrame::s_blipQuads[2][j].y * this->m_layoutScale + point.y;
            vertices[j].z = CGMinimapFrame::s_blipQuads[2][j].z * this->m_layoutScale + point.z;
        }

        s_blipVertices.Add(4, vertices);
        uint16_t indices[6];
        indices[2] = base + 2;
        indices[3] = base + 2;
        indices[0] = base;
        indices[1] = base + 1;
        indices[4] = base + 1;
        indices[5] = base + 3;
        s_blipIndices.Add(6, indices);

        facing = facing - s_arrowRotation;
        CMath::fsincos_(facing, &sine, &cosine);
        C2Vector texCoords[4];
        texCoords[0].x = sine + 0.5f;
        texCoords[0].y = 0.5f - cosine;
        texCoords[1].x = 0.5f - cosine;
        texCoords[1].y = 0.5f - sine;
        texCoords[2].x = cosine + 0.5f;
        texCoords[2].y = sine + 0.5f;
        texCoords[3].x = 0.5f - sine;
        texCoords[3].y = cosine + 0.5f;
        s_blipTexCoords2.Add(4, texCoords);
    }

    if (!s_blipVertices.m_count) {
        return;
    }

    CGxTex* gxTex = TextureGetGxTex(s_groupArrowTexture);

    if (!gxTex) {
        return;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    static C3Vector s_normal(0.0f, 0.0f, 1.0f);
    static CImVector s_color(0xFF, 0xFF, 0xFF, 0xFF);
    GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &s_normal, 0, &s_color, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), NULL, 0);
    GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    GxDrawLockedElements();
    GxPrimUnlockVertexPtrs();
    GxPrimUnlockIndexPtr();
    s_blipVertices.SetCount(0);
    s_blipTexCoords2.SetCount(0);
    s_blipIndices.m_count = 0;
}

// OFFSET: 0x009D6D90
void CGMinimapFrame::RenderPings(const C44Matrix& rotation) {
    if (!s_pingTime || !s_pingCenterTexture || !s_pingRotateTexture || !s_pingExpandTexture) {
        return;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    C2Vector playerPosition(player->GetPosition());
    float dx = CGMinimapFrame::s_pingPosition.x - playerPosition.x;
    float dy = CGMinimapFrame::s_pingPosition.y - playerPosition.y;
    float radius = static_cast<float>(MinimapGetViewRadius());
    float limit = radius * radius * 0.9f;
    float distSqr = dx * dx + dy * dy;
    float fade = 1.0f;

    if (distSqr > limit * 0.8f) {
        if (distSqr > limit) {
            return;
        }

        fade = 1.0f - (distSqr - limit * 0.8f) / (limit - limit * 0.8f);
    }

    uint32_t elapsed = FrameTime::GetCurTimeMs() - s_pingLastTime;
    s_pingLastTime = FrameTime::GetCurTimeMs();
    uint32_t remaining = elapsed >= s_pingTime ? 0 : s_pingTime - elapsed;
    s_pingTime = remaining;
    float alpha = 1.0f;

    if (remaining < 500) {
        alpha = static_cast<float>(remaining * 0.002f);
    }

    if (alpha > fade) {
        alpha = fade;
    }

    float spin = static_cast<float>(1.0 - (remaining % 1800) * 0.00055555557f);
    C3Vector origin = player->GetPosition();
    float viewRadius = static_cast<float>(MinimapGetViewRadius());
    float half = this->m_layoutScale * this->GetWidth() * 0.5f;
    C3Vector point;
    point.y = half * (CGMinimapFrame::s_pingPosition.x - origin.x) * (1.0f / viewRadius) + half;
    point.x = half - half * (CGMinimapFrame::s_pingPosition.y - origin.y) * (1.0f / viewRadius);
    point.z = 0.0f;

    if (g_rotateMinimapCVar->m_intValue) {
        point *= rotation;
    }

    uint16_t indices[6];
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 1;
    indices[5] = 3;
    static C3Vector s_normal(0.0f, 0.0f, 1.0f);
    C3Vector vertices[4];
    C2Vector texCoords[4];

    for (uint32_t j = 0; j < 4; j++) {
        vertices[j].x = CGMinimapFrame::s_blipQuads[0][j].x * this->m_layoutScale + point.x;
        vertices[j].y = CGMinimapFrame::s_blipQuads[0][j].y * this->m_layoutScale + point.y;
        vertices[j].z = 0.1f;
    }

    s_blipVertices.Add(4, vertices);
    s_blipIndices.Add(6, indices);
    CImVector color(0xFF, 0xFF, 0xFF, static_cast<uint8_t>(static_cast<int32_t>(alpha * 255.0f)));
    texCoords[0].x = 0.0f;
    texCoords[1].x = 1.0f;
    texCoords[0].y = 1.0f;
    texCoords[1].y = 1.0f;
    texCoords[2].y = 0.0f;
    texCoords[2].x = 0.0f;
    texCoords[3].x = 1.0f;
    texCoords[3].y = 0.0f;
    s_blipTexCoords2.Add(4, texCoords);
    CGxTex* gxTex = TextureGetGxTex(s_pingCenterTexture);

    if (!gxTex) {
        return;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &s_normal, 0, &color, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), NULL, 0);
    GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    GxDrawLockedElements();
    GxPrimUnlockVertexPtrs();
    GxPrimUnlockIndexPtr();
    s_blipVertices.SetCount(0);
    s_blipTexCoords2.SetCount(0);
    s_blipIndices.SetCount(0);

    for (uint32_t j = 0; j < 4; j++) {
        vertices[j].x = CGMinimapFrame::s_blipQuads[0][j].x * this->m_layoutScale * 1.5f + point.x;
        vertices[j].y = CGMinimapFrame::s_blipQuads[0][j].y * this->m_layoutScale * 1.5f + point.y;
        vertices[j].z = 0.0f;
    }

    s_blipVertices.Add(4, vertices);
    s_blipIndices.Add(6, indices);
    float angle = spin * 6.2831855f;
    float sine = std::sin(angle);
    float cosine = std::cos(angle);
    texCoords[0].x = sine * 0.5f - cosine * 0.5f + 0.5f;
    texCoords[0].y = cosine * 0.5f + sine * 0.5f + 0.5f;
    texCoords[1].x = texCoords[0].y;
    texCoords[1].y = cosine * 0.5f - sine * 0.5f + 0.5f;
    texCoords[2].y = texCoords[0].x;
    texCoords[2].x = sine * -0.5f - cosine * 0.5f + 0.5f;
    texCoords[3].x = texCoords[1].y;
    texCoords[3].y = texCoords[2].x;
    s_blipTexCoords2.Add(4, texCoords);
    gxTex = TextureGetGxTex(s_pingRotateTexture);

    if (!gxTex) {
        return;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &s_normal, 0, &color, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), NULL, 0);
    GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
    GxDrawLockedElements();
    GxPrimUnlockVertexPtrs();
    GxPrimUnlockIndexPtr();
    s_blipVertices.SetCount(0);
    s_blipTexCoords2.SetCount(0);
    s_blipIndices.SetCount(0);
    float expand = static_cast<float>((1.0 - (s_pingTime % 800) * 0.00125f) * 1.5 + 0.2);

    for (uint32_t j = 0; j < 4; j++) {
        vertices[j].x = CGMinimapFrame::s_blipQuads[0][j].x * this->m_layoutScale * expand + point.x;
        vertices[j].y = CGMinimapFrame::s_blipQuads[0][j].y * this->m_layoutScale * expand + point.y;
        vertices[j].z = 0.0f;
    }

    s_blipVertices.Add(4, vertices);
    s_blipIndices.Add(6, indices);
    texCoords[0].x = 0.0f;
    texCoords[1].x = 1.0f;
    texCoords[0].y = 1.0f;
    texCoords[1].y = 1.0f;
    texCoords[2].y = 0.0f;
    texCoords[2].x = 0.0f;
    texCoords[3].x = 1.0f;
    texCoords[3].y = 0.0f;
    s_blipTexCoords2.Add(4, texCoords);
    gxTex = TextureGetGxTex(s_pingExpandTexture);

    if (!gxTex) {
        return;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &s_normal, 0, &color, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), NULL, 0);
    GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
    GxDrawLockedElements();
    GxPrimUnlockVertexPtrs();
    GxPrimUnlockIndexPtr();
    s_blipVertices.SetCount(0);
    s_blipTexCoords2.SetCount(0);
    s_blipIndices.SetCount(0);
}

// OFFSET: 0x009D78E0
void CGMinimapFrame::RenderQuestCompletionNPC(const C44Matrix& rotation, C3Vector position) {
    static C3Vector s_normal(0.0f, 0.0f, 1.0f);
    CImVector color(0xFF, 0xFF, 0xFF, 0xFF);
    float radius = static_cast<float>(MinimapGetViewRadius());
    float limit = radius * radius * 0.8f;

    for (uint32_t i = 0; i < CGQuestLog::m_numQuest; i++) {
        if (!CGQuestLog::IsQuestComplete(i) && !CGQuestLog::HasNoObjectives(i)) {
            continue;
        }

        int32_t questId = CGQuestLog::GetQuestID(i);
        int32_t numNPC = CGQuestLog::GetQuestNumCompletionNPC(questId);
        bool show = true;

        for (int32_t j = 0; j < numNPC; j++) {
            int32_t entry = CGQuestLog::GetQuestCompletionNPC(questId, j);

            if (entry && (IsCreatureInBlipList(10, entry) || IsCreatureInBlipList(12, entry))) {
                show = false;
            }
        }

        if (!show) {
            continue;
        }

        C2Vector loc(0.0f, 0.0f);
        float distSqr = CGQuestLog::GetQuestDistSqr(questId, loc);

        if (!(limit > distSqr)) {
            continue;
        }

        float half = this->m_layoutScale * this->GetWidth() * 0.5f;
        C3Vector point;
        point.x = half - (1.0f / radius) * (half * (loc.y - position.y));
        point.y = half + (1.0f / radius) * (half * (loc.x - position.x));
        point.z = 0.0f;

        if (g_rotateMinimapCVar->m_intValue) {
            point *= rotation;
        }

        uint16_t base = static_cast<uint16_t>(s_blipVertices.m_count);
        C3Vector vertices[4];

        for (uint32_t j = 0; j < 4; j++) {
            vertices[j].x = CGMinimapFrame::s_blipQuads[0][j].x * this->m_layoutScale + point.x;
            vertices[j].y = this->m_layoutScale * CGMinimapFrame::s_blipQuads[0][j].y + point.y;
            vertices[j].z = 0.0f;
        }

        s_blipVertices.Add(4, vertices);
        uint16_t indices[6];
        indices[2] = base + 2;
        indices[3] = base + 2;
        indices[0] = base;
        indices[1] = base + 1;
        indices[4] = base + 1;
        indices[5] = base + 3;
        s_blipIndices.Add(6, indices);
        C2Vector texCoords[4];

        for (uint32_t j = 0; j < 4; j++) {
            texCoords[j] = s_blipTexCoords[10][j];
        }

        s_blipTexCoords2.Add(4, texCoords);
    }

    if (!s_blipTexture) {
        return;
    }

    CGxTex* gxTex = TextureGetGxTex(s_blipTexture);

    if (!gxTex || !s_blipVertices.m_count) {
        return;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &s_normal, 0, &color, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), NULL, 0);
    GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    GxDrawLockedElements();
    GxPrimUnlockVertexPtrs();
    GxPrimUnlockIndexPtr();
    s_blipVertices.SetCount(0);
    s_blipTexCoords2.SetCount(0);
    s_blipIndices.m_count = 0;
}

// OFFSET: 0x009D7DA0
void CGMinimapFrame::RenderHeightArrow(const C2Vector& position, float height, const C44Matrix& rotation) {
    if (!s_positionArrowsTexture || !(s_heightArrowThreshold < std::fabs(height))) {
        return;
    }

    C3Vector point(position.x, position.y, 0.0f);

    if (g_rotateMinimapCVar->m_intValue) {
        point *= rotation;
    }

    float offset = CGMinimapFrame::s_blipHalfSize1 * 0.5f;
    uint16_t base = static_cast<uint16_t>(s_blipVertices.m_count);
    C3Vector vertices[4];

    for (uint32_t j = 0; j < 4; j++) {
        vertices[j].x = CGMinimapFrame::s_blipQuads[0][j].x * this->m_layoutScale + offset + point.x;
        vertices[j].y = this->m_layoutScale * CGMinimapFrame::s_blipQuads[0][j].y + offset + point.y;
        vertices[j].z = 0.0f;
    }

    s_blipVertices.Add(4, vertices);
    uint16_t indices[6];
    indices[2] = base + 2;
    indices[3] = base + 2;
    indices[0] = base;
    indices[1] = base + 1;
    indices[4] = base + 1;
    indices[5] = base + 3;
    s_blipIndices.Add(6, indices);

    float top = height < 0.0f ? 0.5f : 0.0f;
    C2Vector texCoords[4];
    texCoords[0].x = 0.0f;
    texCoords[1].x = 1.0f;
    texCoords[0].y = top + 0.5f;
    texCoords[2].y = top;
    texCoords[1].y = top + 0.5f;
    texCoords[2].x = 0.0f;
    texCoords[3].x = 1.0f;
    texCoords[3].y = top;
    s_blipTexCoords2.Add(4, texCoords);

    CGxTex* gxTex = TextureGetGxTex(s_positionArrowsTexture);

    if (!gxTex) {
        return;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    static CImVector s_color(0xFF, 0xFF, 0xFF, 0xFF);
    static C3Vector s_normal(0.0f, 0.0f, 1.0f);
    GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &s_normal, 0, &s_color, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), NULL, 0);
    GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    GxDrawLockedElements();
    GxPrimUnlockVertexPtrs();
    GxPrimUnlockIndexPtr();
    s_blipVertices.SetCount(0);
    s_blipTexCoords2.SetCount(0);
    s_blipIndices.m_count = 0;
}

// OFFSET: 0x009D80E0/0x14076BF30
void CGMinimapFrame::RenderInsideTexture(const CImVector* color) {
    C44Matrix savedProjection;
    savedProjection = g_theGxDevicePtr->m_projection;
    C44Matrix ortho;
    float extent = s_indoor.m_radius * 1.5f;
    GxuXformCreateOrtho(-extent, extent, -extent, extent, -2500.0f, 2500.0f, ortho);
    g_theGxDevicePtr->XformSetProjection(ortho);

    C44Matrix savedView;
    CGxMatrixStack& viewStack = g_theGxDevicePtr->m_xforms[GxXform_View];
    savedView = viewStack.m_mtx[viewStack.m_level];
    C44Matrix identity;
    g_theGxDevicePtr->XformSetView(identity);

    g_theGxDevicePtr->RsPush();
    g_theGxDevicePtr->RsInit();
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_ColorWrite, 7);
    GxRsSet(GxRs_BlendingMode, 1);
    GxRsSet(GxRs_AlphaRef, 254);
    DefaultShaders::SetFogEnabled(0);

    g_theGxDevicePtr->XformPush(GxXform_World);
    CGxMatrixStack& world = g_theGxDevicePtr->m_xforms[GxXform_World];
    world.Identity();
    world.m_dirty = 1;
    world.m_flags[world.m_level] &= ~1u;
    world.m_mtx[world.m_level] *= s_indoor.m_matrix;
    C3Vector center = s_indoor.m_center * s_indoor.m_invMatrix;
    GxXformTranslate(GxXform_World, C3Vector(-center.x, -center.y, -center.z));

    MINIMAPTILE* head = NULL;
    CGMinimapFrame::RenderInsideSortQuads(&head);

    for (MINIMAPTILE* tile = head; tile; tile = tile->m_nextSorted) {
        CGMinimapFrame::RenderInsideQuad(tile, color);
    }

    GxXformPop(GxXform_World);
    g_theGxDevicePtr->XformSetView(savedView);
    g_theGxDevicePtr->XformSetProjection(savedProjection);
    CShaderEffect::UpdateProjMatrix();
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x009D8740
void CGMinimapFrame::RenderInside(float scale, const C2Vector* center, const CImVector* color) {
    GxRsSet(GxRs_VertexShader, static_cast<void*>(NULL));
    GxRsSet(GxRs_PixelShader, static_cast<void*>(NULL));
    C44Matrix savedProjection;
    savedProjection = g_theGxDevicePtr->m_projection;
    C44Matrix ortho;
    float half = scale * 0.5f;
    GxuXformCreateOrtho(-half, half, -half, half, -1000.0f, 1000.0f, ortho);
    g_theGxDevicePtr->XformSetProjection(ortho);
    g_theGxDevicePtr->RsPush();
    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(s_indoor.m_texture));
    GxRsSet(GxRs_ColorOp0, 0);
    GxRsSet(GxRs_AlphaOp0, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTexBlocking(s_maskTexture));
    GxRsSet(GxRs_ColorOp1, 0);
    GxRsSet(GxRs_AlphaOp1, 0);
    GxRsSet(GxRs_TextureShader0, 1);
    g_theGxDevicePtr->XformPush(GxXform_Tex0);
    g_theGxDevicePtr->XformPush(GxXform_Tex1);
    g_theGxDevicePtr->XformPush(GxXform_World);
    GxXformScale(GxXform_World, C3Vector(half, half, half));

    C44Matrix texMatrix;
    texMatrix.Translate(C3Vector(0.0f, 1.0f, 0.0f));
    texMatrix.Scale(C3Vector(1.0f, -1.0f, 1.0f));
    texMatrix.Translate(C3Vector(center->x, center->y, 0.0f));
    texMatrix.Scale(0.33333334f);

    if (g_rotateMinimapCVar->m_intValue) {
        texMatrix.RotateAroundZ(this->m_facing);
    }

    GxXformSet(GxXform_Tex0, texMatrix);
    GxRsSet(GxRs_TextureShader1, 1);
    C44Matrix maskMatrix;
    maskMatrix.Translate(C3Vector(0.0f, 1.0f, 0.0f));
    maskMatrix.Scale(C3Vector(1.0f, -1.0f, 1.0f));
    maskMatrix.Translate(C3Vector(0.5f, 0.5f, 0.0f));
    maskMatrix.Scale(C3Vector(0.5f, 0.5f, 0.5f));
    GxXformSet(GxXform_Tex1, maskMatrix);

    static C3Vector s_positions[4] = { C3Vector(-1.0f, -1.0f, 0.0f), C3Vector(-1.0f, 1.0f, 0.0f), C3Vector(1.0f, 1.0f, 0.0f), C3Vector(1.0f, -1.0f, 0.0f) };
    static C2Vector s_texCoords[4] = { C2Vector(-1.0f, -1.0f), C2Vector(-1.0f, 1.0f), C2Vector(1.0f, 1.0f), C2Vector(1.0f, -1.0f) };

    C3Vector normal(0.0f, 0.0f, 1.0f);
    GxPrimLockVertexPtrs(4, s_positions, sizeof(C3Vector), &normal, 0, color, 0, NULL, 0, s_texCoords, sizeof(C2Vector), s_texCoords, sizeof(C2Vector));
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
    GxDrawLockedElements(GxPrim_TriangleStrip, 4, s_insideIndices);
    GxPrimUnlockVertexPtrs();
    g_theGxDevicePtr->XformSetProjection(savedProjection);
    CShaderEffect::UpdateProjMatrix();
    GxXformPop(GxXform_World);
    GxXformPop(GxXform_Tex1);
    GxXformPop(GxXform_Tex0);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x009D8F10/0x14076D510
void CGMinimapFrame::MinimapTextureCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    if (command != GxTex_Latch) {
        return;
    }

    float minX = g_theGxDevicePtr->m_viewport.m_x.m_l;
    float maxX = g_theGxDevicePtr->m_viewport.m_x.m_h;
    float minY = g_theGxDevicePtr->m_viewport.m_y.m_l;
    float maxY = g_theGxDevicePtr->m_viewport.m_y.m_h;
    float minZ = g_theGxDevicePtr->m_viewport.m_z.m_l;
    float maxZ = g_theGxDevicePtr->m_viewport.m_z.m_h;
    GxFrameBufferSet(s_indoor.m_frameBuffer);
    GxXformSetViewport(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    uint8_t alpha;

    if (CGMinimapFrame::s_minimapFrame) {
        alpha = CGMinimapFrame::s_minimapFrame->m_alpha * CGMinimapFrame::s_minimapFrame->m_parentAlpha / 255;
    } else {
        alpha = 0xFF;
    }

    GxSceneClear(15, CImVector(0, 0, 0, alpha));
    CImVector color(0xFF, 0xFF, 0xFF, alpha);
    CGMinimapFrame::RenderInsideTexture(&color);
    GxFrameBufferSet(g_theGxDevicePtr->BackBuffer());
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

// OFFSET: 0x009D9060/0x14076D6B0
void CGMinimapFrame::EnterWorld() {
    s_enteredWorld = 1;
    MinimapInitialize(CGPlayer_C::GetNewContinentID());
    s_indoor.m_radius = 10.0f;
    EGxTexFormat format = GxTex_Unknown;

    if (g_theGxDevicePtr->Caps().m_rttFormat[GxTex_Argb8888]) {
        format = GxTex_Argb8888;
    } else if (g_theGxDevicePtr->Caps().m_rttFormat[GxTex_Abgr8888]) {
        format = GxTex_Abgr8888;
    } else if (g_theGxDevicePtr->Caps().m_rttFormat[GxTex_Rgb565]) {
        format = GxTex_Rgb565;
    }

    CGxTexFlags flags(GxTex_Linear, 0, 0, 0, 1, 1, 0, 0, 0);
    s_indoor.m_texture = TextureCreate(256, 256, format, format, flags, NULL, CGMinimapFrame::MinimapTextureCallback, "MinimapRenderTarget", 0);
    s_indoor.m_frameBuffer = GxFrameBufferCreate(NULL, TextureGetGxTex(s_indoor.m_texture));
    memset(s_hoverObjects, 0, sizeof(s_hoverObjects));
    CGMinimapFrame::s_trackingMask = 0;
    CVar* var = CVar::Lookup("minimapTrackedInfov2");

    if (var) {
        CGMinimapFrame::SetTrackingMask(var->m_intValue);
    }

    ClntObjMgrEnumVisibleObjects(UpdateObjectInteractIcon, NULL);
}

// OFFSET: 0x009D91A0
void CGMinimapFrame::AddShapeshiftTrackingSpell(int32_t spellId) {
    for (uint32_t i = 0; i < CGMinimapFrame::s_shapeshiftTrackingSpells.Count(); i++) {
        if (CGMinimapFrame::s_shapeshiftTrackingSpells.m_data[i] == spellId) {
            return;
        }
    }

    CGMinimapFrame::s_shapeshiftTrackingSpells.Add(1, &spellId);
}

// OFFSET: 0x009D91E0/0x14076D940
void CGMinimapFrame::RemoveShapeshiftTrackingSpell(int32_t spellId) {
    for (int32_t i = CGMinimapFrame::s_shapeshiftTrackingSpells.Count() - 1; i >= 0; i--) {
        if (CGMinimapFrame::s_shapeshiftTrackingSpells.m_data[i] == spellId) {
            CGMinimapFrame::s_shapeshiftTrackingSpells.m_data[i] = CGMinimapFrame::s_shapeshiftTrackingSpells.m_data[CGMinimapFrame::s_shapeshiftTrackingSpells.Count() - 1];
            CGMinimapFrame::s_shapeshiftTrackingSpells.SetCount(CGMinimapFrame::s_shapeshiftTrackingSpells.Count() - 1);
        }
    }
}

// OFFSET: 0x009D9270
void CGMinimapFrame::LoadShapeshiftTracking() {
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("minimapShapeshiftTracking");
    list.m_pos = 2;
    list.m_state = 0;
    CGMinimapFrame::s_shapeshiftTrackingSpells.SetCount(0);

    for (int32_t spellId = list.GetNextValue(); spellId; spellId = list.GetNextValue()) {
        CGMinimapFrame::AddShapeshiftTrackingSpell(spellId);
    }

    if (list.m_state != 1) {
        list.m_buffer[0] = 'v';
        list.m_buffer[1] = 1;
        list.m_buffer[list.m_pos] = 0;
        list.m_cvar->Set(list.m_buffer, true, false, false, true);
    }
}

// OFFSET: 0x009D95F0
void CGMinimapFrame::RenderMarkedBlips(const C44Matrix& rotation) {
    for (uint32_t marker = 0; marker < 2; marker++) {
        int32_t list = s_markedBlips[marker].m_list;

        if (list == -1) {
            continue;
        }

        CTexture* texture = (list == 1 || list == 0) ? s_classBlipTexture : s_blipTexture;

        if (!texture) {
            continue;
        }

        OBJINFO info = s_objInfo[list].m_data[s_markedBlips[marker].m_index];
        C3Vector point(info.m_position.x, info.m_position.y, 0.0f);

        if (g_rotateMinimapCVar->m_intValue) {
            point = C3Vector(info.m_rotated.x, info.m_rotated.y, 0.0f);
            point *= rotation;
            info.m_position.x = point.x;
            info.m_position.y = point.y;
        }

        for (uint32_t pass = 0; pass < 2; pass++) {
            uint16_t base = static_cast<uint16_t>(s_blipVertices.m_count);
            C3Vector vertices[4];

            for (uint32_t j = 0; j < 4; j++) {
                vertices[j].x = CGMinimapFrame::s_blipQuads[0][j].x * this->m_layoutScale + point.x;
                vertices[j].y = this->m_layoutScale * CGMinimapFrame::s_blipQuads[0][j].y + point.y;
                vertices[j].z = 0.0f;
            }

            s_blipVertices.Add(4, vertices);
            uint16_t indices[6];
            indices[1] = base + 1;
            indices[4] = base + 1;
            indices[0] = base;
            indices[2] = base + 2;
            indices[3] = base + 2;
            indices[5] = base + 3;
            s_blipIndices.Add(6, indices);
        }

        C2Vector texCoords[4];
        C2Vector markerTexCoords[4];

        if (list >= 2) {
            for (uint32_t j = 0; j < 4; j++) {
                texCoords[j] = s_blipTexCoords[list][j];
            }

            for (uint32_t j = 0; j < 4; j++) {
                markerTexCoords[j] = s_blipTexCoords[(marker != 0) + 30][j];
            }
        } else {
            int32_t icon;

            if (info.m_icon) {
                icon = info.m_icon - 1;

                if (!list) {
                    icon += 16;
                }
            } else {
                icon = 30;
            }

            for (uint32_t j = 0; j < 4; j++) {
                texCoords[j] = s_classBlipTexCoords[icon][j];
            }

            for (uint32_t j = 0; j < 4; j++) {
                markerTexCoords[j] = s_classBlipTexCoords[marker ? 31 : 15][j];
            }
        }

        s_blipTexCoords2.Add(4, markerTexCoords);
        s_blipTexCoords2.Add(4, texCoords);

        if (!s_blipVertices.m_count || !s_blipIndices.m_count) {
            continue;
        }

        CGxTex* gxTex = TextureGetGxTex(texture);

        if (!gxTex) {
            continue;
        }

        g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
        static CImVector s_color(0xFF, 0xFF, 0xFF, 0xFF);
        static C3Vector s_normal(0.0f, 0.0f, 1.0f);
        GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &s_normal, 0, &s_color, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), NULL, 0);
        GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
        DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
        GxDrawLockedElements();
        GxPrimUnlockVertexPtrs();
        GxPrimUnlockIndexPtr();
        s_blipVertices.SetCount(0);
        s_blipTexCoords2.SetCount(0);
        s_blipIndices.m_count = 0;
        this->RenderHeightArrow(info.m_rotated, info.m_height, rotation);
    }
}

// OFFSET: 0x009D9D30
void CGMinimapFrame::RenderBlobRing(const C44Matrix& rotation, MINIMAPBLOB* blob) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || blob->m_points.m_count < 3 || !blob->m_rings.m_count) {
        return;
    }

    CGxTex* gxTex = TextureGetGxTex(blob->m_ringTexture);

    if (!gxTex) {
        return;
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    GxRsSet(GxRs_MatDiffuse, CImVector(0xFF, 0xFF, 0xFF, blob->m_ringAlpha));
    C3Vector position = player->GetPosition();
    float radius = static_cast<float>(MinimapGetViewRadius() * this->m_questBlobRingScalar);

    for (uint32_t i = 0; i < blob->m_rings.m_count; i++) {
        if (std::fabs(blob->m_rings.m_data[i].m_start - blob->m_rings.m_data[i].m_end) < 0.00000023841858f) {
            s_blipVertices.SetCount(4);
            s_blipVertices.m_data[0].x = position.x + radius;
            s_blipVertices.m_data[0].y = position.y + radius;
            s_blipVertices.m_data[0].z = 0.0f;
            s_blipVertices.m_data[1].z = 0.0f;
            s_blipVertices.m_data[1].y = position.y - radius;
            s_blipVertices.m_data[1].x = position.x + radius;
            s_blipVertices.m_data[2].z = 0.0f;
            s_blipVertices.m_data[2].y = position.y - radius;
            s_blipVertices.m_data[2].x = position.x - radius;
            s_blipVertices.m_data[3].x = position.x - radius;
            s_blipVertices.m_data[3].y = position.y + radius;
            s_blipVertices.m_data[3].z = 0.0f;
            s_blipTexCoords2.SetCount(4);
            s_blipTexCoords2.m_data[0].x = 1.0f;
            s_blipTexCoords2.m_data[0].y = 1.0f;
            s_blipTexCoords2.m_data[1].x = 1.0f;
            s_blipTexCoords2.m_data[1].y = 0.0f;
            s_blipTexCoords2.m_data[2].x = 0.0f;
            s_blipTexCoords2.m_data[2].y = 0.0f;
            s_blipTexCoords2.m_data[3].x = 0.0f;
            s_blipTexCoords2.m_data[3].y = 1.0f;
            s_blipIndices.SetCount(6);
            s_blipIndices.m_data[0] = 0;
            s_blipIndices.m_data[1] = 1;
            s_blipIndices.m_data[2] = 2;
            s_blipIndices.m_data[3] = 0;
            s_blipIndices.m_data[4] = 2;
            s_blipIndices.m_data[5] = 3;
        } else {
            int32_t quadrant = GetAngleQuadrant(blob->m_rings.m_data[i].m_start);
            uint32_t endQuadrant = GetAngleQuadrant(blob->m_rings.m_data[i].m_end);
            float from = blob->m_rings.m_data[i].m_start;
            float to = blob->m_rings.m_data[i].m_end;

            while (quadrant != endQuadrant) {
                float corner;

                if (blob->m_rings.m_data[i].m_reverse) {
                    corner = s_quadrantAngles[(quadrant + 3) % 4];
                } else {
                    corner = s_quadrantAngles[quadrant % 4];
                }

                this->AddBlobRingSegment(blob, from, corner);

                if (blob->m_rings.m_data[i].m_reverse) {
                    if (quadrant) {
                        quadrant--;
                    } else {
                        quadrant = 3;
                    }
                } else {
                    quadrant = (quadrant + 1) % 4;
                }

                from = corner;
            }

            this->AddBlobRingSegment(blob, from, to);
        }

        for (uint32_t j = 0; j < s_blipVertices.m_count; j++) {
            C3Vector* vertex = &s_blipVertices.m_data[j];
            C3Vector origin = player->GetPosition();
            float viewRadius = static_cast<float>(MinimapGetViewRadius());
            float half = this->m_layoutScale * this->GetWidth() * 0.5f;
            C3Vector point;
            point.x = half - half * (vertex->y - origin.y) * (1.0f / viewRadius);
            point.y = half * (vertex->x - origin.x) * (1.0f / viewRadius) + half;
            point.z = 0.0f;

            if (g_rotateMinimapCVar->m_intValue) {
                point *= rotation;
            }

            s_blipVertices.m_data[j].x = point.x;
            s_blipVertices.m_data[j].y = point.y;
            s_blipVertices.m_data[j].z = 0.0f;
        }

        GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), NULL, 0, NULL, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), NULL, 0);
        GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
        DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
        GxDrawLockedElements();
        GxPrimUnlockVertexPtrs();
        GxPrimUnlockIndexPtr();
        s_blipVertices.SetCount(0);
        s_blipTexCoords2.SetCount(0);
        s_blipIndices.m_count = 0;
    }
}

// OFFSET: 0x009DA300
void CGMinimapFrame::LoadTrackingState() {
    CGMinimapFrame::LoadShapeshiftTracking();
    CGMinimapFrame::s_trackingMask = 0;
    CVar* var = CVar::Lookup("minimapTrackedInfov2");

    if (var) {
        CGMinimapFrame::SetTrackingMask(var->m_intValue);
    }

    ClntObjMgrEnumVisibleObjects(UpdateObjectInteractIcon, NULL);
    MinimapLoadZoom();
}

// OFFSET: 0x009DA340
void CGMinimapFrame::RenderQuestBlob(const C44Matrix& rotation, uint32_t blobIndex) {
    BLOB* blob = CGQuestLog::GetQuestPOIBlob(CGQuestLog::m_superTrackedQuest, blobIndex);

    if (!blob || blob->m_points.m_count < 3) {
        return;
    }

    MINIMAPBLOB data;
    CGBlobFrame::BuildBlobOutline(blob, &data.m_points, true, s_blobSplinePoints);
    data.m_insideTexture = this->m_questBlobInsideTexture;
    data.m_outsideTexture = this->m_questBlobOutsideTexture;
    data.m_ringTexture = this->m_questBlobRingTexture;
    data.m_insideAlpha = this->m_questBlobInsideAlpha;
    data.m_outsideAlpha = this->m_questBlobOutsideAlpha;
    data.m_ringAlpha = this->m_questBlobRingAlpha;

    if (s_blobsEnabled) {
        if (s_questBlobInsideEnabled) {
            this->RenderBlobInside(rotation, &data);
        }

        if (s_questBlobOutsideEnabled) {
            this->RenderBlobOutside(rotation, &data);
        }

        if (this->BuildBlobRings(&data)) {
            this->RenderBlobRing(rotation, &data);
        }
    }
}

// OFFSET: 0x009DA450
void CGMinimapFrame::RenderArchBlob(const C44Matrix& rotation, uint32_t siteIndex) {
    BLOB* blob = reinterpret_cast<BLOB*>(CGResearchFrame::GetResearchSiteBlobs(siteIndex));

    if (!blob || blob->m_points.m_count < 3) {
        return;
    }

    MINIMAPBLOB data;
    CGBlobFrame::BuildBlobOutline(blob, &data.m_points, true, s_blobSplinePoints);
    data.m_insideTexture = this->m_archBlobInsideTexture;
    data.m_outsideTexture = this->m_archBlobOutsideTexture;
    data.m_ringTexture = this->m_archBlobRingTexture;
    data.m_insideAlpha = this->m_archBlobInsideAlpha;
    data.m_outsideAlpha = this->m_archBlobOutsideAlpha;
    data.m_ringAlpha = this->m_archBlobRingAlpha;

    if (s_blobsEnabled) {
        this->RenderBlobInside(rotation, &data);
        this->RenderBlobOutside(rotation, &data);

        if (this->BuildBlobRings(&data)) {
            this->RenderBlobRing(rotation, &data);
        }
    }
}

// OFFSET: 0x009DA550/0x14076F3D0
void CGMinimapFrame::InitializeGame() {
    CStatus status;
    CGxTexFlags flags(GxTex_Linear, 0, 0, 0, 0, 1);

    s_maskTexture = TextureCreate("Textures\\MinimapMask", flags, &status, 1, NULL, 2);
    s_iconTexture = TextureCreate("Interface\\Minimap\\POIIcons", flags, &status, 1, NULL, 2);
    s_iconTexCoordsReady = 0;
    s_blipTexture = TextureCreate("Interface\\Minimap\\ObjectIcons", flags, &status, 1, NULL, 2);
    s_classBlipTexture = TextureCreate("Interface\\Minimap\\PartyRaidBlips", flags, &status, 1, NULL, 2);
    s_vehicleTextures[0] = TextureCreate("Interface\\Minimap\\Vehicle-Ground-Unoccupied", flags, &status, 1, NULL, 2);
    s_vehicleTextures[1] = TextureCreate("Interface\\Minimap\\Vehicle-Ground-Occupied", flags, &status, 1, NULL, 2);
    s_vehicleTextures[2] = TextureCreate("Interface\\Minimap\\Vehicle-Air-Unoccupied", flags, &status, 1, NULL, 2);
    s_vehicleTextures[3] = TextureCreate("Interface\\Minimap\\Vehicle-Air-Occupied", flags, &status, 1, NULL, 2);
    s_vehicleTextures[4] = TextureCreate("Interface\\Minimap\\Vehicle-Ground-Unoccupied", flags, &status, 1, NULL, 2);
    s_vehicleTextures[5] = TextureCreate("Interface\\Minimap\\Vehicle-Air-Horde", flags, &status, 1, NULL, 2);
    s_vehicleTextures[6] = TextureCreate("Interface\\Minimap\\Vehicle-Air-Alliance", flags, &status, 1, NULL, 2);
    s_vehicleTextures[7] = TextureCreate("Interface\\Minimap\\Vehicle-Carriage", flags, &status, 1, NULL, 2);
    s_staticPOIArrowTexture = TextureCreate("Interface\\Minimap\\Rotating-MinimapArrow", flags, &status, 1, NULL, 2);
    s_poiArrowTexture = TextureCreate("Interface\\Minimap\\Rotating-MinimapGuideArrow", flags, &status, 1, NULL, 2);
    s_corpseArrowTexture = TextureCreate("Interface\\Minimap\\MiniMap-DeadArrow", flags, &status, 1, NULL, 2);
    s_groupArrowTexture = TextureCreate("Interface\\Minimap\\Rotating-MinimapGroupArrow", flags, &status, 1, NULL, 2);
    s_questArrowTexture = TextureCreate("Interface\\Minimap\\MiniMap-QuestArrow", flags, &status, 1, NULL, 2);
    s_pingCenterTexture = TextureCreate("Interface\\Minimap\\UI-Minimap-Ping-Center", flags, &status, 1, NULL, 2);
    s_pingRotateTexture = TextureCreate("Interface\\Minimap\\UI-Minimap-Ping-Rotate", flags, &status, 1, NULL, 2);
    s_pingExpandTexture = TextureCreate("Interface\\Minimap\\UI-Minimap-Ping-Expand", flags, &status, 1, NULL, 2);
    s_positionArrowsTexture = TextureCreate("Interface\\Minimap\\Minimap-PositionArrows", flags, &status, 1, NULL, 2);

    for (uint32_t i = 0; i < 64; i++) {
        float left = static_cast<int32_t>(i & 7) * 0.125f;
        float bottom = ((i >> 3) + 1) * 0.125f;
        float right = ((i & 7) + 1) * 0.125f;
        float top = static_cast<int32_t>(i >> 3) * 0.125f;

        s_blipTexCoords[i][0].x = left;
        s_blipTexCoords[i][0].y = bottom;
        s_blipTexCoords[i][1].x = right;
        s_blipTexCoords[i][1].y = bottom;
        s_blipTexCoords[i][2].x = left;
        s_blipTexCoords[i][2].y = top;
        s_blipTexCoords[i][3].x = right;
        s_blipTexCoords[i][3].y = top;
    }

    for (uint32_t i = 0; i < 32; i++) {
        float left = static_cast<int32_t>(i & 7) * 0.125f;
        float bottom = ((i >> 3) + 1) * 0.25f;
        float right = ((i & 7) + 1) * 0.125f;
        float top = static_cast<int32_t>(i >> 3) * 0.25f;

        s_classBlipTexCoords[i][0].x = left;
        s_classBlipTexCoords[i][0].y = bottom;
        s_classBlipTexCoords[i][1].x = right;
        s_classBlipTexCoords[i][1].y = bottom;
        s_classBlipTexCoords[i][2].x = left;
        s_classBlipTexCoords[i][2].y = top;
        s_classBlipTexCoords[i][3].x = right;
        s_classBlipTexCoords[i][3].y = top;
    }

    CGMinimapFrame::s_trackingMask = 0;
}

// OFFSET: 0x009DA940/0x14076F8E0
int32_t CGMinimapFrame::SetQuestBlobInsideTexture(const char* fileName) {
    if (this->m_questBlobInsideTexture) {
        TextureRelease(this->m_questBlobInsideTexture);
        this->m_questBlobInsideTexture = NULL;
    }

    CStatus status;
    this->m_questBlobInsideTexture = TextureCreate(fileName, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), &status, 0, NULL, 2);
    return 1;
}

// OFFSET: 0x009DA9C0/0x14076F990
int32_t CGMinimapFrame::SetQuestBlobOutsideTexture(const char* fileName) {
    if (this->m_questBlobOutsideTexture) {
        TextureRelease(this->m_questBlobOutsideTexture);
        this->m_questBlobOutsideTexture = NULL;
    }

    CStatus status;
    this->m_questBlobOutsideTexture = TextureCreate(fileName, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), &status, 0, NULL, 2);
    return 1;
}

// OFFSET: 0x009DAA40/0x14076FA40
int32_t CGMinimapFrame::SetQuestBlobRingTexture(const char* fileName) {
    if (this->m_questBlobRingTexture) {
        TextureRelease(this->m_questBlobRingTexture);
        this->m_questBlobRingTexture = NULL;
    }

    CStatus status;
    this->m_questBlobRingTexture = TextureCreate(fileName, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), &status, 0, NULL, 2);
    return 1;
}

// OFFSET: 0x009DAAC0/0x14076FAF0
int32_t CGMinimapFrame::SetArchBlobInsideTexture(const char* fileName) {
    if (this->m_archBlobInsideTexture) {
        TextureRelease(this->m_archBlobInsideTexture);
        this->m_archBlobInsideTexture = NULL;
    }

    CStatus status;
    this->m_archBlobInsideTexture = TextureCreate(fileName, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), &status, 0, NULL, 2);
    return 1;
}

// OFFSET: 0x009DAB40/0x14076FBA0
int32_t CGMinimapFrame::SetArchBlobOutsideTexture(const char* fileName) {
    if (this->m_archBlobOutsideTexture) {
        TextureRelease(this->m_archBlobOutsideTexture);
        this->m_archBlobOutsideTexture = NULL;
    }

    CStatus status;
    this->m_archBlobOutsideTexture = TextureCreate(fileName, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), &status, 0, NULL, 2);
    return 1;
}

// OFFSET: 0x009DABC0/0x14076FC50
int32_t CGMinimapFrame::SetArchBlobRingTexture(const char* fileName) {
    if (this->m_archBlobRingTexture) {
        TextureRelease(this->m_archBlobRingTexture);
        this->m_archBlobRingTexture = NULL;
    }

    CStatus status;
    this->m_archBlobRingTexture = TextureCreate(fileName, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), &status, 0, NULL, 2);
    return 1;
}

// OFFSET: 0x009DAC40/0x14076FD00
void CGMinimapFrame::PostLoadXML(linkObject* node, CStatus* status) {
    CSimpleFrame::PostLoadXML(node, status);
    Minimap* minimap = static_cast<Minimap*>(node);

    if (!this->m_playerTexture->SetTexture((minimap->m_minimapFlags & 0x1) ? minimap->m_playerTexture.m_string : "Interface\\Minimap\\MinimapArrow.tga", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal)) {
        status->Add(STATUS_WARNING, "Invalid minimapPlayerTexture in Minimap.xml");
    }

    if (!this->SetQuestBlobInsideTexture((minimap->m_minimapFlags & 0x2) ? minimap->m_questBlobInsideTexture.m_string : "Interface\\Minimap\\UI-QuestBlobMinimap-Inside")) {
        status->Add(STATUS_WARNING, "Invalid minimapQuestBlobInsideTexture in Minimap.xml");
    }

    if (!this->SetQuestBlobOutsideTexture((minimap->m_minimapFlags & 0x4) ? minimap->m_questBlobOutsideTexture.m_string : "Interface\\Minimap\\UI-QuestBlobMinimap-Outside")) {
        status->Add(STATUS_WARNING, "Invalid minimapQuestBlobOutsideTexture in Minimap.xml");
    }

    if (!this->SetQuestBlobRingTexture((minimap->m_minimapFlags & 0x8) ? minimap->m_questBlobRingTexture.m_string : "Interface\\Minimap\\UI-QuestBlob-MinimapRing")) {
        status->Add(STATUS_WARNING, "Invalid minimapQuestBlobRingTexture in Minimap.xml");
    }

    if (!this->SetArchBlobInsideTexture((minimap->m_minimapFlags & 0x10) ? minimap->m_archBlobInsideTexture.m_string : "Interface\\Minimap\\UI-ArchBlobMinimap-Inside")) {
        status->Add(STATUS_WARNING, "Invalid minimapArchBlobInsideTexture in Minimap.xml");
    }

    if (!this->SetArchBlobOutsideTexture((minimap->m_minimapFlags & 0x10) ? minimap->m_archBlobOutsideTexture.m_string : "Interface\\Minimap\\UI-ArchBlobMinimap-Outside")) {
        status->Add(STATUS_WARNING, "Invalid minimapArchBlobOutsideTexture in Minimap.xml");
    }

    if (!this->SetArchBlobRingTexture((minimap->m_minimapFlags & 0x20) ? minimap->m_archBlobRingTexture.m_string : "Interface\\Minimap\\UI-ArchBlob-MinimapRing")) {
        status->Add(STATUS_WARNING, "Invalid minimapArchBlobRingTexture in Minimap.xml");
    }

    CLayoutFrame* compass = this->GetLayoutFrameByName("MinimapCompassTexture");
    this->m_compassTexture = compass ? static_cast<CScriptRegion*>(compass) : NULL;
}

// OFFSET: 0x009DBA50
void CGMinimapFrame::RenderQuestBlobs(const C44Matrix& rotation) {
    this->m_questBlobVisible = 0;

    if (!IsTrackingType(7)) {
        return;
    }

    float range = static_cast<float>(MinimapGetViewRadius() * 1.5);
    int32_t questId = CGQuestLog::m_superTrackedQuest;

    if (!questId) {
        return;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();
    CGObject_C* target = ClntObjMgrObjectPtr(camera->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);
    C3Vector position = target ? target->GetPosition() : StaticSingleton<CCameraManager>::Instance()->m_shot.m_position;
    CRect rect;
    rect.minY = position.x + range;
    rect.minX = position.y - range;
    rect.maxY = position.x - range;
    rect.maxX = position.y + range;
    TSGrowableArray<uint32_t> blobs;
    TSGrowableArray<uint32_t> visible;
    TSGrowableArray<uint32_t> merged;

    if (!CGQuestLog::GetQuestPOIBlobsOnContinent(questId, blobs)) {
        return;
    }

    for (uint32_t i = 0; i < blobs.m_count; i++) {
        if (CGQuestLog::QuestPOIBlobIntersectsRect(questId, blobs.m_data[i], rect) || IsPlayerInQuestBlob(questId, blobs.m_data[i])) {
            visible.Add(1, &blobs.m_data[i]);
        }
    }

    if (!visible.m_count) {
        return;
    }

    this->m_questBlobVisible = 1;

    if (s_mergeQuestBlobs) {
        CGQuestLog::MergeOverlappingBlobs(questId, visible, merged);
    } else {
        merged.SetCount(visible.m_count);

        for (uint32_t i = 0; i < visible.m_count; i++) {
            merged.m_data[i] = visible.m_data[i];
        }
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_BlendingMode, 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Multisample, 0);

    for (uint32_t i = 0; i < merged.m_count; i++) {
        this->RenderQuestBlob(rotation, merged.m_data[i]);
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x009DBE30
void CGMinimapFrame::RenderArchBlobs(const C44Matrix& rotation) {
    if (!IsTrackingType(8)) {
        return;
    }

    float radius = static_cast<float>(MinimapGetViewRadius());
    float range = radius + radius;
    CGCamera* camera = CGWorldFrame::GetActiveCamera();
    CGObject_C* target = ClntObjMgrObjectPtr(camera->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);
    C3Vector position = target ? target->GetPosition() : StaticSingleton<CCameraManager>::Instance()->m_shot.m_position;
    CRect rect;
    rect.minY = position.x + range;
    rect.minX = position.y - range;
    rect.maxY = position.x - range;
    rect.maxX = position.y + range;
    TSGrowableArray<int32_t> sites;
    TSGrowableArray<int32_t> visible;

    if (!CGResearchFrame::UpdateArchaeologyDigSiteBlobs(sites)) {
        return;
    }

    for (uint32_t i = 0; i < sites.m_count; i++) {
        if (CGResearchFrame::BlobIntersectsRect(sites.m_data[i], rect)) {
            visible.Add(1, &sites.m_data[i]);
        }
    }

    if (!visible.m_count) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_BlendingMode, 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Multisample, 0);
    DefaultShaders::SetDefaultShaders(GxVBF_PT, 1);

    for (uint32_t i = 0; i < visible.m_count; i++) {
        this->RenderArchBlob(rotation, visible.m_data[i]);
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x009DC170
void CGMinimapFrame::Render() {
    if (!s_maskTexture || !(this->m_layoutFlags & 0x1) || !s_enteredWorld) {
        return;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();
    CGObject_C* target = ClntObjMgrObjectPtr(camera->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);
    C3Vector position = target ? target->GetPosition() : StaticSingleton<CCameraManager>::Instance()->m_shot.m_position;
    uintptr_t worldObject;

    if (target) {
        worldObject = target->m_worldObject;
    } else {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (!player) {
            return;
        }

        worldObject = player->m_worldObject;
    }

    if (!World::ObjectHasParent(worldObject)) {
        return;
    }

    float viewportMinX;
    float viewportMaxX;
    float viewportMinY;
    float viewportMaxY;
    float viewportMinZ;
    float viewportMaxZ;
    GxXformViewport(viewportMinX, viewportMaxX, viewportMinY, viewportMaxY, viewportMinZ, viewportMaxZ);
    CRect rect;
    rect.minY = 0.0f;
    rect.minX = 0.0f;
    rect.maxY = 0.0f;
    rect.maxX = 0.0f;

    if (!this->GetRect(&rect)) {
        return;
    }

    float ndcMinX = 0.0f;
    float ndcMinY = 0.0f;
    float ndcMaxX = 0.0f;
    float ndcMaxY = 0.0f;
    DDCToNDC(rect.minX, rect.minY, &ndcMinX, &ndcMinY);
    DDCToNDC(rect.maxX, rect.maxY, &ndcMaxX, &ndcMaxY);

    if (!GxXformIsValidViewport(ndcMinX, ndcMaxX, ndcMinY, ndcMaxY, 0.0f, 1.0f)) {
        return;
    }

    GxXformSetViewport(ndcMinX, ndcMaxX, ndcMinY, ndcMaxY, 0.0f, 1.0f);
    C44Matrix savedProjection;
    C44Matrix savedView;
    GxXformProjection(savedProjection);
    GxXformView(savedView);
    C2Vector offset;
    offset.x = rect.minX;
    offset.y = rect.minY;
    CameraSetupScreenProjection(rect, offset, 0.0f, 1);

    C44Matrix view;
    GxXformView(view);
    C3Vector translation(view.d0, view.d1, view.d2);
    C44Matrix rotatedView;

    if (g_rotateMinimapCVar->m_intValue) {
        this->SetCompassRotation(-this->m_facing);
        rotatedView.Translate(translation);
        C3Vector axis(0.0f, 0.0f, 1.0f);
        rotatedView *= C44Matrix::Rotation(-this->m_facing, axis, true);
        C44Matrix untranslate;
        untranslate.Translate(C3Vector(-translation.x, -translation.y, -translation.z));
        rotatedView *= untranslate;
        rotatedView *= view;
        GxXformSetView(rotatedView);
    } else {
        this->SetCompassRotation(0.0f);
    }

    C2Vector center(0.0f, 0.0f);
    float scale;
    int32_t updated = MinimapUpdate(worldObject, CGPlayer_C::GetNewContinentID(), &position, &center, &scale, s_tiles, &s_indoor, s_scaleChanged);

    if ((updated || s_scaleChanged) && !s_indoor.m_active) {
        this->UpdateQuads(center, scale);
        s_scaleChanged = 0;
    }

    static C3Vector s_normal(0.0f, 0.0f, 1.0f);

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_BlendingMode, 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);

    DefaultShaders::SetFogEnabled(0);
    const DayNight::LightInfo* info = DayNight::GetMinimapLightInfo();
    CImVector diffuse = info->m_directColor;
    CImVector ambient = info->m_ambientColor;
    CImVector white(0xFF, 0xFF, 0xFF, 0xFF);
    uint32_t luminance = static_cast<uint8_t>(ColorToLuminance(ambient)) + 96;

    if (luminance > 0xFF) {
        luminance = 0xFF;
    }

    CImVector target2 = white;
    ambient.InterpColorRGB(luminance, &target2);
    target2 = ambient;
    diffuse.InterpColorRGB(192, &target2);
    diffuse.a = this->GetEffectiveAlpha();

    if (s_indoor.m_active) {
        C44Matrix savedIndoorView;
        GxXformView(savedIndoorView);
        C44Matrix identity;
        GxXformSetView(identity);

        if (GxIsWindowVisible() && GxHasContext()) {
            if (s_indoor.m_dirty) {
                GxTexUpdate(TextureGetGxTex(s_indoor.m_texture), 0, 0, 0, 0, 0);
                s_indoor.m_dirty = 0;
            }

            C3Vector direction(s_indoor.m_position.x - s_indoor.m_center.x, s_indoor.m_position.y - s_indoor.m_center.y, s_indoor.m_position.z - s_indoor.m_center.z);
            C3Vector local = TransformDirection(direction, s_indoor.m_invMatrix);
            direction = TransformDirection(local, s_indoor.m_matrix);
            float inv = 1.0f / s_indoor.m_radius;
            CImVector color(0xFF, 0xFF, 0xFF, diffuse.a);
            C2Vector indoorCenter;
            indoorCenter.x = direction.x * inv * s_indoorTexScale + 0.5f;
            indoorCenter.y = inv * direction.y * s_indoorTexScale + 0.5f;
            this->RenderInside(s_indoor.m_radius, &indoorCenter, &color);
        }

        GxXformSetView(savedIndoorView);
    } else {
        for (uint32_t i = 0; i < 4; i++) {
            s_tiles[i].Render(i, &diffuse);
        }
    }

    if (g_rotateMinimapCVar->m_intValue) {
        GxXformSetView(view);
    }

    int32_t poisChanged;
    TSGrowableArray<AreaPOIRec*>* pois = MinimapGetPOI(&poisChanged);

    if (poisChanged) {
        s_poiInfo.SetCount(pois->m_count);

        for (uint32_t i = 0; i < s_poiInfo.m_count; i++) {
            AreaPOIRec* poi = pois->m_data[i];
            uint32_t state = 0;

            if (poi->m_worldStateID) {
                state = GetWorldState(poi->m_worldStateID) - 1;

                if (state >= 9) {
                    state = 0;
                }
            }

            s_poiInfo.m_data[i].m_icon = pois->m_data[i]->m_icon[state];
            s_poiInfo.m_data[i].m_name = pois->m_data[i]->m_name;
        }
    }

    CImVector iconColor(0xFF, 0xFF, 0xFF, 0xFF);
    float viewRadius = static_cast<float>(MinimapGetViewRadius());
    C44Matrix rotation;

    if (g_rotateMinimapCVar->m_intValue) {
        rotation.Translate(translation);
        C3Vector axis(0.0f, 0.0f, 1.0f);
        rotation *= C44Matrix::Rotation(-this->m_facing, axis, true);
        C44Matrix untranslate;
        untranslate.Translate(C3Vector(-translation.x, -translation.y, -translation.z));
        rotation *= untranslate;
    }

    if (TextureIsLoaded(s_iconTexture)) {
        if (!s_iconTexCoordsReady) {
            ComputeIconTexCoords();
            s_iconTexCoordsReady = 1;
        }

        if (pois->m_count >= s_poiInfo.m_count) {
            for (uint32_t i = 0; i < s_poiInfo.m_count; i++) {
                if (static_cast<uint32_t>(s_poiInfo.m_data[i].m_icon) > 195) {
                    continue;
                }

                C3Vector origin = position;
                AreaPOIRec* poi = pois->m_data[i];
                float poiX = poi->m_pos[0];
                float poiY = poi->m_pos[1];
                float half = this->m_layoutScale * this->GetWidth() * 0.5f;
                float inv = 1.0f / viewRadius;
                C3Vector point;
                point.x = half - inv * (half * (poiY - origin.y));
                point.y = half * (poiX - origin.x) * inv + half;
                point.z = 0.0f;

                if (g_rotateMinimapCVar->m_intValue) {
                    point *= rotation;
                }

                s_poiInfo.m_data[i].m_position.x = point.x;
                s_poiInfo.m_data[i].m_position.y = point.y;

                C3Vector vertices[4];
                uint16_t base = static_cast<uint16_t>(s_blipVertices.m_count);

                for (uint32_t j = 0; j < 4; j++) {
                    vertices[j].x = this->m_layoutScale * CGMinimapFrame::s_blipQuads[0][j].x + point.x;
                    vertices[j].y = point.y + this->m_layoutScale * CGMinimapFrame::s_blipQuads[0][j].y;
                    vertices[j].z = this->m_layoutScale * CGMinimapFrame::s_blipQuads[0][j].z + point.z;
                }

                s_blipVertices.Add(4, vertices);
                uint16_t indices[6];
                indices[0] = base;
                indices[2] = base + 2;
                indices[3] = base + 2;
                indices[1] = base + 1;
                indices[4] = base + 1;
                indices[5] = base + 3;
                s_blipIndices.Add(6, indices);

                C2Vector texCoords[4];

                for (uint32_t j = 0; j < 4; j++) {
                    texCoords[j] = s_iconTexCoords[s_poiInfo.m_data[i].m_icon][j];
                }

                s_blipTexCoords2.Add(4, texCoords);
            }
        }

        if (s_blipVertices.m_count) {
            g_theGxDevicePtr->RsSet(GxRs_Texture0, s_iconTexture ? TextureGetGxTex(s_iconTexture) : NULL);
            GxPrimLockVertexPtrs(s_blipVertices.m_count, s_blipVertices.m_data, sizeof(C3Vector), &s_normal, 0, &iconColor, 0, NULL, 0, s_blipTexCoords2.m_data, sizeof(C2Vector), NULL, 0);
            GxPrimLockIndexPtr(GxPrim_Triangles, s_blipIndices.m_count, s_blipIndices.m_data);
            DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, -1);
            GxDrawLockedElements();
            GxPrimUnlockVertexPtrs();
            GxPrimUnlockIndexPtr();
            s_blipVertices.SetCount(0);
            s_blipTexCoords2.SetCount(0);
            s_blipIndices.SetCount(0);
        }
    }

    uint32_t lastUpdate = this->m_lastBlipUpdate;
    bool expired = FrameTime::GetCurTimeMs() - lastUpdate >= 1000;

    if (updated || s_rotateMinimap != g_rotateMinimapCVar->m_intValue || !lastUpdate || expired) {
        uint32_t now = FrameTime::GetCurTimeMs();
        this->m_lastBlipUpdate = now;

        if (!now) {
            this->m_lastBlipUpdate = 1;
        }

        for (uint32_t i = 0; i < 41; i++) {
            s_objInfo[i].SetCount(0);
        }

        uint32_t wmoId = 0;
        uint32_t wmoHash = 0;
        uint32_t groupNum;

        if (World::QueryObjectInside(worldObject, 0)) {
            World::QueryMapObjIDs(worldObject, wmoId, wmoHash, groupNum);
        }

        MINIMAPENUMPARAM param;
        param.m_position = C3Vector(0.0f, 0.0f, 0.0f);
        param.m_player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        param.m_position = position;
        param.m_wmoId = wmoId;
        param.m_wmoHash = wmoHash;
        param.m_radius = viewRadius;
        param.m_scale = this->m_layoutScale;
        param.m_frame = this;
        ClntObjMgrEnumVisibleObjects(CGMinimapFrame::ObjectEnumProc, &param);

        CImVector vehicleColor(0xFF, 0xFF, 0xFF, 0xAA);
        CImVector* playerColor;

        if (param.m_player->m_vehiclePassenger && param.m_player->m_vehiclePassenger->m_state) {
            playerColor = &vehicleColor;
        } else {
            playerColor = &iconColor;
        }

        for (uint32_t i = 0; i < CGBattlefieldInfo::m_numVehicles; i++) {
            WOWGUID vehicleGuid;

            if (i < CGBattlefieldInfo::m_numVehicles) {
                vehicleGuid = CGBattlefieldInfo::m_vehicleGuids[i];
            } else {
                vehicleGuid = WOWGUID();
            }

            CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(vehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!vehicle || vehicle->IsDeadOrGhost()) {
                continue;
            }

            C3Vector vehiclePosition = vehicle->GetPosition();
            float dx = vehiclePosition.x - position.x;
            float dy = vehiclePosition.y - position.y;
            float dz = vehiclePosition.z - position.z;
            float limit = viewRadius * 0.8f;

            if (dx * dx + dz * dz + dy * dy > limit * limit) {
                continue;
            }

            if (vehicle->IsPossessedBy(param.m_player->m_obj->m_guid)) {
                playerColor = &iconColor;
                continue;
            }

            VehicleRec* vehicleRec = vehicle->m_vehicle ? vehicle->m_vehicle->m_vehicleRec : NULL;

            if (!vehicleRec || !(vehicleRec->m_flags & 0x10000000)) {
                continue;
            }

            TSGrowableArray<OBJINFO>* list;

            switch (vehicleRec->m_uiLocomotionType) {
                case 0:
                    list = (vehicle->m_unit->m_unitFieldFlags & 0x1000000) ? &s_objInfo[34] : &s_objInfo[33];
                    break;

                case 1:
                    list = (vehicle->m_unit->m_unitFieldFlags & 0x1000000) ? &s_objInfo[36] : &s_objInfo[35];
                    break;

                case 3:
                    list = &s_objInfo[38];
                    break;

                case 4:
                    list = &s_objInfo[39];
                    break;

                case 5:
                    list = &s_objInfo[40];
                    break;

                default:
                    continue;
            }

            OBJINFO* info2 = list->New();
            info2->m_guid = vehicleGuid;
            info2->m_otherFloor = 0;
            C3Vector blipPosition = vehicle->GetPosition();
            C2Vector coords = CGMinimapFrame::WorldPosToMinimapFrameCoords(param.m_position, param.m_radius, blipPosition.x, blipPosition.y, param.m_scale, this->GetWidth());
            info2->m_position.y = coords.y;
            info2->m_rotated.y = coords.y;
            info2->m_position.x = coords.x;
            info2->m_rotated.x = coords.x;
            info2->m_facing = vehicle->GetFacing();
        }

        this->m_playerTexture->SetVertexColor(*playerColor);
        WOWGUID trackedTarget = IsTrackingType(5) ? CGGameUI::m_lockedTarget : WOWGUID();
        WOWGUID trackedFocus = IsTrackingType(6) ? CGGameUI::m_focusTarget : WOWGUID();

        for (uint32_t i = 0; i < 5; i++) {
            if (!s_partyMembers[i].m_onMap) {
                continue;
            }

            OBJINFO* info2 = s_objInfo[1].New();
            info2->m_guid = s_partyMembers[i].m_guid;
            info2->m_otherFloor = 0;
            info2->m_questGiver = 0;
            info2->m_marker = -1;
            C2Vector coords = CGMinimapFrame::WorldPosToMinimapFrameCoords(param.m_position, param.m_radius, s_partyMembers[i].m_position.x, s_partyMembers[i].m_position.y, param.m_scale, this->GetWidth());
            info2->m_position.y = coords.y;
            info2->m_rotated.y = coords.y;
            info2->m_position.x = coords.x;
            info2->m_rotated.x = coords.x;
            info2->m_icon = s_partyMembers[i].m_class;

            if (trackedTarget && trackedTarget == s_partyMembers[i].m_guid) {
                info2->m_marker = 0;
            }

            if (trackedFocus && trackedFocus == s_partyMembers[i].m_guid) {
                info2->m_marker = 1;
            }

            info2->m_height = s_partyMembers[i].m_position.z - param.m_position.z;
        }

        for (uint32_t i = 0; i < 40; i++) {
            if (!s_raidMembers[i].m_onMap) {
                continue;
            }

            OBJINFO* info2 = s_objInfo[0].New();
            info2->m_guid = s_raidMembers[i].m_guid;
            info2->m_otherFloor = 0;
            info2->m_questGiver = 0;
            info2->m_marker = -1;
            C2Vector coords = CGMinimapFrame::WorldPosToMinimapFrameCoords(param.m_position, param.m_radius, s_raidMembers[i].m_position.x, s_raidMembers[i].m_position.y, param.m_scale, this->GetWidth());
            info2->m_position.x = coords.x;
            info2->m_position.y = coords.y;
            info2->m_rotated.x = coords.x;
            info2->m_rotated.y = coords.y;
            info2->m_icon = s_raidMembers[i].m_class;

            if (trackedTarget && trackedTarget == s_raidMembers[i].m_guid) {
                info2->m_marker = 0;
            }

            if (trackedFocus && trackedFocus == s_raidMembers[i].m_guid) {
                info2->m_marker = 1;
            }

            info2->m_height = s_raidMembers[i].m_position.z - param.m_position.z;
        }
    }

    s_markedBlips[0].m_list = -1;
    s_markedBlips[1].m_list = -1;
    this->RenderQuestBlobs(rotation);
    this->RenderArchBlobs(rotation);
    this->RenderPartyRaidBlips(rotation);
    this->RenderVehicleBlips(rotation);
    this->RenderQuestCompletionNPC(rotation, position);

    if (!this->m_questBlobVisible) {
        this->RenderSuperTrackedQuestArrow(rotation, position);
    }

    this->RenderObjectBlips(rotation, false);
    this->RenderObjectBlips(rotation, true);
    this->RenderMarkedBlips(rotation);
    this->RenderPOIArrows(rotation);
    this->RenderPartyArrows(rotation);
    this->RenderRaidArrows(rotation);
    this->RenderPings(rotation);
    g_theGxDevicePtr->RsPop();
    g_theGxDevicePtr->XformSetProjection(savedProjection);
    g_theGxDevicePtr->XformSetView(savedView);
    GxXformSetViewport(viewportMinX, viewportMaxX, viewportMinY, viewportMaxY, viewportMinZ, viewportMaxZ);
    CShaderEffect::UpdateProjMatrix();
    s_rotateMinimap = g_rotateMinimapCVar->m_intValue;
}

// PROVEN: d9f90a6e IDENTICAL/NONE
// OFFSET: 0x009DDED0
void CGMinimapFrame::RenderCallback(void* param) {
    if (param) {
        static_cast<CGMinimapFrame*>(param)->Render();
    }
}

// PROVEN: da772788 IDENTICAL/NONE
// OFFSET: 0x009DDEF0
void CGMinimapFrame::OnFrameRender(CRenderBatch* batch, uint32_t layer) {
    CSimpleFrame::OnFrameRender(batch, layer);

    if (!layer) {
        batch->QueueCallback(CGMinimapFrame::RenderCallback, this);
    }
}
