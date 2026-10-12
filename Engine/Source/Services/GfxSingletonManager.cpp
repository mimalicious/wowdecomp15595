#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>
#include "Engine/Source/Services/GfxSingletonManager.h"
#include "Engine/Source/Services/ParticleSystem.h"
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/Services/RibbonEmitter.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRandom.h"

// STATIC: 0x00D27C88/0x140BE64B0
static float s_scaler = 1.0f;

// STATIC: 0x01008B38/0x140EE04E0
ParticleSystemManager* ParticleSystemManager::s_instance;

// STATIC: 0x01008B3C/0x140EE04E8
RibbonManager* RibbonManager::s_instance;

// OFFSET: 0x00B13AD0
float ParticleSystemManager::GetScaler() {
    return s_scaler;
}

// OFFSET: 0x00B13B80/0x1408D98D0
void ParticleSystemManager::SetScaler(float scaler) {
    if (scaler < 0.0f) {
        s_scaler = 0.0f;
    } else if (scaler < 1.0f) {
        s_scaler = scaler;
    } else {
        s_scaler = 1.0f;
    }
}

// OFFSET: 0x00B13C10/0x1408D9990
void ParticleSystemManager::Flush() {
    for (uint32_t i = this->m_emitters2.Count(); i; i--) {
        this->m_emitters2[i - 1]->DecRef();
    }

    this->m_emitters2.m_count = 0;

    for (uint32_t i = this->m_pendingEmitters2.Count(); i; i--) {
        this->m_pendingEmitters2[i - 1]->DecRef();
    }

    this->m_pendingEmitters2.m_count = 0;
}

// OFFSET: 0x00B13C60/0x1408D9A10
CParticleEmitter2* ParticleSystemManager::DuplicateEmitter(const CParticleEmitter2* emitter, int32_t copyParticles) {
    if (!emitter) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    CParticleEmitter2* duplicate = const_cast<CParticleEmitter2*>(emitter)->Clone(copyParticles);
    duplicate->AddRef();
    *this->m_emitters2.New() = duplicate;

    return duplicate;
}

// OFFSET: 0x00B13CB0/0x1408D9A70
void RibbonManager::Flush() {
    for (uint32_t i = this->m_emitters.Count(); i; i--) {
        this->m_emitters[i - 1]->DecRef();
    }

    this->m_emitters.m_count = 0;

    for (uint32_t i = this->m_pendingEmitters.Count(); i; i--) {
        this->m_pendingEmitters[i - 1]->DecRef();
    }

    this->m_pendingEmitters.m_count = 0;
}

// OFFSET: 0x00B13D00/0x1408D9AF0
ParticleSystemManager* ParticleSystemManager::GetInstance() {
    if (ParticleSystemManager::s_instance) {
        return ParticleSystemManager::s_instance;
    }

    void* mem = SMemAlloc(sizeof(ParticleSystemManager), __FILE__, __LINE__, 0);
    ParticleSystemManager::s_instance = mem ? new (mem) ParticleSystemManager : NULL;

    CRndSeed seed((rand() << 16) | static_cast<uint16_t>(rand()));

    for (uint32_t i = 0; i < 128; i++) {
        CParticleEmitter2::s_rndTable[i] = CRandom::ufloat_(seed);
    }

    CParticleEmitter2::Init();

    return ParticleSystemManager::s_instance;
}

// OFFSET: 0x00B13DC0
ParticleSystemManager::~ParticleSystemManager() {
    this->Flush();
}

// OFFSET: 0x00B13EA0/0x1408D9BF0
void ParticleSystemManager::Destroy() {
    if (ParticleSystemManager::s_instance) {
        CParticleEmitter2::Destroy();
        delete ParticleSystemManager::s_instance;
    }

    ParticleSystemManager::s_instance = NULL;
}

// OFFSET: 0x00B13ED0/0x1408D9CB0
void RibbonManager::Destroy() {
    delete RibbonManager::s_instance;
    RibbonManager::s_instance = NULL;
}

// OFFSET: 0x00B13E30
RibbonManager::~RibbonManager() {
    this->Flush();
}
