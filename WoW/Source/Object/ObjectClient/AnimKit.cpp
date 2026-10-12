#include "AnimKitManager.h"
#include <cmath>
#include <cstring>
#include <new>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/AnimReplacementSet.h"

struct CustomFallbackPathStep {
    /* 0000 */ uint16_t anim;
    /* 0002 */ uint16_t flags;
};

class CustomFallbackPath : public TSHashObject<CustomFallbackPath, HASHKEY_NONE> {
    public:
    /* 0018 */ uint16_t m_anim;
    /* 001A */ uint8_t m_count;
    /* 001B */ uint8_t m_keepFallbackCount;
    /* 001C */ const CustomFallbackPathStep* m_steps;

    CustomFallbackPath();
    CustomFallbackPath(TSHashTable<CustomFallbackPath, HASHKEY_NONE>& table, uint16_t anim, uint8_t count, uint8_t keepFallbackCount, const CustomFallbackPathStep* steps);
};

struct AnimKitBoneSetHierarchy {
    /* 0000 */ uint32_t descendants;
    /* 0004 */ uint32_t ancestors;
};

// STATIC: 0x00DD58F8/0x140CBC010
static int32_t s_boneDataBoneSetIDs[35];

// STATIC: 0x00DD5988/0x140CBC0A0
static AnimKitBoneSetHierarchy s_boneSetHierarchy[32];

// STATIC: 0x00DD5A88/0x140CBC1A0
uint32_t CAnimKitManager::s_animKitDefCount;

// STATIC: 0x00DD5A8C
AnimKitDef** CAnimKitManager::s_animKitDefs;

// STATIC: 0x00DD5A90/0x140CBC1A4
uint32_t CAnimKitManager::s_animKitConfigDefCount;

// STATIC: 0x00DD5A94/0x140CBC1B0
AnimKitConfigDef** CAnimKitManager::s_animKitConfigDefs;

// STATIC: 0x00DD5A98/0x140CBC1B8
uint16_t CAnimKitManager::s_animDataCount;

// STATIC: 0x00DD5A9C/0x140CBC1BC
uint32_t CAnimKitManager::s_animKitBoneSetCount;

// STATIC: 0x00DD5AA0/0x140CBC1C0
int32_t CAnimKitManager::s_updateTolerance;

// STATIC: 0x00DD5AA4/0x140CBC1C4
int32_t CAnimKitManager::s_initialized;

// STATIC: 0x00DD5AA8/0x140CBC1C8
int32_t CAnimKitManager::s_inChainedSegmentStart;

// STATIC: 0x00CA5954
static const uint32_t s_tierFallback[ANIM_KIT_TIER_COUNT] = { 0, 0, 3, 0, 0 };

// STATIC: 0x00CA5E68
static const uint32_t s_handItemBoneDataIDs[6] = { 1, 0, 3, 2, 4, 0xFFFFFFFF };

// STATIC: 0x00DD5AD8
static TSGrowableArray<uint16_t> s_behaviorAnims[ANIM_KIT_TIER_COUNT];

// STATIC: 0x00DD5AAC
static TSHashTable<CustomFallbackPath, HASHKEY_NONE> s_glueFallbackPaths;

// STATIC: 0x00CA5968
static const CustomFallbackPathStep s_glueFallbackSteps229[] = { { 229, 0 }, { 193, 0 }, { 548, 0 }, { 41, 0 }, { 532, 0 }, { 0, 0 } };

// STATIC: 0x00CA5980
static CustomFallbackPath s_glueFallbackPath229(s_glueFallbackPaths, 229, 6, 1, s_glueFallbackSteps229);

// STATIC: 0x00CA59A0
static const CustomFallbackPathStep s_glueFallbackSteps422[] = { { 422, 0 }, { 229, 0 }, { 193, 0 }, { 548, 0 }, { 41, 0 }, { 532, 0 }, { 0, 0 } };

// STATIC: 0x00CA59BC
static CustomFallbackPath s_glueFallbackPath422(s_glueFallbackPaths, 422, 7, 3, s_glueFallbackSteps422);

// STATIC: 0x00CA59DC
static const CustomFallbackPathStep s_glueFallbackSteps240[] = { { 240, 0 }, { 229, 0 }, { 193, 0 }, { 548, 0 }, { 41, 0 }, { 532, 0 }, { 0, 0 } };

// STATIC: 0x00CA59F8
static CustomFallbackPath s_glueFallbackPath240(s_glueFallbackPaths, 240, 7, 2, s_glueFallbackSteps240);

// STATIC: 0x00CA5A18
static const CustomFallbackPathStep s_glueFallbackSteps241[] = { { 241, 0 }, { 229, 0 }, { 193, 0 }, { 548, 0 }, { 41, 0 }, { 532, 0 }, { 0, 0 } };

// STATIC: 0x00CA5A34
static CustomFallbackPath s_glueFallbackPath241(s_glueFallbackPaths, 241, 7, 2, s_glueFallbackSteps241);

// STATIC: 0x00CA5A54
static const CustomFallbackPathStep s_glueFallbackSteps320[] = { { 320, 0 }, { 91, 0 }, { 0, 0 } };

// STATIC: 0x00CA5A60
static CustomFallbackPath s_glueFallbackPath320(s_glueFallbackPaths, 320, 3, 2, s_glueFallbackSteps320);

// STATIC: 0x00CA5A80
static const CustomFallbackPathStep s_glueFallbackSteps364[] = { { 364, 0 }, { 271, 0 }, { 233, 0 }, { 229, 0 }, { 135, 0 }, { 556, 0 }, { 560, 0 }, { 558, 0 }, { 42, 0 }, { 540, 0 }, { 544, 0 }, { 542, 0 }, { 4, 0 }, { 0, 0 } };

// STATIC: 0x00CA5AB8
static CustomFallbackPath s_glueFallbackPath364(s_glueFallbackPaths, 364, 14, 6, s_glueFallbackSteps364);

// STATIC: 0x00CA5AD8
static const CustomFallbackPathStep s_glueFallbackSteps360[] = { { 360, 0 }, { 230, 0 }, { 131, 0 }, { 1, 0 }, { 0, 0 } };

// STATIC: 0x00CA5AEC
static CustomFallbackPath s_glueFallbackPath360(s_glueFallbackPaths, 360, 5, 3, s_glueFallbackSteps360);

// STATIC: 0x00CA5B0C
static const CustomFallbackPathStep s_glueFallbackSteps361[] = { { 361, 0 }, { 360, 0x20 }, { 235, 0xFFFF }, { 230, 0x20 }, { 132, 0xFFFF }, { 131, 0x20 }, { 6, 0xFFFF }, { 1, 0x20 }, { 0, 0 } };

// STATIC: 0x00CA5B30
static CustomFallbackPath s_glueFallbackPath361(s_glueFallbackPaths, 361, 9, 5, s_glueFallbackSteps361);

// STATIC: 0x00DD5B28
static TSHashTable<CustomFallbackPath, HASHKEY_NONE> s_gameObjectFallbackPaths;

// STATIC: 0x00CA5B50
static const CustomFallbackPathStep s_gameObjectFallbackSteps148[] = { { 148, 0 }, { 146, 0x10 }, { 150, 0xFFFF }, { 0, 0 } };

// STATIC: 0x00CA5B60
static CustomFallbackPath s_gameObjectFallbackPath148(s_gameObjectFallbackPaths, 148, 4, 4, s_gameObjectFallbackSteps148);

// STATIC: 0x00CA5B80
static const CustomFallbackPathStep s_gameObjectFallbackSteps146[] = { { 146, 0 }, { 148, 0x10 }, { 0, 0xFFFF } };

// STATIC: 0x00CA5B8C
static CustomFallbackPath s_gameObjectFallbackPath146(s_gameObjectFallbackPaths, 146, 3, 3, s_gameObjectFallbackSteps146);

// STATIC: 0x00CA5BAC
static const CustomFallbackPathStep s_gameObjectFallbackSteps149[] = { { 149, 0 }, { 148, 0x20 }, { 146, 0x10 }, { 151, 0xFFFF }, { 0, 0 } };

// STATIC: 0x00CA5BC0
static CustomFallbackPath s_gameObjectFallbackPath149(s_gameObjectFallbackPaths, 149, 5, 5, s_gameObjectFallbackSteps149);

// STATIC: 0x00CA5BE0
static const CustomFallbackPathStep s_gameObjectFallbackSteps147[] = { { 147, 0 }, { 146, 0x20 }, { 148, 0x10 }, { 0, 0xFFFF }, { 0, 0 } };

// STATIC: 0x00CA5BF4
static CustomFallbackPath s_gameObjectFallbackPath147(s_gameObjectFallbackPaths, 147, 5, 5, s_gameObjectFallbackSteps147);

// STATIC: 0x00DD5B54/0x140CBC248
static CDataAllocator s_managerAllocator(sizeof(CAnimKitManager), 64);

// STATIC: 0x00CA5C14/0x140B44870
static TSExplicitList<CAnimKitInst, offsetof(CAnimKitInst, m_link)> s_pendingDestroyKitInsts;

// STATIC: 0x00CA5C20/0x140B44888
static TSExplicitList<AnimKitSegmentInst, offsetof(AnimKitSegmentInst, link)> s_pendingDestroySegmentInsts;

// STATIC: 0x00CA5C2C/0x140B448A0
static TSExplicitList<AnimKitSegmentBoneSetInst, offsetof(AnimKitSegmentBoneSetInst, link)> s_pendingDestroySegmentBoneSetInsts;

// STATIC: 0x00DD5B68/0x140CBC268
static CDataAllocator s_kitInstAllocator(sizeof(CAnimKitInst), 128);

// STATIC: 0x00DD5B7C/0x140CBC288
static CDataAllocator s_segmentInstAllocator(sizeof(AnimKitSegmentInst), 128);

// STATIC: 0x00DD5B90/0x140CBC2A8
static CDataAllocator s_segmentBoneSetInstAllocator(sizeof(AnimKitSegmentBoneSetInst), 128);

// STATIC: 0x00CA5C38/0x140B448B8
static TSExplicitList<CAnimKitManager, offsetof(CAnimKitManager, m_updateLink)> s_updateList;

// STATIC: 0x00CA5C44/0x140B448D0
static TSExplicitList<CAnimKitManager, offsetof(CAnimKitManager, m_updateLink)> s_idleList;

// INLINED
CustomFallbackPath::CustomFallbackPath() {
    this->m_anim = 0xFFFF;
    this->m_count = 0;
    this->m_keepFallbackCount = 0;
    this->m_steps = NULL;
}

// INLINED
CustomFallbackPath::CustomFallbackPath(TSHashTable<CustomFallbackPath, HASHKEY_NONE>& table, uint16_t anim, uint8_t count, uint8_t keepFallbackCount, const CustomFallbackPathStep* steps) {
    this->m_anim = anim;
    this->m_count = count;
    this->m_keepFallbackCount = keepFallbackCount;
    this->m_steps = steps;
    table.Insert(this, anim, HASHKEY_NONE());
}

// INLINED
void CAnimKitManager::ScheduleUpdate(uint16_t flags) {
    uint16_t oldFlags = this->m_flags;
    int32_t schedule = (oldFlags & ANIM_KIT_MANAGER_FLAG_UPDATE_MASK) == 0;
    this->m_flags = oldFlags | flags;

    if (schedule) {
        s_updateList.LinkToTail(this);
    }
}

// OFFSET: 0x0058BD90/0x1401F5540
int32_t CAnimKitInst::IsStoppedOrPendingDestroy() const {
    return (this->m_flags & (KIT_INST_FLAG_STOPPED | KIT_INST_FLAG_PENDING_DESTROY)) != 0;
}

// OFFSET: 0x0058BDB0/0x1401F5560
uint32_t CAnimKitInst::GetID() const {
    return this->m_def->rec->m_id;
}

// OFFSET: 0x0058BDC0/0x1401F5570
static void InitBoneSetAncestors(uint32_t boneSetID, uint32_t ancestors) {
    uint32_t descendants = s_boneSetHierarchy[boneSetID].descendants;
    s_boneSetHierarchy[boneSetID].ancestors = ancestors;
    uint32_t mask = (1 << boneSetID) | ancestors;
    uint32_t child = 1;

    for (uint32_t bit = 2; descendants; bit *= 2) {
        if (bit & descendants) {
            InitBoneSetAncestors(child, mask);
            descendants &= ~bit;
        }

        child++;
    }
}

// OFFSET: 0x0058BE20/0x1401F55F0
int32_t CAnimKitManager::IsFrozen() const {
    return this->m_flags & ANIM_KIT_MANAGER_FLAG_FREEZE_MASK;
}

// OFFSET: 0x0058BE30/0x1401F5600
void CAnimKitManager::SetExpressionCallback(AnimKitExpressionCallback callback, uint64_t param) {
    this->m_expressionCallback = callback;
    this->m_expressionCallbackParam = param;
}

// OFFSET: 0x0058BE60/0x1401F5610
void CAnimKitManager::PickSegmentInstVariationAndRepetitions(AnimKitSegmentInst* segmentInst) {
    int8_t variation = segmentInst->forcedVariation;
    const AnimKitSegmentRec* rec = segmentInst->rec;

    if (variation == -1 || (segmentInst->instFlags & SEGMENT_INST_FLAG_NO_LOOP)) {
        variation = segmentInst->inheritedVariation;

        if (variation == -1) {
            if (segmentInst->instFlags & SEGMENT_INST_FLAG_NO_LOOP) {
                segmentInst->variation = 0;
                goto repetitions;
            }

            if (rec->m_segmentFlags & 0x2) {
                variation = rec->m_forcedVariation;
            } else if (this->m_model->IsLoaded()) {
                variation = this->m_model->PickSequenceVariation(segmentInst->curAnim);
            } else {
                variation = 0;
            }
        }
    }

    segmentInst->variation = variation;

repetitions:
    if (segmentInst->forcedRepetitions) {
        segmentInst->repetitions = segmentInst->forcedRepetitions;
        return;
    }

    if (rec->m_endCondition || rec->m_endConditionParam <= 0) {
        if (!this->m_model->IsLoaded()) {
            segmentInst->repetitions = 0;
            return;
        }

        uint32_t repetitions = this->m_model->PickSequenceRepetitions(segmentInst->curAnim, segmentInst->variation);

        if (repetitions == 0xFFFFFFFF) {
            segmentInst->repetitions = 0;
            return;
        }

        segmentInst->repetitions = repetitions;
        return;
    }

    segmentInst->repetitions = rec->m_endConditionParam;
}

// OFFSET: 0x0058C490/0x1401F5E90
uint32_t CAnimKitManager::FindBestAnimKitBoneSetIDForBoneDataID(uint32_t boneDataID) {
    if (boneDataID >= 35) {
        return 0;
    }

    int32_t boneSetID = s_boneDataBoneSetIDs[boneDataID];

    if (boneSetID != -1) {
        return boneSetID;
    }

    for (int32_t i = 0; i < g_animKitBoneSetAliasDB.m_numRecords; i++) {
        const AnimKitBoneSetAliasRec* rec = g_animKitBoneSetAliasDB.GetRecordByIndex(i);

        if (rec->m_boneDataID == static_cast<int32_t>(boneDataID)) {
            return rec->m_animKitBoneSetID;
        }
    }

    return 0;
}

// OFFSET: 0x0058C4F0/0x1401F5EF0
void CAnimKitManager::SetBoneDataIDBlocked(int32_t boneDataID, int32_t blocked, int32_t blockAlt) {
    if (boneDataID >= 35) {
        return;
    }

    int32_t boneSetID = s_boneDataBoneSetIDs[boneDataID];

    if (boneSetID == -1 || !boneSetID) {
        return;
    }

    if (blocked) {
        this->m_blockedBoneSetMask |= 1 << boneSetID;
    } else {
        this->m_blockedBoneSetMask &= ~(1 << boneSetID);
    }

    const AnimKitBoneSetRec* rec = g_animKitBoneSetDB.GetRecord(boneSetID);

    if (!rec) {
        return;
    }

    int32_t altBoneSetID = rec->m_altAnimKitBoneSetID;

    if (altBoneSetID == -1 || !altBoneSetID) {
        return;
    }

    if (blockAlt) {
        this->m_blockedBoneSetMask |= 1 << altBoneSetID;
    } else {
        this->m_blockedBoneSetMask &= ~(1 << altBoneSetID);
    }
}

// OFFSET: 0x0058C580
uint32_t CAnimKitManager::GetCurTime() {
    if (this->m_flags & ANIM_KIT_MANAGER_FLAG_FROZEN) {
        return this->m_frozenTime;
    }

    return this->m_model->m_scene->m_time;
}

// OFFSET: 0x0058C5A0/0x1401F5F80
void CAnimKitManager::PostInitSegmentInst(AnimKitSegmentInst* segmentInst, TSStackArray<AnimKitSegmentInst*>& play, TSStackArray<AnimKitSegmentInst*>& stop) {
    const AnimKitSegmentRec* rec = segmentInst->rec;
    uint32_t time = this->m_model->m_scene->m_time;
    CAnimKitInst* kitInst = segmentInst->kitInst;

    switch (rec->m_startCondition) {
        case 0:
            if (rec->m_startConditionDelay <= 0) {
                play.m_data[play.m_count++] = segmentInst;
            } else {
                this->AddSegmentEvent(segmentInst, time, rec->m_startConditionDelay, ANIM_KIT_SEGMENT_EVENT_START, 1);
            }

            break;

        case 1:
            kitInst->m_flags |= KIT_INST_FLAG_REACT_TO_SEGMENT_START;
            break;

        case 2:
            kitInst->m_flags |= KIT_INST_FLAG_REACT_TO_SEGMENT_END;
            break;
    }

    switch (rec->m_endCondition) {
        case 2:
            if (rec->m_endConditionDelay <= 0) {
                stop.m_data[stop.m_count++] = segmentInst;
            } else {
                this->AddSegmentEvent(segmentInst, time, rec->m_endConditionDelay, ANIM_KIT_SEGMENT_EVENT_END, 1);
            }

            break;

        case 4:
            kitInst->m_flags |= KIT_INST_FLAG_REACT_TO_SEGMENT_START;
            break;

        case 5:
            kitInst->m_flags |= KIT_INST_FLAG_REACT_TO_SEGMENT_END;
            break;
    }
}

// OFFSET: 0x0058C690/0x1401F60A0
float CAnimKitManager::ComputeSegmentAnimSpeed(AnimKitSegmentInst* segmentInst) {
    float speed;

    switch (segmentInst->speedType) {
        case 1:
            speed = segmentInst->speed;
            break;

        case 2:
            speed = segmentInst->rec->m_speed * segmentInst->speed;
            break;

        case 3:
            if (this->m_model) {
                M2SequenceInfo info;
                this->m_model->GetSequenceInfo(segmentInst->curAnim, 0, info);
                float moveSpeed = fabs(info.m_moveSpeed);

                if (moveSpeed > 0.0000099999997f) {
                    speed = segmentInst->speed / moveSpeed;
                    break;
                }
            }

            speed = segmentInst->rec->m_speed;
            break;

        default:
            speed = segmentInst->rec->m_speed;
            break;
    }

    if (segmentInst->instFlags & SEGMENT_INST_FLAG_REVERSE) {
        return -speed;
    }

    return speed;
}

