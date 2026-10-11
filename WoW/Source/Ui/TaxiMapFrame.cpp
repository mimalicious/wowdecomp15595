#include "TaxiMapFrame.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/TaxiNodesRec.h"
#include "WoW/Source/DB/Rec/TaxiPathRec.h"
#include "WoW/Source/DB/Rec/TaxiPathNodeRec.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Base/Types.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/WorldMapContinentRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/WorldMap.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cmath>
#include <cstdlib>

// STATIC: 0x00F75E48
CTexture* CGTaxiMap::m_mapTexture;

// STATIC: 0x00F75E4C
uint32_t CGTaxiMap::m_numNodes;

// STATIC: 0x00F762A4
TSGrowableArray<TaxiWaypoint> CGTaxiMap::m_nodes;

// STATIC: 0x00F762E4
TSFixedArray<TSGrowableArray<TAXIROUTELINE>> CGTaxiMap::m_routeLines;

// STATIC: 0x00F76260
WOWGUID CGTaxiMap::m_unit;

// STATIC: 0x00DD5018
uint32_t CGTaxiMap::m_maxNodeID;

// STATIC: 0x00F75E50
static int32_t s_mapID;

// STATIC: 0x00F75E54
static TaxiNodesRec* s_currentNode;

// STATIC: 0x00F75E58
static int32_t s_initialized;

// STATIC: 0x00F76268
static CRect s_taxiBounds;

// STATIC: 0x00D16868
static int32_t s_selectedNode = -1;

// STATIC: 0x00F76278
static TSHashTable<TAXIPATHNODESCLIENT, HASHKEY_NONE> s_pathNodes;

// STATIC: 0x00F762B4
static TSGrowableArray<uint8_t> s_nodeMask;

// STATIC: 0x00F762C4
static TSGrowableArray<TAXILINE> s_taxiLines;

// STATIC: 0x00F762D4
static TSGrowableArray<bool> s_unkF762D4;

// OFFSET: 0x009E2260
static float GetNodeX(const TaxiNodesRec* node) {
    int32_t mapID;
    float x;
    float y;
    int32_t dungeonMapID;
    bool forceDungeonMap;
    CGWorldMap::DetermineAlternateMapPosition(node->m_continentID, node->m_pos[0], node->m_pos[1], mapID, x, y, dungeonMapID, forceDungeonMap);
    return x;
}

// OFFSET: 0x009E22A0
static float GetNodeY(const TaxiNodesRec* node) {
    int32_t mapID;
    float x;
    float y;
    int32_t dungeonMapID;
    bool forceDungeonMap;
    CGWorldMap::DetermineAlternateMapPosition(node->m_continentID, node->m_pos[0], node->m_pos[1], mapID, x, y, dungeonMapID, forceDungeonMap);
    return y;
}

// OFFSET: 0x008A61E0
static int CompareTaxiPath(const void* a, const void* b) {
    const TaxiPathRec* l = static_cast<const TaxiPathRec*>(a);
    const TaxiPathRec* r = static_cast<const TaxiPathRec*>(b);
    int32_t result = l->m_fromTaxiNode - r->m_fromTaxiNode;

    if (!result) {
        result = l->m_toTaxiNode - r->m_toTaxiNode;
    }

    return result;
}

// OFFSET: 0x009E25B0/0x140779880
static TaxiPathRec* FindTaxiPath(uint32_t from, uint32_t to) {
    TaxiPathRec key;
    key.m_fromTaxiNode = from;
    key.m_toTaxiNode = to;
    return static_cast<TaxiPathRec*>(bsearch(&key, g_taxiPathDB.GetRecordByIndex(0), g_taxiPathDB.m_numRecords, sizeof(TaxiPathRec), CompareTaxiPath));
}

// INLINED
static bool TaxiRouteExists(uint32_t from, uint32_t to) {
    return from && from <= CGTaxiMap::m_maxNodeID && to && to <= CGTaxiMap::m_maxNodeID && FindTaxiPath(from, to);
}

// OFFSET: 0x009E25F0
static TaxiNodesRec* GetNodeMaskBit(int32_t nodeID, int32_t* byte, int32_t* bit) {
    TaxiNodesRec* node = g_taxiNodesDB.GetRecord(nodeID);

    if (!node) {
        return NULL;
    }

    *byte = (nodeID - 1) / 8;
    *bit = (nodeID - 1) % 8;
    return node;
}

