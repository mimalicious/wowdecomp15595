#include "WoW/Source/Ui/ResearchFrame.h"
#include <malloc.h>
#include <new>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/DungeonMapRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/QuestPOIBlobRec.h"
#include "WoW/Source/DB/Rec/QuestPOIPointRec.h"
#include "WoW/Source/DB/Rec/ResearchBranchRec.h"
#include "WoW/Source/DB/Rec/ResearchFieldRec.h"
#include "WoW/Source/DB/Rec/ResearchProjectRec.h"
#include "WoW/Source/DB/Rec/ResearchSiteRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/WorldMapAreaRec.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/WorldMap.h"

// STATIC: 0x00F76988
int32_t CGResearchFrame::m_itemCount;

// STATIC: 0x00F7698C
const ResearchFieldRec* CGResearchFrame::m_fieldRec;

// STATIC: 0x00F76990
const ResearchProjectRec* CGResearchFrame::m_projectRec;

// STATIC: 0x00F76994
bool CGResearchFrame::m_historyRequested;

// STATIC: 0x00F76995
bool CGResearchFrame::m_historyLoaded;

// STATIC: 0x00F76998
TSObjectAllocPool<CGResearchFrame::BRANCH>* CGResearchFrame::s_branchPool;

// STATIC: 0x00F7699C
TSObjectAllocPool<CGResearchFrame::PROJECT>* CGResearchFrame::s_projectPool;

// STATIC: 0x00F769A0
int32_t CGResearchFrame::m_skillLine;

// STATIC: 0x00F769A4
TSHashTable<CGResearchFrame::BRANCH, HASHKEY_NONE> CGResearchFrame::m_projectsByBranch;

// STATIC: 0x00F769D0
TSCGrowableArray<const ResearchBranchRec*, 32> CGResearchFrame::m_branches;

// STATIC: 0x00F76A54
TSHashTable<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE> CGResearchFrame::m_blobHash;

// OFFSET: 0x009F4810
void CGResearchFrame::SelectProject(int32_t projectID) {
    const ResearchProjectRec* project = g_researchProjectDB.GetRecord(projectID);
    CGResearchFrame::m_itemCount = 0;
    CGResearchFrame::m_projectRec = project;
}

// OFFSET: 0x009F4860/0x1407901C0
static int32_t FindFirstQuestPOIPoint(int32_t blobID) {
    int32_t high = g_questPOIPointDB.m_numRecords;
    int32_t low = 0;

    if (high <= 0) {
        return -1;
    }

    do {
        int32_t mid = (high + low) / 2;

        if (!(g_questPOIPointDB.GetRecordByIndex(0)[mid].m_questPOIBlobID >= blobID)) {
            low = mid + 1;
        } else {
            high = mid;
        }
    } while (low < high);

    if (low >= g_questPOIPointDB.m_numRecords || g_questPOIPointDB.GetRecordByIndex(0)[low].m_questPOIBlobID != blobID) {
        return -1;
    }

    while (low - 1 >= 0 && g_questPOIPointDB.GetRecordByIndex(0)[low - 1].m_questPOIBlobID == blobID) {
        low--;
    }

    return low;
}

// OFFSET: 0x009F4BC0
const ResearchBranchRec* CGResearchFrame::GetBranch(int32_t index) {
    if (index < 0 || index >= static_cast<int32_t>(CGResearchFrame::m_branches.Count())) {
        return NULL;
    }

    return CGResearchFrame::m_branches[index];
}

// OFFSET: 0x009F4C00/0x140790610
bool CGResearchFrame::ApplyItemToProject() {
    if (!CGResearchFrame::m_projectRec) {
        return false;
    }

    const ResearchBranchRec* branch = g_researchBranchDB.GetRecord(CGResearchFrame::m_projectRec->m_researchBranchID);

    if (!branch) {
        return false;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    int32_t count = CGResearchFrame::m_itemCount + 1;

    if (count > player->m_bag.GetItemTypeCount(branch->m_itemID, 6) || count > CGResearchFrame::m_projectRec->m_numSockets) {
        return false;
    }

    CGResearchFrame::m_itemCount = count;
    FrameScript_SignalEvent(SCRIPT_EVENT_ARTIFACT_UPDATE, NULL);

    return true;
}

// OFFSET: 0x009F4CA0/0x140790700
const ResearchProjectRec* CGResearchFrame::GetActiveProject(int32_t branchID) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return NULL;
    }

    for (int32_t i = 0; i < 16; i++) {
        const ResearchProjectRec* project = g_researchProjectDB.GetRecord(player->m_player->m_playerFieldResearching1[16 * CGResearchFrame::m_fieldRec->m_slot + i]);

        if (project && project->m_researchBranchID == branchID) {
            return project;
        }
    }

    return NULL;
}

