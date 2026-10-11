#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/ParticleSystem.h"
#include "Engine/Source/Storm/H/storm.h"

// INLINED
CParticle::~CParticle() {
    this->m_age = 0.0f;
}

// INLINED
CParticleEmitter::~CParticleEmitter() {
}

// INLINED
void CParticleEmitter::DecRef() {
    if (this->m_refCount-- == 1) {
        delete this;
    }
}
