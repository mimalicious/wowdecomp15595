#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_ANIMKITMANAGER_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_ANIMKITMANAGER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class AnimKitConfigBoneSetRec;
class AnimKitConfigRec;
class AnimKitRec;
class AnimKitSegmentBoneSetInst;
class AnimKitSegmentInst;
class AnimKitSegmentRec;
class CAnimKitInst;
class CAnimKitManager;
class CAnimReplacementSetDef;
class CM2Model;

typedef void (*AnimKitStopCallback)(CAnimKitInst* inst, uint64_t param);
typedef void (*AnimKitExpressionCallback)(CAnimKitManager* manager, uint64_t param);

enum ANIM_KIT_TIER {
    ANIM_KIT_TIER_COUNT = 5
};

enum ANIM_KIT_SEGMENT_EVENT_TYPE {
    ANIM_KIT_SEGMENT_EVENT_START = 0,
    ANIM_KIT_SEGMENT_EVENT_END = 1
};

enum ANIM_KIT_SEGMENT_STATE {
    ANIM_KIT_SEGMENT_STATE_NONE = 0,
    ANIM_KIT_SEGMENT_STATE_PLAYING = 1,
    ANIM_KIT_SEGMENT_STATE_STOPPED = 2
};

enum ANIM_KIT_DEFERRED_COMMAND_TYPE {
    ANIM_KIT_DEFERRED_SET_ANIM = 0,
    ANIM_KIT_DEFERRED_SET_SPEED = 1,
    ANIM_KIT_DEFERRED_SET_USER_FLAGS = 2,
    ANIM_KIT_DEFERRED_RESTART = 3,
    ANIM_KIT_DEFERRED_STOP = 4,
    ANIM_KIT_DEFERRED_SET_STOP_CALLBACK = 5
};

enum {
    KIT_INST_FLAG_MAINTAIN = 0x1,
    KIT_INST_FLAG_DEFERRED = 0x2,
    KIT_INST_FLAG_STOPPING = 0x4,
    KIT_INST_FLAG_STOPPED = 0x8,
    KIT_INST_FLAG_DURATION_ELAPSED = 0x10,
    KIT_INST_FLAG_REACT_TO_SEGMENT_START = 0x20,
    KIT_INST_FLAG_REACT_TO_SEGMENT_END = 0x40,
    KIT_INST_FLAG_PENDING_DESTROY = 0x80,
    KIT_INST_FLAG_SPEED_IS_FRACTION = 0x100
};

enum {
    SEGMENT_INST_FLAG_STATE_MASK = 0x3,
    SEGMENT_INST_FLAG_RESTART_MASK = 0xC,
    SEGMENT_INST_FLAG_ANIM_HELD = 0x4,
    SEGMENT_INST_FLAG_RESTART_PENDING = 0x8,
    SEGMENT_INST_FLAG_HOLD_LAST_FRAME = 0x10,
    SEGMENT_INST_FLAG_REVERSE = 0x20,
    SEGMENT_INST_FLAG_PENDING_DESTROY = 0x40,
    SEGMENT_INST_FLAG_NO_LOOP = 0x80,
    SEGMENT_INST_FLAG_FALLBACK_MASK = 0x300
};

enum {
    SEGMENT_BONE_SET_INST_FLAG_SUPPRESSIBLE = 0x1,
    SEGMENT_BONE_SET_INST_FLAG_DIRTY = 0x2,
    SEGMENT_BONE_SET_INST_FLAG_NO_BLEND = 0x4,
    SEGMENT_BONE_SET_INST_FLAG_EXPRESSING = 0x8,
    SEGMENT_BONE_SET_INST_FLAG_PENDING_DESTROY = 0x10,
    SEGMENT_BONE_SET_INST_FLAG_SIMPLE = 0x20
};

enum {
    ANIM_KIT_MANAGER_FLAG_KIT_EVENT = 0x1,
    ANIM_KIT_MANAGER_FLAG_SEGMENT_EVENT = 0x2,
    ANIM_KIT_MANAGER_FLAG_EXPRESS = 0x4,
    ANIM_KIT_MANAGER_FLAG_UPDATE_MASK = 0x7,
    ANIM_KIT_MANAGER_FLAG_EXPRESSIONS_BLOCKED = 0x10,
    ANIM_KIT_MANAGER_FLAG_SNAP = 0x20,
    ANIM_KIT_MANAGER_FLAG_FREEZE_PENDING = 0x40,
    ANIM_KIT_MANAGER_FLAG_FROZEN = 0x80,
    ANIM_KIT_MANAGER_FLAG_FREEZE_MASK = 0xC0,
    ANIM_KIT_MANAGER_FLAG_FORCE_EXPRESS = 0x100,
    ANIM_KIT_MANAGER_FLAG_LAYERED = 0x200,
    ANIM_KIT_MANAGER_FLAG_INITIALIZED = 0x400,
    ANIM_KIT_MANAGER_FLAG_GLUE_MODE = 0x800,
    ANIM_KIT_MANAGER_FLAG_GAME_OBJECT_MODE = 0x1000
};