// OFFSET: 0x009F4D20/0x1407907A0
bool CGResearchFrame::IsInsideResearchSite(int32_t siteId) {
    const ResearchSiteRec* site = g_researchSiteDB.GetRecord(siteId);
    uint32_t count = 0;

    if (!site || site->m_mapID != CGGameUI::m_iCurrentMapID) {
        return false;
    }

    const QuestPOIBlobRec* blob = g_questPOIBlobDB.GetRecord(site->m_questPOIBlobID);
    C3Vector* verts = static_cast<C3Vector*>(_alloca(sizeof(C3Vector) * blob->m_numPoints));
    int32_t index = FindFirstQuestPOIPoint(blob->m_id);

    if (index < 0) {
        return false;
    }

    const QuestPOIPointRec* point = g_questPOIPointDB.GetRecordByIndex(index);

    do {
        new (&verts[count]) C3Vector(static_cast<float>(point->m_x), static_cast<float>(point->m_y), 0.0f);
        count++;
        index++;
        point = g_questPOIPointDB.GetRecordByIndex(index);
    } while (point && point->m_questPOIBlobID == blob->m_id && count < static_cast<uint32_t>(blob->m_numPoints));

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    C3Vector position = player->GetPosition();
    position.z = 0.0f;

    return NTempest::Intersect(position, verts, count, C3Vector::AXIS_Z);
}

// OFFSET: 0x009F4E90/0x140790980
bool CGResearchFrame::CanScanResearchSite(int32_t researchField) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    const ResearchFieldRec* field = g_researchFieldDB.GetRecord(researchField);

    if (!player || !field) {
        return false;
    }

    for (uint32_t i = 0; i < 16; i++) {
        uint16_t site = player->m_player->m_playerFieldReserachSite1[16 * field->m_slot + i];

        if (site && CGResearchFrame::IsInsideResearchSite(site)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009F5310
CGResearchFrame::PROJECT* CGResearchFrame::FindProject(int32_t projectID) {
    const ResearchProjectRec* project = g_researchProjectDB.GetRecord(projectID);

    if (!project) {
        return NULL;
    }

    const ResearchBranchRec* branchRec = g_researchBranchDB.GetRecord(project->m_researchBranchID);

    if (!branchRec) {
        return NULL;
    }

    HASHKEY_NONE key;
    BRANCH* branch = CGResearchFrame::m_projectsByBranch.Ptr(branchRec->m_id, key);

    if (!branch) {
        return NULL;
    }

    for (PROJECT* node = branch->m_projects.Head(); node; node = branch->m_projects.Next(node)) {
        if (node->m_project->m_id == projectID) {
            return node;
        }
    }

    return NULL;
}

// OFFSET: 0x009F53A0
CGResearchFrame::PROJECT* CGResearchFrame::GetProject(int32_t branchID, int32_t index) {
    HASHKEY_NONE key;
    BRANCH* branch = CGResearchFrame::m_projectsByBranch.Ptr(branchID, key);

    if (!branch) {
        return NULL;
    }

    int32_t i = 0;

    for (PROJECT* node = branch->m_projects.Head(); node; node = branch->m_projects.Next(node)) {
        if (i == index) {
            return node;
        }

        i++;
    }

    return NULL;
}

// OFFSET: 0x009F53F0/0x1407910E0
int32_t CGResearchFrame::UpdateArchaeologyDigSites() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    const WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetCurrentAreaID());

    if (CGWorldMap::GetCurrentAreaID() == -1 || !area) {
        return 0;
    }

    int32_t count = 0;

    for (BLOBPOIHASH* node = CGResearchFrame::m_blobHash.Head(); node; node = CGResearchFrame::m_blobHash.Next(node)) {
        node->m_data.m_visible = false;
    }

    for (uint32_t i = 0; i < 16; i++) {
        uint16_t siteID = player->m_player->m_playerFieldReserachSite1[i];

        if (!siteID) {
            continue;
        }

        const ResearchSiteRec* site = g_researchSiteDB.GetRecord(siteID);

        if (!site) {
            continue;
        }

        HASHKEY_NONE key;
        BLOBPOIHASH* node = CGResearchFrame::m_blobHash.Ptr(site->m_questPOIBlobID, key);

        if (node && node->m_data.m_blobs.m_count) {
            if (node->m_data.m_blobs.m_data[0].m_worldMapAreaID == area->m_id) {
                node->m_data.m_visible = true;
                count++;
            } else {
                node->m_data.m_visible = false;
            }
        }
    }

    return count;
}

