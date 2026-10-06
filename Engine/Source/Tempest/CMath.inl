// OFFSET: %0x00447E00
inline float CMath::cos_(float x) {
    float frac;
    int32_t whole;
    CMath::split_(x * 0.31830987f, &frac, &whole);

    float result = 1.0f - frac * ((6.0f - 4.0f * frac) * frac);

    return (whole & 1) ? -result : result;
}

// INLINED
inline int32_t CMath::fint_(float f) {
    return static_cast<int32_t>(lrintf(f));
}

// INLINED
inline int32_t CMath::ftol_round_0_255_(float f) {
    return static_cast<int32_t>(f * 255.0f + 0.5f);
}

// INLINED
inline CSoftFloat::CSoftFloat() {
}

// INLINED
inline CSoftFloat::CSoftFloat(uint32_t bits) {
    this->m_bits = bits;
}

// INLINED
inline CSoftFloat::~CSoftFloat() {
}

// INLINED
inline void CMath::fsincos_(float x, float* s, float* c) {
#if defined(_M_IX86)
    __asm {
        mov eax, c
        mov edx, s
        fld x
        fsincos
        fstp dword ptr [eax]
        fstp dword ptr [edx]
    }
#else
    *c = cosf(x);
    *s = sinf(x);
#endif
}

// OFFSET: %0x0041F1C0
inline bool fequal(float a, float b) {
    return fabsf(a - b) < 0.00000023841858f;
}

// OFFSET: %0x004CD070
inline bool CMath::fequal(float a, float b) {
    return fabsf(a - b) < 0.00000095367432f;
}
