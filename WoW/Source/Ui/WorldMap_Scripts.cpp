#include "WorldMap.h"
#include "WoW/Source/Game/GameClient/PhaseShift_C.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaPOIRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/DungeonMapRec.h"
#include "WoW/Source/DB/Rec/WorldMapAreaRec.h"
#include "WoW/Source/DB/Rec/WorldMapContinentRec.h"
#include "WoW/Source/DB/Rec/WorldMapOverlayRec.h"
#include "WoW/Source/DB/Rec/WorldSafeLocsRec.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "WoW/Source/DB/Rec/ResearchSiteRec.h"
#include "WoW/Source/DB/Rec/TaxiNodesRec.h"
#include <cmath>

// STATIC: 0x00D1143C
static const int32_t s_outdoorPVPZones[2][3] = { { 501, 3801, 4354 }, { 708, 5387, 5332 } };

// OFFSET: 0x00919D40/0x14067AC90
static int32_t Script_GetNumMapLandmarks(lua_State* L) {
    lua_pushnumber(L, CGWorldMap::m_numLandmarks);
    return 1;
}

// OFFSET: 0x00919D80/0x14067ACC0
static int32_t Script_GetNumMapOverlays(lua_State* L) {
    lua_pushnumber(L, CGWorldMap::m_numOverlays);
    return 1;
}

// OFFSET: 0x00919DC0/0x14067ACF0
static int32_t Script_PositionWorldMapArrowFrame(lua_State* L) {
    CSimpleModel* frame = CGWorldMap::m_playerArrowFrame;
    CSimpleModel* effect = CGWorldMap::m_playerArrowEffectFrame;

    if (!frame || !effect) {
        return 0;
    }

    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: PositionWorldMapArrowFrame(\"point\" \"frame\" [, relativePoint] [, offsetX, offsetY])");
        return 0;
    }

    float offsetX = 0.0f;
    float offsetY = 0.0f;
    FRAMEPOINT point;

    if (!StringToFramePoint(lua_tolstring(L, 1, NULL), point)) {
        luaL_error(L, "Unknown frame point");
        return 0;
    }

    FRAMEPOINT relativePoint = point;
    const char* name = lua_tolstring(L, 2, NULL);
    CLayoutFrame* relative = frame->GetLayoutFrameByName(name);
    char error[128];

    if (!relative) {
        SStrPrintf(error, sizeof(error), "Couldn't find frame named '%s'", name);
        luaL_error(L, error);
        return 0;
    }

    if (relative == frame) {
        SStrPrintf(error, sizeof(error), "Error: %s is anchored to itself", name);
        luaL_error(L, error);
        return 0;
    }

    if (lua_isstring(L, 3)) {
        if (!StringToFramePoint(lua_tolstring(L, 3, NULL), relativePoint)) {
            luaL_error(L, "Unknown frame point");
            return 0;
        }

        if (lua_isnumber(L, 4) && lua_isnumber(L, 5)) {
            float value = lua_tonumber(L, 4);
            offsetX = NDCToDDCWidth(value / (CoordinateGetAspectCompensation() * 1024.0f));
            value = lua_tonumber(L, 5);
            offsetY = NDCToDDCWidth(value / (CoordinateGetAspectCompensation() * 1024.0f));
        }
    }

    frame->SetPoint(point, relative, relativePoint, offsetX, offsetY, 1);
    effect->SetPoint(point, relative, relativePoint, offsetX, offsetY, 1);
    return 0;
}

// OFFSET: 0x00919FE0/0x14067AFA0
static int32_t Script_PositionMiniWorldMapArrowFrame(lua_State* L) {
    CSimpleModel* frame = CGWorldMap::m_playerMiniArrowFrame;
    CSimpleModel* effect = CGWorldMap::m_playerMiniArrowEffectFrame;

    if (!frame || !effect) {
        return 0;
    }

    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: PositionMiniWorldMapArrowFrame(\"point\" \"frame\" [, relativePoint] [, offsetX, offsetY])");
        return 0;
    }

    float offsetX = 0.0f;
    float offsetY = 0.0f;
    FRAMEPOINT point;

    if (!StringToFramePoint(lua_tolstring(L, 1, NULL), point)) {
        luaL_error(L, "Unknown frame point");
        return 0;
    }

    FRAMEPOINT relativePoint = point;
    const char* name = lua_tolstring(L, 2, NULL);
    CLayoutFrame* relative = frame->GetLayoutFrameByName(name);
    char error[128];

    if (!relative) {
        SStrPrintf(error, sizeof(error), "Couldn't find frame named '%s'", name);
        luaL_error(L, error);
        return 0;
    }

    if (relative == frame) {
        SStrPrintf(error, sizeof(error), "Error: %s is anchored to itself", name);
        luaL_error(L, error);
        return 0;
    }

    if (lua_isstring(L, 3)) {
        if (!StringToFramePoint(lua_tolstring(L, 3, NULL), relativePoint)) {
            luaL_error(L, "Unknown frame point");
            return 0;
        }

        if (lua_isnumber(L, 4) && lua_isnumber(L, 5)) {
            float value = lua_tonumber(L, 4);
            offsetX = NDCToDDCWidth(value / (CoordinateGetAspectCompensation() * 1024.0f));
            value = lua_tonumber(L, 5);
            offsetY = NDCToDDCWidth(value / (CoordinateGetAspectCompensation() * 1024.0f));
        }
    }

    frame->SetPoint(point, relative, relativePoint, offsetX, offsetY, 1);
    effect->SetPoint(point, relative, relativePoint, offsetX, offsetY, 1);
    return 0;
}

// OFFSET: 0x0091B790/0x14067CC10
static int32_t Script_GetMapContinents(lua_State* L) {
    uint32_t count = CGWorldMap::m_continents.m_count;

    for (uint32_t i = 0; i < count; i++) {
        lua_pushstring(L, CGWorldMap::GetContinentName(i));
    }

    return count;
}

