// INLINED
inline AnimKitBoneSetInst::AnimKitBoneSetInst() {
    for (uint32_t i = 0; i < 2; i++) {
        this->expressing[i] = NULL;
        this->flags[i] = 0;
    }
}

// OFFSET: 0x00590510
inline AnimKitBoneSetInstStack::AnimKitBoneSetInstStack() {
    this->dirty = 0;
}

// OFFSET: 0x0080A240
inline CAnimKitManager::CAnimKitManager(const CAnimKitManager& source)
    : m_flags(source.m_flags)
    , m_tier(source.m_tier)
    , m_model(source.m_model)
    , m_kitInsts(source.m_kitInsts)
    , m_segmentEvents(source.m_segmentEvents)
    , m_segmentBoneSetInsts(source.m_segmentBoneSetInsts)
    , m_boneSetInsts(source.m_boneSetInsts)
    , m_maxExpressedBoneSetID(source.m_maxExpressedBoneSetID)
    , m_blockedBoneSetMask(source.m_blockedBoneSetMask)
    , m_nextKitEventTime(source.m_nextKitEventTime)
    , m_nextSegmentEventTime(source.m_nextSegmentEventTime)
    , m_expressionCallback(source.m_expressionCallback)
    , m_expressionCallbackParam(source.m_expressionCallbackParam)
    , m_freezeTime(source.m_freezeTime)
    , m_frozenTime(source.m_frozenTime)
    , m_replacementSets(source.m_replacementSets) {
}