// OFFSET: 0x009F5570
int32_t CGResearchFrame::GetArchaeologyBlobID(uint32_t index) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || index > 15) {
        return 0;
    }

    uint32_t i = 0;

    for (BLOBPOIHASH* node = CGResearchFrame::m_blobHash.Head(); node; node = CGResearchFrame::m_blobHash.Next(node)) {
        if (node->m_data.m_visible) {
            if (index == i++) {
                return node->m_hashval;
            }
        }
    }

    return 0;
}

// OFFSET: 0x009F5600
CGResearchFrame::BLOBPOIDATA* CGResearchFrame::GetPOIData(int32_t blobID) {
    HASHKEY_NONE key;
    BLOBPOIHASH* node = CGResearchFrame::m_blobHash.Ptr(blobID, key);

    return node ? &node->m_data : NULL;
}

// OFFSET: 0x009F5630
uint32_t CGResearchFrame::UpdateArchaeologyDigSiteBlobs(TSGrowableArray<int32_t>& sites) {
    sites.Clear();
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    const WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetCurrentAreaID());

    if (CGWorldMap::GetCurrentAreaID() == -1 || !area) {
        return 0;
    }

    for (int32_t i = 0; i < 16; i++) {
        uint16_t siteID = player->m_player->m_playerFieldReserachSite1[i];

        if (!siteID) {
            continue;
        }

        const ResearchSiteRec* site = g_researchSiteDB.GetRecord(siteID);

        if (!site) {
            continue;
        }

        HASHKEY_NONE key;
        BLOBPOIHASH* node = CGResearchFrame::m_blobHash.Ptr(site->m_questPOIBlobID, key);

        if (node && node->m_data.m_blobs.m_count && node->m_data.m_blobs.m_data[0].m_worldMapAreaID == area->m_id) {
            sites.New(i);
        }
    }

    return sites.m_count;
}

// OFFSET: 0x009F57C0
bool CGResearchFrame::BlobIntersectsRect(int32_t siteIndex, const CRect& rect) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || siteIndex < 0) {
        return false;
    }

    C3Vector quad[4];
    quad[0] = C3Vector(rect.minY, rect.minX, 0.0f);
    quad[1] = C3Vector(rect.minY, rect.maxX, 0.0f);
    quad[2] = C3Vector(rect.maxY, rect.maxX, 0.0f);
    quad[3] = C3Vector(rect.maxY, rect.minX, 0.0f);
    uint16_t siteID = player->m_player->m_playerFieldReserachSite1[siteIndex];

    if (!siteID) {
        return false;
    }

    const ResearchSiteRec* site = g_researchSiteDB.GetRecord(siteID);

    if (!site) {
        return false;
    }

    HASHKEY_NONE key;
    BLOBPOIHASH* node = CGResearchFrame::m_blobHash.Ptr(site->m_questPOIBlobID, key);

    if (!node || !node->m_data.m_blobs.m_count) {
        return false;
    }

    for (uint32_t i = 0; i < node->m_data.m_blobs.m_count; i++) {
        BLOBPOI& blob = node->m_data.m_blobs.m_data[i];

        for (uint32_t j = 0; j < blob.m_points.m_count; j++) {
            C3Vector point(static_cast<float>(blob.m_points.m_data[j].x), static_cast<float>(blob.m_points.m_data[j].y), 0.0f);

            if (NTempest::Intersect(point, quad, 4, C3Vector::AXIS_Z)) {
                return true;
            }
        }
    }

    return false;
}