// OFFSET: 0x0091BAA0/0x14067CF80
static int32_t Script_GetCurrentMapContinent(lua_State* L) {
    DungeonMapRec* rec = g_dungeonMapDB.GetRecord(CGWorldMap::m_currentOrphanZone == -1 ? -1 : CGWorldMap::m_currentDungeonMap);
    int32_t mapID;

    if (!rec) {
        WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetCurrentAreaID());

        if (!area || (!(area->m_flags & 0x4) && area->m_defaultDungeonFloor != -1)) {
            lua_pushnumber(L, CGWorldMap::m_currentContinent + 1);
            return 1;
        }

        mapID = area->m_mapID;
    } else {
        mapID = rec->m_mapID;
    }

    if (mapID >= 0) {
        uint32_t count = CGWorldMap::m_continents.m_count;

        for (uint32_t i = 0; i < count; i++) {
            if (CGWorldMap::IsMapOnContinent(mapID, i)) {
                lua_pushnumber(L, i + 1);
                return 1;
            }
        }
    }

    lua_pushnumber(L, CGWorldMap::m_currentContinent + 1);
    return 1;
}

// OFFSET: 0x0091BBC0/0x14067D0D0
static int32_t Script_GetCurrentMapAreaID(lua_State* L) {
    int32_t areaID;

    if (CGWorldMap::m_currentContinent >= 0) {
        if (CGWorldMap::m_currentZone >= 0) {
            areaID = CGWorldMap::m_continents.m_data[CGWorldMap::m_currentContinent].m_zones.m_data[CGWorldMap::m_currentZone];
        } else {
            areaID = CGWorldMap::m_continents.m_data[CGWorldMap::m_currentContinent].m_areaID;
        }
    } else {
        areaID = CGWorldMap::m_currentOrphanZone;
    }

    lua_pushnumber(L, areaID);
    return 1;
}

// OFFSET: 0x0091BC20/0x14067D140
static int32_t Script_GetCurrentMapZone(lua_State* L) {
    int32_t currentZone = CGWorldMap::m_currentZone;
    WorldMapAreaRec* area;

    if (currentZone >= 0) {
        area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetZoneID(CGWorldMap::m_currentContinent, currentZone));
    } else {
        area = g_worldMapAreaDB.GetRecord(CGWorldMap::m_currentContinent >= 0 ? CGWorldMap::m_continents.m_data[CGWorldMap::m_currentContinent].m_areaID : CGWorldMap::m_currentOrphanZone);

        if (area) {
            for (uint32_t continent = 0; continent < CGWorldMap::m_continents.m_count; continent++) {
                if (!CGWorldMap::IsMapOnContinent(area->m_mapID, continent)) {
                    continue;
                }

                uint32_t numZones = CGWorldMap::GetNumZones(continent);

                for (uint32_t zone = 0; zone < numZones; zone++) {
                    if (CGWorldMap::GetZoneID(continent, zone) == area->m_id) {
                        lua_pushnumber(L, zone + 1);
                        lua_pushnumber(L, area->m_locLeft);
                        lua_pushnumber(L, area->m_locTop);
                        lua_pushnumber(L, area->m_locRight);
                        lua_pushnumber(L, area->m_locBottom);
                        return 5;
                    }
                }
            }

            currentZone = CGWorldMap::m_currentZone;
        }
    }

    lua_pushnumber(L, currentZone + 1);

    if (!area) {
        return 1;
    }

    lua_pushnumber(L, area->m_locLeft);
    lua_pushnumber(L, area->m_locTop);
    lua_pushnumber(L, area->m_locRight);
    lua_pushnumber(L, area->m_locBottom);
    return 5;
}

// OFFSET: 0x0091BE40/0x14067D350
static int32_t Script_GetCurrentMapDungeonLevel(lua_State* L) {
    DungeonMapRec* rec = NULL;
    lua_pushnumber(L, CGWorldMap::GetCurrentDungeonFloor(&rec));

    if (rec) {
        lua_pushnumber(L, rec->m_minX);
        lua_pushnumber(L, rec->m_minY);
        lua_pushnumber(L, rec->m_maxX);
        lua_pushnumber(L, rec->m_maxY);
        return 5;
    }

    return 1;
}

// OFFSET: 0x0091B920/0x14067CDE0
static int32_t Script_DungeonUsesTerrainMap(lua_State* L) {
    WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::m_currentOrphanZone);

    if (area && ((area->m_flags & 0x4) || area->m_defaultDungeonFloor == -1)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0091A200/0x14067B250
static int32_t Script_ShowWorldMapArrowFrame(lua_State* L) {
    CSimpleModel* frame = CGWorldMap::m_playerArrowFrame;
    CSimpleModel* effect = CGWorldMap::m_playerArrowEffectFrame;

    if (frame && effect) {
        if (StringToBOOL(L, 1, 1)) {
            frame->Show();
            effect->Show();
            return 0;
        }

        frame->Hide();
        effect->Hide();
    }

    return 0;
}

// OFFSET: 0x0091A260/0x14067B2C0
static int32_t Script_ShowMiniWorldMapArrowFrame(lua_State* L) {
    CSimpleModel* frame = CGWorldMap::m_playerMiniArrowFrame;
    CSimpleModel* effect = CGWorldMap::m_playerMiniArrowEffectFrame;

    if (frame && effect) {
        if (StringToBOOL(L, 1, 1)) {
            frame->Show();
            effect->Show();
            return 0;
        }

        frame->Hide();
        effect->Hide();
    }

    return 0;
}

// OFFSET: 0x0091BF80/0x14067D540
static int32_t Script_IsZoomOutAvailable(lua_State* L) {
    bool available = true;
    int32_t dungeonMap = CGWorldMap::m_currentOrphanZone == -1 ? -1 : CGWorldMap::m_currentDungeonMap;

    if (!g_dungeonMapDB.GetRecord(dungeonMap) && CGWorldMap::m_currentContinent == -2) {
        WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetCurrentAreaID());

        if (!area || (!(area->m_flags & 0x4) && area->m_defaultDungeonFloor != -1 && !g_worldMapAreaDB.GetRecord(area->m_parentWorldMapID))) {
            available = false;
        }
    }

    lua_pushboolean(L, available);
    return 1;
}

