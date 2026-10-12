#include "CGlueLoading.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/LoadingScreen.h"

// STATIC: 0x00EBC4A0/0x140DAD480
uint32_t CGlueLoading::m_loadStartTime;

// STATIC: 0x00EBC4A4/0x140DAD484
uint32_t CGlueLoading::m_loadLastTime;

// STATIC: 0x00EBC4A8/0x140DAD488
uint32_t CGlueLoading::m_loadCompleteTime;

// STATIC: 0x00EBC4AC/0x140DAD48C
float CGlueLoading::m_startProgress;

// STATIC: 0x00EBC4B0/0x140DAD490
float CGlueLoading::m_loadProgress;

// STATIC: 0x00EBC4B4/0x140DAD494
int32_t CGlueLoading::m_loadState;

// STATIC: 0x00EBC4B8/0x140DAD498
void (*CGlueLoading::m_loadCompleteCallback)(uint64_t data);

// STATIC: 0x00EBC4BC/0x140DAD4A8
uint32_t CGlueLoading::m_loadCompleteDelayFrames;

// STATIC: 0x00EBC4C0/0x140DAD4A0
uint64_t CGlueLoading::m_loadCompleteData;

// PROVEN: b0a143f2 IDENTICAL/IDENTICAL
// OFFSET: 0x00811460/0x140528D20
void CGlueLoading::CancelLoadCallback() {
    CGlueLoading::m_loadState = 4;
    CGlueLoading::m_loadCompleteCallback = NULL;
    CGlueLoading::m_loadCompleteData = 0;
    CGlueLoading::m_loadCompleteDelayFrames = 0;
}

// OFFSET: 0x00811490
double CGlueLoading::ComputeLoadProgress(CCharacterComponent** components, int32_t count, int64_t& loaded, int64_t& total) {
    loaded = 0;
    total = 0;

    if (count <= 0) {
        return 0.0;
    }

    for (int32_t i = 0; i < count; i++) {
        CCharacterComponent* component = components[i];
        int64_t componentLoaded = 0;
        int64_t componentTotal = 0;

        if (component) {
            component->ComputeLoadProgress(componentLoaded, componentTotal);
            component->m_data.m_model->ComputeLoadProgress(componentLoaded, componentTotal);
        }

        loaded += componentLoaded;
        total += componentTotal;
    }

    if (total) {
        return static_cast<double>(loaded) / static_cast<double>(total);
    }

    return 0.0;
}

// OFFSET: 0x00811540/0x140528D50
int32_t CGlueLoading::Idle(CCharacterComponent** components, int32_t count) {
    uint32_t time = FrameTime::GetCurTimeMs();
    uint32_t elapsedMs = time - CGlueLoading::m_loadLastTime;
    double elapsed = static_cast<double>(elapsedMs) * 0.001;
    CGlueLoading::m_loadLastTime = time;

    switch (CGlueLoading::m_loadState) {
        case 1: {
            int64_t loaded;
            int64_t total;
            CGlueLoading::ComputeLoadProgress(components, count, loaded, total);

            if (loaded == total) {
                CGlueLoading::m_loadState = 4;
                LoadingScreenMiniDisable();
            } else if (time - CGlueLoading::m_loadStartTime > 500) {
                CGlueLoading::m_loadState = 2;
            }

            for (int32_t i = 0; i < count; i++) {
                CCharacterComponent* component = components[i];

                if (!component) {
                    continue;
                }

                CM2Model* model = component->m_data.m_model;
                uint32_t visible = component->m_baseTexture != NULL;

                if (model->m_attachParent) {
                    model->m_fFlags = (model->m_fFlags & ~0x40u) | (visible << 6);
                } else {
                    model->m_fFlags = (model->m_fFlags & ~0x8u) | (visible << 3);
                }

                if (model->m_attachParent) {
                    model->m_fFlags = (model->m_fFlags & ~0x10000u) | (visible << 16);
                } else {
                    model->m_fFlags = (model->m_fFlags & ~0x8000u) | (visible << 15);
                }
            }

            break;
        }

        case 2: {
            int64_t loaded;
            int64_t total;
            double progress = CGlueLoading::ComputeLoadProgress(components, count, loaded, total);
            CGlueLoading::m_loadProgress = static_cast<float>(elapsed * 0.25) + CGlueLoading::m_loadProgress;
            float current = CGlueLoading::m_loadProgress;

            if (CGlueLoading::m_loadProgress > progress) {
                current = static_cast<float>(progress);
                CGlueLoading::m_loadProgress = current;
            }

            if (loaded == total) {
                CGlueLoading::m_loadState = 3;
                CGlueLoading::m_loadCompleteTime = time + 250;
            }

            if (count > 0) {
                for (int32_t i = 0; i < count; i++) {
                    CCharacterComponent* component = components[i];

                    if (!component) {
                        continue;
                    }

                    CM2Model* model = component->m_data.m_model;
                    uint32_t visible = component->m_baseTexture != NULL;

                    if (model->m_attachParent) {
                        model->m_fFlags = (model->m_fFlags & ~0x40u) | (visible << 6);
                    } else {
                        model->m_fFlags = (model->m_fFlags & ~0x8u) | (visible << 3);
                    }

                    if (model->m_attachParent) {
                        model->m_fFlags = (model->m_fFlags & ~0x10000u) | (visible << 16);
                    } else {
                        model->m_fFlags = (model->m_fFlags & ~0x8000u) | (visible << 15);
                    }
                }

                current = CGlueLoading::m_loadProgress;
            }

            LoadingScreenMiniUpdate((current - CGlueLoading::m_startProgress) / (1.0f - CGlueLoading::m_startProgress));
            break;
        }

        case 3: {
            CGlueLoading::m_loadProgress = static_cast<float>(elapsed * 4.0) + CGlueLoading::m_loadProgress;
            float current = CGlueLoading::m_loadProgress;

            if (CGlueLoading::m_loadProgress > 1.0f) {
                current = 1.0f;
                CGlueLoading::m_loadProgress = 1.0f;
            }

            LoadingScreenMiniUpdate((current - CGlueLoading::m_startProgress) / (1.0f - CGlueLoading::m_startProgress));

            if (static_cast<int32_t>(time - CGlueLoading::m_loadCompleteTime) > 0) {
                LoadingScreenMiniDisable();
                CGlueLoading::m_loadState = 4;
            }

            break;
        }
    }

    if (!CGlueLoading::m_loadCompleteCallback || CGlueLoading::m_loadState != 4) {
        return 1;
    }

    if (!CGlueLoading::m_loadCompleteDelayFrames) {
        CGlueLoading::m_loadCompleteCallback(CGlueLoading::m_loadCompleteData);
        CGlueLoading::m_loadCompleteCallback = NULL;
        return 1;
    }

    CGlueLoading::m_loadCompleteDelayFrames--;
    return 1;
}