// OFFSET: 0x009E2650
uint32_t GetRouteSegmentCount(uint32_t index) {
    if (index < CGTaxiMap::m_numNodes) {
        if (!CGTaxiMap::m_nodes.m_data[index].m_noDirectPath) {
            return 1;
        }

        return CGTaxiMap::m_nodes.m_data[index].m_numHops - 1;
    }

    return 0;
}

// OFFSET: 0x009E4430/0x14077BB20
static void FreeData() {
    if (!s_initialized) {
        return;
    }

    CGTaxiMap::m_numNodes = 0;
    s_nodeMask.Clear();
    s_unkF762D4.Clear();
    s_taxiLines.Clear();

    for (uint32_t i = 0; i < CGTaxiMap::m_maxNodeID; i++) {
        if (CGTaxiMap::m_nodes.m_data[i].m_path) {
            SMemFree(CGTaxiMap::m_nodes.m_data[i].m_path, "delete[]", -1, 0);
        }

        CGTaxiMap::m_routeLines.m_data[i].Clear();
    }

    CGTaxiMap::m_routeLines.Clear();
    CGTaxiMap::m_nodes.Clear();
    s_pathNodes.Clear();
    s_initialized = 0;
}

// OFFSET: 0x009E4EA0/0x14077C9A0
static void InitializeData(uint32_t nodeMaskSize) {
    if (s_initialized) {
        return;
    }

    CGTaxiMap::m_maxNodeID = g_taxiNodesDB.m_maxID;
    CGTaxiMap::m_nodes.SetCount(CGTaxiMap::m_maxNodeID);

    for (uint32_t i = 0; i < CGTaxiMap::m_maxNodeID; i++) {
        CGTaxiMap::m_nodes.m_data[i].m_path = NULL;
    }

    s_nodeMask.SetCount(nodeMaskSize);
    s_unkF762D4.SetCount(CGTaxiMap::m_maxNodeID);
    CGTaxiMap::m_routeLines.SetCount(CGTaxiMap::m_maxNodeID);
    int32_t lastPathID = -1;
    TAXIPATHNODESCLIENT* pathNodes = NULL;

    uint32_t numRecords = g_taxiPathNodeDB.m_numRecords;

    for (uint32_t i = 0; i < numRecords; i++) {
        TaxiPathNodeRec* node = g_taxiPathNodeDB.GetRecordByIndex(i);
        int32_t pathID = node->m_pathID;

        if (pathID != lastPathID) {
            pathNodes = s_pathNodes.New(pathID, HASHKEY_NONE(), 0, 0);
            pathNodes->m_firstNode = node;
            pathNodes->m_numNodes = 1;
            lastPathID = node->m_pathID;
        } else {
            pathNodes->m_numNodes++;
        }
    }

    s_initialized = 1;
}

// OFFSET: 0x009E22E0/0x140779490
void CGTaxiMap::InitializeGame() {
    CGTaxiMap::m_unit.m_guid = 0;
    CGTaxiMap::m_numNodes = 0;
}

// INLINED
void CGTaxiMap::EnterWorld() {
}

// OFFSET: 0x009E2690/0x140779900
const char* CGTaxiMap::GetNodeType(uint32_t index) {
    if (index >= CGTaxiMap::m_numNodes) {
        return "NONE";
    }

    int32_t nodeID = CGTaxiMap::m_nodes.m_data[index].m_rec->m_id;

    if (nodeID == s_currentNode->m_id) {
        return "CURRENT";
    }

    int32_t byte;
    int32_t bit;

    if (!CGTaxiMap::m_nodes.m_data[index].m_reachable || !GetNodeMaskBit(nodeID, &byte, &bit)) {
        return "NONE";
    }

    if (s_nodeMask.m_data[byte] & (1 << bit)) {
        return "REACHABLE";
    }

    return "DISTANT";
}