// OFFSET: 0x009F5940
CGResearchFrame::BLOBPOI* CGResearchFrame::GetResearchSiteBlobs(uint32_t siteIndex) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint16_t siteID;
    const ResearchSiteRec* site;
    BLOBPOIHASH* node;
    HASHKEY_NONE key;

    if (player && siteIndex <= 15 && (siteID = player->m_player->m_playerFieldReserachSite1[siteIndex]) != 0 && (site = g_researchSiteDB.GetRecord(siteID)) != NULL && (node = CGResearchFrame::m_blobHash.Ptr(site->m_questPOIBlobID, key)) != NULL && node->m_data.m_blobs.m_count) {
        return node->m_data.m_blobs.m_data;
    }

    return NULL;
}

// OFFSET: 0x009F6140/0x140792300
int32_t CGResearchFrame::OnResearchSiteChanged(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGResearchFrame::UpdateArchaeologyDigSites();
    FrameScript_SignalEvent(SCRIPT_EVENT_ARTIFACT_DIG_SITE_UPDATED, NULL);
    return 1;
}

// OFFSET: 0x009F64A0
bool CGResearchFrame::CanItemBeApplied(int32_t itemID) {
    WOWGUID none;
    none.m_guid = 0;
    const ResearchBranchRec* branch;

    return g_itemSparseDB.GetRecord(itemID, none, NULL, NULL, false) && CGResearchFrame::m_projectRec && (branch = g_researchBranchDB.GetRecord(CGResearchFrame::m_projectRec->m_researchBranchID)) != NULL && branch->m_itemID == itemID;
}

// OFFSET: 0x009F6520/0x140792870
void CGResearchFrame::GetProgress(int32_t& currency, int32_t& items, int32_t& required) {
    currency = 0;
    items = 0;
    required = 0;

    if (!CGResearchFrame::m_projectRec) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !g_spellDB.GetRecord(CGResearchFrame::m_projectRec->m_spellID)) {
        return;
    }

    const ResearchBranchRec* branch = g_researchBranchDB.GetRecord(CGResearchFrame::m_projectRec->m_researchBranchID);

    if (!branch) {
        return;
    }

    CGPlayer_C* activePlayer = ClntObjMgrGetActivePlayerObj();

    if (!activePlayer) {
        return;
    }

    int32_t itemID = branch->m_itemID;
    WOWGUID none;
    none.m_guid = 0;
    const ItemRecSparse* sparse = g_itemSparseDB.GetRecord(itemID, none, NULL, NULL, false);

    if (sparse) {
        int32_t count = CGResearchFrame::m_itemCount;

        if (count >= activePlayer->m_bag.GetItemTypeCount(branch->m_itemID, 6)) {
            count = activePlayer->m_bag.GetItemTypeCount(branch->m_itemID, 6);
            CGResearchFrame::m_itemCount = count;
        }

        items = count * sparse->m_currencySubstitutionCount;
    }

    required = CGResearchFrame::m_projectRec->m_requiredWeight;
    PlayerCurrency* playerCurrency = CGCurrencyTypes::GetCurrency(branch->m_currencyID);

    if (playerCurrency) {
        currency = playerCurrency->m_quantity;
    }
}

// OFFSET: 0x009F6670
bool CGResearchFrame::PutWeightForSpell(CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t items = 0;

    if (!CGResearchFrame::m_projectRec || !player) {
        return false;
    }

    const SpellRec* spell = g_spellDB.GetRecord(CGResearchFrame::m_projectRec->m_spellID);
    const ResearchBranchRec* branch = g_researchBranchDB.GetRecord(CGResearchFrame::m_projectRec->m_researchBranchID);

    if (!spell || !branch) {
        return false;
    }

    uint32_t pos = msg->m_size;
    int32_t required = CGResearchFrame::m_projectRec->m_requiredWeight;
    uint32_t count = 0;
    msg->Put(static_cast<uint32_t>(0));
    WOWGUID none = WOWGUID();
    const ItemRecSparse* sparse = g_itemSparseDB.GetRecord(branch->m_itemID, none, NULL, NULL, false);

    if (sparse) {
        int32_t itemCount = CGResearchFrame::m_itemCount;

        if (itemCount >= player->m_bag.GetItemTypeCount(branch->m_itemID, 6)) {
            itemCount = player->m_bag.GetItemTypeCount(branch->m_itemID, 6);
            CGResearchFrame::m_itemCount = itemCount;
        }

        items = itemCount * sparse->m_currencySubstitutionCount;
    }

    int32_t currency = required - items;

    if (!CGCurrencyTypes::HasCurrency(branch->m_currencyID, currency)) {
        return false;
    }

    if (CGResearchFrame::m_itemCount > 0) {
        msg->Put(static_cast<uint8_t>(2));
        msg->Put(static_cast<uint32_t>(branch->m_itemID));
        msg->Put(static_cast<uint32_t>(CGResearchFrame::m_itemCount));
        count = 1;
    }

    if (currency) {
        msg->Put(static_cast<uint8_t>(1));
        msg->Put(static_cast<uint32_t>(branch->m_currencyID));
        msg->Put(static_cast<uint32_t>(currency));
        count++;
    }

    msg->Set(pos, count);

    return true;
}

