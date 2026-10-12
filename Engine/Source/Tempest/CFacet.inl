#ifndef ENGINE_SOURCE_TEMPEST_CFACET_INL
#define ENGINE_SOURCE_TEMPEST_CFACET_INL

// INLINED
inline CFacet::CFacet(float value) {
    this->plane.n.x = 0.0f;
    this->plane.n.y = 0.0f;
    this->plane.n.z = 1.0f;
    this->plane.d = 0.0f;
    this->Set(value);
}

#endif