// OFFSET: 0x009E2D20/0x140779F20
int32_t CGTaxiMap::GetNodeCost(uint32_t index) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    CGUnit_C* taxiUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGTaxiMap::m_unit, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!taxiUnit) {
        return 0;
    }

    uint32_t nodeID = CGTaxiMap::m_nodes.m_data[index].m_rec->m_id;

    if (nodeID && nodeID <= CGTaxiMap::m_maxNodeID && FindTaxiPath(s_currentNode->m_id, nodeID)) {
        float scale = 1.0f - taxiUnit->GetPlayerDiscount(player);
        return CMath::fint_(FindTaxiPath(s_currentNode->m_id, nodeID)->m_cost * scale);
    }

    if (!CGTaxiMap::m_nodes.m_data[index].m_noDirectPath) {
        return 0;
    }

    int32_t reaction = taxiUnit->UnitReaction(player);
    FactionTemplateRec* factionTemplate = g_factionTemplateDB.GetRecord(taxiUnit->m_unit->m_unitFieldFactiontemplate);

    if (factionTemplate && !CGUnit_C::FactionHasReputation(factionTemplate->m_faction)) {
        reaction = 3;
    }

    int32_t cost = CGTaxiMap::m_nodes.m_data[index].m_costs[0];

    if (reaction > 3) {
        cost = CGTaxiMap::m_nodes.m_data[index].m_costs[reaction - 3];
    }

    return cost;
}

// OFFSET: 0x009E2EA0/0x14077A0D0
void CGTaxiMap::TakeTaxi(uint32_t index) {
    if (index >= CGTaxiMap::m_numNodes) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player->m_mountDisplayId > 0 && !(player->m_clientFlags & 0x10000000)) {
        CGGameUI::DisplayError(ERR_TAXIPLAYERALREADYMOUNTED);
        return;
    }

    uint32_t source = s_currentNode->m_id;
    uint32_t destination = CGTaxiMap::m_nodes.m_data[index].m_rec->m_id;

    if (source == destination) {
        CGGameUI::DisplayError(ERR_TAXISAMENODE);
    } else if (!TaxiRouteExists(source, destination)) {
        int32_t cost = CGTaxiMap::GetNodeCost(index);

        if (CGTaxiMap::m_nodes.m_data[index].m_noDirectPath && CGTaxiMap::m_nodes.m_data[index].m_path && cost >= 0) {
            if (static_cast<uint64_t>(cost) > player->GetCoinage()) {
                CGGameUI::DisplayError(ERR_TAXINOTENOUGHMONEY);
            } else {
                player->StartTaxiExpress(CGTaxiMap::m_unit, CGTaxiMap::m_nodes.m_data[index].m_path, CGTaxiMap::m_nodes.m_data[index].m_numHops);
            }
        } else {
            CGGameUI::DisplayError(ERR_TAXINOSUCHPATH);
        }
    } else {
        player->StartTaxi(CGTaxiMap::m_unit, s_currentNode->m_id, CGTaxiMap::m_nodes.m_data[index].m_rec->m_id);
    }
}

// OFFSET: 0x009E3440/0x14077A710
float CGTaxiMap::CalculateAccurateDistance(uint32_t index, uint32_t parent) {
    C3Vector prev(0.0f, 0.0f, 0.0f);
    C3Vector pos(0.0f, 0.0f, 0.0f);
    float distance = 0.0f;
    TaxiPathRec* path = FindTaxiPath(CGTaxiMap::m_nodes.m_data[parent].m_rec->m_id, CGTaxiMap::m_nodes.m_data[index].m_rec->m_id);

    if (!path) {
        return 9.9999998e10f;
    }

    TAXIPATHNODESCLIENT* pathNodes = s_pathNodes.Ptr(path->m_id, HASHKEY_NONE());

    if (!pathNodes) {
        return 9.9999998e10f;
    }

    int32_t numNodes = pathNodes->m_numNodes;
    TaxiPathNodeRec* nodes = pathNodes->m_firstNode;

    for (int32_t i = 0; i < numNodes; i++) {
        if (i) {
            int32_t mapID;
            int32_t dungeonMapID;
            bool forceDungeonMap;
            CGWorldMap::DetermineAlternateMapPosition(nodes[i - 1].m_continentID, nodes[i - 1].m_loc[0], nodes[i - 1].m_loc[1], mapID, prev.x, prev.y, dungeonMapID, forceDungeonMap);
            prev.z = nodes[i - 1].m_loc[2];
            CGWorldMap::DetermineAlternateMapPosition(nodes[i].m_continentID, nodes[i].m_loc[0], nodes[i].m_loc[1], mapID, pos.x, pos.y, dungeonMapID, forceDungeonMap);
            pos.z = nodes[i].m_loc[2];
            distance = sqrt((pos - prev).SquaredMag()) + distance;
        }
    }

    return distance;
}