// OFFSET: 0x0058C770/0x1401F6190
int32_t CAnimKitManager::ComputeSegmentAnimTime(AnimKitSegmentInst* segmentInst, float speed) {
    if ((this->m_flags & ANIM_KIT_MANAGER_FLAG_FREEZE_PENDING) && this->m_freezeTime != 0xFFFFFFFF) {
        return this->m_freezeTime;
    }

    if (fabs(speed) <= 0.0000099999997f) {
        return segmentInst->startTime;
    }

    int32_t elapsed = static_cast<int32_t>(this->GetCurTime() - segmentInst->startTime);

    return segmentInst->rec->m_animStartTime + static_cast<int32_t>(static_cast<float>(elapsed) * speed);
}

// OFFSET: 0x0058C7E0/0x1401F61F0
int32_t CAnimKitManager::ComputeSegmentSceneTimeDurationUntilAnimEndTime(AnimKitSegmentInst* segmentInst, float speed) {
    float absSpeed = fabs(speed);
    int32_t duration = segmentInst->repetitions * this->m_model->GetSequenceDuration(segmentInst->curAnim, segmentInst->variation);

    if (absSpeed <= 0.0000099999997f || duration <= 0) {
        return 0;
    }

    int32_t time = this->ComputeSegmentAnimTime(segmentInst, speed);

    if (speed <= 0.0f) {
        int32_t remaining = (time - 1) % duration;

        if (remaining < 0) {
            remaining += duration;
        }

        return static_cast<int32_t>(static_cast<float>(remaining + 1) / absSpeed);
    }

    int32_t elapsed = time % duration;

    if (elapsed < 0) {
        elapsed += duration;
    }

    return static_cast<int32_t>(static_cast<float>(duration - elapsed) / speed);
}

// OFFSET: 0x0058C890/0x1401F62C0
int32_t CAnimKitManager::WrapSegmentTiming(AnimKitSegmentInst* segmentInst, float speed, int32_t& time) {
    float absSpeed = fabs(speed);

    if (absSpeed <= 0.0000099999997f) {
        return 0;
    }

    if (segmentInst->curAnim >= CAnimKitManager::s_animDataCount) {
        return 0;
    }

    if (!this->m_model->IsLoaded()) {
        return 0;
    }

    int32_t duration = segmentInst->repetitions * this->m_model->GetSequenceDuration(segmentInst->curAnim, segmentInst->variation);

    if (!duration) {
        return 0;
    }

    int32_t current = time;

    if (static_cast<uint32_t>(current) < static_cast<uint32_t>(duration)) {
        return 0;
    }

    int32_t wraps;
    int32_t remainder;

    if (current < 0) {
        wraps = 1 - current / duration;
        remainder = duration + current % duration;
    } else {
        remainder = current % duration;
        wraps = current / duration;
    }

    time = remainder;
    segmentInst->startTime += static_cast<uint32_t>(wraps * static_cast<int64_t>(static_cast<double>(duration) / absSpeed));

    return 1;
}

// OFFSET: 0x0058C970/0x1401F6390
int32_t CAnimKitManager::FilterAnimKitBoneSetID(uint32_t boneSetID, uint32_t segmentFlags) {
    while (boneSetID) {
        const AnimKitBoneSetRec* rec = g_animKitBoneSetDB.GetRecord(boneSetID);

        if (!((1 << boneSetID) & this->m_blockedBoneSetMask) && ((segmentFlags & 0x4000) || this->m_model->HasBone(rec->m_boneDataID))) {
            break;
        }

        int32_t altBoneSetID = rec->m_altAnimKitBoneSetID;

        if (altBoneSetID != -1 && !((1 << altBoneSetID) & this->m_blockedBoneSetMask)) {
            if (!altBoneSetID) {
                return 0;
            }

            const AnimKitBoneSetRec* altRec = g_animKitBoneSetDB.GetRecord(altBoneSetID);

            if (altRec && this->m_model->HasBone(altRec->m_boneDataID)) {
                return altRec->m_id;
            }
        }

        if (!(segmentFlags & 0x20)) {
            return -1;
        }

        boneSetID = rec->m_parentAnimKitBoneSetID;
    }

    return boneSetID;
}

// OFFSET: 0x0058CF80/0x1401F6C10
float CAnimKitInst::GetSegmentProgress(uint32_t index) {
    if (this->m_flags & (KIT_INST_FLAG_PENDING_DESTROY | KIT_INST_FLAG_DEFERRED)) {
        return 0.0f;
    }

    if (index >= this->m_segmentInsts.Count()) {
        return 0.0f;
    }

    AnimKitSegmentInst* segmentInst = this->m_segmentInsts[index];

    if (segmentInst) {
        float speed = this->m_manager->ComputeSegmentAnimSpeed(segmentInst);
        CAnimKitManager* manager = this->m_manager;
        int32_t time = manager->ComputeSegmentAnimTime(segmentInst, speed);

        if (time > 0) {
            return 1.0 - static_cast<double>(manager->ComputeSegmentSceneTimeDurationUntilAnimEndTime(segmentInst, speed)) / static_cast<double>(time);
        }
    }

    return 0.0f;
}

// OFFSET: 0x0058D020/0x1401F6CD0
uint32_t CAnimKitInst::GetSegmentCount() const {
    return this->m_def->segmentRec.Count();
}

// OFFSET: 0x0058D040/0x1401F6CF0
void CAnimKitManager::OnModelSequenceCallback(uint32_t boneId, uint32_t sequenceId, int32_t interrupted, int32_t primary) {
    if (interrupted || (this->m_flags & (ANIM_KIT_MANAGER_FLAG_EXPRESSIONS_BLOCKED | ANIM_KIT_MANAGER_FLAG_FROZEN))) {
        return;
    }

    uint32_t boneSetID;

    if (boneId >= 35) {
        boneSetID = 0;
    } else {
        boneSetID = s_boneDataBoneSetIDs[boneId];

        if (boneSetID == 0xFFFFFFFF) {
            return;
        }
    }

    AnimKitSegmentBoneSetInst* expressing = this->m_boneSetInsts[boneSetID].expressing[primary == 0];

    if (!expressing) {
        return;
    }

    AnimKitSegmentInst* segmentInst = expressing->segmentInst;

    if (!segmentInst || sequenceId != segmentInst->curAnim) {
        return;
    }

    M2BoneSequenceInfo info;
    this->m_model->GetBoneSequenceInfo(boneId, info);
    const AnimKitSegmentRec* rec = segmentInst->rec;
    int32_t hold = 0;

    if (!(segmentInst->userFlags & 0x4)) {
        uint32_t segmentFlags = rec->m_segmentFlags;

        if ((segmentFlags & 0x10) || (info.m_finished && !(segmentFlags & 0x20))) {
            hold = 1;
        }
    }

    float speed = this->ComputeSegmentAnimSpeed(segmentInst);

    if (hold) {
        segmentInst->instFlags |= SEGMENT_INST_FLAG_ANIM_HELD;

        if (!info.m_finished) {
            uint32_t duration = this->m_model->GetSequenceDuration(sequenceId, segmentInst->variation);
            uint32_t time = speed < 0.0f ? 0 : duration - 1;
            this->m_model->SetBoneSequenceForward(boneId, sequenceId, segmentInst->variation, time, 0.0f, 0, primary, 1, 0, 0);
        }

        return;
    }

    int32_t time = this->ComputeSegmentAnimTime(segmentInst, speed);
    uint8_t variation = segmentInst->variation;
    uint8_t repetitions = segmentInst->repetitions;

    if (rec->m_segmentFlags & 0x4) {
        this->WrapSegmentTiming(segmentInst, speed, time);
    } else {
        while (this->WrapSegmentTiming(segmentInst, speed, time)) {
            this->PickSegmentInstVariationAndRepetitions(segmentInst);
        }
    }

    if (info.m_finished || variation != segmentInst->variation || repetitions != segmentInst->repetitions) {
        int32_t blend = primary && !(segmentInst->segmentFlags & 0x1) && !(segmentInst->userFlags & 0x1) && !(expressing->flags & SEGMENT_BONE_SET_INST_FLAG_NO_BLEND);
        this->m_model->SetBoneSequenceForward(boneId, sequenceId, segmentInst->variation, time, speed, blend, primary, segmentInst->repetitions, 0, 0);
    }
}

// OFFSET: 0x0058D240/0x1401F6FB0
void CAnimKitManager::UpdateHandItemAnim(uint32_t type, CM2Model* model, void* callback, uint64_t param, uint32_t sheathedAnim) {
    uint32_t boneDataID;

    switch (type) {
        case 0:
        case 2:
            boneDataID = 0;
            break;

        case 1:
            boneDataID = 1;
            break;

        case 5:
            boneDataID = 3;
            break;

        case 6:
            boneDataID = 2;
            break;

        case 11:
            boneDataID = 6;
            break;

        default:
            return;
    }

    if (!model || !model->IsLoaded()) {
        return;
    }

    uint32_t boneSetID = s_boneDataBoneSetIDs[boneDataID];

    if (boneSetID == 0xFFFFFFFF) {
        return;
    }

    AnimKitSegmentBoneSetInst* expressing = this->m_boneSetInsts[boneSetID].expressing[0];

    while (!expressing && boneSetID < CAnimKitManager::s_animKitBoneSetCount) {
        boneSetID = g_animKitBoneSetDB.GetRecord(boneSetID)->m_parentAnimKitBoneSetID;

        if (boneSetID < CAnimKitManager::s_animKitBoneSetCount) {
            expressing = this->m_boneSetInsts[boneSetID].expressing[0];
        }
    }

    uint32_t anim = expressing ? expressing->segmentInst->curAnim : 0;
    uint32_t sequenceId = model->GetFallbackSequenceIdFromSequenceId(anim, NULL);

    if (sequenceId == sheathedAnim && sheathedAnim != 0xFFFFFFFF) {
        sequenceId = 0;

        if (expressing) {
            while (boneSetID < CAnimKitManager::s_animKitBoneSetCount) {
                boneSetID = g_animKitBoneSetDB.GetRecord(boneSetID)->m_parentAnimKitBoneSetID;

                if (boneSetID < CAnimKitManager::s_animKitBoneSetCount) {
                    expressing = this->m_boneSetInsts[boneSetID].expressing[0];
                }

                if (!expressing) {
                    break;
                }

                uint32_t parentSequenceId = model->GetFallbackSequenceIdFromSequenceId(expressing->segmentInst->curAnim, NULL);

                if (parentSequenceId != sheathedAnim) {
                    sequenceId = parentSequenceId;
                    break;
                }
            }
        }
    }

    M2BoneSequenceInfo current;
    model->GetBoneSequenceInfo(0xFFFFFFFF, current);

    if (sequenceId == current.m_sequenceId) {
        return;
    }

    M2SequenceInfo info;

    if (!sequenceId && sheathedAnim == 0xFFFFFFFF) {
        info.Clear();
        model->GetSequenceInfo(current.m_sequenceId, current.m_variationIndex, info);

        if (info.m_flags & 0x1) {
            return;
        }
    }

    int32_t oneShot = 0;

    if (sequenceId) {
        info.Clear();
        model->GetSequenceInfo(sequenceId, 0, info);

        if (info.m_flags & 0x1) {
            oneShot = 1;
        }
    }

    uint32_t time = 0;
    float speed = 1.0f;

    if (!oneShot) {
        if (expressing) {
            speed = this->ComputeSegmentAnimSpeed(expressing->segmentInst);
            time = this->ComputeSegmentAnimTime(expressing->segmentInst, speed);
        } else {
            M2BoneSequenceInfo modelInfo;
            this->m_model->GetBoneSequenceInfo(0xFFFFFFFF, modelInfo);
            time = modelInfo.m_time;
            speed = modelInfo.m_speed;
        }
    }

    model->SetSequenceCallback(NULL, 0);
    model->SetBoneSequence(0xFFFFFFFF, sequenceId, 0xFFFFFFFF, time, speed, 1, 1, 0, 0, 0);

    if (oneShot) {
        model->SetSequenceCallback(callback, param);
    }
}

// OFFSET: 0x0058D4E0
AnimKitSegmentBoneSetInst* CAnimKitManager::GetSegmentBoneSetInstStackTop(AnimKitBoneSetInstStack& stack) {
    for (int32_t i = stack.stack.Count(); i > 0;) {
        i--;
        AnimKitSegmentBoneSetInst* segmentBoneSetInst = stack.stack[i];

        if (segmentBoneSetInst && !(segmentBoneSetInst->flags & SEGMENT_BONE_SET_INST_FLAG_PENDING_DESTROY)) {
            return segmentBoneSetInst;
        }
    }

    return NULL;
}

// OFFSET: 0x0058D530/0x1401F7440
AnimKitSegmentInst* CAnimKitManager::GetExpressingHandItemSegmentInst(uint32_t& boneDataID, int32_t primary) {
    boneDataID = 0xFFFFFFFF;

    for (uint32_t i = 0; i < 6; i++) {
        uint32_t handBoneDataID = s_handItemBoneDataIDs[i];
        uint32_t boneSetID;

        if (handBoneDataID >= 35) {
            boneSetID = 0;
        } else {
            boneSetID = s_boneDataBoneSetIDs[handBoneDataID];

            if (boneSetID == 0xFFFFFFFF) {
                continue;
            }
        }

        AnimKitSegmentBoneSetInst* expressing = this->m_boneSetInsts[boneSetID].expressing[primary == 0];

        if (!expressing || !expressing->segmentInst) {
            continue;
        }

        boneDataID = handBoneDataID;

        return expressing->segmentInst;
    }

    return NULL;
}

// OFFSET: 0x0058D5B0/0x1401F7650
int32_t CAnimKitManager::HandleKitReactionToSegmentStart(CAnimKitInst* kitInst, uint8_t segmentIndex, uint32_t time, int32_t allowStop) {
    AnimKitSegmentInst* trigger = kitInst->m_segmentInsts[segmentIndex];
    uint32_t count = kitInst->m_segmentInsts.Count();

    for (uint32_t i = 0; i < count; i++) {
        AnimKitSegmentInst* segmentInst = kitInst->m_segmentInsts[i];

        if (!segmentInst) {
            continue;
        }

        const AnimKitSegmentRec* rec = segmentInst->rec;

        if (segmentIndex == static_cast<uint8_t>(rec->m_startConditionParam) && segmentInst->index != segmentIndex && rec->m_startCondition == 1) {
            if (trigger && (rec->m_segmentFlags & 0x40)) {
                segmentInst->inheritedVariation = trigger->variation;
            }

            if (!kitInst->m_manager->AddSegmentEvent(segmentInst, time, rec->m_startConditionDelay, ANIM_KIT_SEGMENT_EVENT_START, allowStop)) {
                return 0;
            }
        }

        if (segmentIndex == static_cast<uint8_t>(rec->m_endConditionParam) && rec->m_endCondition == 4) {
            if (!kitInst->m_manager->AddSegmentEvent(segmentInst, time, rec->m_endConditionDelay, ANIM_KIT_SEGMENT_EVENT_END, allowStop)) {
                return 0;
            }
        }
    }

    return 1;
}

// OFFSET: 0x0058D6C0/0x1401F74F0
int32_t CAnimKitManager::HandleKitReactionToSegmentEnd(CAnimKitInst* kitInst, uint8_t segmentIndex, uint32_t time, int32_t allowStop) {
    AnimKitSegmentInst* trigger = kitInst->m_segmentInsts[segmentIndex];
    uint32_t count = kitInst->m_segmentInsts.Count();

    for (uint32_t i = 0; i < count; i++) {
        AnimKitSegmentInst* segmentInst = kitInst->m_segmentInsts[i];

        if (!segmentInst) {
            continue;
        }

        const AnimKitSegmentRec* rec = segmentInst->rec;

        if (segmentIndex == static_cast<uint8_t>(rec->m_startConditionParam) && rec->m_startCondition == 2) {
            if (trigger && (rec->m_segmentFlags & 0x40)) {
                segmentInst->inheritedVariation = trigger->variation;
            }

            if (!kitInst->m_manager->AddSegmentEvent(segmentInst, time, rec->m_startConditionDelay, ANIM_KIT_SEGMENT_EVENT_START, allowStop)) {
                return 0;
            }
        }

        if (segmentIndex == static_cast<uint8_t>(rec->m_endConditionParam) && segmentInst->index != segmentIndex && rec->m_endCondition == 5) {
            if (!kitInst->m_manager->AddSegmentEvent(segmentInst, time, rec->m_endConditionDelay, ANIM_KIT_SEGMENT_EVENT_END, allowStop)) {
                return 0;
            }
        }
    }

    return 1;
}

// OFFSET: 0x0058D7D0/0x1401F77B0
void CAnimKitManager::ClearExpressions() {
    uint32_t count = this->m_boneSetInsts.Count();

    for (uint32_t i = 0; i < count; i++) {
        AnimKitBoneSetInst& boneSetInst = this->m_boneSetInsts[i];

        if (boneSetInst.expressing[0]) {
            boneSetInst.expressing[0]->flags &= ~SEGMENT_BONE_SET_INST_FLAG_EXPRESSING;
            boneSetInst.expressing[0] = NULL;
        }

        if (boneSetInst.expressing[1]) {
            boneSetInst.expressing[1]->flags &= ~SEGMENT_BONE_SET_INST_FLAG_EXPRESSING;
            boneSetInst.expressing[1] = NULL;
        }
    }
}

// OFFSET: 0x0058D840/0x1401F7820
void CAnimKitManager::DefaultAnimEndHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t primary) {
    CAnimKitManager* manager = reinterpret_cast<CAnimKitManager*>(static_cast<uintptr_t>(param));

    if (manager) {
        manager->OnModelSequenceCallback(boneId, sequenceId, interrupted, primary);
    }
}

// OFFSET: 0x0058DB90/0x1401F7D30
CAnimKitInst* CAnimKitManager::GetExpressingAnimKitInst(uint32_t boneDataID, int32_t primary) const {
    uint32_t boneSetID;

    if (boneDataID >= 35) {
        boneSetID = 0;
    } else {
        boneSetID = s_boneDataBoneSetIDs[boneDataID];

        if (boneSetID == 0xFFFFFFFF) {
            return NULL;
        }
    }

    AnimKitSegmentBoneSetInst* expressing = const_cast<CAnimKitManager*>(this)->m_boneSetInsts[boneSetID].expressing[primary == 0];

    if (!expressing) {
        return NULL;
    }

    AnimKitSegmentInst* segmentInst = expressing->segmentInst;

    if (!segmentInst) {
        return NULL;
    }

    return segmentInst->kitInst;
}

