#ifndef WOW_SOURCE_UI_RESEARCHFRAME_H
#define WOW_SOURCE_UI_RESEARCHFRAME_H

#include <cstdint>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "WoW/Source/Object/WGUID.h"

class CDataStore;
class ClientResearchComplete;
class ClientSetupResearchHistory;
class CRect;
class ResearchBranchRec;
class ResearchFieldRec;
class ResearchProjectRec;

class CGResearchFrame {
    public:
    struct PROJECT : public TSLinkedNode<PROJECT> {
        /* 0008 */ uint32_t m_handle;
        /* 000C */ const ResearchProjectRec* m_project;
        /* 0010 */ bool m_active;
        /* 0014 */ int32_t m_firstCompleted;
        /* 0018 */ int32_t m_completionCount;
    };

    struct BRANCH : public TSHashObject<BRANCH, HASHKEY_NONE> {
        /* 0018 */ uint32_t m_handle;
        /* 001C */ TSList<PROJECT, TSGetLink<PROJECT> > m_projects;
        /* 0028 */ uint32_t m_numProjects;
    };

    struct BLOBPOI {
        /* 0000 */ TSGrowableArray<C2iVector> m_points;
        /* 0010 */ C2iVector m_center;
        /* 0018 */ int32_t unk_0018;
        /* 001C */ int32_t unk_001C;
        /* 0020 */ int32_t m_mapID;
        /* 0024 */ int32_t m_worldMapAreaID;
        /* 0028 */ int32_t unk_0028;
        /* 002C */ int32_t unk_002C;
        /* 0030 */ int32_t unk_0030;
    };

    struct BLOBPOIDATA {
        /* 0000 */ int32_t m_siteID;
        /* 0004 */ TSGrowableArray<BLOBPOI> m_blobs;
        /* 0014 */ int32_t unk_0014;
        /* 0018 */ int32_t unk_0018;
        /* 001C */ int32_t unk_001C;
        /* 0020 */ int32_t unk_0020;
        /* 0024 */ int32_t unk_0024;
        /* 0028 */ bool m_visible;
        /* 002C */ int32_t unk_002C;
    };

    struct BLOBPOIHASH : public TSHashObject<BLOBPOIHASH, HASHKEY_NONE> {
        /* 0018 */ BLOBPOIDATA m_data;
    };

    static uint8_t s_enteredWorld;
    static int32_t m_itemCount;
    static const ResearchFieldRec* m_fieldRec;
    static const ResearchProjectRec* m_projectRec;
    static bool m_historyRequested;
    static bool m_historyLoaded;
    static TSObjectAllocPool<BRANCH>* s_branchPool;
    static TSObjectAllocPool<PROJECT>* s_projectPool;
    static int32_t m_skillLine;
    static TSHashTable<BRANCH, HASHKEY_NONE> m_projectsByBranch;
    static TSCGrowableArray<const ResearchBranchRec*, 32> m_branches;
    static TSHashTable<BLOBPOIHASH, HASHKEY_NONE> m_blobHash;

    static void SelectProject(int32_t projectID);
    static const ResearchBranchRec* GetBranch(int32_t index);
    static bool ApplyItemToProject();
    static const ResearchProjectRec* GetActiveProject(int32_t branchID);
    static bool IsInsideResearchSite(int32_t siteId);
    static bool CanScanResearchSite(int32_t researchField);
    static PROJECT* FindProject(int32_t projectID);
    static PROJECT* GetProject(int32_t branchID, int32_t index);
    static int32_t UpdateArchaeologyDigSites();
    static int32_t GetArchaeologyBlobID(uint32_t index);
    static BLOBPOIDATA* GetPOIData(int32_t blobID);
    static uint32_t UpdateArchaeologyDigSiteBlobs(TSGrowableArray<int32_t>& sites);
    static bool BlobIntersectsRect(int32_t siteIndex, const CRect& rect);
    static BLOBPOI* GetResearchSiteBlobs(uint32_t siteIndex);
    static int32_t OnResearchSiteChanged(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);
    static bool CanItemBeApplied(int32_t itemID);
    static void GetProgress(int32_t& currency, int32_t& items, int32_t& required);
    static bool PutWeightForSpell(CDataStore* msg);
    static void Solve();
    static void DestroyArchaeologyBlobs();
    static void ShutdownGame();
    static bool CanCastSpell(int32_t spellId, int32_t& reason);
    static void AddProject(int32_t projectID, bool active, int32_t firstCompleted, int32_t completionCount);
    static bool OnSetupResearchHistory(ClientSetupResearchHistory* msg);
    static bool OnResearchComplete(ClientResearchComplete* msg);
    static void CreateArchaeologyBlobs();
    static void InitializeGame();
    static int32_t OnResearchChanged(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);
    static void EnterWorld();
    static void LeaveWorld();
};

#endif