// PROVEN: 9f0de39d IDENTICAL/NONE
// OFFSET: 0x0080C320/0x1405229B0
static int32_t Script_GetNumMapDebugObjects(lua_State* L) {
    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_HasDebugZoneMap(lua_State* L) {
    return 0;
}

// OFFSET: 0x009201F0/0x1406826A0
static int32_t Script_SetMapToCurrentZone(lua_State* L) {
    CGWorldMap::SetMapToCurrentZone(0);
    return 0;
}

// OFFSET: 0x0091DC00/0x14067F840
static int32_t Script_GetNumDungeonMapLevels(lua_State* L) {
    lua_pushnumber(L, CGWorldMap::GetNumDungeonLevels());
    return 1;
}

// OFFSET: 0x0091B990/0x14067CE50
static int32_t Script_GetMapInfo(lua_State* L) {
    lua_pushstring(L, CGWorldMap::GetMapName(CGWorldMap::GetCurrentAreaID()));
    int32_t height = CGWorldMap::GetMapHeight();
    lua_pushnumber(L, height);
    uint8_t low = static_cast<uint8_t>(height);
    uint32_t size = low;

    if ((low - 1) & low) {
        for (int32_t bit = 7; bit >= 0; bit--) {
            if ((1 << bit) & low) {
                size = 1 << (bit + 1);
                break;
            }
        }
    }

    lua_pushnumber(L, height - low + size);
    return 3;
}

// OFFSET: 0x0091DC30/0x14067F870
static int32_t Script_GetPlayerMapPosition(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetPlayerMapPosition(\"player\")");
        return 0;
    }

    float x = 0.0f;
    float y = 0.0f;
    WOWGUID guid = GetGuidByKeyword(lua_tolstring(L, 1, NULL));

    if (guid == ClntObjMgrGetActivePlayer() || CGPartyInfo::IsMember(guid) || IsGuidInMyRaid(guid)) {
        CGWorldMap::GetPlayerPosition(guid, &x, &y);
    }

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

// OFFSET: 0x0091C020/0x14067D600
static int32_t Script_GetMapLandmarkInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMapLandmarkInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    WorldMapLandmarkInfo* landmark = index < CGWorldMap::m_numLandmarks ? &CGWorldMap::m_landmarks.m_data[index] : NULL;

    if (landmark) {
        if (landmark->m_type == 1) {
            WorldSafeLocsRec* location = g_worldSafeLocsDB.GetRecord(landmark->m_id);

            if (location) {
                lua_pushstring(L, location->m_areaName);
                lua_pushnil(L);
                lua_pushnumber(L, 6.0);
                lua_pushnumber(L, landmark->m_x);
                lua_pushnumber(L, landmark->m_y);
                lua_pushnil(L);
                return 6;
            }
        } else if (landmark->m_type == 3) {
            ResearchSiteRec* site = g_researchSiteDB.GetRecord(landmark->m_id);

            if (site) {
                lua_pushstring(L, site->m_name);
                lua_pushnil(L);
                lua_pushnumber(L, site->m_areaPOIIconEnum);
                lua_pushnumber(L, landmark->m_x);
                lua_pushnumber(L, landmark->m_y);
                lua_pushnil(L);
                lua_pushnil(L);
                lua_pushnil(L);
            }

            return 8;
        } else if (landmark->m_type == 4) {
            TaxiNodesRec* node = g_taxiNodesDB.GetRecord(landmark->m_id);

            if (node) {
                int32_t icon = 178;

                if (node->unk_0020 & 0x2) {
                    if (!(node->unk_0020 & 0x1)) {
                        icon = 179;
                    }
                } else if (node->unk_0020 & 0x1) {
                    icon = 180;
                }

                lua_pushstring(L, node->m_name);
                lua_pushnil(L);
                lua_pushnumber(L, icon);
                lua_pushnumber(L, landmark->m_x);
                lua_pushnumber(L, landmark->m_y);
                lua_pushnil(L);
                lua_pushnil(L);
                lua_pushnil(L);
            }

            return 8;
        }

        const AreaPOIRec* poi = landmark->m_type == 2 ? MinimapGetGossipPoi() : g_areaPOIDB.GetRecord(landmark->m_id);

        if (poi) {
            lua_pushstring(L, poi->m_name);
            lua_pushstring(L, poi->m_description);
            uint32_t state = 0;
            int32_t icon = 0;

            if (landmark->m_type == 2 || (poi->m_flags & 0x80) || (CGWorldMap::m_currentZone == -1 && CGWorldMap::m_currentContinent != -2)) {
                if (poi->m_worldStateID) {
                    state = GetWorldState(poi->m_worldStateID) - 1;

                    if (state >= 9) {
                        state = 0;
                    }
                }

                icon = poi->m_icon[state];
            }

            lua_pushnumber(L, icon);
            lua_pushnumber(L, landmark->m_x);
            lua_pushnumber(L, landmark->m_y);

            if (poi->m_worldMapLink) {
                lua_pushnumber(L, poi->m_worldMapLink);
            } else {
                lua_pushnil(L);
            }

            lua_pushboolean(L, poi->m_flags & 0x200);
            int32_t graveyard = 0;

            if (poi->m_graveyardID && (poi->m_flags & 0x800)) {
                for (uint32_t i = 0; i < CGGameUI::s_numCemeteries; i++) {
                    if (CGGameUI::GetCemetery(i) == poi->m_graveyardID) {
                        graveyard = poi->m_graveyardID;
                        break;
                    }
                }
            }

            lua_pushnumber(L, graveyard);
            lua_pushnumber(L, poi->m_areaID);
            return 9;
        }
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return 8;
}

// OFFSET: 0x0091DFE0/0x14067FCB0
static int32_t Script_UpdateMapHighlight(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: UpdateMapHighlight(x, y)");
        return 0;
    }

    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);

    if (CGWorldMap::m_currentZone >= 0) {
        for (uint32_t i = 0; i < CGWorldMap::m_numOverlays; i++) {
            WorldMapOverlayRec* overlay = g_worldMapOverlayDB.GetRecord(CGWorldMap::m_overlays.m_data[i]);

            if (!overlay) {
                continue;
            }

            CRect rect;
            rect.maxX = overlay->m_hitRectRight * 0.00099800399f;
            rect.minX = overlay->m_hitRectLeft * 0.00099800399f;
            rect.minY = overlay->m_hitRectTop * 0.001497006f;
            rect.maxY = overlay->m_hitRectBottom * 0.001497006f;

            if (rect.maxX - rect.minX == 0.0f || rect.maxY - rect.minY == 0.0f) {
                continue;
            }

            if (!rect.IsPointInside(C2Vector(x, y))) {
                continue;
            }

            AreaTableRec* areaRec = g_areaTableDB.GetRecord(overlay->m_areaID[0]);

            if (areaRec) {
                lua_pushstring(L, areaRec->m_areaName);
                lua_pushnil(L);

                for (int32_t j = 0; j < 8; j++) {
                    lua_pushnumber(L, 0.0);
                }

                return 10;
            }
        }
    }

    int32_t highlight = CGWorldMap::GetMapHighlight(x, y);
    const WorldMapAreaRec* area;

    if (!highlight || !(area = PhaseShift_C_GetPhasedWorldMap(g_worldMapAreaDB.GetRecord(highlight)))) {
        lua_pushnil(L);
        lua_pushnil(L);

        for (int32_t j = 0; j < 8; j++) {
            lua_pushnumber(L, 0.0);
        }

        return 10;
    }

    if (CGWorldMap::m_currentZone >= 0 && CGWorldMap::GetWorldMapChildDepth(CGWorldMap::GetCurrentAreaID(), highlight, NULL) <= 0) {
        AreaTableRec* areaRec = g_areaTableDB.GetRecord(area->m_areaID);

        if (areaRec) {
            lua_pushstring(L, areaRec->m_areaName);
        } else {
            lua_pushnil(L);
        }

        lua_pushnil(L);

        for (int32_t j = 0; j < 6; j++) {
            lua_pushnumber(L, 0.0);
        }

        lua_pushnumber(L, area->unk_0030);
        lua_pushnumber(L, area->unk_0034);
        return 10;
    }

    if ((area->m_flags & 0x20) || (!fnotequal(area->m_locLeft, 0.0f) && !fnotequal(area->m_locRight, 0.0f) && !fnotequal(area->m_locTop, 0.0f) && !fnotequal(area->m_locBottom, 0.0f) && !CGWorldMap::IsContinent(area))) {
        lua_pushnil(L);
        lua_pushnil(L);

        for (int32_t j = 0; j < 8; j++) {
            lua_pushnumber(L, 0.0);
        }

        return 10;
    }

    if (area->m_areaID) {
        AreaTableRec* areaRec = g_areaTableDB.GetRecord(area->m_areaID);

        if (areaRec) {
            lua_pushstring(L, areaRec->m_areaName);
        } else {
            lua_pushnil(L);
        }
    } else {
        MapRec* map = g_mapDB.GetRecord(area->m_mapID);

        if (map) {
            lua_pushstring(L, map->m_mapName);
        } else {
            lua_pushnil(L);
        }
    }

    lua_pushstring(L, area->m_areaName);
    float width = 64.0f;
    float height = 64.0f;
    int32_t size = 128;
    WorldMapContinentRec* continentRec = NULL;

    if (CGWorldMap::IsContinent(area)) {
        for (uint32_t i = 0; i < static_cast<uint32_t>(g_worldMapContinentDB.m_numRecords); i++) {
            continentRec = g_worldMapContinentDB.GetRecordByIndex(i);

            if (continentRec->m_mapID == area->m_mapID) {
                width = static_cast<float>(continentRec->m_rightBoundary - continentRec->m_leftBoundary + 1);
                height = static_cast<float>(continentRec->m_bottomBoundary - continentRec->m_topBoundary + 1);
                size = static_cast<uint32_t>(512.0 * height / width);
                break;
            }
        }
    } else {
        width = area->m_locLeft - area->m_locRight;
        height = area->m_locTop - area->m_locBottom;
        size = static_cast<uint32_t>(128.0 * height / width);
    }

    uint8_t low = static_cast<uint8_t>(size);
    uint32_t textureSize = low;

    if (low) {
        if ((low - 1) & low) {
            for (int32_t bit = 7; bit >= 0; bit--) {
                if ((1 << bit) & low) {
                    textureSize = 1 << (bit + 1);
                    break;
                }
            }
        }

        while (8 * textureSize < low) {
            textureSize *= 2;
        }

        if (static_cast<uint32_t>(size) > 256) {
            textureSize += (size >> 8 << 8) + 256;
        }
    } else {
        textureSize = size;
    }

    if ((area->m_flags & 0x8) || !area->m_areaID) {
        if (width > height) {
            lua_pushnumber(L, 1.0);
            lua_pushnumber(L, height / width);
        } else {
            lua_pushnumber(L, width / height);
            lua_pushnumber(L, 1.0);
        }

        if (continentRec) {
            if (continentRec->m_mapID != -1) {
                float scale = continentRec->m_scale;
                float left = continentRec->m_leftBoundary * scale + (31.3125f - scale * 32.0f) + continentRec->m_continentOffset[0];
                float top = continentRec->m_topBoundary * scale + (20.875f - scale * 32.0f) + continentRec->m_continentOffset[1];
                lua_pushnumber(L, scale * width * 0.015968064f);
                lua_pushnumber(L, continentRec->m_scale * height * 0.023952097f);
                lua_pushnumber(L, left * 0.015968064f);
                lua_pushnumber(L, top * 0.023952097f);
                lua_pushnumber(L, 0.0);
                lua_pushnumber(L, 0.0);
                return 10;
            }

            lua_pushnumber(L, 1.0);
            lua_pushnumber(L, 1.5);
            lua_pushnumber(L, 0.0);
            lua_pushnumber(L, 0.0);
            lua_pushnumber(L, 0.0);
            lua_pushnumber(L, 0.0);
            return 10;
        }

        for (int32_t j = 0; j < 8; j++) {
            lua_pushnumber(L, 0.0);
        }

        return 10;
    }

    lua_pushnumber(L, 1.0);
    lua_pushnumber(L, static_cast<double>(size) / static_cast<double>(textureSize));
    const WorldMapAreaRec* parent = NULL;

    if (CGWorldMap::m_currentZone < 0) {
        int32_t displayMapID = area->m_displayMapID;

        for (uint32_t i = 0; i < static_cast<uint32_t>(g_worldMapAreaDB.m_numRecords); i++) {
            WorldMapAreaRec* rec = g_worldMapAreaDB.GetRecordByIndex(i);
            int32_t recMapID = rec->m_displayMapID;

            if (recMapID < 0) {
                recMapID = rec->m_mapID;
            }

            if (!rec->m_areaID && recMapID == (displayMapID >= 0 ? displayMapID : area->m_mapID)) {
                parent = rec;
                break;
            }
        }
    } else {
        parent = g_worldMapAreaDB.GetRecord(CGWorldMap::GetCurrentAreaID());
    }

    if (!parent) {
        for (int32_t j = 0; j < 8; j++) {
            lua_pushnumber(L, 0.0);
        }

        return 10;
    }

    float parentWidth = parent->m_locLeft - parent->m_locRight;
    float parentHeight = parent->m_locTop - parent->m_locBottom;
    float offsetX;
    float offsetY;

    if (area->m_displayMapID < 0) {
        offsetX = parent->m_locLeft - area->m_locLeft;
        offsetY = parent->m_locTop - area->m_locTop;
    } else {
        int32_t mapID;
        int32_t areaID;
        int32_t dungeonMapID;
        bool forceDungeonMap;
        float transformedX;
        float transformedY;
        CGWorldMap::DetermineAlternateMapPosition(area->m_mapID, 0, area->m_locTop, area->m_locLeft, mapID, areaID, transformedX, transformedY, dungeonMapID, forceDungeonMap);
        offsetX = parent->m_locLeft - transformedY;
        offsetY = parent->m_locTop - transformedX;
    }

    float invWidth = 1.0f / parentWidth;
    lua_pushnumber(L, invWidth * width);
    float invHeight = 1.0f / parentHeight;
    lua_pushnumber(L, invHeight * height);
    lua_pushnumber(L, offsetX * invWidth);
    lua_pushnumber(L, offsetY * invHeight);
    lua_pushnumber(L, area->unk_0030);
    lua_pushnumber(L, area->unk_0034);
    return 10;
}