// OFFSET: 0x0058DBF0
void CAnimKitManager::UpdateNextKitEventTime() {
    CAnimKitInst* kitInst = this->m_kitInsts.Head();

    if (!kitInst || kitInst->m_nextEventTime == 0xFFFFFFFF) {
        this->m_flags &= ~ANIM_KIT_MANAGER_FLAG_KIT_EVENT;
        this->m_nextKitEventTime = 0xFFFFFFFF;
    } else {
        this->m_nextKitEventTime = kitInst->m_nextEventTime;
        this->ScheduleUpdate(ANIM_KIT_MANAGER_FLAG_KIT_EVENT);
    }
}

// OFFSET: 0x0058DC40/0x1401F7D90
void CAnimKitManager::UpdateNextSegmentEventTime() {
    AnimKitSegmentInst* segmentInst = this->m_segmentEvents.Head();

    if (!segmentInst) {
        this->m_flags &= ~ANIM_KIT_MANAGER_FLAG_SEGMENT_EVENT;
        this->m_nextSegmentEventTime = 0xFFFFFFFF;
    } else {
        uint32_t startTime = segmentInst->startEventTime;
        uint32_t time = segmentInst->endEventTime;

        if (startTime != 0xFFFFFFFF && (time == 0xFFFFFFFF || static_cast<int32_t>(startTime - time) < 0)) {
            time = startTime;
        }

        this->m_nextSegmentEventTime = time;
        this->ScheduleUpdate(ANIM_KIT_MANAGER_FLAG_SEGMENT_EVENT);
    }
}

// OFFSET: 0x0058DCB0/0x1401F7E20
int32_t CAnimKitManager::OnPlaySegmentInst(AnimKitSegmentInst* segmentInst, int32_t allowStop) {
    this->ScheduleUpdate(ANIM_KIT_MANAGER_FLAG_EXPRESS);
    segmentInst->instFlags &= ~SEGMENT_INST_FLAG_RESTART_MASK;
    uint32_t now = this->GetCurTime();
    float speed = this->ComputeSegmentAnimSpeed(segmentInst);
    float absSpeed = fabs(speed);

    if (absSpeed <= 0.0000099999997f) {
        segmentInst->startTime = segmentInst->rec->m_animStartTime;
    } else {
        segmentInst->startTime = now;
    }

    this->PickSegmentInstVariationAndRepetitions(segmentInst);
    uint32_t count = segmentInst->boneSetInsts.Count();

    for (uint32_t i = 0; i < count; i++) {
        AnimKitSegmentBoneSetInst* segmentBoneSetInst = segmentInst->boneSetInsts[i];

        if (segmentBoneSetInst) {
            segmentBoneSetInst->flags |= SEGMENT_BONE_SET_INST_FLAG_DIRTY;
        }
    }

    CAnimKitInst* kitInst = segmentInst->kitInst;

    if (kitInst->m_startTime) {
        int32_t duration = this->ComputeSegmentSceneTimeDurationUntilAnimEndTime(segmentInst, speed);
        kitInst = segmentInst->kitInst;
        int32_t offset;

        if (kitInst->m_flags & KIT_INST_FLAG_SPEED_IS_FRACTION) {
            offset = static_cast<int32_t>(static_cast<float>(kitInst->m_startTime) * 0.000015259022f * static_cast<float>(duration));
        } else {
            offset = kitInst->m_startTime % duration;
        }

        segmentInst->startTime -= offset;
    }

    const AnimKitSegmentRec* rec = segmentInst->rec;
    int32_t result;

    switch (rec->m_endCondition) {
        case 1:
            if (!(kitInst->m_flags & KIT_INST_FLAG_DURATION_ELAPSED)) {
                goto react;
            }

        case 0: {
            if (absSpeed <= 0.0000099999997f && !(kitInst->m_flags & KIT_INST_FLAG_DURATION_ELAPSED)) {
                goto react;
            }

            int32_t delay = this->ComputeSegmentSceneTimeDurationUntilAnimEndTime(segmentInst, speed);

            if (!rec->m_endCondition) {
                delay += rec->m_endConditionDelay;
            }

            result = this->AddSegmentEvent(segmentInst, now, delay, ANIM_KIT_SEGMENT_EVENT_END, allowStop);
            break;
        }

        case 4:
            if (segmentInst->index != static_cast<uint8_t>(rec->m_endConditionParam)) {
                goto react;
            }

        case 3:
            result = this->AddSegmentEvent(segmentInst, now, rec->m_endConditionDelay, ANIM_KIT_SEGMENT_EVENT_END, allowStop);
            break;

        default:
            goto react;
    }

    if (!result) {
        return 0;
    }

react:
    if (kitInst->m_flags & KIT_INST_FLAG_REACT_TO_SEGMENT_START) {
        return CAnimKitManager::HandleKitReactionToSegmentStart(kitInst, segmentInst->index, now, allowStop);
    }

    return 1;
}

// OFFSET: 0x0058DE90/0x1401F8020
int32_t CAnimKitManager::OnStopSegmentInst(AnimKitSegmentInst* segmentInst, int32_t allowStop) {
    this->ScheduleUpdate(ANIM_KIT_MANAGER_FLAG_EXPRESS);
    segmentInst->link.Unlink();
    segmentInst->startEventTime = 0xFFFFFFFF;
    segmentInst->endEventTime = 0xFFFFFFFF;
    this->UpdateNextSegmentEventTime();
    CAnimKitInst* kitInst = segmentInst->kitInst;

    if (kitInst->m_flags & KIT_INST_FLAG_REACT_TO_SEGMENT_END) {
        if (!CAnimKitManager::HandleKitReactionToSegmentEnd(kitInst, segmentInst->index, this->m_model->m_scene->m_time, allowStop)) {
            return 0;
        }
    }

    uint32_t loopIndex = segmentInst->rec->m_loopToSegmentIndex;
    int32_t result = 1;

    if (loopIndex != 0xFFFFFFFF && !(kitInst->m_flags & KIT_INST_FLAG_DURATION_ELAPSED) && loopIndex < kitInst->m_segmentInsts.Count() && kitInst->m_segmentInsts[loopIndex] && !CAnimKitManager::s_inChainedSegmentStart) {
        CAnimKitManager::s_inChainedSegmentStart = 1;
        result = this->SetSegmentInstState(kitInst->m_segmentInsts[loopIndex], ANIM_KIT_SEGMENT_STATE_PLAYING, allowStop);
        CAnimKitManager::s_inChainedSegmentStart = 0;
    }

    return result;
}

// OFFSET: 0x0058DFB0/0x1401F81D0
void CAnimKitManager::UninitSegmentBoneSetInst(AnimKitSegmentBoneSetInst* segmentBoneSetInst) {
    AnimKitBoneSetInst& boneSetInst = this->m_boneSetInsts[segmentBoneSetInst->animKitBoneSetID];

    for (uint32_t i = 0; i < 2; i++) {
        if (boneSetInst.expressing[i] == segmentBoneSetInst) {
            boneSetInst.expressing[i] = NULL;
            boneSetInst.flags[i] = 1;

            if (segmentBoneSetInst->flags & SEGMENT_BONE_SET_INST_FLAG_SIMPLE) {
                boneSetInst.flags[i] = 3;
            }

            if (segmentBoneSetInst->flags & SEGMENT_BONE_SET_INST_FLAG_NO_BLEND) {
                boneSetInst.flags[i] |= 4;
            }
        }
    }

    segmentBoneSetInst->flags |= SEGMENT_BONE_SET_INST_FLAG_PENDING_DESTROY;
    segmentBoneSetInst->segmentInst = NULL;
    segmentBoneSetInst->link.Unlink();
    s_pendingDestroySegmentBoneSetInsts.LinkToTail(segmentBoneSetInst);
}

// OFFSET: 0x0058E3E0
void CAnimKitManager::ScheduleFullUpdate() {
    this->m_flags |= ANIM_KIT_MANAGER_FLAG_FORCE_EXPRESS;
    this->ScheduleUpdate(ANIM_KIT_MANAGER_FLAG_EXPRESS);
}

// OFFSET: 0x0058E410/0x1401F8C20
int32_t CAnimKitManager::FindCustomFallbackAnim(uint32_t anim, uint16_t* instFlags) {
    uint16_t flags = this->m_flags;
    CustomFallbackPath* path;

    if ((flags & ANIM_KIT_MANAGER_FLAG_GLUE_MODE) && (path = s_glueFallbackPaths.Ptr(anim, HASHKEY_NONE())) && path->m_count) {
        goto found;
    }

    if ((flags & ANIM_KIT_MANAGER_FLAG_GAME_OBJECT_MODE) && (path = s_gameObjectFallbackPaths.Ptr(anim, HASHKEY_NONE())) && path->m_count) {
        goto found;
    }

    return -1;

found:
    *instFlags = SEGMENT_INST_FLAG_FALLBACK_MASK;
    uint32_t step = 0;
    int32_t result;

    for (const CustomFallbackPathStep* entry = path->m_steps;; entry++) {
        if (entry->flags == 0xFFFF) {
            *instFlags &= 0xFF4F;
        } else {
            if (entry->flags & 0x10) {
                *instFlags ^= SEGMENT_INST_FLAG_REVERSE;
            }

            if (entry->flags & 0x20) {
                *instFlags |= SEGMENT_INST_FLAG_HOLD_LAST_FRAME;
            }

            if (entry->flags & 0x80) {
                *instFlags |= SEGMENT_INST_FLAG_NO_LOOP;
            }
        }

        result = entry->anim;

        if (this->m_model->HasSequence(result)) {
            break;
        }

        step++;

        if (step >= path->m_count) {
            return -1;
        }
    }

    *instFlags &= ~0x200;

    if (step < path->m_keepFallbackCount) {
        *instFlags &= ~0x100;
    }

    return result;
}

// OFFSET: 0x0058E540/0x1401F8E00
void CAnimKitManager::AddKitEvent(CAnimKitInst* kitInst, uint32_t time, int32_t duration) {
    kitInst->m_link.Unlink();
    uint32_t eventTime = time + duration;
    kitInst->m_nextEventTime = eventTime;

    for (CAnimKitInst* node = this->m_kitInsts.Head(); !this->m_kitInsts.IsEnd(node); node = this->m_kitInsts.RawNext(node)) {
        if (node->m_nextEventTime == 0xFFFFFFFF || static_cast<int32_t>(eventTime - node->m_nextEventTime) < 0) {
            this->m_kitInsts.LinkNode(kitInst, 2, node);
            this->UpdateNextKitEventTime();
            return;
        }
    }

    this->m_kitInsts.LinkToTail(kitInst);
    this->UpdateNextKitEventTime();
}

// OFFSET: 0x0058E6D0/0x1401F8FF0
AnimKitSegmentBoneSetInst* CAnimKitManager::InitSegmentBoneSetInst(AnimKitSegmentInst* segmentInst, const AnimKitConfigBoneSetRec* rec, int32_t index, uint8_t boneSetID) {
    uint32_t requestedBoneSetID = boneSetID;

    if (boneSetID >= CAnimKitManager::s_animKitBoneSetCount) {
        requestedBoneSetID = rec->m_animKitBoneSetID;
    }

    int32_t filteredBoneSetID = this->FilterAnimKitBoneSetID(requestedBoneSetID, segmentInst->segmentFlags);

    if (filteredBoneSetID == -1) {
        return NULL;
    }

    AnimKitSegmentBoneSetInst* segmentBoneSetInst = s_segmentBoneSetInstAllocator.New<AnimKitSegmentBoneSetInst>(0);
    segmentBoneSetInst->segmentInst = segmentInst;
    segmentBoneSetInst->index = index;
    segmentBoneSetInst->flags = 0;

    if (segmentInst->segmentFlags & 0x3E00) {
        segmentBoneSetInst->flags = SEGMENT_BONE_SET_INST_FLAG_SUPPRESSIBLE;
    }

    segmentBoneSetInst->animKitBoneSetID = filteredBoneSetID;
    uint8_t priority = g_animKitPriorityDB.GetRecord(rec->m_animKitPriorityID)->m_priority;
    segmentBoneSetInst->priority = priority;

    for (AnimKitSegmentBoneSetInst* node = this->m_segmentBoneSetInsts.Head(); !this->m_segmentBoneSetInsts.IsEnd(node); node = this->m_segmentBoneSetInsts.RawNext(node)) {
        if (priority < node->priority) {
            this->m_segmentBoneSetInsts.LinkNode(segmentBoneSetInst, 2, node);
            return segmentBoneSetInst;
        }
    }

    this->m_segmentBoneSetInsts.LinkToTail(segmentBoneSetInst);

    return segmentBoneSetInst;
}

// OFFSET: 0x0058EB50/0x1401F9440
void CAnimKitInst::SetStopCallback(AnimKitStopCallback callback, uint64_t param) {
    if (this->m_flags & KIT_INST_FLAG_PENDING_DESTROY) {
        return;
    }

    if (this->m_flags & KIT_INST_FLAG_DEFERRED) {
        AnimKitDeferredCommand command;
        command.type = ANIM_KIT_DEFERRED_SET_STOP_CALLBACK;
        command.setStopCallback.callback = callback;
        command.setStopCallback.param = param;
        this->m_deferredCommands.New(command);
        return;
    }

    this->m_stopCallback = callback;
    this->m_stopCallbackParam = param;

    if (callback && (this->m_flags & KIT_INST_FLAG_STOPPED)) {
        callback(this, param);
    }
}

// OFFSET: 0x0058EBC0/0x1401F94F0
void CAnimKitManager::SetGlueMode(int32_t enable) {
    uint16_t flags = this->m_flags;
    uint16_t newFlags = enable ? flags | ANIM_KIT_MANAGER_FLAG_GLUE_MODE : flags & ~ANIM_KIT_MANAGER_FLAG_GLUE_MODE;

    if (newFlags == flags) {
        return;
    }

    this->m_flags = newFlags;

    if (this->m_model && this->m_model->IsLoaded()) {
        this->ScheduleFullUpdate();
    }
}

// OFFSET: 0x0058EC10/0x1401F9550
void CAnimKitManager::SetGameObjectMode(int32_t enable) {
    uint16_t flags = this->m_flags;
    uint16_t newFlags = enable ? flags | ANIM_KIT_MANAGER_FLAG_GAME_OBJECT_MODE : flags & ~ANIM_KIT_MANAGER_FLAG_GAME_OBJECT_MODE;

    if (newFlags == flags) {
        return;
    }

    this->m_flags = newFlags;

    if (this->m_model && this->m_model->IsLoaded()) {
        this->ScheduleFullUpdate();
    }
}

// OFFSET: 0x0058EC60/0x1401F7D90
void CAnimKitManager::Freeze(uint32_t time) {
    this->m_flags = (this->m_flags & ~ANIM_KIT_MANAGER_FLAG_FREEZE_MASK) | ANIM_KIT_MANAGER_FLAG_FREEZE_PENDING;

    if (time != 0xFFFFFFFF) {
        this->m_flags |= ANIM_KIT_MANAGER_FLAG_SNAP;
    }

    this->m_freezeTime = time;
    this->m_flags |= ANIM_KIT_MANAGER_FLAG_FORCE_EXPRESS;
    this->ScheduleUpdate(ANIM_KIT_MANAGER_FLAG_EXPRESS);
}

// OFFSET: 0x0058ECC0/0x1401F95F0
void CAnimKitManager::Init() {
    if (this->m_flags & ANIM_KIT_MANAGER_FLAG_INITIALIZED) {
        return;
    }

    this->m_flags = 0;
    this->m_tier = 0;
    this->m_model = NULL;
    new (&this->m_kitInsts) TSExplicitList<CAnimKitInst, offsetof(CAnimKitInst, m_link)>();
    new (&this->m_segmentEvents) TSExplicitList<AnimKitSegmentInst, offsetof(AnimKitSegmentInst, link)>();
    new (&this->m_segmentBoneSetInsts) TSExplicitList<AnimKitSegmentBoneSetInst, offsetof(AnimKitSegmentBoneSetInst, link)>();
    this->m_boneSetInsts.SetCount(CAnimKitManager::s_animKitBoneSetCount);
    memset(this->m_boneSetInsts.Ptr(), 0, sizeof(AnimKitBoneSetInst) * this->m_boneSetInsts.Count());
    this->m_maxExpressedBoneSetID = -1;
    this->m_blockedBoneSetMask = 0;
    this->m_nextKitEventTime = 0xFFFFFFFF;
    this->m_nextSegmentEventTime = 0xFFFFFFFF;
    this->m_expressionCallback = NULL;
    this->m_expressionCallbackParam = 0;
    this->m_freezeTime = 0;
    this->m_frozenTime = 0;
    s_idleList.LinkToTail(this);
    this->m_flags |= ANIM_KIT_MANAGER_FLAG_INITIALIZED;
}

// OFFSET: 0x0058EDC0/0x1401F9790
uint32_t CAnimKitManager::FilterSegmentAnim(uint32_t anim, int32_t ignoreTier, int32_t* changed, uint16_t* instFlags) {
    if (anim == 0xFFFFFFFF) {
        return 0xFFFFFFFF;
    }

    if (!this->m_model->IsLoaded()) {
        return 0;
    }

    uint32_t setCount = this->m_replacementSets.Count();

    for (uint32_t i = 0; i < setCount; i++) {
        if (this->m_replacementSets[i]) {
            anim = this->m_replacementSets[i]->FilterAnim(anim);
        }
    }

    uint16_t fallbackFlags = 0;
    const AnimationDataRec* animRec = g_animationDataDB.GetRecord(anim);
    M2SequenceFallback fallback;
    fallback.m_id = CAnimKitManager::s_animDataCount;
    fallback.m_mode = 0;
    fallback.m_flags = 0;
    fallback.m_usedDefault = 0;
    uint32_t result;
    int32_t isChanged;

    if (!ignoreTier && (animRec->m_behaviorTier || this->m_tier)) {
        uint32_t tier = this->m_tier;
        uint32_t behavior = animRec->m_behaviorID;
        result = this->FindCustomFallbackAnim(s_behaviorAnims[tier].m_data[behavior], &fallbackFlags);

        if (result == 0xFFFFFFFF) {
            while (true) {
                fallback.m_id = CAnimKitManager::s_animDataCount;
                result = this->m_model->GetFallbackSequenceIdFromSequenceId(s_behaviorAnims[tier].m_data[behavior], &fallback);

                if (result != 0xFFFFFFFF) {
                    break;
                }

                if (tier == s_tierFallback[tier]) {
                    if (changed) {
                        *changed = 1;
                    }

                    return 0xFFFFFFFF;
                }

                tier = s_tierFallback[tier];
            }
        }

        const AnimationDataRec* resultRec = g_animationDataDB.GetRecord(result);
        isChanged = behavior != static_cast<uint32_t>(resultRec->m_behaviorID) || tier != this->m_tier;
    } else {
        result = this->FindCustomFallbackAnim(anim, &fallbackFlags);

        if (result == 0xFFFFFFFF) {
            result = this->m_model->GetFallbackSequenceIdFromSequenceId(anim, &fallback);
        }

        isChanged = result != anim;
    }

    if (changed) {
        *changed = isChanged;
    }

    if (instFlags) {
        if (fallback.m_id < CAnimKitManager::s_animDataCount) {
            if (fallback.m_mode == 1 || fallback.m_mode == 2) {
                *instFlags |= SEGMENT_INST_FLAG_REVERSE;
            }

            if (fallback.m_mode == 3 || fallback.m_mode == 2) {
                *instFlags |= SEGMENT_INST_FLAG_HOLD_LAST_FRAME;
            }

            if (fallback.m_flags) {
                *instFlags |= SEGMENT_INST_FLAG_NO_LOOP;
            }
        }

        *instFlags = fallbackFlags | (*instFlags & ~SEGMENT_INST_FLAG_FALLBACK_MASK);
    }

    return result;
}