// OFFSET: 0x009E35C0/0x14077A950
void CGTaxiMap::RecursePath(uint32_t index, uint32_t parent, int32_t cost) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    TaxiNodesRec* node = CGTaxiMap::m_nodes.m_data[index].m_rec;

    if (node && player) {
        FactionTemplateRec* factionTemplate = g_factionTemplateDB.GetRecord(player->m_unit->m_unitFieldFactiontemplate);

        if (factionTemplate) {
            if (factionTemplate->m_factionGroup == 3) {
                if (!node->m_mountCreatureID[1]) {
                    return;
                }
            } else if (factionTemplate->m_factionGroup == 5) {
                if (!node->m_mountCreatureID[0]) {
                    return;
                }
            }
        }
    }

    TaxiWaypoint& parentNode = CGTaxiMap::m_nodes.m_data[parent];
    uint32_t hops = parentNode.m_numHops + 1;
    float distance = CGTaxiMap::CalculateAccurateDistance(index, parent) + parentNode.m_distance;

    if (CGTaxiMap::m_nodes.m_data[index].m_path) {
        if (CGTaxiMap::m_nodes.m_data[index].m_distance > distance) {
            SMemFree(CGTaxiMap::m_nodes.m_data[index].m_path, "delete[]", -1, 0);
            CGTaxiMap::m_nodes.m_data[index].m_path = new (__FILE__, __LINE__) uint32_t[hops];
            uint32_t i = 0;

            for (; i < hops - 1; i++) {
                CGTaxiMap::m_nodes.m_data[index].m_path[i] = CGTaxiMap::m_nodes.m_data[parent].m_path[i];
            }

            CGTaxiMap::m_nodes.m_data[index].m_path[i] = CGTaxiMap::m_nodes.m_data[index].m_rec->m_id;
            CGTaxiMap::m_nodes.m_data[index].m_numHops = hops;
            float scale = 1.0f;

            for (uint32_t j = 0; j < 5; j++) {
                CGTaxiMap::m_nodes.m_data[index].m_costs[j] = CGTaxiMap::m_nodes.m_data[parent].m_costs[j] + CMath::fint_(cost * scale);
                scale = scale - 0.050000001f;
            }

            CGTaxiMap::m_nodes.m_data[index].m_distance = distance;

            for (uint32_t j = 0; j < CGTaxiMap::m_numNodes; j++) {
                if (TaxiRouteExists(CGTaxiMap::m_nodes.m_data[index].m_rec->m_id, CGTaxiMap::m_nodes.m_data[j].m_rec->m_id)) {
                    CGTaxiMap::RecursePath(j, index, FindTaxiPath(CGTaxiMap::m_nodes.m_data[index].m_rec->m_id, CGTaxiMap::m_nodes.m_data[j].m_rec->m_id)->m_cost);
                }
            }
        }
    } else {
        CGTaxiMap::m_nodes.m_data[index].m_path = new (__FILE__, __LINE__) uint32_t[hops];
        CGTaxiMap::m_nodes.m_data[index].m_reachable = 1;
        uint32_t i = 0;

        for (; i < hops - 1; i++) {
            CGTaxiMap::m_nodes.m_data[index].m_path[i] = CGTaxiMap::m_nodes.m_data[parent].m_path[i];
        }

        CGTaxiMap::m_nodes.m_data[index].m_path[i] = CGTaxiMap::m_nodes.m_data[index].m_rec->m_id;
        CGTaxiMap::m_nodes.m_data[index].m_numHops = hops;
        float scale = 1.0f;

        for (uint32_t j = 0; j < 5; j++) {
            CGTaxiMap::m_nodes.m_data[index].m_costs[j] = CGTaxiMap::m_nodes.m_data[parent].m_costs[j] + CMath::fint_(cost * scale);
            scale = scale - 0.050000001f;
        }

        CGTaxiMap::m_nodes.m_data[index].m_distance = distance;

        for (uint32_t j = 0; j < CGTaxiMap::m_numNodes; j++) {
            if (TaxiRouteExists(CGTaxiMap::m_nodes.m_data[index].m_rec->m_id, CGTaxiMap::m_nodes.m_data[j].m_rec->m_id)) {
                CGTaxiMap::RecursePath(j, index, FindTaxiPath(CGTaxiMap::m_nodes.m_data[index].m_rec->m_id, CGTaxiMap::m_nodes.m_data[j].m_rec->m_id)->m_cost);
            }
        }
    }
}

