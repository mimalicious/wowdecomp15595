#ifndef ENGINE_SOURCE_SERVICES_PARTICLESYSTEM2_INL
#define ENGINE_SOURCE_SERVICES_PARTICLESYSTEM2_INL

// INLINED
inline bool operator<(const CSortableParticleRecord& l, const CSortableParticleRecord& r) {
    return l.m_depth < r.m_depth;
}

#endif