// OFFSET: 0x0058EFF0/0x1401F9A40
void CAnimKitManager::SuppressSegmentBoneSetInstStack(AnimKitBoneSetInstStack& stack, TSCGrowableArray<AnimKitSegmentInst*, 8>& suppressed) {
    AnimKitSegmentBoneSetInst* top = CAnimKitManager::GetSegmentBoneSetInstStackTop(stack);

    if (top && (top->flags & SEGMENT_BONE_SET_INST_FLAG_SUPPRESSIBLE)) {
        AnimKitSegmentInst* segmentInst = top->segmentInst;
        suppressed.New(segmentInst);
        segmentInst->suppressedCount++;
    }
}

// OFFSET: 0x0058F030
void CAnimKitManager::SuppressSegmentBoneSetInstStacksByAncestor(uint32_t boneSetID, int32_t layer, TSCGrowableArray<AnimKitSegmentInst*, 8>& suppressed, TSCGrowableArray<AnimKitDesiredBoneSetInst, 10>& desired, int32_t maxBoneSetID) {
    uint32_t bit = 1 << boneSetID;

    for (int32_t i = 0; i <= maxBoneSetID; i++) {
        if (!(bit & s_boneSetHierarchy[i].ancestors)) {
            continue;
        }

        AnimKitBoneSetInstStack& stack = desired.Ptr()[i].layers[layer];

        if (stack.stack.Count()) {
            AnimKitSegmentBoneSetInst* top = CAnimKitManager::GetSegmentBoneSetInstStackTop(stack);

            if (top && (top->flags & SEGMENT_BONE_SET_INST_FLAG_SUPPRESSIBLE)) {
                AnimKitSegmentInst* segmentInst = top->segmentInst;
                suppressed.New(segmentInst);
                segmentInst->suppressedCount++;
            }

            stack.stack.SetCount(0);
        }
    }
}

// OFFSET: 0x0058F100/0x1401F9BF0
void CAnimKitManager::UninitSegmentInst(AnimKitSegmentInst* segmentInst) {
    uint32_t count = segmentInst->boneSetInsts.Count();

    for (uint32_t i = 0; i < count; i++) {
        AnimKitSegmentBoneSetInst*& segmentBoneSetInst = segmentInst->boneSetInsts[i];

        if (segmentBoneSetInst) {
            this->UninitSegmentBoneSetInst(segmentBoneSetInst);
            segmentBoneSetInst = NULL;
        }
    }

    segmentInst->boneSetInsts.SetCount(0);
    segmentInst->instFlags |= SEGMENT_INST_FLAG_PENDING_DESTROY;
    segmentInst->kitInst = NULL;
    segmentInst->link.Unlink();
    s_pendingDestroySegmentInsts.LinkToTail(segmentInst);
}

// OFFSET: 0x0058F210/0x1401F9D50
int32_t CAnimKitManager::ProcessSegmentRec(const AnimKitSegmentRec* rec, uint8_t boneSetID, uint32_t requestedAnim, uint32_t* anim, uint16_t* instFlags) {
    uint32_t configID = rec->m_animKitConfigID;

    if (configID >= CAnimKitManager::s_animKitConfigDefCount) {
        return 2;
    }

    AnimKitConfigDef* configDef = CAnimKitManager::s_animKitConfigDefs[configID];

    if (!configDef) {
        return 2;
    }

    uint32_t flags = rec->m_overrideConfigFlags | configDef->rec->m_configFlags;
    int32_t changed;
    *anim = this->FilterSegmentAnim(rec->m_animID, rec->m_segmentFlags & 0x1, &changed, instFlags);

    if (!(flags & 0xC00D8)) {
        return 0;
    }

    uint32_t checkAnim = *anim;

    if (requestedAnim < CAnimKitManager::s_animDataCount) {
        changed = 0;
        checkAnim = this->FilterSegmentAnim(requestedAnim, rec->m_segmentFlags & 0x1, &changed, NULL);
    }

    uint32_t boneSetCount = CAnimKitManager::s_animKitBoneSetCount;
    uint32_t missing = boneSetCount;

    if (boneSetID >= boneSetCount) {
        uint32_t count = configDef->configBoneSetRec.Count();

        for (uint32_t i = 0; i < count; i++) {
            int32_t id = configDef->configBoneSetRec[i]->m_animKitBoneSetID;

            if (id && !this->m_model->HasBone(g_animKitBoneSetDB.GetRecord(id)->m_boneDataID)) {
                missing = i;
                break;
            }
        }
    } else if (boneSetID) {
        if (!this->m_model->HasBone(g_animKitBoneSetDB.GetRecord(boneSetID)->m_boneDataID)) {
            missing = boneSetID;
        }
    }

    if (missing < boneSetCount && (flags & 0x80)) {
        return 2;
    }

    if (changed && ((flags & 0x10) || (!checkAnim && (flags & 0x80000)))) {
        return 2;
    }

    return (missing < boneSetCount && (flags & 0x40)) || (changed && ((flags & 0x8) || (!checkAnim && (flags & 0x40000))));
}

// OFFSET: 0x0058F3E0/0x1401F9F70
void CAnimKitManager::UpdateLayeredFlag(int32_t schedule) {
    uint32_t layered = 0;

    for (uint32_t i = 0; i < CAnimKitManager::s_animKitBoneSetCount; i++) {
        AnimKitSegmentBoneSetInst* expressing = this->m_boneSetInsts[i].expressing[0];

        if (expressing && expressing->segmentInst) {
            layered |= expressing->segmentInst->segmentFlags & 0x8000;

            if (layered) {
                break;
            }
        }
    }

    if ((layered == 0) == ((this->m_flags & ANIM_KIT_MANAGER_FLAG_LAYERED) == 0)) {
        return;
    }

    if (layered) {
        this->m_flags |= ANIM_KIT_MANAGER_FLAG_LAYERED;
    } else {
        this->m_flags &= ~ANIM_KIT_MANAGER_FLAG_LAYERED;
    }

    if (schedule) {
        this->m_flags |= ANIM_KIT_MANAGER_FLAG_FORCE_EXPRESS;
        this->ScheduleUpdate(ANIM_KIT_MANAGER_FLAG_EXPRESS);
    }
}

// OFFSET: 0x0058F490/0x1401FA050
AnimKitSegmentInst::~AnimKitSegmentInst() {
}

// OFFSET: 0x0058F590/0x1401FA1A0
void CAnimKitManager::InitSystem() {
    if (CAnimKitManager::s_initialized) {
        return;
    }

    memset(s_boneDataBoneSetIDs, 0xFF, sizeof(s_boneDataBoneSetIDs));
    CAnimKitManager::s_animKitBoneSetCount = g_animKitBoneSetDB.m_maxID + 1;
    memset(s_boneSetHierarchy, 0, sizeof(s_boneSetHierarchy));

    for (int32_t i = 0; i < g_animKitBoneSetDB.m_numRecords; i++) {
        const AnimKitBoneSetRec* rec = g_animKitBoneSetDB.GetRecordByIndex(i);

        if (static_cast<uint32_t>(rec->m_parentAnimKitBoneSetID) < CAnimKitManager::s_animKitBoneSetCount) {
            s_boneSetHierarchy[rec->m_parentAnimKitBoneSetID].descendants |= 1 << rec->m_id;
        }

        if (static_cast<uint32_t>(rec->m_boneDataID) < 35) {
            s_boneDataBoneSetIDs[rec->m_boneDataID] = i;
        }
    }

    InitBoneSetAncestors(0, 0);

    CAnimKitManager::s_animKitDefCount = g_animKitDB.m_maxID + 1;
    CAnimKitManager::s_animKitDefs = static_cast<AnimKitDef**>(SMemAlloc(sizeof(AnimKitDef*) * CAnimKitManager::s_animKitDefCount, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));

    for (int32_t i = 0; i < g_animKitDB.m_numRecords; i++) {
        const AnimKitRec* rec = g_animKitDB.GetRecordByIndex(i);
        AnimKitDef* def = new (SMemAlloc(sizeof(AnimKitDef), __FILE__, __LINE__, 0)) AnimKitDef;
        CAnimKitManager::s_animKitDefs[rec->m_id] = def;
        CAnimKitManager::s_animKitDefs[rec->m_id]->rec = rec;
    }

    for (int32_t i = 0; i < g_animKitSegmentDB.m_numRecords; i++) {
        const AnimKitSegmentRec* rec = g_animKitSegmentDB.GetRecordByIndex(i);
        CAnimKitManager::s_animKitDefs[rec->m_parentAnimKitID]->segmentRec.New(rec);
    }

    CAnimKitManager::s_animKitConfigDefCount = g_animKitConfigDB.m_maxID + 1;
    CAnimKitManager::s_animKitConfigDefs = static_cast<AnimKitConfigDef**>(SMemAlloc(sizeof(AnimKitConfigDef*) * CAnimKitManager::s_animKitConfigDefCount, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));

    for (int32_t i = 0; i < g_animKitConfigDB.m_numRecords; i++) {
        const AnimKitConfigRec* rec = g_animKitConfigDB.GetRecordByIndex(i);
        AnimKitConfigDef* def = new (SMemAlloc(sizeof(AnimKitConfigDef), __FILE__, __LINE__, 0)) AnimKitConfigDef;
        CAnimKitManager::s_animKitConfigDefs[rec->m_id] = def;
        CAnimKitManager::s_animKitConfigDefs[rec->m_id]->rec = rec;
    }

    for (int32_t i = 0; i < g_animKitConfigBoneSetDB.m_numRecords; i++) {
        const AnimKitConfigBoneSetRec* rec = g_animKitConfigBoneSetDB.GetRecordByIndex(i);
        CAnimKitManager::s_animKitConfigDefs[rec->m_parentAnimKitConfigID]->configBoneSetRec.New(rec);
    }

    CAnimKitManager::s_animDataCount = g_animationDataDB.m_maxID + 1;

    for (uint32_t tier = 0; tier < ANIM_KIT_TIER_COUNT; tier++) {
        s_behaviorAnims[tier].SetCount(CAnimKitManager::s_animDataCount);
        memset(s_behaviorAnims[tier].m_data, 0xFF, sizeof(uint16_t) * CAnimKitManager::s_animDataCount);
    }

    for (uint32_t anim = 0; anim < CAnimKitManager::s_animDataCount; anim++) {
        const AnimationDataRec* rec = g_animationDataDB.GetRecord(anim);

        if (rec) {
            s_behaviorAnims[rec->m_behaviorTier].m_data[rec->m_behaviorID] = anim;
        }
    }

    CAnimKitManager::s_initialized = 1;
}

// OFFSET: 0x0058FBB0/0x1401FA840
CAnimKitManager::CAnimKitManager() {
    this->m_flags &= ~ANIM_KIT_MANAGER_FLAG_INITIALIZED;
    this->Init();
}

// OFFSET: 0x0058FC20/0x1401FA8D0
uint32_t CAnimKitManager::FilterAnim(uint32_t anim, int32_t allowFallback) {
    if (anim == 0xFFFFFFFF) {
        return 0xFFFFFFFF;
    }

    int32_t changed;
    uint32_t result = this->FilterSegmentAnim(anim, 0, &changed, NULL);

    if (!allowFallback && changed) {
        return 0xFFFFFFFF;
    }

    return result;
}

// OFFSET: 0x0058FC50/0x1401FA910
void CAnimKitManager::ResetModel() {
    CM2Model* model = this->m_model;

    if (!model || !model->IsLoaded()) {
        return;
    }

    uint32_t anim = this->FilterSegmentAnim(0, 0, NULL, NULL);

    if (model->GetBoneSequenceId(0xFFFFFFFF) != anim) {
        model->SetBoneSequence(0xFFFFFFFF, anim, 0xFFFFFFFF, 0, 1.0f, 0, 1, 0, 0, 0);
    }

    for (uint32_t boneSetID = 1; boneSetID < CAnimKitManager::s_animKitBoneSetCount; boneSetID++) {
        const AnimKitBoneSetRec* rec = g_animKitBoneSetDB.GetRecord(boneSetID);
        uint32_t boneId = rec->m_boneDataID;

        if (model->HasBone(boneId) && model->GetBoneSequenceId(boneId) != 0xFFFFFFFF) {
            model->UnsetBoneSequence(rec->m_boneDataID, 0, 1);

            for (uint32_t last = boneId + rec->m_extraBoneCount; boneId <= last; boneId++) {
                if (model->HasBone(boneId)) {
                    model->UnsetBoneSequence(boneId, 0, 1);
                }
            }
        }
    }

    this->m_flags |= ANIM_KIT_MANAGER_FLAG_FORCE_EXPRESS;
    this->ScheduleUpdate(ANIM_KIT_MANAGER_FLAG_EXPRESS);
    this->m_flags |= ANIM_KIT_MANAGER_FLAG_SNAP;
}

// OFFSET: 0x0058FD90
uint32_t CAnimKitManager::GetSegmentInstExpressingAnim(AnimKitSegmentInst* segmentInst, uint32_t boneDataID, int32_t requested) {
    if (segmentInst) {
        uint16_t anim;

        if (requested) {
            anim = segmentInst->requestedAnim >= CAnimKitManager::s_animDataCount ? static_cast<uint16_t>(segmentInst->rec->m_animID) : segmentInst->requestedAnim;
        } else {
            anim = segmentInst->curAnim;
        }

        if (anim < CAnimKitManager::s_animDataCount) {
            return anim;
        }
    }

    if (boneDataID == 0xFFFFFFFF) {
        return this->FilterSegmentAnim(0, 0, NULL, NULL);
    }

    return 0xFFFFFFFF;
}

// OFFSET: 0x0058FDF0/0x1401FAAD0
void CAnimKitManager::SetExpressionsBlocked(int32_t blocked, int32_t reset) {
    if ((blocked == 0) == ((this->m_flags & ANIM_KIT_MANAGER_FLAG_EXPRESSIONS_BLOCKED) == 0)) {
        return;
    }

    if (reset) {
        this->ResetModel();
    }

    if (blocked) {
        this->m_flags |= ANIM_KIT_MANAGER_FLAG_EXPRESSIONS_BLOCKED;
    } else {
        this->m_flags &= ~ANIM_KIT_MANAGER_FLAG_EXPRESSIONS_BLOCKED;
    }
}

