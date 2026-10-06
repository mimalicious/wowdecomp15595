#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/ParticleSystem.h"
#include "Engine/Source/Storm/H/storm.h"

// INLINED
CParticle::~CParticle() {
    this->m_age = 0.0f;
}

// OFFSET: %0x00A5FC60
CParticleEmitter::~CParticleEmitter() {
}

// OFFSET: %0x00A5FCD0
void CParticleEmitter::DecRef() {
    if (this->m_refCount-- == 1) {
        delete this;
    }
}
