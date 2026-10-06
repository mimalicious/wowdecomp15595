// OFFSET: #0x004079C0
inline C2Vector::C2Vector() {
    this->x = 0.0f;
    this->y = 0.0f;
}

// INLINED
inline C2Vector::C2Vector(float x, float y) {
    this->x = x;
    this->y = y;
}