// OFFSET: 0x0058FE40/0x1401FAB30
void CAnimKitManager::MirrorToModel(CM2Model* model, CAnimKitManager* filter) {
    CM2Model* source = this->m_model;

    if (!model || !source || !model->IsLoaded() || !source->IsLoaded()) {
        return;
    }

    M2BoneSequenceInfo info;
    source->GetBoneSequenceInfo(0xFFFFFFFF, info);

    if (static_cast<int32_t>(info.m_time) < 0) {
        info.m_time = 0;
    }

    if (info.m_sequenceId >= 0x294) {
        uint32_t anim = this->FilterSegmentAnim(0, 0, NULL, NULL);
        model->SetBoneSequence(0xFFFFFFFF, anim, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    } else {
        uint32_t anim = info.m_sequenceId;

        if (filter) {
            anim = filter->FilterSegmentAnim(info.m_sequenceId, 0, NULL, NULL);
        }

        model->SetBoneSequence(0xFFFFFFFF, anim, info.m_variationIndex, info.m_time, info.m_speed, 1, 1, 0, 0, 0);
    }

    for (uint32_t boneId = 0; boneId < 35; boneId++) {
        int32_t sourceHasBone = source->HasBone(boneId);
        int32_t modelHasBone = model->HasBone(boneId);

        if (sourceHasBone) {
            if (!modelHasBone) {
                continue;
            }

            source->GetBoneSequenceInfo(boneId, info);

            if (static_cast<int32_t>(info.m_time) < 0) {
                info.m_time = 0;
            }

            if (info.m_sequenceId < 0x294) {
                uint32_t anim = info.m_sequenceId;

                if (filter) {
                    anim = filter->FilterSegmentAnim(info.m_sequenceId, 0, NULL, NULL);
                }

                model->SetBoneSequence(boneId, anim, info.m_variationIndex, info.m_time, info.m_speed, 1, 1, 0, 0, 0);
                continue;
            }
        } else if (!modelHasBone) {
            continue;
        }

        if (model->GetBoneSequenceId(boneId) != 0xFFFFFFFF) {
            model->UnsetBoneSequence(boneId, 1, 1);
        }
    }

    for (uint32_t boneSetID = 1; boneSetID < CAnimKitManager::s_animKitBoneSetCount; boneSetID++) {
        const AnimKitBoneSetRec* rec = g_animKitBoneSetDB.GetRecord(boneSetID);
        uint32_t first = rec->m_boneDataID;
        uint32_t last = first + rec->m_extraBoneCount;

        for (uint32_t boneId = first; boneId <= last; boneId++) {
            if (source->HasBone(boneId) || !model->HasBone(boneId)) {
                continue;
            }

            for (uint32_t layer = 0; layer < 2; layer++) {
                AnimKitSegmentBoneSetInst* expressing = this->m_boneSetInsts[boneSetID].expressing[layer];
                AnimKitSegmentInst* segmentInst = expressing ? expressing->segmentInst : NULL;

                if (segmentInst) {
                    uint32_t anim = segmentInst->curAnim;

                    if (filter) {
                        anim = filter->FilterSegmentAnim(anim, 0, NULL, NULL);
                    }

                    float speed = this->ComputeSegmentAnimSpeed(segmentInst);
                    int32_t time;

                    if (segmentInst->rec->m_segmentFlags & 0x8) {
                        time = segmentInst->rec->m_animStartTime;
                        speed = 0.0f;
                    } else if (segmentInst->instFlags & (SEGMENT_INST_FLAG_ANIM_HELD | SEGMENT_INST_FLAG_HOLD_LAST_FRAME)) {
                        if (speed < 0.0f) {
                            time = 0;
                        } else {
                            time = segmentInst->repetitions * this->m_model->GetSequenceDuration(anim, segmentInst->variation) - 1;
                        }

                        speed = 0.0f;
                    } else {
                        time = this->ComputeSegmentAnimTime(segmentInst, speed);
                        this->WrapSegmentTiming(segmentInst, speed, time);
                    }

                    if (time < 0) {
                        time = 0;
                    }

                    model->SetBoneSequence(boneId, anim, segmentInst->variation, time, speed, 1, layer == 0, segmentInst->repetitions, 0, 0);
                } else if (!layer && model->GetBoneSequenceId(boneId) != 0xFFFFFFFF) {
                    model->UnsetBoneSequence(boneId, 1, 1);
                }
            }
        }
    }

    if (source->m_lastAnimTime) {
        uint32_t time = model->m_scene->m_time;

        if (!time) {
            time = 1;
        }

        model->m_lastAnimTime = time;
    } else {
        model->m_lastAnimTime = 0;
    }
}

// OFFSET: 0x005901D0/0x1401FB110
void CAnimKitManager::MirrorFrom(CAnimKitManager* source, int32_t filter) {
    source->MirrorToModel(this->m_model, filter ? this : NULL);
}

// OFFSET: 0x005901F0/0x1401FB180
void CAnimKitManager::UninitKitInst(CAnimKitInst* kitInst) {
    uint32_t count = kitInst->m_segmentInsts.Count();

    for (uint32_t i = 0; i < count; i++) {
        AnimKitSegmentInst*& segmentInst = kitInst->m_segmentInsts[i];

        if (segmentInst) {
            this->UninitSegmentInst(segmentInst);
            segmentInst = NULL;
        }
    }

    kitInst->m_segmentInsts.SetCount(0);
    kitInst->m_flags |= KIT_INST_FLAG_PENDING_DESTROY;
    kitInst->m_link.Unlink();
    s_pendingDestroyKitInsts.LinkToTail(kitInst);
    this->UpdateNextKitEventTime();
    this->UpdateNextSegmentEventTime();
}

// OFFSET: 0x005903B0/0x1401FB410
AnimKitDesiredBoneSetInst::~AnimKitDesiredBoneSetInst() {
}

// OFFSET: 0x00590520/0x1401FB630
CAnimKitInst::~CAnimKitInst() {
}

// OFFSET: 0x005905C0/0x1401FB720
CAnimKitManager* CAnimKitManager::Alloc() {
    return s_managerAllocator.New<CAnimKitManager>(0);
}

// OFFSET: 0x005905F0/0x1401FB760
uint32_t CAnimKitManager::GetExpressingAnim(uint32_t boneDataID, int32_t primary, int32_t requested) {
    uint32_t boneSetID;

    if (boneDataID >= 35) {
        boneSetID = 0;
    } else {
        boneSetID = s_boneDataBoneSetIDs[boneDataID];

        if (boneSetID == 0xFFFFFFFF) {
            return this->GetSegmentInstExpressingAnim(NULL, boneDataID, requested);
        }
    }

    AnimKitSegmentBoneSetInst* expressing = this->m_boneSetInsts[boneSetID].expressing[primary == 0];

    if (!expressing) {
        return this->GetSegmentInstExpressingAnim(NULL, boneDataID, requested);
    }

    return this->GetSegmentInstExpressingAnim(expressing->segmentInst, boneDataID, requested);
}

// OFFSET: 0x00590660/0x1401FB830
uint32_t CAnimKitManager::GetExpressingHandItemAnim(uint32_t& boneDataID, int32_t primary, int32_t requested) {
    boneDataID = 0xFFFFFFFF;
    AnimKitSegmentInst* segmentInst = this->GetExpressingHandItemSegmentInst(boneDataID, primary);

    return this->GetSegmentInstExpressingAnim(segmentInst, boneDataID, requested);
}

// OFFSET: 0x00590690/0x1401FB8E0
void CAnimKitManager::Uninit() {
    if (!(this->m_flags & ANIM_KIT_MANAGER_FLAG_INITIALIZED)) {
        return;
    }

    CAnimKitInst* kitInst;

    while ((kitInst = this->m_kitInsts.Head()) != NULL) {
        this->UninitKitInst(kitInst);
    }

    s_updateList.UnlinkNode(this);
    s_idleList.UnlinkNode(this);
    this->m_flags &= ~ANIM_KIT_MANAGER_FLAG_INITIALIZED;
}

// OFFSET: 0x005906E0/0x1401FB9E0
void CAnimKitManager::SuppressSegmentBoneSetInstStacksByDescendant(uint32_t boneSetID, int32_t layer, TSCGrowableArray<AnimKitSegmentInst*, 8>& suppressed, TSCGrowableArray<AnimKitDesiredBoneSetInst, 10>& desired, int32_t maxBoneSetID) {
    uint32_t bit = 1 << boneSetID;

    for (int32_t i = 0; i <= maxBoneSetID; i++) {
        if (!(bit & s_boneSetHierarchy[i].descendants)) {
            continue;
        }

        AnimKitBoneSetInstStack& stack = desired.Ptr()[i].layers[layer];

        if (!stack.dirty) {
            stack.dirty = 0;
            continue;
        }

        AnimKitBoneSetInstStack removed;
        int32_t index = stack.stack.Count();

        while (index > 0) {
            index--;
            AnimKitSegmentBoneSetInst* segmentBoneSetInst = stack.stack[index];
            AnimKitSegmentInst* segmentInst = segmentBoneSetInst->segmentInst;

            if (!(segmentBoneSetInst->flags & SEGMENT_BONE_SET_INST_FLAG_PENDING_DESTROY) && !(segmentInst->segmentFlags & 0x20000)) {
                stack.stack.SetCount(index + 1);
                break;
            }

            if (segmentInst->segmentFlags & 0x20000) {
                suppressed.New(segmentInst);
                segmentInst->suppressedCount++;
            } else {
                removed.stack.New(segmentBoneSetInst);
            }
        }

        for (int32_t j = removed.stack.Count(); j > 0;) {
            j--;
            AnimKitSegmentBoneSetInst* segmentBoneSetInst = removed.stack[j];
            stack.stack.New(segmentBoneSetInst);
            AnimKitSegmentInst* segmentInst = segmentBoneSetInst->segmentInst;
            int32_t found = 0;

            if (segmentInst) {
                for (uint32_t k = 0; k < suppressed.Count(); k++) {
                    if (suppressed[k] == segmentInst) {
                        found = 1;
                        suppressed[k] = NULL;
                    }
                }

                if (found) {
                    segmentInst->suppressedCount--;
                }
            }
        }

        stack.dirty = 0;
    }
}

// OFFSET: 0x005910C0/0x1401FCA50
CAnimKitManager::~CAnimKitManager() {
    this->Uninit();
}

// OFFSET: 0x00591240/0x1401FCC30
AnimKitSegmentInst* CAnimKitManager::InitSegmentInst(CAnimKitInst* kitInst, int32_t index, uint16_t anim, uint16_t requestedAnim, uint8_t boneSetID, uint16_t instFlags, uint8_t blendTimeMode, uint16_t blendTime) {
    const AnimKitSegmentRec* rec = kitInst->m_def->segmentRec[index];
    uint16_t filteredAnim;

    if (requestedAnim == CAnimKitManager::s_animDataCount) {
        filteredAnim = CAnimKitManager::s_animDataCount;
    } else {
        filteredAnim = this->FilterSegmentAnim(requestedAnim, rec->m_segmentFlags & 0x1, NULL, &instFlags);
    }

    AnimKitSegmentInst* segmentInst = s_segmentInstAllocator.New<AnimKitSegmentInst>(0);
    segmentInst->kitInst = kitInst;
    segmentInst->instFlags = instFlags;
    segmentInst->index = index;
    segmentInst->rec = rec;
    segmentInst->boneSetInstCount = 0;
    segmentInst->suppressedCount = 0;
    segmentInst->variation = 0;
    segmentInst->repetitions = 0;
    segmentInst->userFlags = 0;
    segmentInst->speedType = 0;
    segmentInst->forcedVariation = -1;
    segmentInst->forcedRepetitions = 0;
    segmentInst->requestedAnim = requestedAnim;
    segmentInst->speed = 0.0f;
    segmentInst->curAnim = filteredAnim < CAnimKitManager::s_animDataCount ? filteredAnim : anim;
    segmentInst->inheritedVariation = -1;

    uint32_t configID = rec->m_animKitConfigID;
    AnimKitConfigDef* configDef;

    if (configID >= CAnimKitManager::s_animKitConfigDefCount || (configDef = CAnimKitManager::s_animKitConfigDefs[configID]) == NULL) {
        segmentInst->link.Unlink();
        s_segmentInstAllocator.Delete(segmentInst);
        return NULL;
    }

    segmentInst->startTime = 0;
    segmentInst->segmentFlags = rec->m_overrideConfigFlags | configDef->rec->m_configFlags;
    segmentInst->blendTimeMode = blendTimeMode;
    segmentInst->startEventTime = 0xFFFFFFFF;
    segmentInst->endEventTime = 0xFFFFFFFF;
    segmentInst->blendTime = blendTime;
    uint32_t count = configDef->configBoneSetRec.Count();

    for (uint32_t i = 0; i < count; i++) {
        AnimKitSegmentBoneSetInst* segmentBoneSetInst = this->InitSegmentBoneSetInst(segmentInst, configDef->configBoneSetRec[i], i, boneSetID);

        if (segmentBoneSetInst) {
            segmentInst->boneSetInstCount++;
        }

        segmentInst->boneSetInsts.New(segmentBoneSetInst);
    }

    if (!segmentInst->boneSetInstCount) {
        segmentInst->link.Unlink();
        s_segmentInstAllocator.Delete(segmentInst);
        return NULL;
    }

    return segmentInst;
}

// OFFSET: 0x005915D0/0x1401FD0C0
void CAnimKitManager::GarbageCollect() {
    CAnimKitInst* kitInst = s_pendingDestroyKitInsts.Head();

    while (!s_pendingDestroyKitInsts.IsEnd(kitInst)) {
        CAnimKitInst* next = s_pendingDestroyKitInsts.RawNext(kitInst);
        kitInst->m_link.Unlink();
        s_kitInstAllocator.Delete(kitInst);
        kitInst = next;
    }

    AnimKitSegmentInst* segmentInst = s_pendingDestroySegmentInsts.Head();

    while (!s_pendingDestroySegmentInsts.IsEnd(segmentInst)) {
        AnimKitSegmentInst* next = s_pendingDestroySegmentInsts.RawNext(segmentInst);
        segmentInst->link.Unlink();
        s_segmentInstAllocator.Delete(segmentInst);
        segmentInst = next;
    }

    AnimKitSegmentBoneSetInst* segmentBoneSetInst = s_pendingDestroySegmentBoneSetInsts.Head();

    while (!s_pendingDestroySegmentBoneSetInsts.IsEnd(segmentBoneSetInst)) {
        AnimKitSegmentBoneSetInst* next = s_pendingDestroySegmentBoneSetInsts.RawNext(segmentBoneSetInst);
        segmentBoneSetInst->link.Unlink();
        s_segmentBoneSetInstAllocator.Delete(segmentBoneSetInst);
        segmentBoneSetInst = next;
    }
}

// OFFSET: 0x005917F0/0x1401FD380
int32_t CAnimKitManager::PostInitKitInst(CAnimKitInst* kitInst) {
    int32_t duration = kitInst->m_def->rec->m_oneShotDuration;

    if (duration <= 0) {
        if (!(kitInst->m_flags & KIT_INST_FLAG_MAINTAIN)) {
            kitInst->m_flags |= KIT_INST_FLAG_DURATION_ELAPSED;
        }
    } else {
        this->AddKitEvent(kitInst, this->m_model->m_scene->m_time, duration);
    }

    uint32_t count = kitInst->m_def->segmentRec.Count();
    uint32_t* anims = static_cast<uint32_t*>(alloca(sizeof(uint32_t) * count));
    uint16_t* instFlags = static_cast<uint16_t*>(alloca(sizeof(uint16_t) * count));
    int32_t* valid = static_cast<int32_t*>(alloca(sizeof(int32_t) * count));
    memset(instFlags, 0, sizeof(uint16_t) * count);
    memset(valid, 0, sizeof(int32_t) * count);
    uint32_t i;

    for (i = 0; i < count; i++) {
        int32_t result = this->ProcessSegmentRec(kitInst->m_def->segmentRec[i], kitInst->m_boneSetID, kitInst->m_anim, &anims[i], &instFlags[i]);

        if (result == 2) {
            break;
        }

        if (result != 1 && anims[i] != 0xFFFFFFFF) {
            valid[i] = 1;
        }
    }

    if (i == count) {
        TSStackArray<AnimKitSegmentInst*> play(static_cast<AnimKitSegmentInst**>(alloca(sizeof(AnimKitSegmentInst*) * count)), count, 0);
        TSStackArray<AnimKitSegmentInst*> stop(static_cast<AnimKitSegmentInst**>(alloca(sizeof(AnimKitSegmentInst*) * count)), count, 0);

        for (uint32_t index = 0; index < count; index++) {
            AnimKitSegmentInst* segmentInst = NULL;

            if (valid[index]) {
                segmentInst = this->InitSegmentInst(kitInst, index, anims[index], kitInst->m_anim, kitInst->m_boneSetID, instFlags[index], kitInst->m_blendTimeMode, kitInst->m_blendTime);

                if (segmentInst) {
                    kitInst->m_blendTime = 0;
                    kitInst->m_segmentCount++;
                    kitInst->m_blendTimeMode = 0;
                    this->PostInitSegmentInst(segmentInst, play, stop);
                }
            }

            kitInst->m_segmentInsts.New(segmentInst);
        }

        for (uint32_t j = 0; j < play.m_count; j++) {
            if (!this->SetSegmentInstState(play.m_data[j], ANIM_KIT_SEGMENT_STATE_PLAYING, 0)) {
                return kitInst->Stop(0, NULL);
            }
        }

        for (uint32_t j = 0; j < stop.m_count; j++) {
            if (!this->SetSegmentInstState(stop.m_data[j], ANIM_KIT_SEGMENT_STATE_STOPPED, 0)) {
                return kitInst->Stop(0, NULL);
            }
        }

        if (kitInst->m_segmentCount && (kitInst->m_flags & (KIT_INST_FLAG_REACT_TO_SEGMENT_START | KIT_INST_FLAG_REACT_TO_SEGMENT_END))) {
            uint32_t time = this->m_model->m_scene->m_time;

            for (uint32_t k = 0; k < count; k++) {
                if (kitInst->m_segmentInsts[k]) {
                    continue;
                }

                if ((kitInst->m_flags & KIT_INST_FLAG_REACT_TO_SEGMENT_START) && !CAnimKitManager::HandleKitReactionToSegmentStart(kitInst, k, time, 0)) {
                    return kitInst->Stop(0, NULL);
                }

                if ((kitInst->m_flags & KIT_INST_FLAG_REACT_TO_SEGMENT_END) && !CAnimKitManager::HandleKitReactionToSegmentEnd(kitInst, k, time, 0)) {
                    return kitInst->Stop(0, NULL);
                }
            }
        }
    }

    if (!kitInst->m_segmentCount) {
        return kitInst->Stop(0, NULL);
    }

    kitInst->m_startTime = 0;

    return 1;
}

// OFFSET: 0x00591E70/0x1401FDE50
void CAnimKitManager::UninitSystem() {
    if (!CAnimKitManager::s_initialized) {
        return;
    }

    CAnimKitManager::GarbageCollect();

    if (CAnimKitManager::s_animKitDefs) {
        for (uint32_t i = 0; i < CAnimKitManager::s_animKitDefCount; i++) {
            if (CAnimKitManager::s_animKitDefs[i]) {
                delete CAnimKitManager::s_animKitDefs[i];
            }
        }

        SMemFree(CAnimKitManager::s_animKitDefs, __FILE__, __LINE__, 0);
        CAnimKitManager::s_animKitDefs = NULL;
    }

    CAnimKitManager::s_animKitDefCount = 0;

    if (CAnimKitManager::s_animKitConfigDefs) {
        for (uint32_t i = 0; i < CAnimKitManager::s_animKitConfigDefCount; i++) {
            if (CAnimKitManager::s_animKitConfigDefs[i]) {
                delete CAnimKitManager::s_animKitConfigDefs[i];
            }
        }

        SMemFree(CAnimKitManager::s_animKitConfigDefs, __FILE__, __LINE__, 0);
        CAnimKitManager::s_animKitConfigDefs = NULL;
    }

    CAnimKitManager::s_animKitConfigDefCount = 0;
    CAnimKitManager::s_initialized = 0;
}

// OFFSET: 0x00591FA0/0x1401FDFF0
void CAnimKitManager::Free(CAnimKitManager* manager) {
    s_managerAllocator.Delete(manager);
}

// OFFSET: 0x00591FC0/0x1401FE020
CAnimKitInst* CAnimKitManager::InitKitInst(uint32_t animKitID, int32_t maintain, uint32_t anim, AnimKitExtraPlayParams* params, CAnimKitInst* kitInst) {
    if (!this->m_model) {
        return NULL;
    }

    if (animKitID >= CAnimKitManager::s_animKitDefCount) {
        return NULL;
    }

    AnimKitDef* def = CAnimKitManager::s_animKitDefs[animKitID];

    if (!def) {
        return NULL;
    }

    CAnimKitInst* inst;

    if (kitInst) {
        kitInst->m_link.Unlink();
        inst = kitInst;
    } else {
        inst = s_kitInstAllocator.New<CAnimKitInst>(0);
    }

    this->m_kitInsts.LinkToTail(inst);
    inst->m_segmentCount = 0;
    inst->m_stoppedSegmentCount = 0;
    inst->m_flags = maintain != 0 ? KIT_INST_FLAG_MAINTAIN : 0;

    if (params && params->oneShot && def->rec->m_oneShotDuration <= 0) {
        inst->m_flags |= KIT_INST_FLAG_DURATION_ELAPSED;
    }

    if (!kitInst) {
        inst->m_boneSetID = params ? params->boneSetID : 0xFF;
        inst->m_flags &= ~KIT_INST_FLAG_SPEED_IS_FRACTION;
        inst->m_anim = anim == 0xFFFFFFFF ? CAnimKitManager::s_animDataCount : anim;
        inst->m_startTime = 0;

        if (params) {
            float fraction = params->startFraction;

            if (fraction <= 0.0f) {
                inst->m_startTime = params->startTime;
            } else {
                inst->m_startTime = static_cast<int32_t>(fraction * 65535.0f);
                inst->m_flags |= KIT_INST_FLAG_SPEED_IS_FRACTION;
            }
        }
    }

    inst->m_blendTimeMode = params ? params->blendTimeMode : 0;
    inst->m_blendTime = params ? params->blendTime : 0;
    inst->m_manager = this;
    inst->m_def = def;

    if (!kitInst) {
        inst->m_stopCallback = NULL;
    }

    inst->m_nextEventTime = 0xFFFFFFFF;

    if (this->m_model->IsLoaded()) {
        return this->PostInitKitInst(inst) ? inst : NULL;
    }

    inst->m_flags |= KIT_INST_FLAG_DEFERRED;

    return inst;
}

// OFFSET: 0x005921F0/0x1401FE320
int32_t CAnimKitInst::Stop(int32_t playStopKit, CAnimKitInst** stopKitInst) {
    if (this->m_flags & KIT_INST_FLAG_PENDING_DESTROY) {
        return 0;
    }

    if (this->m_flags & KIT_INST_FLAG_DEFERRED) {
        AnimKitDeferredCommand command;
        command.type = ANIM_KIT_DEFERRED_STOP;
        command.stop.playStopKit = playStopKit;
        this->m_deferredCommands.New(command);
        return 1;
    }

    if (this->m_flags & KIT_INST_FLAG_STOPPED) {
        return 1;
    }

    this->m_flags |= KIT_INST_FLAG_STOPPING | KIT_INST_FLAG_STOPPED;
    uint32_t count = this->m_segmentInsts.Count();

    for (uint32_t i = 0; i < count; i++) {
        AnimKitSegmentInst* segmentInst = this->m_segmentInsts[i];

        if (segmentInst) {
            this->m_manager->SetSegmentInstState(segmentInst, ANIM_KIT_SEGMENT_STATE_STOPPED, 0);
        }
    }

    this->m_manager->m_kitInsts.LinkToTail(this);
    this->m_manager->UpdateNextKitEventTime();

    if (playStopKit) {
        int32_t stopKitID = this->m_def->rec->m_oneShotStopAnimKitID;

        if (stopKitID > 0) {
            CAnimKitInst* stopKit = this->m_manager->InitKitInst(stopKitID, 0, 0xFFFFFFFF, NULL, NULL);

            if (stopKitInst) {
                *stopKitInst = stopKit;
            }
        }
    }

    if (this->m_stopCallback) {
        this->m_stopCallback(this, this->m_stopCallbackParam);
    }

    if (!(this->m_flags & KIT_INST_FLAG_MAINTAIN)) {
        this->m_manager->UninitKitInst(this);
        return 0;
    }

    this->m_flags &= ~KIT_INST_FLAG_STOPPING;

    return 1;
}

// OFFSET: 0x005923B0/0x1401FE5F0
CAnimKitInst* CAnimKitManager::PlayAndForget(uint32_t animKitID, uint32_t anim, AnimKitExtraPlayParams* params) {
    return this->InitKitInst(animKitID, 0, anim, params, NULL);
}

// OFFSET: 0x005923D0/0x1401FE620
CAnimKitInst* CAnimKitManager::PlayAndMaintain(uint32_t animKitID, uint32_t anim, AnimKitExtraPlayParams* params) {
    return this->InitKitInst(animKitID, 1, anim, params, NULL);
}

// OFFSET: 0x005923F0/0x1401FE750
int32_t CAnimKitManager::SetSegmentInstState(AnimKitSegmentInst* segmentInst, uint32_t state, int32_t allowStop) {
    uint32_t oldState = segmentInst->instFlags & SEGMENT_INST_FLAG_STATE_MASK;

    if (state == oldState) {
        return 1;
    }

    segmentInst->instFlags = state | (segmentInst->instFlags & ~SEGMENT_INST_FLAG_STATE_MASK);

    if (state == ANIM_KIT_SEGMENT_STATE_PLAYING) {
        if (!this->OnPlaySegmentInst(segmentInst, allowStop)) {
            return 0;
        }
    } else if (state == ANIM_KIT_SEGMENT_STATE_STOPPED && !this->OnStopSegmentInst(segmentInst, allowStop)) {
        return 0;
    }

    CAnimKitInst* kitInst = segmentInst->kitInst;

    if ((oldState & ANIM_KIT_SEGMENT_STATE_STOPPED) != (state & ANIM_KIT_SEGMENT_STATE_STOPPED)) {
        if (oldState & ANIM_KIT_SEGMENT_STATE_STOPPED) {
            kitInst->m_stoppedSegmentCount--;
        } else {
            kitInst->m_stoppedSegmentCount++;
        }
    }

    if (kitInst->m_segmentCount == kitInst->m_stoppedSegmentCount) {
        if (allowStop) {
            kitInst->Stop(1, NULL);
        }

        return 0;
    }

    return 1;
}

// OFFSET: 0x005924C0/0x1401FE830
int32_t CAnimKitInst::Restart() {
    if (this->m_flags & KIT_INST_FLAG_PENDING_DESTROY) {
        return 0;
    }

    if (this->m_flags & KIT_INST_FLAG_DEFERRED) {
        AnimKitDeferredCommand command;
        command.type = ANIM_KIT_DEFERRED_RESTART;
        this->m_deferredCommands.New(command);
        return 1;
    }

    this->m_flags &= ~KIT_INST_FLAG_STOPPED;
    int32_t duration = this->m_def->rec->m_oneShotDuration;

    if (duration) {
        this->m_manager->AddKitEvent(this, this->m_manager->m_model->m_scene->m_time, duration);
    } else {
        this->m_manager->m_kitInsts.LinkToTail(this);

        if (!(this->m_flags & KIT_INST_FLAG_MAINTAIN)) {
            this->m_flags |= KIT_INST_FLAG_DURATION_ELAPSED;
        }
    }

    uint32_t count = this->m_segmentInsts.Count();
    TSStackArray<AnimKitSegmentInst*> play(static_cast<AnimKitSegmentInst**>(alloca(sizeof(AnimKitSegmentInst*) * count)), count, 0);
    TSStackArray<AnimKitSegmentInst*> stop(static_cast<AnimKitSegmentInst**>(alloca(sizeof(AnimKitSegmentInst*) * count)), count, 0);

    for (uint32_t i = 0; i < count; i++) {
        AnimKitSegmentInst* segmentInst = this->m_segmentInsts[i];

        if (!segmentInst) {
            continue;
        }

        segmentInst->link.Unlink();
        uint16_t instFlags = segmentInst->instFlags;
        uint32_t oldState = instFlags & SEGMENT_INST_FLAG_STATE_MASK;
        segmentInst->startEventTime = 0xFFFFFFFF;
        segmentInst->endEventTime = 0xFFFFFFFF;

        if (oldState) {
            segmentInst->instFlags = instFlags & ~SEGMENT_INST_FLAG_STATE_MASK;
            CAnimKitInst* kitInst = segmentInst->kitInst;

            if (oldState & ANIM_KIT_SEGMENT_STATE_STOPPED) {
                kitInst->m_stoppedSegmentCount--;
            }

            if (kitInst->m_segmentCount == kitInst->m_stoppedSegmentCount) {
                kitInst->Stop(1, NULL);
            }
        }

        this->m_manager->PostInitSegmentInst(segmentInst, play, stop);
        uint32_t boneSetCount = segmentInst->boneSetInsts.Count();

        for (uint32_t j = 0; j < boneSetCount; j++) {
            AnimKitSegmentBoneSetInst* segmentBoneSetInst = segmentInst->boneSetInsts[j];

            if (segmentBoneSetInst) {
                segmentBoneSetInst->flags |= SEGMENT_BONE_SET_INST_FLAG_DIRTY;
            }
        }
    }

    this->m_manager->UpdateNextSegmentEventTime();
    this->m_manager->ScheduleUpdate(ANIM_KIT_MANAGER_FLAG_EXPRESS);

    for (uint32_t i = 0; i < play.m_count; i++) {
        if (!this->m_manager->SetSegmentInstState(play.m_data[i], ANIM_KIT_SEGMENT_STATE_PLAYING, 0)) {
            return this->Stop(1, NULL);
        }
    }

    for (uint32_t i = 0; i < stop.m_count; i++) {
        if (!this->m_manager->SetSegmentInstState(stop.m_data[i], ANIM_KIT_SEGMENT_STATE_STOPPED, 0)) {
            return this->Stop(1, NULL);
        }
    }

    return 1;
}

// OFFSET: 0x005927F0/0x1401FEC90
int32_t CAnimKitManager::AddSegmentEvent(AnimKitSegmentInst* segmentInst, uint32_t time, int32_t delay, ANIM_KIT_SEGMENT_EVENT_TYPE type, int32_t allowStop) {
    if (segmentInst->kitInst->m_flags & (KIT_INST_FLAG_STOPPED | KIT_INST_FLAG_PENDING_DESTROY)) {
        return 0;
    }

    uint16_t instFlags = segmentInst->instFlags;

    if (((instFlags & ANIM_KIT_SEGMENT_STATE_STOPPED) && type == ANIM_KIT_SEGMENT_EVENT_END) || ((instFlags & ANIM_KIT_SEGMENT_STATE_PLAYING) && type == ANIM_KIT_SEGMENT_EVENT_START)) {
        return 1;
    }

    if (delay <= 0) {
        if (type == ANIM_KIT_SEGMENT_EVENT_START) {
            return this->SetSegmentInstState(segmentInst, ANIM_KIT_SEGMENT_STATE_PLAYING, allowStop);
        }

        return this->SetSegmentInstState(segmentInst, ANIM_KIT_SEGMENT_STATE_STOPPED, allowStop);
    }

    if (delay <= CAnimKitManager::s_updateTolerance) {
        delay = CAnimKitManager::s_updateTolerance + 1;
    }

    uint32_t eventTime = time + delay;

    if (eventTime == 0xFFFFFFFF) {
        eventTime = 0;
    }

    if (type != ANIM_KIT_SEGMENT_EVENT_START) {
        segmentInst->endEventTime = eventTime;

        if (type == ANIM_KIT_SEGMENT_EVENT_END) {
            uint32_t startTime = segmentInst->startEventTime;

            if (startTime != 0xFFFFFFFF && static_cast<int32_t>(eventTime - startTime) > 0) {
                return 1;
            }
        }
    } else {
        uint32_t endTime = segmentInst->endEventTime;
        segmentInst->startEventTime = eventTime;

        if (endTime != 0xFFFFFFFF && static_cast<int32_t>(eventTime - endTime) > 0) {
            return 1;
        }
    }

    segmentInst->link.Unlink();

    for (AnimKitSegmentInst* node = this->m_segmentEvents.Head(); !this->m_segmentEvents.IsEnd(node); node = this->m_segmentEvents.RawNext(node)) {
        uint32_t nodeTime = node->startEventTime;

        if (nodeTime == 0xFFFFFFFF) {
            nodeTime = node->endEventTime;
        } else {
            uint32_t nodeEndTime = node->endEventTime;

            if (nodeEndTime != 0xFFFFFFFF && static_cast<int32_t>(nodeTime - nodeEndTime) >= 0) {
                nodeTime = nodeEndTime;
            }
        }

        if (static_cast<int32_t>(eventTime - nodeTime) < 0) {
            this->m_segmentEvents.LinkNode(segmentInst, 2, node);
            goto linked;
        }
    }

    this->m_segmentEvents.LinkToTail(segmentInst);

linked:
    if ((segmentInst->instFlags & ANIM_KIT_SEGMENT_STATE_STOPPED) && !this->SetSegmentInstState(segmentInst, ANIM_KIT_SEGMENT_STATE_NONE, allowStop)) {
        return 0;
    }

    this->UpdateNextSegmentEventTime();

    return 1;
}

// OFFSET: 0x00592A90/0x1401FF050
void CAnimKitManager::HandleNextSegmentEvent() {
    AnimKitSegmentInst* segmentInst = this->m_segmentEvents.Head();
    segmentInst->link.Unlink();
    uint32_t endTime = segmentInst->endEventTime;

    if (endTime == 0xFFFFFFFF) {
        segmentInst->startEventTime = 0xFFFFFFFF;
        this->SetSegmentInstState(segmentInst, ANIM_KIT_SEGMENT_STATE_PLAYING, 1);
    } else {
        uint32_t startTime = segmentInst->startEventTime;

        if (startTime == 0xFFFFFFFF) {
            segmentInst->endEventTime = 0xFFFFFFFF;
            this->SetSegmentInstState(segmentInst, ANIM_KIT_SEGMENT_STATE_STOPPED, 1);
        } else if (static_cast<int32_t>(endTime - startTime) < 0) {
            segmentInst->startEventTime = 0xFFFFFFFF;
            segmentInst->endEventTime = 0xFFFFFFFF;
            this->SetSegmentInstState(segmentInst, ANIM_KIT_SEGMENT_STATE_STOPPED, 1);
        } else {
            uint32_t now = this->m_model->m_scene->m_time;
            segmentInst->startEventTime = 0xFFFFFFFF;
            segmentInst->endEventTime = 0xFFFFFFFF;

            if (this->SetSegmentInstState(segmentInst, ANIM_KIT_SEGMENT_STATE_PLAYING, 1) && !segmentInst->link.IsLinked() && (segmentInst->instFlags & ANIM_KIT_SEGMENT_STATE_PLAYING)) {
                this->AddSegmentEvent(segmentInst, now, endTime - now, ANIM_KIT_SEGMENT_EVENT_END, 1);
            }
        }
    }

    this->UpdateNextSegmentEventTime();
}

// OFFSET: 0x00592EF0/0x1401FF670
void CAnimKitManager::ReactToSegmentInstSuppression(TSCGrowableArray<AnimKitSegmentInst*, 8>& suppressed, TSCGrowableArray<AnimKitDesiredBoneSetInst, 10>& desired, TSCGrowableArray<CAnimKitInst*, 4>& stopKits, uint32_t maxBoneSetID) {
    uint32_t count = suppressed.Count();

    for (uint32_t i = 0; i < count; i++) {
        AnimKitSegmentInst* segmentInst = suppressed[i];

        if (!segmentInst) {
            continue;
        }

        uint8_t suppressedCount = segmentInst->suppressedCount;

        if (!suppressedCount) {
            continue;
        }

        int32_t allSuppressed = suppressedCount == segmentInst->boneSetInstCount;
        uint32_t segmentFlags = segmentInst->segmentFlags;
        segmentInst->suppressedCount = 0;

        if (segmentFlags & 0x2000) {
            segmentInst->instFlags |= SEGMENT_INST_FLAG_RESTART_PENDING;
        }

        if (!((segmentFlags & 0xA00) || ((segmentFlags & 0x1400) && allSuppressed))) {
            continue;
        }

        for (uint32_t j = 0; j < maxBoneSetID; j++) {
            for (uint32_t layer = 0; layer < 2; layer++) {
                AnimKitBoneSetInstStack& stack = desired[j].layers[layer];

                for (uint32_t k = 0; k < stack.stack.Count(); k++) {
                    if (stack.stack[k] && stack.stack[k]->segmentInst == segmentInst) {
                        stack.stack[k] = NULL;
                    }
                }
            }
        }

        if ((segmentInst->instFlags & SEGMENT_INST_FLAG_STATE_MASK) != ANIM_KIT_SEGMENT_STATE_STOPPED && !this->SetSegmentInstState(segmentInst, ANIM_KIT_SEGMENT_STATE_STOPPED, 0)) {
            goto stopKit;
        }

        if (!((segmentFlags & 0x800) || ((segmentFlags & 0x1000) && allSuppressed))) {
            continue;
        }

    stopKit:
        CAnimKitInst* kitInst = segmentInst->kitInst;

        for (uint32_t j = 0; j < maxBoneSetID; j++) {
            for (uint32_t layer = 0; layer < 2; layer++) {
                AnimKitBoneSetInstStack& stack = desired[j].layers[layer];

                for (uint32_t k = 0; k < stack.stack.Count(); k++) {
                    if (stack.stack[k] && stack.stack[k]->segmentInst->kitInst == kitInst) {
                        stack.stack[k] = NULL;
                    }
                }
            }
        }

        for (uint32_t j = i + 1; j < count; j++) {
            if (suppressed[j] && suppressed[j]->kitInst == kitInst) {
                suppressed[j] = NULL;
            }
        }

        stopKits.New(kitInst);
    }
}

// OFFSET: 0x00593280/0x1401FFB90
int32_t CAnimKitManager::UpdateSegmentInstEndEvent(AnimKitSegmentInst* segmentInst, float speed, int32_t allowStop) {
    int32_t endCondition = segmentInst->rec->m_endCondition;
    uint32_t elapsed = segmentInst->kitInst->m_flags & KIT_INST_FLAG_DURATION_ELAPSED;

    if (endCondition) {
        if (endCondition != 1 || !elapsed) {
            return 1;
        }
    } else if (!elapsed && fabs(speed) <= 0.0000099999997f) {
        return 1;
    }

    segmentInst->link.Unlink();
    int32_t delay = this->ComputeSegmentSceneTimeDurationUntilAnimEndTime(segmentInst, speed);

    if (!endCondition) {
        delay += segmentInst->rec->m_endConditionDelay;
    }

    return this->AddSegmentEvent(segmentInst, this->m_model->m_scene->m_time, delay, ANIM_KIT_SEGMENT_EVENT_END, allowStop);
}

// OFFSET: 0x00593500/0x1401FFEE0
void CAnimKitInst::Release() {
    if (this->m_flags & KIT_INST_FLAG_PENDING_DESTROY) {
        return;
    }

    this->m_flags &= ~KIT_INST_FLAG_MAINTAIN;

    if (this->m_flags & KIT_INST_FLAG_STOPPING) {
        return;
    }

    if (!(this->m_flags & (KIT_INST_FLAG_STOPPED | KIT_INST_FLAG_PENDING_DESTROY)) && this->m_def->rec->m_oneShotDuration <= 0) {
        this->m_flags |= KIT_INST_FLAG_DURATION_ELAPSED;
        uint32_t now = this->m_manager->m_model->m_scene->m_time;
        uint32_t count = this->m_segmentInsts.Count();

        for (uint32_t i = 0; i < count; i++) {
            AnimKitSegmentInst* segmentInst = this->m_segmentInsts[i];

            if (!segmentInst) {
                continue;
            }

            int32_t endCondition = segmentInst->rec->m_endCondition;

            if (!(segmentInst->instFlags & ANIM_KIT_SEGMENT_STATE_PLAYING) || endCondition > 1) {
                continue;
            }

            float speed = this->m_manager->ComputeSegmentAnimSpeed(segmentInst);
            int32_t delay = this->m_manager->ComputeSegmentSceneTimeDurationUntilAnimEndTime(segmentInst, speed);

            if (!endCondition) {
                delay += segmentInst->rec->m_endConditionDelay;
            }

            if (!this->m_manager->AddSegmentEvent(segmentInst, now, delay, ANIM_KIT_SEGMENT_EVENT_END, 1)) {
                return;
            }
        }
    }

    if (this->m_flags & (KIT_INST_FLAG_STOPPED | KIT_INST_FLAG_PENDING_DESTROY)) {
        this->m_manager->UninitKitInst(this);
    }
}

// OFFSET: 0x00593800/0x1402002B0
int32_t CAnimKitInst::OnSegmentInstTimingChange(AnimKitSegmentInst* segmentInst, float speed) {
    if (this->m_flags & KIT_INST_FLAG_PENDING_DESTROY) {
        return 0;
    }

    uint32_t count = segmentInst->boneSetInsts.Count();

    for (uint32_t i = 0; i < count; i++) {
        AnimKitSegmentBoneSetInst* segmentBoneSetInst = segmentInst->boneSetInsts[i];

        if (segmentBoneSetInst) {
            segmentBoneSetInst->flags |= SEGMENT_BONE_SET_INST_FLAG_DIRTY;
        }
    }

    this->m_manager->ScheduleUpdate(ANIM_KIT_MANAGER_FLAG_EXPRESS);
    int32_t endCondition = segmentInst->rec->m_endCondition;
    uint32_t elapsed = segmentInst->kitInst->m_flags & KIT_INST_FLAG_DURATION_ELAPSED;

    if (endCondition) {
        if (endCondition != 1 || !elapsed) {
            return 1;
        }
    } else if (!elapsed && fabs(speed) <= 0.0000099999997f) {
        return 1;
    }

    int32_t delay = this->m_manager->ComputeSegmentSceneTimeDurationUntilAnimEndTime(segmentInst, speed);

    if (!endCondition) {
        delay += segmentInst->rec->m_endConditionDelay;
    }

    if (!this->m_manager->AddSegmentEvent(segmentInst, this->m_manager->m_model->m_scene->m_time, delay, ANIM_KIT_SEGMENT_EVENT_END, 0)) {
        return this->Stop(1, NULL);
    }

    return 1;
}

// OFFSET: 0x00593AF0/0x140200690
int32_t CAnimKitManager::OnModelSequenceStartCallback(uint32_t boneId) {
    uint32_t boneSetID;

    if (boneId >= 35) {
        boneSetID = 0;
    } else {
        boneSetID = s_boneDataBoneSetIDs[boneId];

        if (boneSetID == 0xFFFFFFFF) {
            return 0;
        }
    }

    AnimKitSegmentBoneSetInst* expressing = this->m_boneSetInsts[boneSetID].expressing[0];

    if (!expressing) {
        return 0;
    }

    AnimKitSegmentInst* segmentInst = expressing->segmentInst;

    if (!segmentInst || (expressing->flags & SEGMENT_BONE_SET_INST_FLAG_SIMPLE)) {
        return 0;
    }

    if (!(this->m_flags & (ANIM_KIT_MANAGER_FLAG_EXPRESSIONS_BLOCKED | ANIM_KIT_MANAGER_FLAG_FROZEN))) {
        float speed = this->ComputeSegmentAnimSpeed(segmentInst);

        if (fabs(speed) > 0.0000099999997f) {
            uint32_t modelTime = this->m_model->GetBoneSequenceTime(boneId);
            int32_t drift = static_cast<int32_t>(static_cast<float>(static_cast<int32_t>(this->ComputeSegmentAnimTime(segmentInst, speed) - modelTime)) / speed);

            if (drift > 0) {
                segmentInst->startTime += drift;
                this->UpdateSegmentInstEndEvent(segmentInst, speed, 1);
            }
        }
    }

    return 1;
}

// OFFSET: 0x00593BC0/0x1402007A0
void CAnimKitManager::Unfreeze() {
    if (!(this->m_flags & ANIM_KIT_MANAGER_FLAG_FREEZE_MASK)) {
        return;
    }

    this->m_flags &= ~ANIM_KIT_MANAGER_FLAG_FREEZE_MASK;
    this->m_model->m_lastAnimTime = 0;
    uint32_t now = this->m_model->m_scene->m_time;
    CAnimKitInst* kitInst = this->m_kitInsts.Head();

    while (!this->m_kitInsts.IsEnd(kitInst)) {
        CAnimKitInst* next = this->m_kitInsts.RawNext(kitInst);
        uint32_t count = kitInst->m_segmentInsts.Count();

        for (uint32_t i = 0; i < count; i++) {
            AnimKitSegmentInst* segmentInst = kitInst->m_segmentInsts[i];

            if (!segmentInst) {
                continue;
            }

            float speed = (segmentInst->rec->m_segmentFlags & 0x8) ? 0.0f : this->ComputeSegmentAnimSpeed(segmentInst);
            uint32_t freezeTime = this->m_freezeTime;

            if (freezeTime == 0xFFFFFFFF) {
                if (fabs(speed) > 0.0000099999997f) {
                    segmentInst->startTime += now - this->m_frozenTime;
                }
            } else {
                if (fabs(speed) > 0.0000099999997f) {
                    freezeTime = now - static_cast<int64_t>(static_cast<double>(static_cast<int32_t>(freezeTime - segmentInst->rec->m_animStartTime)) / speed);
                }

                segmentInst->startTime = freezeTime;
            }

            this->UpdateSegmentInstEndEvent(segmentInst, speed, 1);
        }

        kitInst = next;
    }

    this->m_flags |= ANIM_KIT_MANAGER_FLAG_FORCE_EXPRESS;
    this->ScheduleUpdate(ANIM_KIT_MANAGER_FLAG_EXPRESS);
}

// OFFSET: 0x00593D40/0x1402009F0
void CAnimKitManager::RestartSegmentInstDueToExpression(AnimKitSegmentInst* segmentInst, TSCGrowableArray<CAnimKitInst*, 4>& stopKits) {
    segmentInst->instFlags &= ~SEGMENT_INST_FLAG_RESTART_MASK;
    float speed = (segmentInst->rec->m_segmentFlags & 0x8) ? 0.0f : this->ComputeSegmentAnimSpeed(segmentInst);

    if (fabs(speed) <= 0.0000099999997f) {
        segmentInst->startTime = segmentInst->rec->m_animStartTime;
    } else {
        segmentInst->startTime = this->GetCurTime();
    }

    if (this->UpdateSegmentInstEndEvent(segmentInst, speed, 0)) {
        return;
    }

    CAnimKitInst* kitInst = segmentInst->kitInst;
    uint32_t count = stopKits.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (stopKits[i] == kitInst) {
            return;
        }
    }

    stopKits.New(kitInst);
}

