#ifndef WOW_SOURCE_GAME_GAMECLIENT_MINIMAP_H
#define WOW_SOURCE_GAME_GAMECLIENT_MINIMAP_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Source/Object/WGUID.h"

class AreaPOIRec;
class CTexture;

struct MINIMAPTILE {
    /* 0000 */ C3Vector m_vertices[4];
    /* 0030 */ C2Vector m_texCoords[4];
    /* 0050 */ C2Vector m_maskCoords[4];
    /* 0070 */ CAaBox m_bounds;
    /* 0088 */ MINIMAPTILE* m_nextSorted;
    /* 008C */ float m_depth;
    /* 0090 */ CTexture* m_texture;
    /* 0094 */ uint32_t m_flags;
    /* 0098 */ int32_t m_x;
    /* 009C */ int32_t m_y;
    /* 00A0 */ int32_t m_groupId;

    void BuildQuad(const CRect& rect, uint32_t index, const C2Vector* center, float scale, const CRect& bounds, float layoutScale, float width);
    void Render(uint32_t index, const CImVector* color);
    void UpdateQuad(uint32_t index, C2Vector center, float scale, float layoutScale, float width);
};

struct MINIMAPINDOOR {
    /* 0000 */ uint8_t m_active;
    /* 0001 */ uint8_t m_pad1[3];
    /* 0004 */ CTexture* m_texture;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint8_t m_dirty;
    /* 000D */ uint8_t m_pad2[3];
    /* 0010 */ float m_radius;
    /* 0014 */ C3Vector m_center;
    /* 0020 */ C3Vector m_position;
    /* 002C */ C44Matrix m_matrix;
    /* 006C */ C44Matrix m_invMatrix;
};

struct MEMBERINFO {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ char m_name[48];
    /* 0038 */ int32_t m_offMap;
    /* 003C */ int32_t m_onMap;
    /* 0040 */ C3Vector m_position;
    /* 004C */ float m_facing;
    /* 0050 */ int32_t m_class;
};

struct POIDIRECTIONDATA {
    /* 0000 */ char m_name[64];
    /* 0040 */ float m_facing;
    /* 0044 */ int32_t m_alwaysShow;
    /* 0048 */ int32_t m_dynamicIndex;
};

void MinimapShutdown();
void MinimapSetZoom(uint32_t zoom);
void MinimapSetForceUpdate();
int32_t MinimapGetZoom();
int32_t MinimapBuildSource(const int32_t* tile, char* path, uint32_t size);
double MinimapGetViewRadius();
TSGrowableArray<AreaPOIRec*>* MinimapGetPOI(int32_t* changed);
int32_t MinimapSetNoLiquid(int32_t noLiquid);
AreaPOIRec* MinimapGetGossipPoi();
uint8_t MinimapIsIndoors();
int32_t MinimapGetOutdoorZoom();
int32_t MinimapGetIndoorZoom();
float MinimapGetOutdoorZoomTiles(int32_t zoom);
float MinimapGetIndoorZoomRadius(int32_t zoom);
void MinimapGetPartyMembers(MEMBERINFO* members);
void MinimapGetRaidMembers(MEMBERINFO* members);
void UpdatePointsOfInterest();
void RefreshWorldStatePOIs(uint32_t state);
void MinimapSetDynamicPOI(int32_t index, int32_t flags, float x, float y, int32_t icon, int32_t importance, const char* name);
void DisplayQuestPoint(float x, float y, int32_t importance, const char* name);
void MinimapSetCorpsePOI(float x, float y);
int32_t MinimapBuildPOIDirectionData(TSGrowableArray<POIDIRECTIONDATA>* directions);
int32_t MinimapInitialize(int32_t mapId);
int32_t MinimapUpdate(uintptr_t entity, int32_t mapId, const C3Vector* position, C2Vector* center, float* scale, MINIMAPTILE* tiles, MINIMAPINDOOR* indoor, int32_t force);

#endif