// OFFSET: 0x0091B7D0/0x14067CC60
static int32_t Script_GetMapZones(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMapZones(continentIndex)");
        return 0;
    }

    uint32_t continent = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t count = continent < CGWorldMap::m_continents.m_count ? CGWorldMap::m_continents.m_data[continent].m_zones.m_count : 0;
    lua_checkstack(L, count);

    for (uint32_t i = 0; i < count; i++) {
        lua_pushstring(L, CGWorldMap::GetZoneName(continent, i));
    }

    return count;
}

// OFFSET: 0x0091C370/0x14067DA70
static int32_t Script_GetMapOverlayInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMapOverlayInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    char texture[260];
    WorldMapOverlayRec* overlay = GetMapOverlayTexture(index, texture, sizeof(texture));

    if (overlay) {
        lua_pushstring(L, texture);
        lua_pushnumber(L, overlay->m_textureWidth);
        lua_pushnumber(L, overlay->m_textureHeight);
        lua_pushnumber(L, overlay->m_offsetX);
        lua_pushnumber(L, overlay->m_offsetY);
    } else {
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 5;
}

// OFFSET: 0x0091F630/0x1406817F0
static int32_t Script_SetMapZoom(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetMapZoom(continentIndex [,zoneIndex])");
        return 0;
    }

    int32_t continent = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if ((continent > 0 && static_cast<uint32_t>(continent) >= CGWorldMap::m_continents.m_count)) {
        luaL_error(L, "Usage: SetMapZoom(continentIndex [,zoneIndex]) .. requested continent out of bounds");
        return 0;
    }

    if ((continent < 0 && continent < -2)) {
        luaL_error(L, "Usage: SetMapZoom(continentIndex [,zoneIndex]) .. requested continent out of bounds");
        return 0;
    }

    int32_t zone = -1;

    if (lua_isnumber(L, 2)) {
        zone = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    }

    CGWorldMap::SetMap(continent, zone, -1);
    return 0;
}