// OFFSET: 0x009E3A50/0x14077AEC0
void CGTaxiMap::GeneratePaths() {
    uint32_t start = 0;

    for (uint32_t i = 0; i < CGTaxiMap::m_numNodes; i++) {
        if (CGTaxiMap::m_nodes.m_data[i].m_rec->m_id == s_currentNode->m_id) {
            start = i;
            break;
        }
    }

    for (uint32_t i = 0; i < 5; i++) {
        CGTaxiMap::m_nodes.m_data[start].m_costs[i] = 0;
    }

    CGTaxiMap::m_nodes.m_data[start].m_numHops = 1;
    CGTaxiMap::m_nodes.m_data[start].m_path = new (__FILE__, __LINE__) uint32_t[1];
    CGTaxiMap::m_nodes.m_data[start].m_path[0] = s_currentNode->m_id;
    CGTaxiMap::m_nodes.m_data[start].m_reachable = 1;
    CGTaxiMap::m_nodes.m_data[start].m_distance = 0.0f;

    for (uint32_t i = 0; i < CGTaxiMap::m_numNodes; i++) {
        if (TaxiRouteExists(s_currentNode->m_id, CGTaxiMap::m_nodes.m_data[i].m_rec->m_id)) {
            CGTaxiMap::RecursePath(i, start, FindTaxiPath(s_currentNode->m_id, CGTaxiMap::m_nodes.m_data[i].m_rec->m_id)->m_cost);
        }
    }
}

// OFFSET: 0x009E3C00/0x14077B0B0
void CGTaxiMap::TaxiNodeSetCurrent(uint32_t index) {
    s_selectedNode = -1;

    if (index >= CGTaxiMap::m_numNodes || !CGTaxiMap::m_nodes.m_data[index].m_reachable) {
        return;
    }

    s_selectedNode = index;
    s_taxiLines.Clear();
    CGTaxiMap::m_routeLines.m_data[index].Clear();

    if (!CGTaxiMap::m_nodes.m_data[index].m_noDirectPath) {
        TAXILINE* line = s_taxiLines.New();
        line->m_src = s_currentNode;
        line->m_dest = CGTaxiMap::m_nodes.m_data[s_selectedNode].m_rec;
        TAXIROUTELINE* route = CGTaxiMap::m_routeLines.m_data[index].New();
        float srcX = GetNodeX(s_taxiLines.m_data[0].m_src);
        route->m_src = C3Vector((s_taxiBounds.maxY - GetNodeY(s_taxiLines.m_data[0].m_src)) / (s_taxiBounds.maxY - s_taxiBounds.minY), (srcX - s_taxiBounds.minX) / (s_taxiBounds.maxX - s_taxiBounds.minX), 0.0f);
        float destX = GetNodeX(s_taxiLines.m_data[0].m_dest);
        route->m_dest = C3Vector((s_taxiBounds.maxY - GetNodeY(s_taxiLines.m_data[0].m_dest)) / (s_taxiBounds.maxY - s_taxiBounds.minY), (destX - s_taxiBounds.minX) / (s_taxiBounds.maxX - s_taxiBounds.minX), 0.0f);
        return;
    }

    for (uint32_t i = 0; i < GetRouteSegmentCount(index); i++) {
        TAXILINE* line = s_taxiLines.New();
        line->m_src = g_taxiNodesDB.GetRecord(CGTaxiMap::m_nodes.m_data[s_selectedNode].m_path[i]);
        line->m_dest = g_taxiNodesDB.GetRecord(CGTaxiMap::m_nodes.m_data[s_selectedNode].m_path[i + 1]);
        TAXIROUTELINE* route = CGTaxiMap::m_routeLines.m_data[index].New();
        float srcY = GetNodeY(s_taxiLines.m_data[i].m_src);
        route->m_src = C3Vector((s_taxiBounds.maxY - srcY) / (s_taxiBounds.maxY - s_taxiBounds.minY), (GetNodeX(s_taxiLines.m_data[i].m_src) - s_taxiBounds.minX) / (s_taxiBounds.maxX - s_taxiBounds.minX), 0.0f);
        float destY = GetNodeY(s_taxiLines.m_data[i].m_dest);
        route->m_dest = C3Vector((s_taxiBounds.maxY - destY) / (s_taxiBounds.maxY - s_taxiBounds.minY), (GetNodeX(s_taxiLines.m_data[i].m_dest) - s_taxiBounds.minX) / (s_taxiBounds.maxX - s_taxiBounds.minX), 0.0f);
    }
}