struct AnimKitExtraPlayParams {
    /* 0000 */ int32_t boneSetID;
    /* 0004 */ uint16_t startTime;
    /* 0008 */ float startFraction;
    /* 000C */ uint8_t oneShot;
    /* 000D */ uint8_t blendTimeMode;
    /* 000E */ uint16_t blendTime;
};

struct AnimKitDef {
    /* 0000 */ const AnimKitRec* rec;
    /* 0004 */ TSCGrowableArray<const AnimKitSegmentRec*, 3> segmentRec;
};

struct AnimKitConfigDef {
    /* 0000 */ const AnimKitConfigRec* rec;
    /* 0004 */ TSCGrowableArray<const AnimKitConfigBoneSetRec*, 3> configBoneSetRec;
};

struct AnimKitDeferredCommand {
    struct SetAnimArgs {
        /* 0000 */ uint32_t segmentIndex;
        /* 0004 */ uint32_t anim;
        /* 0008 */ uint8_t variation;
        /* 0009 */ uint8_t repetitions;
        /* 000A */ uint8_t blendTimeMode;
        /* 000C */ uint16_t blendTime;
    };

    struct SetSpeedArgs {
        /* 0000 */ uint32_t segmentIndex;
        /* 0004 */ uint32_t speedType;
        /* 0008 */ float speed;
    };

    struct SetUserFlagsArgs {
        /* 0000 */ uint32_t segmentIndex;
        /* 0004 */ uint8_t userFlags;
    };

    struct StopArgs {
        /* 0000 */ int32_t playStopKit;
    };

    struct SetStopCallbackArgs {
        /* 0000 */ AnimKitStopCallback callback;
        /* 0008 */ uint64_t param;
    };

    /* 0000 */ uint32_t type;
    /* 0008 */ union {
        SetAnimArgs setAnim;
        SetSpeedArgs setSpeed;
        SetUserFlagsArgs setUserFlags;
        StopArgs stop;
        SetStopCallbackArgs setStopCallback;
    };
};

struct AnimKitBoneSetInst {
    /* 0000 */ AnimKitSegmentBoneSetInst* expressing[2];
    /* 0008 */ uint8_t flags[2];

    AnimKitBoneSetInst();
};

struct AnimKitBoneSetInstStack {
    /* 0000 */ uint8_t dirty : 1;
    /* 0004 */ TSCGrowableArray<AnimKitSegmentBoneSetInst*, 5> stack;

    AnimKitBoneSetInstStack();
};

struct AnimKitDesiredBoneSetInst {
    /* 0000 */ AnimKitBoneSetInstStack layers[2];

    ~AnimKitDesiredBoneSetInst();
};

class AnimKitSegmentBoneSetInst {
    public:
    /* 0000 */ AnimKitSegmentInst* segmentInst;
    /* 0004 */ uint8_t index;
    /* 0005 */ uint8_t flags;
    /* 0006 */ uint8_t animKitBoneSetID;
    /* 0007 */ uint8_t priority;
    /* 0008 */ TSLink<AnimKitSegmentBoneSetInst> link;
};

class AnimKitSegmentInst {
    public:
    /* 0000 */ CAnimKitInst* kitInst;
    /* 0004 */ const AnimKitSegmentRec* rec;
    /* 0008 */ uint8_t index;
    /* 0009 */ uint8_t boneSetInstCount;
    /* 000A */ uint8_t suppressedCount;
    /* 000B */ uint8_t variation;
    /* 000C */ uint8_t repetitions;
    /* 000D */ uint8_t userFlags;
    /* 000E */ uint8_t speedType;
    /* 000F */ int8_t forcedVariation;
    /* 0010 */ uint8_t forcedRepetitions;
    /* 0011 */ uint8_t blendTimeMode;
    /* 0012 */ uint16_t blendTime;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ int8_t inheritedVariation;
    /* 0016 */ uint16_t instFlags;
    /* 0018 */ uint16_t curAnim;
    /* 001A */ uint16_t requestedAnim;
    /* 001C */ float speed;
    /* 0020 */ uint32_t segmentFlags;
    /* 0024 */ TSCGrowableArray<AnimKitSegmentBoneSetInst*, 3> boneSetInsts;
    /* 0038 */ uint32_t startTime;
    /* 003C */ uint32_t startEventTime;
    /* 0040 */ uint32_t endEventTime;
    /* 0044 */ TSLink<AnimKitSegmentInst> link;

