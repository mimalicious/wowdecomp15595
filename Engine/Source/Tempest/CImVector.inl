// INLINED
inline CImVector::CImVector() {
    *reinterpret_cast<uint32_t*>(this) = 0;
}

// INLINED
inline CImVector::CImVector(uint32_t value) {
    *reinterpret_cast<uint32_t*>(this) = value;
}

// INLINED
inline CImVector::CImVector(uint8_t b, uint8_t g, uint8_t r, uint8_t a) {
    this->b = b;
    this->g = g;
    this->r = r;
    this->a = a;
}