// OFFSET: 0x00593E50/0x140200B20
int32_t CAnimKitManager::ReinitSegmentInstAnimAndEvents(AnimKitSegmentInst* segmentInst) {
    uint32_t anim;
    int32_t result = this->ProcessSegmentRec(segmentInst->rec, segmentInst->kitInst->m_boneSetID, segmentInst->kitInst->m_anim, &anim, &segmentInst->instFlags);

    if (anim == 0xFFFFFFFF) {
        segmentInst->kitInst->Stop(1, NULL);
        return 0;
    }

    uint16_t requestedAnim = segmentInst->requestedAnim;

    if (requestedAnim >= CAnimKitManager::s_animDataCount) {
        segmentInst->curAnim = anim;
    } else {
        int32_t ignoreTier = (segmentInst->rec->m_segmentFlags & 0x1) || (segmentInst->userFlags & 0x8);
        uint16_t filteredAnim = this->FilterSegmentAnim(requestedAnim, ignoreTier, NULL, &segmentInst->instFlags);
        segmentInst->curAnim = filteredAnim;

        if (filteredAnim >= CAnimKitManager::s_animDataCount) {
            segmentInst->curAnim = requestedAnim;
        }
    }

    uint32_t now = this->m_model->m_scene->m_time;

    if (result == 2) {
        segmentInst->kitInst->Stop(1, NULL);
        return 0;
    }

    if (result == 1) {
        uint32_t state = segmentInst->instFlags & SEGMENT_INST_FLAG_STATE_MASK;

        if (!state) {
            if (!CAnimKitManager::HandleKitReactionToSegmentStart(segmentInst->kitInst, segmentInst->index, now, 1)) {
                return 0;
            }

            state = ANIM_KIT_SEGMENT_STATE_PLAYING;
        }

        if (state == ANIM_KIT_SEGMENT_STATE_PLAYING && !CAnimKitManager::HandleKitReactionToSegmentEnd(segmentInst->kitInst, segmentInst->index, now, 1)) {
            return 0;
        }

        segmentInst->kitInst->m_segmentInsts[segmentInst->index] = NULL;
        this->UninitSegmentInst(segmentInst);

        return 1;
    }

    if (!(segmentInst->instFlags & ANIM_KIT_SEGMENT_STATE_PLAYING)) {
        return 1;
    }

    float speed = this->ComputeSegmentAnimSpeed(segmentInst);
    int32_t time = this->ComputeSegmentAnimTime(segmentInst, speed);
    this->WrapSegmentTiming(segmentInst, speed, time);

    do {
        this->PickSegmentInstVariationAndRepetitions(segmentInst);
    } while (this->WrapSegmentTiming(segmentInst, speed, time));

    return this->UpdateSegmentInstEndEvent(segmentInst, speed, 1) != 0;
}