    ~AnimKitSegmentInst();
};

class CAnimKitInst {
    public:
    /* 0000 */ uint32_t m_segmentCount : 6;
    /* 0000 */ uint32_t m_stoppedSegmentCount : 6;
    /* 0000 */ uint32_t m_flags : 12;
    /* 0004 */ uint8_t m_boneSetID;
    /* 0006 */ uint16_t m_anim;
    /* 0008 */ uint16_t m_startTime;
    /* 000A */ uint16_t m_blendTime;
    /* 000C */ uint8_t m_blendTimeMode;
    /* 0010 */ CAnimKitManager* m_manager;
    /* 0014 */ AnimKitDef* m_def;
    /* 0018 */ TSCGrowableArray<AnimKitSegmentInst*, 3> m_segmentInsts;
    /* 002C */ AnimKitStopCallback m_stopCallback;
    /* 0030 */ uint64_t m_stopCallbackParam;
    /* 0038 */ TSCGrowableArray<AnimKitDeferredCommand, 2> m_deferredCommands;
    /* 006C */ uint32_t m_nextEventTime;
    /* 0070 */ TSLink<CAnimKitInst> m_link;

    ~CAnimKitInst();
    uint32_t GetID() const;
    uint32_t GetSegmentCount() const;
    float GetSegmentProgress(uint32_t index);
    int32_t IsStoppedOrPendingDestroy() const;
    void OnModelLoaded();
    int32_t OnSegmentInstTimingChange(AnimKitSegmentInst* segmentInst, float speed);
    void Release();
    int32_t Restart();
    int32_t SetAnim(uint32_t segmentIndex, uint32_t anim, uint8_t variation, uint8_t repetitions, uint8_t blendTimeMode, uint16_t blendTime);
    int32_t SetSpeed(uint32_t segmentIndex, uint32_t speedType, float speed);
    void SetStopCallback(AnimKitStopCallback callback, uint64_t param);
    int32_t SetUserFlags(uint32_t segmentIndex, uint8_t userFlags);
    int32_t Stop(int32_t playStopKit, CAnimKitInst** stopKitInst);
};

class CAnimKitManager {
    public:
    static AnimKitDef** s_animKitDefs;
    static uint32_t s_animKitDefCount;
    static AnimKitConfigDef** s_animKitConfigDefs;
    static uint32_t s_animKitConfigDefCount;
    static uint16_t s_animDataCount;
    static uint32_t s_animKitBoneSetCount;
    static int32_t s_updateTolerance;
    static int32_t s_initialized;
    static int32_t s_inChainedSegmentStart;