// OFFSET: 0x009E4610
void CGTaxiMap::ShutdownGame() {
    if (CGTaxiMap::m_mapTexture) {
        TextureRelease(CGTaxiMap::m_mapTexture);
        CGTaxiMap::m_mapTexture = NULL;
    }

    FreeData();
}

// OFFSET: 0x009E4640/0x14077BE40
void CGTaxiMap::CloseMap() {
    if (!CGTaxiMap::m_unit) {
        return;
    }

    CGGameUI::ClearInteractTarget(CGTaxiMap::m_unit, 0);
    CGTaxiMap::m_unit.m_guid = 0;

    for (uint32_t i = 0; i < CGTaxiMap::m_numNodes; i++) {
        if (CGTaxiMap::m_nodes.m_data[i].m_path) {
            SMemFree(CGTaxiMap::m_nodes.m_data[i].m_path, "delete[]", -1, 0);
            CGTaxiMap::m_nodes.m_data[i].m_path = NULL;
        }
    }

    FreeData();
    FrameScript_SignalEvent(SCRIPT_EVENT_TAXIMAP_CLOSED, NULL);
}

// OFFSET: 0x009E47F0/0x14077C0B0
static bool LoadMapTexture() {
    char fileName[260];
    SStrPrintf(fileName, sizeof(fileName), "Interface\\TaxiFrame\\TAXIMAP%d.blp", s_mapID);

    if (CGTaxiMap::m_mapTexture) {
        if (TextureIsSame(CGTaxiMap::m_mapTexture, fileName)) {
            return true;
        }

        TextureRelease(CGTaxiMap::m_mapTexture);
    }

    CStatus status;
    CGTaxiMap::m_mapTexture = TextureCreate(fileName, CGxTexFlags(), &status, 0, NULL, 2);
    return CGTaxiMap::m_mapTexture != NULL;
}

// OFFSET: 0x009E48B0/0x14077C1A0
void CGTaxiMap::LeaveWorld() {
    CGTaxiMap::ShutdownGame();
    CGTaxiMap::CloseMap();
}