// OFFSET: 0x00594000/0x140200D40
void CAnimKitManager::DefaultAnimStartHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, uint32_t variation, uint32_t time, uint64_t param, int32_t primary) {
    CAnimKitManager* manager = reinterpret_cast<CAnimKitManager*>(static_cast<uintptr_t>(param));

    if (manager) {
        manager->OnModelSequenceStartCallback(boneId);
    }
}

// OFFSET: 0x00594020/0x140200D60
int32_t CAnimKitInst::SetAnim(uint32_t segmentIndex, uint32_t anim, uint8_t variation, uint8_t repetitions, uint8_t blendTimeMode, uint16_t blendTime) {
    if (this->m_flags & KIT_INST_FLAG_PENDING_DESTROY) {
        return 0;
    }

    if (this->m_flags & KIT_INST_FLAG_DEFERRED) {
        AnimKitDeferredCommand command;
        command.setAnim.segmentIndex = segmentIndex;
        command.setAnim.variation = variation;
        command.setAnim.anim = anim;
        command.setAnim.blendTimeMode = blendTimeMode;
        command.setAnim.repetitions = repetitions;
        command.type = ANIM_KIT_DEFERRED_SET_ANIM;
        command.setAnim.blendTime = blendTime;
        this->m_deferredCommands.New(command);
        return 1;
    }

    if (segmentIndex >= this->m_segmentInsts.Count()) {
        return 1;
    }

    AnimKitSegmentInst* segmentInst = this->m_segmentInsts[segmentIndex];

    if (!segmentInst) {
        return 1;
    }

    int8_t oldVariation = segmentInst->forcedVariation;

    if (oldVariation == static_cast<int8_t>(variation) && segmentInst->forcedRepetitions == repetitions) {
        uint16_t requestedAnim = segmentInst->requestedAnim;

        if (requestedAnim == anim || (anim == 0xFFFFFFFF && requestedAnim >= CAnimKitManager::s_animDataCount)) {
            return 1;
        }
    }

    uint16_t oldAnim = segmentInst->curAnim;

    if (static_cast<uint16_t>(anim) >= CAnimKitManager::s_animDataCount) {
        segmentInst->requestedAnim = CAnimKitManager::s_animDataCount;
        uint16_t filteredAnim = this->m_manager->FilterSegmentAnim(segmentInst->rec->m_animID, segmentInst->rec->m_segmentFlags & 0x1, NULL, &segmentInst->instFlags);
        segmentInst->curAnim = filteredAnim;

        if (filteredAnim >= CAnimKitManager::s_animDataCount) {
            segmentInst->curAnim = segmentInst->rec->m_animID;
        }
    } else {
        segmentInst->requestedAnim = anim;
        uint16_t filteredAnim = this->m_manager->FilterSegmentAnim(anim, segmentInst->rec->m_segmentFlags & 0x1, NULL, &segmentInst->instFlags);
        segmentInst->curAnim = filteredAnim;

        if (filteredAnim >= CAnimKitManager::s_animDataCount) {
            segmentInst->curAnim = anim;
        }
    }

    segmentInst->blendTimeMode = blendTimeMode;
    segmentInst->forcedRepetitions = repetitions;
    segmentInst->forcedVariation = variation;
    segmentInst->blendTime = blendTime;
    int32_t changed = oldAnim != segmentInst->curAnim || (static_cast<int8_t>(variation) != -1 && oldVariation != static_cast<int8_t>(variation));

    if (!(segmentInst->instFlags & ANIM_KIT_SEGMENT_STATE_PLAYING) || ((segmentInst->segmentFlags & 0x10000) && !changed)) {
        return 1;
    }

    this->m_manager->PickSegmentInstVariationAndRepetitions(segmentInst);
    float speed = this->m_manager->ComputeSegmentAnimSpeed(segmentInst);

    if (fabs(speed) <= 0.0000099999997f) {
        segmentInst->startTime = segmentInst->rec->m_animStartTime;
    } else {
        segmentInst->startTime = this->m_manager->GetCurTime();
    }

    return this->OnSegmentInstTimingChange(segmentInst, speed);
}

// OFFSET: 0x00594220/0x140201090
int32_t CAnimKitInst::SetSpeed(uint32_t segmentIndex, uint32_t speedType, float speed) {
    if (this->m_flags & KIT_INST_FLAG_PENDING_DESTROY) {
        return 0;
    }

    if (this->m_flags & KIT_INST_FLAG_DEFERRED) {
        AnimKitDeferredCommand command;
        command.setSpeed.speedType = speedType;
        command.type = ANIM_KIT_DEFERRED_SET_SPEED;
        command.setSpeed.segmentIndex = segmentIndex;
        command.setSpeed.speed = speed;
        this->m_deferredCommands.New(command);
        return 1;
    }

    if (segmentIndex >= this->m_segmentInsts.Count()) {
        return 1;
    }

    AnimKitSegmentInst* segmentInst = this->m_segmentInsts[segmentIndex];

    if (!segmentInst || (segmentInst->rec->m_segmentFlags & 0x80)) {
        return 1;
    }

    if (!(segmentInst->instFlags & ANIM_KIT_SEGMENT_STATE_PLAYING)) {
        segmentInst->speedType = speedType;
        segmentInst->speed = speed;
        return 1;
    }

    float oldSpeed = this->m_manager->ComputeSegmentAnimSpeed(segmentInst);
    int32_t time = this->m_manager->ComputeSegmentAnimTime(segmentInst, oldSpeed);
    segmentInst->speedType = speedType;
    segmentInst->speed = speed;
    float newSpeed = this->m_manager->ComputeSegmentAnimSpeed(segmentInst);

    if (fequalz(oldSpeed, newSpeed, 0.0000099999997f)) {
        return 1;
    }

    if (fabs(newSpeed) <= 0.0000099999997f) {
        segmentInst->startTime = time;
        return this->OnSegmentInstTimingChange(segmentInst, newSpeed);
    }

    uint32_t now = this->m_manager->GetCurTime();

    if (fabs(oldSpeed) <= 0.0000099999997f) {
        segmentInst->startTime = now - static_cast<int64_t>(static_cast<double>(static_cast<int32_t>(segmentInst->startTime - segmentInst->rec->m_animStartTime)) * (1.0 / newSpeed));
    } else {
        segmentInst->startTime = now - static_cast<int64_t>(static_cast<double>(static_cast<int32_t>(now - segmentInst->startTime)) * (oldSpeed / newSpeed));
    }

    return this->OnSegmentInstTimingChange(segmentInst, newSpeed);
}

// OFFSET: 0x00594420/0x140201310
int32_t CAnimKitInst::SetUserFlags(uint32_t segmentIndex, uint8_t userFlags) {
    if (this->m_flags & KIT_INST_FLAG_PENDING_DESTROY) {
        return 0;
    }

    if (this->m_flags & KIT_INST_FLAG_DEFERRED) {
        AnimKitDeferredCommand command;
        command.setUserFlags.segmentIndex = segmentIndex;
        command.type = ANIM_KIT_DEFERRED_SET_USER_FLAGS;
        command.setUserFlags.userFlags = userFlags;
        this->m_deferredCommands.New(command);
        return 1;
    }

    if (segmentIndex >= this->m_segmentInsts.Count()) {
        return 1;
    }

    AnimKitSegmentInst* segmentInst = this->m_segmentInsts[segmentIndex];

    if (!segmentInst) {
        return 1;
    }

    uint8_t oldUserFlags = segmentInst->userFlags;

    if (oldUserFlags == userFlags) {
        return 1;
    }

    segmentInst->userFlags = userFlags;

    if (((userFlags ^ oldUserFlags) >> 3) & 1) {
        if (!this->m_manager->ReinitSegmentInstAnimAndEvents(segmentInst)) {
            return 0;
        }
    }

    int32_t blend = !(segmentInst->segmentFlags & 0x2) && !(segmentInst->userFlags & 0x2);
    uint32_t count = segmentInst->boneSetInsts.Count();

    for (uint32_t i = 0; i < count; i++) {
        AnimKitSegmentBoneSetInst* segmentBoneSetInst = segmentInst->boneSetInsts[i];

        if (segmentBoneSetInst) {
            if (blend) {
                segmentBoneSetInst->flags &= ~SEGMENT_BONE_SET_INST_FLAG_NO_BLEND;
            } else {
                segmentBoneSetInst->flags |= SEGMENT_BONE_SET_INST_FLAG_NO_BLEND;
            }
        }
    }

    return 1;
}

// OFFSET: 0x00594530/0x1402014A0
void CAnimKitInst::OnModelLoaded() {
    if ((this->m_flags & KIT_INST_FLAG_PENDING_DESTROY) || !(this->m_flags & KIT_INST_FLAG_DEFERRED)) {
        return;
    }

    this->m_flags &= ~KIT_INST_FLAG_DEFERRED;

    if (!this->m_manager->PostInitKitInst(this)) {
        return;
    }

    uint32_t count = this->m_deferredCommands.Count();

    for (uint32_t i = 0; i < count; i++) {
        AnimKitDeferredCommand& command = this->m_deferredCommands[i];

        switch (command.type) {
            case ANIM_KIT_DEFERRED_SET_ANIM:
                if (!this->SetAnim(command.setAnim.segmentIndex, command.setAnim.anim, command.setAnim.variation, command.setAnim.repetitions, command.setAnim.blendTimeMode, command.setAnim.blendTime)) {
                    return;
                }

                break;

            case ANIM_KIT_DEFERRED_SET_SPEED:
                if (!this->SetSpeed(command.setSpeed.segmentIndex, command.setSpeed.speedType, command.setSpeed.speed)) {
                    return;
                }

                break;

            case ANIM_KIT_DEFERRED_SET_USER_FLAGS:
                if (!this->SetUserFlags(command.setUserFlags.segmentIndex, command.setUserFlags.userFlags)) {
                    return;
                }

                break;

            case ANIM_KIT_DEFERRED_RESTART:
                if (!this->Restart()) {
                    return;
                }

                break;

            case ANIM_KIT_DEFERRED_STOP:
                if (!this->Stop(command.stop.playStopKit, NULL)) {
                    return;
                }

                break;

            case ANIM_KIT_DEFERRED_SET_STOP_CALLBACK:
                this->SetStopCallback(command.setStopCallback.callback, command.setStopCallback.param);
                break;
        }

        if (this->m_flags & KIT_INST_FLAG_PENDING_DESTROY) {
            return;
        }
    }

    this->m_deferredCommands.SetCount(0);
}

// OFFSET: 0x005946A0/0x140201640
void CAnimKitManager::OnModelLoaded() {
    CAnimKitInst* kitInst = this->m_kitInsts.Head();

    while (!this->m_kitInsts.IsEnd(kitInst)) {
        CAnimKitInst* next = this->m_kitInsts.RawNext(kitInst);
        kitInst->OnModelLoaded();
        kitInst = next;
    }

    this->m_flags |= ANIM_KIT_MANAGER_FLAG_FORCE_EXPRESS | ANIM_KIT_MANAGER_FLAG_SNAP;
    this->ScheduleUpdate(ANIM_KIT_MANAGER_FLAG_EXPRESS);
}

