#ifndef ENGINE_SOURCE_TEMPEST_CAASPHERE_INL
#define ENGINE_SOURCE_TEMPEST_CAASPHERE_INL

// INLINED
inline CAaSphere::CAaSphere() {
    this->r = 0.0f;
}

// INLINED
inline CAaBox CAaSphere::Box() const {
    CAaBox result;
    result.b.x = this->c.x - this->r;
    result.b.y = this->c.y - this->r;
    result.b.z = this->c.z - this->r;
    result.t.x = this->c.x + this->r;
    result.t.y = this->c.y + this->r;
    result.t.z = this->c.z + this->r;
    return result;
}

#endif