// OFFSET: 0x009F6820/0x140792C40
void CGResearchFrame::Solve() {
    if (!CGResearchFrame::m_projectRec) {
        return;
    }

    int32_t currency;
    int32_t items;
    int32_t required;
    CGResearchFrame::GetProgress(currency, items, required);

    if (items + currency < required) {
        return;
    }

    const SpellRec* spell = g_spellDB.GetRecord(CGResearchFrame::m_projectRec->m_spellID);

    if (spell) {
        Spell_C_CastSpell(spell->m_id, NULL, WOWGUID(), false, false, 0, 0);
    }
}

// OFFSET: 0x009F68A0
void CGResearchFrame::DestroyArchaeologyBlobs() {
    for (BLOBPOIHASH* node = CGResearchFrame::m_blobHash.Head(); node; node = CGResearchFrame::m_blobHash.Next(node)) {
        node->m_data.m_blobs.Clear();
    }

    CGResearchFrame::m_blobHash.Clear();
}

// OFFSET: 0x009F7350/0x140793C50
void CGResearchFrame::ShutdownGame() {
    BRANCH* next;

    for (BRANCH* branch = CGResearchFrame::m_projectsByBranch.Head(); branch; branch = next) {
        next = CGResearchFrame::m_projectsByBranch.Next(branch);
        PROJECT* nextProject;

        for (PROJECT* project = branch->m_projects.Head(); project; project = nextProject) {
            nextProject = branch->m_projects.Next(project);
            project->m_link.Unlink();
            CGResearchFrame::s_projectPool->Delete(project, "", 0);
        }

        branch->m_projects.UnlinkAll();
        CGResearchFrame::m_projectsByBranch.Unlink(branch);
        CGResearchFrame::s_branchPool->Delete(branch, "", 0);
    }

    CGResearchFrame::m_skillLine = 0;
    CGResearchFrame::m_branches.SetCount(0);
    CGResearchFrame::m_historyLoaded = false;
    CGResearchFrame::m_projectsByBranch.Clear();
    delete CGResearchFrame::s_branchPool;
    delete CGResearchFrame::s_projectPool;
    ConsoleCommandUnregister("startproject");
    ConsoleCommandUnregister("enableresearchsite");
    ConsoleCommandUnregister("spawnallresearchlocs");
    ConsoleCommandUnregister("randomizesurveyhint");
    ClientSetupResearchHistory::s_handler = NULL;
    ClientResearchComplete::s_handler = NULL;
    CGResearchFrame::DestroyArchaeologyBlobs();
}

// OFFSET: 0x009F7640
bool CGResearchFrame::CanCastSpell(int32_t spellId, int32_t& reason) {
    int32_t currency;
    int32_t items;
    int32_t required;

    if (CGResearchFrame::m_projectRec && CGResearchFrame::m_projectRec->m_spellID == spellId && (CGResearchFrame::GetProgress(currency, items, required), items + currency >= required)) {
        reason = 0;
        return true;
    }

    reason = 192;
    return false;
}