// OFFSET: 0x00594750/0x140201720
void CAnimKitManager::ExpressSegmentBoneSetInst(AnimKitSegmentBoneSetInst* segmentBoneSetInst, AnimKitSegmentBoneSetInst*& expressing, uint8_t& expressingFlags, TSCGrowableArray<AnimKitDesiredBoneSetInst, 10>& desired, TSCGrowableArray<CAnimKitInst*, 4>& stopKits, uint32_t boneSetID, int32_t layer, uint16_t managerFlags) {
    AnimKitSegmentBoneSetInst* previous = expressing;
    uint8_t previousFlags = expressingFlags;
    expressingFlags &= 0xF8;
    expressing = segmentBoneSetInst;
    const AnimKitBoneSetRec* boneSetRec = g_animKitBoneSetDB.GetRecord(boneSetID);
    uint32_t boneDataID = boneSetRec->m_boneDataID;
    int32_t primary = layer == 0;
    int32_t express = segmentBoneSetInst && !(segmentBoneSetInst->flags & SEGMENT_BONE_SET_INST_FLAG_PENDING_DESTROY);
    int32_t unexpress = !express;
    AnimKitSegmentInst* segmentInst = NULL;

    if (express) {
        uint8_t flags = (segmentBoneSetInst->flags & 0xF5) | SEGMENT_BONE_SET_INST_FLAG_EXPRESSING;
        segmentBoneSetInst->flags = flags;
        segmentInst = segmentBoneSetInst->segmentInst;

        if (!segmentInst || segmentInst->curAnim >= CAnimKitManager::s_animDataCount) {
            goto unexpressCheck;
        }

        const AnimationDataRec* animRec = g_animationDataDB.GetRecord(segmentInst->curAnim);
        const AnimKitSegmentRec* rec = segmentInst->rec;

        if (!animRec || animRec->m_id || (segmentInst->instFlags & 0xB0) || static_cast<uint8_t>(rec->m_segmentFlags) || !fequal(rec->m_speed, 1.0f) || rec->m_animStartTime) {
            segmentBoneSetInst->flags = flags & ~SEGMENT_BONE_SET_INST_FLAG_SIMPLE;
        } else {
            segmentBoneSetInst->flags = flags | SEGMENT_BONE_SET_INST_FLAG_SIMPLE;
            uint8_t checkFlags;

            if (previous) {
                if (!(previous->flags & SEGMENT_BONE_SET_INST_FLAG_SIMPLE)) {
                    goto parentCheck;
                }

                checkFlags = previousFlags;
            } else {
                checkFlags = previousFlags;

                if (!((!(previousFlags & 0x1) && boneDataID >= 35) || (previousFlags & 0x2))) {
                    goto parentCheck;
                }
            }

            express = 0;

            if ((checkFlags & 0x1) && !(checkFlags & 0x2)) {
                unexpress = 1;
            }
        }

    parentCheck:
        uint32_t parentID = boneSetRec->m_parentAnimKitBoneSetID;

        while (parentID < CAnimKitManager::s_animKitBoneSetCount) {
            AnimKitSegmentBoneSetInst* top = CAnimKitManager::GetSegmentBoneSetInstStackTop(desired.Ptr()[parentID].layers[layer]);

            if (top) {
                if (top->segmentInst == segmentInst) {
                    segmentBoneSetInst->flags |= SEGMENT_BONE_SET_INST_FLAG_DIRTY;
                    goto unexpressBones;
                }

                break;
            }

            const AnimKitBoneSetRec* parentRec = g_animKitBoneSetDB.GetRecord(parentID);
            parentID = parentRec ? parentRec->m_parentAnimKitBoneSetID : 0xFFFFFFFF;
        }

        if (express) {
            if (segmentInst->instFlags & SEGMENT_INST_FLAG_RESTART_PENDING) {
                this->RestartSegmentInstDueToExpression(segmentInst, stopKits);
            }

            uint32_t anim = segmentInst->curAnim;
            float speed = this->ComputeSegmentAnimSpeed(segmentInst);
            int32_t time;

            if (segmentInst->rec->m_segmentFlags & 0x8) {
                time = segmentInst->rec->m_animStartTime;
                speed = 0.0f;
            } else if (segmentInst->instFlags & (SEGMENT_INST_FLAG_ANIM_HELD | SEGMENT_INST_FLAG_HOLD_LAST_FRAME)) {
                if (speed < 0.0f) {
                    time = 0;
                } else {
                    time = segmentInst->repetitions * this->m_model->GetSequenceDuration(anim, segmentInst->variation) - 1;
                }

                speed = 0.0f;
            } else {
                time = this->ComputeSegmentAnimTime(segmentInst, speed);
                this->WrapSegmentTiming(segmentInst, speed, time);
            }

            if (time < 0) {
                time = 0;
            }

            int32_t blend;

            if (!primary || (segmentInst->segmentFlags & 0x1) || (segmentInst->userFlags & 0x1)) {
                blend = 0;
            } else {
                blend = 1;

                if (previous) {
                    blend = !(previous->flags & SEGMENT_BONE_SET_INST_FLAG_NO_BLEND);
                } else if (previousFlags & 0x1) {
                    blend = !(previousFlags & 0x4);
                }
            }

            if ((segmentInst->segmentFlags & 0x2) || (segmentInst->userFlags & 0x2)) {
                segmentBoneSetInst->flags |= SEGMENT_BONE_SET_INST_FLAG_NO_BLEND;
            } else {
                segmentBoneSetInst->flags &= ~SEGMENT_BONE_SET_INST_FLAG_NO_BLEND;
            }

            if (!blend || (managerFlags & ANIM_KIT_MANAGER_FLAG_SNAP)) {
                blend = 0;
            }

            segmentInst->instFlags &= ~SEGMENT_INST_FLAG_FALLBACK_MASK;

            if (boneDataID < 35) {
                int32_t last = boneDataID + boneSetRec->m_extraBoneCount;

                for (int32_t boneId = boneDataID; boneId <= last; boneId++) {
                    if (this->m_model->HasBone(boneId)) {
                        this->m_model->SetBoneSequence(boneId, anim, segmentInst->variation, time, speed, blend, primary, segmentInst->repetitions, segmentInst->blendTimeMode, segmentInst->blendTime);
                    }
                }
            } else {
                this->m_model->SetBoneSequence(0xFFFFFFFF, anim, segmentInst->variation, time, speed, blend, primary, segmentInst->repetitions, segmentInst->blendTimeMode, segmentInst->blendTime);
            }
        }
    }

unexpressCheck:
    if (!unexpress) {
        goto done;
    }

unexpressBones:
    if (this->m_model->HasBone(boneDataID)) {
        M2BoneSequenceInfo info;
        this->m_model->GetBoneSequenceInfo(boneDataID, info);

        if (info.m_sequenceId != 0xFFFFFFFF) {
            int32_t blend;
            uint8_t checkFlags = previous ? previous->flags : previousFlags;

            if ((previous || (previousFlags & 0x1)) && (checkFlags & 0x4)) {
                blend = 0;
            } else {
                blend = !(managerFlags & ANIM_KIT_MANAGER_FLAG_SNAP);
            }

            if (boneDataID < 35) {
                for (uint32_t boneId = boneDataID; boneId <= boneDataID + boneSetRec->m_extraBoneCount; boneId++) {
                    if (this->m_model->HasBone(boneId)) {
                        this->m_model->UnsetBoneSequence(boneId, blend, primary);
                    }
                }
            } else if (primary) {
                uint32_t anim = this->FilterSegmentAnim(0, 0, NULL, NULL);

                if (anim >= CAnimKitManager::s_animDataCount) {
                    anim = 0;
                }

                this->m_model->SetBoneSequence(0xFFFFFFFF, anim, 0xFFFFFFFF, 0, 1.0f, blend, 1, 0, 0, 0);
            }
        }
    }

done:
    if (previous && previous != segmentBoneSetInst) {
        previous->flags &= ~(SEGMENT_BONE_SET_INST_FLAG_EXPRESSING | SEGMENT_BONE_SET_INST_FLAG_SIMPLE);
    }
}

// OFFSET: 0x00594C20/0x140201DF0
void CAnimKitManager::ReinitAllSegmentInstAnimsAndEvents() {
    CAnimKitInst* kitInst = this->m_kitInsts.Head();

    while (!this->m_kitInsts.IsEnd(kitInst)) {
        CAnimKitInst* next = this->m_kitInsts.RawNext(kitInst);
        uint32_t count = kitInst->m_segmentInsts.Count();

        for (uint32_t i = 0; i < count; i++) {
            AnimKitSegmentInst* segmentInst = kitInst->m_segmentInsts[i];

            if (segmentInst && !this->ReinitSegmentInstAnimAndEvents(segmentInst)) {
                break;
            }
        }

        kitInst = next;
    }

    this->UpdateNextKitEventTime();
    this->UpdateNextSegmentEventTime();
    this->m_flags |= ANIM_KIT_MANAGER_FLAG_FORCE_EXPRESS;
    this->ScheduleUpdate(ANIM_KIT_MANAGER_FLAG_EXPRESS);
}

// OFFSET: 0x00594DD0/0x140202050
void CAnimKitManager::SetTier(uint16_t tier) {
    if (tier == this->m_tier) {
        return;
    }

    this->m_tier = tier;
    this->ReinitAllSegmentInstAnimsAndEvents();
}

// OFFSET: 0x00594DF0/0x140202070
void CAnimKitManager::SetModel(CM2Model* model, int32_t setCallbacks) {
    if (model == this->m_model) {
        return;
    }

    this->ClearExpressions();
    this->m_model = model;
    this->ResetModel();

    if (model) {
        this->ReinitAllSegmentInstAnimsAndEvents();
    } else {
        this->Init();
    }

    if (setCallbacks) {
        model->SetSequenceCallback(reinterpret_cast<void*>(CAnimKitManager::DefaultAnimEndHandler), reinterpret_cast<uintptr_t>(this));
        model->SetSequenceStartCallback(CAnimKitManager::DefaultAnimStartHandler, reinterpret_cast<uintptr_t>(this));
    }

    if (this->m_flags & ANIM_KIT_MANAGER_FLAG_FREEZE_MASK) {
        this->Unfreeze();
        this->Freeze(this->m_freezeTime);
    }
}

// OFFSET: 0x00594E70/0x140202140
void CAnimKitManager::UpdateAnimReplacementSets(int32_t count, const int32_t* setIDs) {
    if (!count && !this->m_replacementSets.Count()) {
        return;
    }

    this->m_replacementSets.SetCount(count);

    for (int32_t i = 0; i < count; i++) {
        const CAnimReplacementSetDef* def = CAnimReplacementSetDef::GetDef(setIDs[i]);

        if (!def) {
            this->m_replacementSets[i] = NULL;
            continue;
        }

        int32_t j = i;

        while (j > 0) {
            const CAnimReplacementSetDef* other = this->m_replacementSets[j - 1];

            if (other && def->m_rec->m_execOrder >= other->m_rec->m_execOrder) {
                break;
            }

            this->m_replacementSets[j] = this->m_replacementSets[j - 1];
            j--;
        }

        this->m_replacementSets[j] = def;
    }

    this->ReinitAllSegmentInstAnimsAndEvents();
}

// OFFSET: 0x00594FD0/0x140202340
void CAnimKitManager::UpdateExpressions() {
    if (!this->m_model || !this->m_model->IsLoaded() || (this->m_flags & (ANIM_KIT_MANAGER_FLAG_EXPRESSIONS_BLOCKED | ANIM_KIT_MANAGER_FLAG_FROZEN))) {
        this->m_flags &= ~ANIM_KIT_MANAGER_FLAG_EXPRESS;

        if (this->m_expressionCallback) {
            this->m_expressionCallback(this, this->m_expressionCallbackParam);
        }

        return;
    }

    this->UpdateLayeredFlag(0);
    TSCGrowableArray<AnimKitDesiredBoneSetInst, 10> desired;
    desired.SetCount(CAnimKitManager::s_animKitBoneSetCount);
    TSCGrowableArray<AnimKitSegmentInst*, 8> suppressed;
    TSCGrowableArray<CAnimKitInst*, 4> stopKits;
    int32_t maxBoneSetID = 0;
    int32_t suppressDescendants = 0;
    AnimKitSegmentBoneSetInst* segmentBoneSetInst = this->m_segmentBoneSetInsts.Head();

    while (!this->m_segmentBoneSetInsts.IsEnd(segmentBoneSetInst)) {
        AnimKitSegmentBoneSetInst* next = this->m_segmentBoneSetInsts.RawNext(segmentBoneSetInst);

        if (!(segmentBoneSetInst->flags & SEGMENT_BONE_SET_INST_FLAG_PENDING_DESTROY)) {
            AnimKitSegmentInst* segmentInst = segmentBoneSetInst->segmentInst;

            if (segmentInst && (segmentInst->instFlags & ANIM_KIT_SEGMENT_STATE_PLAYING)) {
                uint32_t layer = (segmentInst->segmentFlags >> 2) & 1;
                AnimKitBoneSetInstStack& stack = desired.Ptr()[segmentBoneSetInst->animKitBoneSetID].layers[layer];

                if (stack.stack.Count()) {
                    CAnimKitManager::SuppressSegmentBoneSetInstStack(stack, suppressed);
                }

                stack.stack.New(segmentBoneSetInst);

                if (layer == 1 && (this->m_flags & ANIM_KIT_MANAGER_FLAG_LAYERED)) {
                    CAnimKitManager::SuppressSegmentBoneSetInstStack(stack, suppressed);
                }

                if (segmentInst->segmentFlags & 0x100) {
                    CAnimKitManager::SuppressSegmentBoneSetInstStacksByAncestor(segmentBoneSetInst->animKitBoneSetID, layer, suppressed, desired, maxBoneSetID);
                }

                if (suppressDescendants) {
                    CAnimKitManager::SuppressSegmentBoneSetInstStacksByDescendant(segmentBoneSetInst->animKitBoneSetID, layer, suppressed, desired, maxBoneSetID);
                }

                if (segmentInst->segmentFlags & 0x20000) {
                    stack.dirty = 1;
                    suppressDescendants = 1;
                } else {
                    stack.dirty = 0;
                }

                if (maxBoneSetID <= segmentBoneSetInst->animKitBoneSetID) {
                    maxBoneSetID = segmentBoneSetInst->animKitBoneSetID;
                }
            }
        }

        segmentBoneSetInst = next;
    }

    if (suppressed.Count()) {
        this->ReactToSegmentInstSuppression(suppressed, desired, stopKits, maxBoneSetID);
    }

    uint16_t oldFlags = this->m_flags;
    this->m_flags &= ~(ANIM_KIT_MANAGER_FLAG_FORCE_EXPRESS | ANIM_KIT_MANAGER_FLAG_SNAP | ANIM_KIT_MANAGER_FLAG_EXPRESS);
    int32_t lastBoneSetID = this->m_maxExpressedBoneSetID > maxBoneSetID ? this->m_maxExpressedBoneSetID : maxBoneSetID;
    this->m_maxExpressedBoneSetID = maxBoneSetID;

    for (int32_t boneSetID = 0; boneSetID <= lastBoneSetID; boneSetID++) {
        for (int32_t layer = 0; layer < 2; layer++) {
            AnimKitSegmentBoneSetInst* top = CAnimKitManager::GetSegmentBoneSetInstStackTop(desired.Ptr()[boneSetID].layers[layer]);
            AnimKitBoneSetInst& boneSetInst = this->m_boneSetInsts[boneSetID];

            if (boneSetInst.expressing[layer] != top || (top && (top->flags & SEGMENT_BONE_SET_INST_FLAG_DIRTY)) || (boneSetInst.flags[layer] & 0x1) || (oldFlags & ANIM_KIT_MANAGER_FLAG_FORCE_EXPRESS)) {
                this->ExpressSegmentBoneSetInst(top, boneSetInst.expressing[layer], boneSetInst.flags[layer], desired, stopKits, boneSetID, layer, oldFlags);
            }
        }
    }

    uint32_t stopCount = stopKits.Count();

    for (uint32_t i = 0; i < stopCount; i++) {
        stopKits[i]->Stop(1, NULL);
    }

    if (this->m_flags & ANIM_KIT_MANAGER_FLAG_FREEZE_PENDING) {
        CM2Model* model = this->m_model;
        this->m_flags = (this->m_flags & ~ANIM_KIT_MANAGER_FLAG_FREEZE_MASK) | ANIM_KIT_MANAGER_FLAG_FROZEN;
        uint32_t time = model->m_scene->m_time;

        if (!time) {
            time = 1;
        }

        model->m_lastAnimTime = time;
        this->m_frozenTime = this->m_model->m_scene->m_time;
    }

    this->UpdateLayeredFlag(1);

    if (this->m_expressionCallback) {
        this->m_expressionCallback(this, this->m_expressionCallbackParam);
    }
}

// OFFSET: 0x00595470
uint32_t CAnimKitManager::Update() {
    if (!(this->m_flags & ANIM_KIT_MANAGER_FLAG_FROZEN)) {
        uint32_t now = CAnimKitManager::s_updateTolerance + this->m_model->m_scene->m_time;

        while ((this->m_flags & ANIM_KIT_MANAGER_FLAG_KIT_EVENT) && static_cast<int32_t>(now - this->m_nextKitEventTime) >= 0) {
            CAnimKitInst* kitInst = this->m_kitInsts.Head();
            int32_t alive = kitInst->Stop(1, NULL);

            if (alive) {
                this->m_kitInsts.UnlinkNode(kitInst);
            }

            this->UpdateNextKitEventTime();

            if (alive) {
                this->m_kitInsts.LinkToTail(kitInst);
            }
        }

        while ((this->m_flags & ANIM_KIT_MANAGER_FLAG_SEGMENT_EVENT) && static_cast<int32_t>(now - this->m_nextSegmentEventTime) >= 0) {
            this->HandleNextSegmentEvent();
        }
    }

    if (this->m_flags & ANIM_KIT_MANAGER_FLAG_EXPRESS) {
        this->UpdateExpressions();
    }

    return this->m_flags & ANIM_KIT_MANAGER_FLAG_UPDATE_MASK;
}

// OFFSET: 0x005955C0/0x140202A40
void CAnimKitManager::UpdateAll(int32_t tolerance) {
    if (!CAnimKitManager::s_initialized) {
        return;
    }

    CAnimKitManager::s_updateTolerance = tolerance;
    CAnimKitManager* manager = s_updateList.Head();

    while (!s_updateList.IsEnd(manager)) {
        CAnimKitManager* next = s_updateList.RawNext(manager);

        if (!manager->Update()) {
            s_idleList.LinkToTail(manager);
        }

        manager = next;
    }

    CAnimKitManager::s_updateTolerance = 0;
}
