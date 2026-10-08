#ifndef WOW_SOURCE_GLUE_CGLUELOADING_H
#define WOW_SOURCE_GLUE_CGLUELOADING_H

#include <cstdint>

class CCharacterComponent;

class CGlueLoading {
    public:
    static uint32_t m_loadStartTime;
    static uint32_t m_loadLastTime;
    static uint32_t m_loadCompleteTime;
    static float m_startProgress;
    static float m_loadProgress;
    static int32_t m_loadState;
    static void (*m_loadCompleteCallback)(uint64_t data);
    static uint32_t m_loadCompleteDelayFrames;
    static uint64_t m_loadCompleteData;

    static void CancelLoadCallback();
    static double ComputeLoadProgress(CCharacterComponent** components, int32_t count, int64_t& loaded, int64_t& total);
    static int32_t Idle(CCharacterComponent** components, int32_t count);
    static void StartLoad(CCharacterComponent** components, uint32_t count, int32_t keepProgress, void (*callback)(uint64_t data), uint64_t data, uint32_t delayFrames);
    static int32_t Idle(CCharacterComponent* component);
    static void StartLoad(CCharacterComponent* component, int32_t keepProgress, void (*callback)(uint64_t data), uint64_t data, uint32_t delayFrames);
};

#endif