// OFFSET: 0x009F78C0/0x140794310
void CGResearchFrame::AddProject(int32_t projectID, bool active, int32_t firstCompleted, int32_t completionCount) {
    const ResearchProjectRec* rec = g_researchProjectDB.GetRecord(projectID);

    if (!rec) {
        return;
    }

    HASHKEY_NONE key;
    BRANCH* branch = CGResearchFrame::m_projectsByBranch.Ptr(rec->m_researchBranchID, key);

    if (!branch) {
        branch = CGResearchFrame::s_branchPool->New("", 0);
        branch->m_numProjects = 0;
        CGResearchFrame::m_projectsByBranch.Insert(branch, rec->m_researchBranchID, key);
    }

    PROJECT* project;

    for (project = branch->m_projects.Head(); project; project = branch->m_projects.Next(project)) {
        if (project->m_project->m_id == projectID) {
            break;
        }
    }

    if (!project) {
        project = CGResearchFrame::s_projectPool->New("", 0);
        project->m_active = active;
        project->m_project = rec;

        if (rec->m_rarity) {
            branch->m_projects.LinkToHead(project);
        } else {
            branch->m_projects.LinkToTail(project);
        }

        branch->m_numProjects++;
    } else if (active && !project->m_active) {
        project->m_active = active;
        project->m_firstCompleted = firstCompleted;
        project->m_completionCount = completionCount;
        return;
    }

    project->m_firstCompleted = firstCompleted;
    project->m_completionCount = completionCount;
}

// OFFSET: 0x009F7A00
bool CGResearchFrame::OnSetupResearchHistory(ClientSetupResearchHistory* msg) {
    CGResearchFrame::m_historyLoaded = true;

    for (uint32_t i = 0; i < msg->m_history.m_count; i++) {
        JamResearchHistory history(msg->m_history.m_data[i]);
        CGResearchFrame::AddProject(history.m_projectID, false, history.m_firstCompleted, history.m_completionCount);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_ARTIFACT_HISTORY_READY, NULL);

    return true;
}

// OFFSET: 0x009F7A70
bool CGResearchFrame::OnResearchComplete(ClientResearchComplete* msg) {
    JamResearchHistory history(msg->m_research);
    const ResearchProjectRec* project = g_researchProjectDB.GetRecord(history.m_projectID);

    if (!project) {
        return true;
    }

    int32_t projectID = project->m_id;

    if (!CGResearchFrame::FindProject(project->m_id)) {
        CGResearchFrame::AddProject(projectID, false, history.m_firstCompleted, history.m_completionCount);
    }

    PROJECT* node = CGResearchFrame::FindProject(project->m_id);
    node->m_firstCompleted = history.m_firstCompleted;
    node->m_completionCount = history.m_completionCount;
    FrameScript_SignalEvent(SCRIPT_EVENT_ARTIFACT_COMPLETE, "%s", static_cast<const char*>(project->m_name));

    return true;
}

// OFFSET: 0x009F7B20/0x1407947A0
void CGResearchFrame::CreateArchaeologyBlobs() {
    for (int32_t i = 0; i < g_researchSiteDB.m_numRecords; i++) {
        const ResearchSiteRec* site = g_researchSiteDB.GetRecordByIndex(i);

        if (!site || !site->m_questPOIBlobID) {
            continue;
        }

        int32_t blobID = site->m_questPOIBlobID;
        const QuestPOIBlobRec* blob = g_questPOIBlobDB.GetRecord(blobID);

        if (!blob) {
            continue;
        }

        HASHKEY_NONE key;
        BLOBPOIHASH* node = CGResearchFrame::m_blobHash.New(blobID, key, 0, 0);
        BLOBPOI* poi = NULL;

        for (int32_t j = 0; j < g_questPOIPointDB.m_numRecords; j++) {
            const QuestPOIPointRec* point = g_questPOIPointDB.GetRecordByIndex(j);

            if (!point || point->m_questPOIBlobID != blob->m_id) {
                continue;
            }

            node->m_data.m_siteID = site->m_id;

            if (!node->m_data.m_blobs.m_count) {
                poi = node->m_data.m_blobs.New();
                poi->m_center.x = 0;
                poi->m_center.y = 0;
                poi->unk_0018 = 0;
                poi->unk_001C = 0;
                poi->m_mapID = blob->m_mapID;
                poi->m_worldMapAreaID = blob->m_worldMapAreaID;
                poi->unk_0028 = 0;
                poi->unk_002C = 0;
                poi->unk_0030 = 0;
            } else {
                poi = node->m_data.m_blobs.m_data;
            }

            C2iVector position;
            position.x = point->m_x;
            position.y = point->m_y;
            poi->m_points.New(position);
        }

        if (poi && poi->m_points.m_count > 0) {
            int32_t x = 0;
            int32_t y = 0;

            for (int32_t k = 0; k < static_cast<int32_t>(poi->m_points.m_count); k++) {
                x += poi->m_points.m_data[k].x;
                y += poi->m_points.m_data[k].y;
            }

            poi->m_center.x = x / static_cast<int32_t>(poi->m_points.m_count);
            poi->m_center.y = y / static_cast<int32_t>(poi->m_points.m_count);
        }
    }
}