// OFFSET: 0x0091C470/0x14067DB90
static int32_t Script_CreateWorldMapArrowFrame(lua_State* L) {
    if (lua_type(L, 1) == LUA_TTABLE) {
        lua_rawgeti(L, 1, 0);
        CSimpleFrame* parent = static_cast<CSimpleFrame*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!parent) {
            luaL_error(L, "CreateWorldMapArrowFrame(): Couldn't find 'this' in parent object");
            return 0;
        }

        if (!parent->IsA(CSimpleFrame::GetObjectType())) {
            luaL_error(L, "CreateWorldMapArrowFrame(): Wrong object type, expected frame");
            return 0;
        }

        CGWorldMap::CreateArrowFrame(parent);
        return 0;
    }

    luaL_error(L, "Usage: CreateWorldMapArrowFrame(parent)");
    return 0;
}

// OFFSET: 0x0091C5E0/0x14067DD60
static int32_t Script_InitWorldMapPing(lua_State* L) {
    if (lua_type(L, 1) == LUA_TTABLE) {
        lua_rawgeti(L, 1, 0);
        CSimpleFrame* parent = static_cast<CSimpleFrame*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!parent) {
            luaL_error(L, "InitWorldMapPing(): Couldn't find 'this' in parent object");
            return 0;
        }

        if (!parent->IsA(CSimpleFrame::GetObjectType())) {
            luaL_error(L, "InitWorldMapPing(): Wrong object type, expected frame");
            return 0;
        }

        CLayoutFrame* layout = parent->GetLayoutFrameByName("WorldMapPing");

        if (!layout) {
            return 0;
        }

        CSimpleModel* ping = static_cast<CSimpleModel*>(layout);

        if (!ping) {
            return 0;
        }

        C3Vector position(layout->GetWidth() * 0.5f, ping->GetHeight() * 0.5f, 0.0f);
        ping->SetPosition(position);
        return 0;
    }

    luaL_error(L, "Usage: InitWorldMapPing(parent)");
    return 0;
}