    static CAnimKitManager* Alloc();
    static void DefaultAnimEndHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t primary);
    static void DefaultAnimStartHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, uint32_t variation, uint32_t time, uint64_t param, int32_t primary);
    static uint32_t FindBestAnimKitBoneSetIDForBoneDataID(uint32_t boneDataID);
    static void Free(CAnimKitManager* manager);
    static void GarbageCollect();
    static AnimKitSegmentBoneSetInst* GetSegmentBoneSetInstStackTop(AnimKitBoneSetInstStack& stack);
    static int32_t HandleKitReactionToSegmentEnd(CAnimKitInst* kitInst, uint8_t segmentIndex, uint32_t time, int32_t allowStop);
    static int32_t HandleKitReactionToSegmentStart(CAnimKitInst* kitInst, uint8_t segmentIndex, uint32_t time, int32_t allowStop);
    static void InitSystem();
    static void SuppressSegmentBoneSetInstStack(AnimKitBoneSetInstStack& stack, TSCGrowableArray<AnimKitSegmentInst*, 8>& suppressed);
    static void SuppressSegmentBoneSetInstStacksByAncestor(uint32_t boneSetID, int32_t layer, TSCGrowableArray<AnimKitSegmentInst*, 8>& suppressed, TSCGrowableArray<AnimKitDesiredBoneSetInst, 10>& desired, int32_t maxBoneSetID);
    static void SuppressSegmentBoneSetInstStacksByDescendant(uint32_t boneSetID, int32_t layer, TSCGrowableArray<AnimKitSegmentInst*, 8>& suppressed, TSCGrowableArray<AnimKitDesiredBoneSetInst, 10>& desired, int32_t maxBoneSetID);
    static void UninitSystem();
    static void UpdateAll(int32_t tolerance);

    /* 0000 */ uint16_t m_flags;
    /* 0002 */ uint16_t m_tier;
    /* 0004 */ CM2Model* m_model;
    /* 0008 */ TSExplicitList<CAnimKitInst, offsetof(CAnimKitInst, m_link)> m_kitInsts;
    /* 0014 */ TSExplicitList<AnimKitSegmentInst, offsetof(AnimKitSegmentInst, link)> m_segmentEvents;
    /* 0020 */ TSExplicitList<AnimKitSegmentBoneSetInst, offsetof(AnimKitSegmentBoneSetInst, link)> m_segmentBoneSetInsts;
    /* 002C */ TSCGrowableArray<AnimKitBoneSetInst, 10> m_boneSetInsts;
    /* 00A8 */ int32_t m_maxExpressedBoneSetID;
    /* 00AC */ uint32_t m_blockedBoneSetMask;
    /* 00B0 */ uint32_t m_nextKitEventTime;
    /* 00B4 */ uint32_t m_nextSegmentEventTime;
    /* 00B8 */ AnimKitExpressionCallback m_expressionCallback;
    /* 00C0 */ uint64_t m_expressionCallbackParam;
    /* 00C8 */ uint32_t m_freezeTime;
    /* 00CC */ uint32_t m_frozenTime;
    /* 00D0 */ TSCGrowableArray<const CAnimReplacementSetDef*, 3> m_replacementSets;
    /* 00E4 */ TSLink<CAnimKitManager> m_updateLink;

    CAnimKitManager();
    CAnimKitManager(const CAnimKitManager& source);
    ~CAnimKitManager();
    void AddKitEvent(CAnimKitInst* kitInst, uint32_t time, int32_t duration);
    int32_t AddSegmentEvent(AnimKitSegmentInst* segmentInst, uint32_t time, int32_t delay, ANIM_KIT_SEGMENT_EVENT_TYPE type, int32_t allowStop);
    int32_t ComputeSegmentAnimTime(AnimKitSegmentInst* segmentInst, float speed);
    float ComputeSegmentAnimSpeed(AnimKitSegmentInst* segmentInst);
    int32_t ComputeSegmentSceneTimeDurationUntilAnimEndTime(AnimKitSegmentInst* segmentInst, float speed);
    void ClearExpressions();
    void ExpressSegmentBoneSetInst(AnimKitSegmentBoneSetInst* segmentBoneSetInst, AnimKitSegmentBoneSetInst*& expressing, uint8_t& expressingFlags, TSCGrowableArray<AnimKitDesiredBoneSetInst, 10>& desired, TSCGrowableArray<CAnimKitInst*, 4>& stopKits, uint32_t boneSetID, int32_t layer, uint16_t managerFlags);
    uint32_t FilterAnim(uint32_t anim, int32_t allowFallback);
    int32_t FilterAnimKitBoneSetID(uint32_t boneSetID, uint32_t segmentFlags);
    uint32_t FilterSegmentAnim(uint32_t anim, int32_t ignoreTier, int32_t* changed, uint16_t* instFlags);
    int32_t FindCustomFallbackAnim(uint32_t anim, uint16_t* instFlags);
    void Freeze(uint32_t time);
    uint32_t GetCurTime();
    uint32_t GetExpressingAnim(uint32_t boneDataID, int32_t primary, int32_t requested);
    CAnimKitInst* GetExpressingAnimKitInst(uint32_t boneDataID, int32_t primary) const;
    AnimKitSegmentInst* GetExpressingHandItemSegmentInst(uint32_t& boneDataID, int32_t primary);
    uint32_t GetExpressingHandItemAnim(uint32_t& boneDataID, int32_t primary, int32_t requested);
    uint32_t GetSegmentInstExpressingAnim(AnimKitSegmentInst* segmentInst, uint32_t boneDataID, int32_t requested);
    void HandleNextSegmentEvent();
    void Init();
    CAnimKitInst* InitKitInst(uint32_t animKitID, int32_t maintain, uint32_t anim, AnimKitExtraPlayParams* params, CAnimKitInst* kitInst);
    AnimKitSegmentBoneSetInst* InitSegmentBoneSetInst(AnimKitSegmentInst* segmentInst, const AnimKitConfigBoneSetRec* rec, int32_t index, uint8_t boneSetID);
    AnimKitSegmentInst* InitSegmentInst(CAnimKitInst* kitInst, int32_t index, uint16_t anim, uint16_t requestedAnim, uint8_t boneSetID, uint16_t instFlags, uint8_t blendTimeMode, uint16_t blendTime);
    int32_t IsFrozen() const;
    void MirrorFrom(CAnimKitManager* source, int32_t filter);
    void MirrorToModel(CM2Model* model, CAnimKitManager* filter);
    void OnModelLoaded();
    void OnModelSequenceCallback(uint32_t boneId, uint32_t sequenceId, int32_t interrupted, int32_t primary);
    int32_t OnModelSequenceStartCallback(uint32_t boneId);
    int32_t OnPlaySegmentInst(AnimKitSegmentInst* segmentInst, int32_t allowStop);
    int32_t OnStopSegmentInst(AnimKitSegmentInst* segmentInst, int32_t allowStop);
    void PickSegmentInstVariationAndRepetitions(AnimKitSegmentInst* segmentInst);
    CAnimKitInst* PlayAndForget(uint32_t animKitID, uint32_t anim, AnimKitExtraPlayParams* params);
    CAnimKitInst* PlayAndMaintain(uint32_t animKitID, uint32_t anim, AnimKitExtraPlayParams* params);
    int32_t PostInitKitInst(CAnimKitInst* kitInst);
    void PostInitSegmentInst(AnimKitSegmentInst* segmentInst, TSStackArray<AnimKitSegmentInst*>& play, TSStackArray<AnimKitSegmentInst*>& stop);
    int32_t ProcessSegmentRec(const AnimKitSegmentRec* rec, uint8_t boneSetID, uint32_t requestedAnim, uint32_t* anim, uint16_t* instFlags);
    void ReactToSegmentInstSuppression(TSCGrowableArray<AnimKitSegmentInst*, 8>& suppressed, TSCGrowableArray<AnimKitDesiredBoneSetInst, 10>& desired, TSCGrowableArray<CAnimKitInst*, 4>& stopKits, uint32_t maxBoneSetID);
    int32_t ReinitSegmentInstAnimAndEvents(AnimKitSegmentInst* segmentInst);
    void ReinitAllSegmentInstAnimsAndEvents();
    void ResetModel();
    void RestartSegmentInstDueToExpression(AnimKitSegmentInst* segmentInst, TSCGrowableArray<CAnimKitInst*, 4>& stopKits);
    void ScheduleFullUpdate();
    void ScheduleUpdate(uint16_t flags);
    void SetBoneDataIDBlocked(int32_t boneDataID, int32_t blocked, int32_t blockAlt);
    void SetExpressionCallback(AnimKitExpressionCallback callback, uint64_t param);
    void SetExpressionsBlocked(int32_t blocked, int32_t reset);
    void SetGameObjectMode(int32_t enable);
    void SetGlueMode(int32_t enable);
    void SetModel(CM2Model* model, int32_t setCallbacks);
    int32_t SetSegmentInstState(AnimKitSegmentInst* segmentInst, uint32_t state, int32_t allowStop);
    void SetTier(uint16_t tier);
    void Uninit();
    void UninitKitInst(CAnimKitInst* kitInst);
    void UninitSegmentBoneSetInst(AnimKitSegmentBoneSetInst* segmentBoneSetInst);
    void UninitSegmentInst(AnimKitSegmentInst* segmentInst);
    void Unfreeze();
    uint32_t Update();
    void UpdateAnimReplacementSets(int32_t count, const int32_t* setIDs);
    void UpdateExpressions();
    void UpdateHandItemAnim(uint32_t type, CM2Model* model, void* callback, uint64_t param, uint32_t sheathedAnim);
    void UpdateLayeredFlag(int32_t schedule);
    void UpdateNextKitEventTime();
    void UpdateNextSegmentEventTime();
    int32_t UpdateSegmentInstEndEvent(AnimKitSegmentInst* segmentInst, float speed, int32_t allowStop);
    int32_t WrapSegmentTiming(AnimKitSegmentInst* segmentInst, float speed, int32_t& time);
};

#include "WoW/Source/Object/ObjectClient/AnimKitManager.inl"

#endif