// OFFSET: 0x00811830/0x140529140
void CGlueLoading::StartLoad(CCharacterComponent** components, uint32_t count, int32_t keepProgress, void (*callback)(uint64_t data), uint64_t data, uint32_t delayFrames) {
    if (SFile::IsStreamingMode()) {
        int64_t loaded;
        int64_t total;
        float progress = static_cast<float>(CGlueLoading::ComputeLoadProgress(components, count, loaded, total));

        if (loaded != total) {
            for (uint32_t i = 0; i < count; i++) {
                CCharacterComponent* component = components[i];

                if (!component) {
                    continue;
                }

                CM2Model* model = component->m_data.m_model;
                uint32_t visible = component->m_baseTexture != NULL;

                if (model->m_attachParent) {
                    model->m_fFlags = (model->m_fFlags & ~0x40u) | (visible << 6);
                } else {
                    model->m_fFlags = (model->m_fFlags & ~0x8u) | (visible << 3);
                }

                if (model->m_attachParent) {
                    model->m_fFlags = (model->m_fFlags & ~0x10000u) | (visible << 16);
                } else {
                    model->m_fFlags = (model->m_fFlags & ~0x8000u) | (visible << 15);
                }
            }

            CGlueLoading::m_startProgress = keepProgress ? progress : 0.0f;
            CGlueLoading::m_loadProgress = progress;
            CGlueLoading::m_loadState = 1;
            CGlueLoading::m_loadLastTime = FrameTime::GetCurTimeMs();
            CGlueLoading::m_loadStartTime = CGlueLoading::m_loadLastTime;
        } else {
            LoadingScreenMiniDisable();
            CGlueLoading::m_loadState = 4;
        }
    } else {
        for (uint32_t i = 0; i < count; i++) {
            CCharacterComponent* component = components[i];

            if (component) {
                component->RenderPrep(1);
                component->m_data.m_model->WaitUntilDrawableRecursive();
            }
        }

        CGlueLoading::m_loadState = 4;
    }

    CGlueLoading::m_loadCompleteCallback = callback;
    CGlueLoading::m_loadCompleteData = data;
    CGlueLoading::m_loadCompleteDelayFrames = delayFrames;
}

// PROVEN: 40ba194a IDENTICAL/IDENTICAL
// OFFSET: 0x008119A0/0x140529360
int32_t CGlueLoading::Idle(CCharacterComponent* component) {
    CCharacterComponent* components = component;
    return CGlueLoading::Idle(&components, 1);
}

// PROVEN: 023ec89f IDENTICAL/IDENTICAL
// OFFSET: 0x008119C0/0x140529380
void CGlueLoading::StartLoad(CCharacterComponent* component, int32_t keepProgress, void (*callback)(uint64_t data), uint64_t data, uint32_t delayFrames) {
    CCharacterComponent* components = component;
    CGlueLoading::StartLoad(&components, 1, keepProgress, callback, data, delayFrames);
}