// PROVEN: 1f07a6dc IDENTICAL/NONE
// OFFSET: 0x0091A2C0/0x14067B330
static int32_t Script_CanQueueForWintergrasp(lua_State* L) {
    if (GetWorldState(4375)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0091E850/0x1406807F0
static int32_t Script_GetCorpseMapPosition(lua_State* L) {
    C3Vector position(0.0f, 0.0f, 0.0f);
    int32_t mapID;
    int32_t actualMapID;
    CGGameUI::GetCorpseLocation(mapID, position, actualMapID);
    float x;
    float y;
    CGWorldMap::GetWorldPosition(position, mapID, &x, &y, NULL, false, 0);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

// OFFSET: 0x0091E8D0/0x1406808A0
static int32_t Script_GetDeathReleasePosition(lua_State* L) {
    C3Vector position(0.0f, 0.0f, 0.0f);
    int32_t mapID;
    CGGameUI::GetDeathReleaseLocation(mapID, position);
    float x;
    float y;
    CGWorldMap::GetWorldPosition(position, mapID, &x, &y, NULL, false, 0);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

// OFFSET: 0x0091C520/0x14067DC70
static int32_t Script_CreateMiniWorldMapArrowFrame(lua_State* L) {
    if (lua_type(L, 1) == LUA_TTABLE) {
        lua_rawgeti(L, 1, 0);
        CSimpleFrame* parent = static_cast<CSimpleFrame*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!parent) {
            luaL_error(L, "CreateMiniWorldMapArrowFrame(): Couldn't find 'this' in parent object");
            return 0;
        }

        if (!parent->IsA(CSimpleFrame::GetObjectType())) {
            luaL_error(L, "CreateMiniWorldMapArrowFrame(): Wrong object type, expected frame");
            return 0;
        }

        CGWorldMap::CreateMiniArrowFrame(parent);
        return 0;
    }

    luaL_error(L, "Usage: CreateMiniWorldMapArrowFrame(parent)");
    return 0;
}

// OFFSET: 0x0091C5D0/0x14067DD50
static int32_t Script_UpdateWorldMapArrowFrames(lua_State* L) {
    CGWorldMap::UpdateArrowFrames();
    return 0;
}

// OFFSET: 0x0091F6D0
static int32_t Script_ZoomOut(lua_State* L) {
    int32_t parent;

    if (CGWorldMap::m_currentOrphanZone == -1 || CGWorldMap::m_currentDungeonMap <= 0) {
        int32_t areaID = CGWorldMap::GetCurrentAreaID();

        if (CGWorldMap::m_currentContinent == -2) {
            WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(areaID);

            if (!area) {
                lua_pushboolean(L, 1);
                return 1;
            }

            parent = area->m_parentWorldMapID;

            if (!parent) {
                for (uint32_t i = 0; i < static_cast<uint32_t>(g_dungeonMapDB.m_numRecords); i++) {
                    DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecordByIndex(i);

                    if (dungeonMap && dungeonMap->m_mapID == area->m_mapID && dungeonMap->m_floorIndex == 1) {
                        parent = dungeonMap->m_parentWorldMapID;
                    }
                }

                if (!parent) {
                    lua_pushboolean(L, 1);
                    return 1;
                }
            }
        } else {
            WorldMapAreaRec* area;

            if (!areaID || !(area = g_worldMapAreaDB.GetRecord(areaID))) {
                return 0;
            }

            parent = area->m_parentWorldMapID;

            if (!parent) {
                AreaTableRec* areaRec;
                int32_t parentAreaID;

                if (!area->m_areaID || !(areaRec = g_areaTableDB.GetRecord(area->m_areaID)) || !(parentAreaID = areaRec->m_parentAreaID)) {
                    return 0;
                }

                uint32_t numAreas = g_worldMapAreaDB.m_numRecords;
                WorldMapAreaRec* rec = NULL;

                for (uint32_t i = 0; ; i++) {
                    if (i >= numAreas) {
                        return 0;
                    }

                    rec = g_worldMapAreaDB.GetRecordByIndex(i);

                    if (rec && rec->m_areaID == parentAreaID) {
                        break;
                    }
                }

                parent = rec->m_id;

                if (!parent) {
                    return 0;
                }
            }
        }
    } else {
        DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecord(CGWorldMap::m_currentDungeonMap);

        if (!dungeonMap || !dungeonMap->m_parentWorldMapID) {
            return 0;
        }

        parent = dungeonMap->m_parentWorldMapID;
    }

    WorldMapAreaRec* parentArea = g_worldMapAreaDB.GetRecord(parent);

    if (!parentArea) {
        return 0;
    }

    int32_t mapID = parentArea->m_mapID;

    if (fabs(parentArea->m_locLeft) >= 0.00000023841858f || fabs(parentArea->m_locRight) >= 0.00000023841858f || !fequal(parentArea->m_locTop, 0.0f) || !fequal(parentArea->m_locBottom, 0.0f)) {
        float newX = 0.0f;
        float newY = 0.0f;
        int32_t newAreaID;
        int32_t dungeonMapID;
        bool forceDungeonMap;
        CGWorldMap::DetermineAlternateMapPosition(mapID, parentArea->m_areaID, (parentArea->m_locBottom + parentArea->m_locTop) * 0.5f, (parentArea->m_locRight + parentArea->m_locLeft) * 0.5f, mapID, newAreaID, newX, newY, dungeonMapID, forceDungeonMap);
    }

    for (uint32_t continent = 0; continent < CGWorldMap::m_continents.m_count; continent++) {
        if (!CGWorldMap::IsMapOnContinent(mapID, continent)) {
            continue;
        }

        uint32_t numZones = CGWorldMap::GetNumZones(continent);

        for (uint32_t zone = 0; zone < numZones; zone++) {
            if (CGWorldMap::GetZoneID(continent, zone) == parentArea->m_id) {
                CGWorldMap::SetMap(continent, zone, -1);
                lua_pushboolean(L, 1);
                return 1;
            }
        }
    }

    if (CGWorldMap::m_currentContinent == -2) {
        CGWorldMap::SetMap(-2, parentArea->m_id, -1);
        lua_pushboolean(L, 1);
        return 1;
    }

    return 0;
}

// OFFSET: 0x0091FA00
static int32_t Script_SetMapByID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetMapByID(mapID)");
        return 0;
    }

    int32_t mapID = static_cast<int32_t>(lua_tonumber(L, 1));
    WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(mapID);

    if (!area) {
        return 0;
    }

    if ((area->m_flags & 0x4) || area->m_defaultDungeonFloor == -1) {
        CGWorldMap::SetMap(-2, mapID, -1);
        lua_pushboolean(L, 1);
        return 1;
    }

    int32_t floor = area->m_defaultDungeonFloor;

    for (uint32_t continent = 0; continent < CGWorldMap::m_continents.m_count; continent++) {
        int32_t displayMapID = area->m_displayMapID;

        if (displayMapID < 0) {
            displayMapID = area->m_mapID;
        }

        if (!CGWorldMap::IsMapOnContinent(displayMapID, continent)) {
            continue;
        }

        if (floor > 0) {
            CGWorldMap::SetMap(-2, area->m_id, floor);
            lua_pushboolean(L, 1);
            return 1;
        }

        if (!area->m_areaID) {
            CGWorldMap::SetMap(continent, -1, -1);
            lua_pushboolean(L, 1);
            return 1;
        }

        uint32_t numZones = CGWorldMap::GetNumZones(continent);

        for (uint32_t zone = 0; zone < numZones; zone++) {
            if (CGWorldMap::GetZoneID(continent, zone) == area->m_id) {
                CGWorldMap::SetMap(continent, zone, -1);
                lua_pushboolean(L, 1);
                return 1;
            }
        }
    }

    for (uint32_t i = 0; i < static_cast<uint32_t>(g_dungeonMapDB.m_numRecords); i++) {
        DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecordByIndex(i);

        if (dungeonMap && dungeonMap->m_mapID == area->m_mapID && dungeonMap->m_floorIndex == 1) {
            CGWorldMap::SetMap(-2, mapID, dungeonMap->m_id);
            lua_pushboolean(L, 1);
            return 1;
        }
    }

    CGWorldMap::SetMap(-2, mapID, -1);
    lua_pushboolean(L, 1);
    return 1;
}

// OFFSET: 0x0091FC10/0x140681F10
static int32_t Script_ProcessMapClick(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: ProcessMapClick(x, y)");
        return 0;
    }

    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    CGWorldMap::ProcessClick(x, y);
    return 0;
}