// OFFSET: 0x009E50E0/0x14077CE20
void CGTaxiMap::SetupMap(const WOWGUID& unit, uint32_t currentNode, const uint8_t* nodeMask, int32_t nodeMaskSize) {
    if (CGTaxiMap::m_unit == unit) {
        return;
    }

    if (CGTaxiMap::m_unit) {
        CGTaxiMap::CloseMap();
    }

    InitializeData(nodeMaskSize);
    CGTaxiMap::m_numNodes = 0;

    for (uint32_t i = 0; i < s_nodeMask.Count(); i++) {
        s_nodeMask.m_data[i] = nodeMask[i];
    }

    s_currentNode = g_taxiNodesDB.GetRecord(currentNode);
    int32_t mapID;
    float x;
    float y;
    int32_t dungeonMapID;
    bool forceDungeonMap;
    CGWorldMap::DetermineAlternateMapPosition(s_currentNode->m_continentID, s_currentNode->m_pos[0], s_currentNode->m_pos[1], mapID, x, y, dungeonMapID, forceDungeonMap);
    s_mapID = mapID;

    for (uint32_t i = 0; i < CGTaxiMap::m_maxNodeID; i++) {
        CGTaxiMap::m_routeLines.m_data[i].SetCount(0);
    }

    WorldMapContinentRec* continent = NULL;
    uint32_t index = 0;

    for (; index < static_cast<uint32_t>(g_worldMapContinentDB.m_numRecords); index++) {
        continent = g_worldMapContinentDB.GetRecordByIndex(index);

        if (continent->m_mapID == s_mapID) {
            break;
        }
    }

    if (index == g_worldMapContinentDB.m_numRecords) {
        return;
    }

    CRect bounds;
    bounds.minY = continent->m_taxiMin[1];
    bounds.minX = continent->m_taxiMin[0];
    bounds.maxY = continent->m_taxiMax[1];
    bounds.maxX = continent->m_taxiMax[0];
    s_taxiBounds = bounds;

    if (bounds.maxX - bounds.minX == 0.0f || bounds.maxY - bounds.minY == 0.0f) {
        return;
    }

    for (uint32_t byte = 0; byte < s_nodeMask.Count(); byte++) {
        for (int32_t bit = 0; bit < 8; bit++) {
            int32_t mask = 1 << bit;

            if (!(mask & s_nodeMask.m_data[byte])) {
                continue;
            }

            TaxiNodesRec* node = g_taxiNodesDB.GetRecord(bit + 8 * byte + 1);

            if (!node) {
                continue;
            }

            MapRec* map = g_mapDB.GetRecord(node->m_continentID);
            CGWorldMap::DetermineAlternateMapPosition(node->m_continentID, node->m_pos[0], node->m_pos[1], mapID, x, y, dungeonMapID, forceDungeonMap);

            if (mapID != s_mapID || (map && map->m_expansionID > ClientServices::Connection()->m_accountExpansion)) {
                s_nodeMask.m_data[byte] &= ~mask;
                continue;
            }

            CGTaxiMap::m_nodes.m_data[CGTaxiMap::m_numNodes].m_rec = node;
            CGTaxiMap::m_nodes.m_data[CGTaxiMap::m_numNodes].m_x = (s_taxiBounds.maxY - GetNodeY(node)) / (s_taxiBounds.maxX - s_taxiBounds.minX);
            CGTaxiMap::m_nodes.m_data[CGTaxiMap::m_numNodes].m_y = (GetNodeX(node) - s_taxiBounds.minX) / (s_taxiBounds.maxY - s_taxiBounds.minY);

            if (TaxiRouteExists(s_currentNode->m_id, node->m_id)) {
                CGTaxiMap::m_nodes.m_data[CGTaxiMap::m_numNodes].m_noDirectPath = 0;
            } else {
                CGTaxiMap::m_nodes.m_data[CGTaxiMap::m_numNodes].m_noDirectPath = 1;
            }

            CGTaxiMap::m_nodes.m_data[CGTaxiMap::m_numNodes].m_path = NULL;

            for (uint32_t j = 0; j < 5; j++) {
                CGTaxiMap::m_nodes.m_data[CGTaxiMap::m_numNodes].m_costs[j] = 999999;
            }

            CGTaxiMap::m_nodes.m_data[CGTaxiMap::m_numNodes].m_numHops = 999999;
            CGTaxiMap::m_nodes.m_data[CGTaxiMap::m_numNodes].m_reachable = 0;
            CGTaxiMap::m_nodes.m_data[CGTaxiMap::m_numNodes].m_distance = 100000000.0f;
            CGTaxiMap::m_numNodes++;
        }
    }

    if (!CGTaxiMap::m_numNodes || !LoadMapTexture()) {
        CGTaxiMap::m_numNodes = 0;
        return;
    }

    CGTaxiMap::GeneratePaths();
    CGGossipInfo::ClearGossip();
    CGGameUI::SetInteractTarget(unit);
    CGTaxiMap::m_unit = unit;

    for (uint32_t i = 0; i < CGTaxiMap::m_numNodes; i++) {
        CGTaxiMap::TaxiNodeSetCurrent(i);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_TAXIMAP_OPENED, NULL);
    CGTutorial::TriggerTutorial(34);

    if (CGTaxiMap::m_numNodes <= 1) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (player->GetClientStandState()) {
        player->ChangeStandState(0);
    }

    if (player->m_mountDisplayId > 0 && !(player->m_clientFlags & 0x10000000)) {
        player->Dismount();
    }

    if (player->IsCancelShapeshiftAtFlightMaster()) {
        player->CancelShapeshift();
    }
}