// OFFSET: 0x009F7D10/0x140794AA0
void CGResearchFrame::InitializeGame() {
    void* branchPool = SMemAlloc(sizeof(TSObjectAllocPool<BRANCH>), __FILE__, __LINE__, 0);
    CGResearchFrame::s_branchPool = branchPool ? new (branchPool) TSObjectAllocPool<BRANCH>("BRANCH", 128) : NULL;
    void* projectPool = SMemAlloc(sizeof(TSObjectAllocPool<PROJECT>), __FILE__, __LINE__, 0);
    CGResearchFrame::s_projectPool = projectPool ? new (projectPool) TSObjectAllocPool<PROJECT>("PROJECT", 128) : NULL;

    if (!CGResearchFrame::m_fieldRec || CGResearchFrame::m_fieldRec->m_id != 1) {
        const ResearchFieldRec* field = g_researchFieldDB.GetRecord(1);

        if (field) {
            CGResearchFrame::m_fieldRec = field;
        }
    }

    ClientSetupResearchHistory::s_handler = &CGResearchFrame::OnSetupResearchHistory;
    ClientResearchComplete::s_handler = &CGResearchFrame::OnResearchComplete;
    CGResearchFrame::CreateArchaeologyBlobs();
}

// OFFSET: 0x009F7E30/0x140794C10
int32_t CGResearchFrame::OnResearchChanged(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    const uint16_t* oldProjects = static_cast<const uint16_t*>(oldValue);

    for (uint32_t i = 0; i < 16; i++) {
        uint16_t projectID = player->m_player->m_playerFieldResearching1[i];

        if (!projectID || projectID == oldProjects[i]) {
            continue;
        }

        CGResearchFrame::AddProject(projectID, true, 0, 0);
        const ResearchProjectRec* oldProject = g_researchProjectDB.GetRecord(oldProjects[i]);

        if (oldProject && CGResearchFrame::m_projectRec && oldProject->m_id == CGResearchFrame::m_projectRec->m_id) {
            CGResearchFrame::m_projectRec = g_researchProjectDB.GetRecord(projectID);
            CGResearchFrame::m_itemCount = 0;
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_ARTIFACT_UPDATE, NULL);

    return 1;
}

// OFFSET: 0x009F7F30/0x140794D40
void CGResearchFrame::EnterWorld() {
    CGResearchFrame::m_skillLine = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offsetof(CGPlayerLocalData, m_playerFieldResearching1), sizeof(((CGPlayerLocalData*)0)->m_playerFieldResearching1), &CGResearchFrame::OnResearchChanged, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offsetof(CGPlayerLocalData, m_playerFieldReserachSite1), sizeof(((CGPlayerLocalData*)0)->m_playerFieldReserachSite1), &CGResearchFrame::OnResearchSiteChanged, NULL, static_cast<HANDLER_PRIORITY>(0), 0);

    if (!CGResearchFrame::m_branches.Count()) {
        for (int32_t i = 0; i < g_researchBranchDB.m_numRecords; i++) {
            const ResearchBranchRec* branch = g_researchBranchDB.GetRecordByIndex(i);

            if (branch) {
                CGResearchFrame::m_branches.Add(1, sizeof(branch), &branch);
            }
        }
    }

    for (uint32_t i = 0; i < 16; i++) {
        uint16_t projectID = player->m_player->m_playerFieldResearching1[i];

        if (projectID) {
            CGResearchFrame::AddProject(projectID, true, 0, 0);
        }
    }

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGWorldMap::GetCurrentAreaID();
    }
}

// OFFSET: 0x009F8060/0x140794FA0
void CGResearchFrame::LeaveWorld() {
    CGResearchFrame::m_skillLine = 0;
    ClntObjMgrUnsetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offsetof(CGPlayerLocalData, m_playerFieldResearching1), &CGResearchFrame::OnResearchChanged, NULL);
    ClntObjMgrUnsetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offsetof(CGPlayerLocalData, m_playerFieldReserachSite1), &CGResearchFrame::OnResearchSiteChanged, NULL);
}