// OFFSET: 0x0091FC80/0x140681F90
static int32_t Script_ClickLandmark(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ClickLandmark(mapLinkID)");
        return 0;
    }

    int32_t id = static_cast<uint32_t>(lua_tonumber(L, 1));
    DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecord(id);

    if (!dungeonMap) {
        return 0;
    }

    uint32_t count = CGWorldMap::m_orphanZones.m_count;

    for (uint32_t i = 0; i < count; i++) {
        WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(i < count ? CGWorldMap::m_orphanZones.m_data[i] : 0);

        if (area && area->m_mapID == dungeonMap->m_mapID) {
            CGWorldMap::SetMap(-2, area->m_id, id);
            return 0;
        }
    }

    return 0;
}

// OFFSET: 0x00920160/0x140682600
static int32_t Script_SetDungeonMapLevel(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetDungeonMapLevel(level)");
        return 0;
    }

    int32_t level = static_cast<int32_t>(lua_tonumber(L, 1));
    WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::m_currentOrphanZone);

    if (area && ((area->m_flags & 0x4) || area->m_defaultDungeonFloor == -1)) {
        if (--level < 0) {
            return 0;
        }

        CGWorldMap::SetDungeonLevel(level);
    } else if (level > 0) {
        CGWorldMap::SetDungeonLevel(level);
    }

    return 0;
}

// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_GetMapDebugObjectInfo(lua_State* L) {
    return 0;
}

// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_TeleportToDebugObject(lua_State* L) {
    return 0;
}

// PROVEN: 721de051 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_GetDebugZoneMap(lua_State* L) {
    return 0;
}

// OFFSET: 0x00919D10/0x14067AC70
static int32_t Script_GetNumberOfDetailTiles(lua_State* L) {
    lua_pushnumber(L, 12.0);
    return 1;
}

// OFFSET: 0x0091FD70/0x1406820B0
static int32_t Script_RefreshWorldMap(lua_State* L) {
    CGWorldMap::SetMap(CGWorldMap::m_currentContinent, CGWorldMap::m_currentZone, CGWorldMap::GetCurrentDungeonMap());
    return 0;
}

// OFFSET: 0x0091B880/0x14067CD20
static int32_t Script_GetMapNameByID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMapNameByID(mapID)");
        return 0;
    }

    const WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(static_cast<int32_t>(lua_tonumber(L, 1)));

    if (!area) {
        return 0;
    }

    const AreaTableRec* areaTable = g_areaTableDB.GetRecord(area->m_areaID);

    if (!areaTable) {
        return 0;
    }

    lua_pushstring(L, areaTable->m_areaName);
    return 1;
}

// OFFSET: 0x0091BED0/0x14067D460
static int32_t Script_GetCurrentMapLevelRange(lua_State* L) {
    const WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetCurrentAreaID());

    if (area) {
        int32_t minLevel = area->unk_0030;

        if (minLevel <= 0 && area->unk_0034 <= 0) {
            return 0;
        }

        lua_pushnumber(L, minLevel);
        lua_pushnumber(L, area->unk_0034);
        return 2;
    }

    return 0;
}

// OFFSET: 0x0091C6F0/0x14067DEC0
static int32_t Script_GetOutdoorPVPWaitTime(lua_State* L) {
    uint32_t i;

    for (i = 0; i < 2; i++) {
        if (CGWorldMap::GetCurrentAreaID() == s_outdoorPVPZones[i][0] && GetWorldState(s_outdoorPVPZones[i][1])) {
            break;
        }
    }

    if (i < 2) {
        int32_t wait = GetWorldState(s_outdoorPVPZones[i][2]) - CGWorldStateInfo::GetUITime();

        if (wait < 0) {
            wait = 0;
        }

        lua_pushnumber(L, wait);
        return 1;
    }

    return 0;
}

// STATIC: 0x00D112C8
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetMapContinents", &Script_GetMapContinents },
    { "GetMapZones", &Script_GetMapZones },
    { "SetMapZoom", &Script_SetMapZoom },
    { "ZoomOut", &Script_ZoomOut },
    { "SetDungeonMapLevel", &Script_SetDungeonMapLevel },
    { "GetNumDungeonMapLevels", &Script_GetNumDungeonMapLevels },
    { "DungeonUsesTerrainMap", &Script_DungeonUsesTerrainMap },
    { "SetMapToCurrentZone", &Script_SetMapToCurrentZone },
    { "GetMapInfo", &Script_GetMapInfo },
    { "GetCurrentMapContinent", &Script_GetCurrentMapContinent },
    { "GetCurrentMapAreaID", &Script_GetCurrentMapAreaID },
    { "GetCurrentMapZone", &Script_GetCurrentMapZone },
    { "GetCurrentMapDungeonLevel", &Script_GetCurrentMapDungeonLevel },
    { "GetCurrentMapLevelRange", &Script_GetCurrentMapLevelRange },
    { "GetNumberOfDetailTiles", &Script_GetNumberOfDetailTiles },
    { "SetMapByID", &Script_SetMapByID },
    { "GetMapNameByID", &Script_GetMapNameByID },
    { "IsZoomOutAvailable", &Script_IsZoomOutAvailable },
    { "ProcessMapClick", &Script_ProcessMapClick },
    { "UpdateMapHighlight", &Script_UpdateMapHighlight },
    { "GetPlayerMapPosition", &Script_GetPlayerMapPosition },
    { "GetCorpseMapPosition", &Script_GetCorpseMapPosition },
    { "GetDeathReleasePosition", &Script_GetDeathReleasePosition },
    { "GetNumMapLandmarks", &Script_GetNumMapLandmarks },
    { "GetMapLandmarkInfo", &Script_GetMapLandmarkInfo },
    { "GetNumMapOverlays", &Script_GetNumMapOverlays },
    { "GetMapOverlayInfo", &Script_GetMapOverlayInfo },
    { "CreateWorldMapArrowFrame", &Script_CreateWorldMapArrowFrame },
    { "InitWorldMapPing", &Script_InitWorldMapPing },
    { "CreateMiniWorldMapArrowFrame", &Script_CreateMiniWorldMapArrowFrame },
    { "UpdateWorldMapArrowFrames", &Script_UpdateWorldMapArrowFrames },
    { "PositionWorldMapArrowFrame", &Script_PositionWorldMapArrowFrame },
    { "PositionMiniWorldMapArrowFrame", &Script_PositionMiniWorldMapArrowFrame },
    { "ShowWorldMapArrowFrame", &Script_ShowWorldMapArrowFrame },
    { "ShowMiniWorldMapArrowFrame", &Script_ShowMiniWorldMapArrowFrame },
    { "ClickLandmark", &Script_ClickLandmark },
    { "GetNumMapDebugObjects", &Script_GetNumMapDebugObjects },
    { "GetMapDebugObjectInfo", &Script_GetMapDebugObjectInfo },
    { "TeleportToDebugObject", &Script_TeleportToDebugObject },
    { "HasDebugZoneMap", &Script_HasDebugZoneMap },
    { "GetDebugZoneMap", &Script_GetDebugZoneMap },
    { "RefreshWorldMap", &Script_RefreshWorldMap },
    { "GetOutdoorPVPWaitTime", &Script_GetOutdoorPVPWaitTime },
    { "CanQueueForWintergrasp", &Script_CanQueueForWintergrasp },
};

// OFFSET: 0x0091A310
void WorldMapRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x0091A340
void WorldMapUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
