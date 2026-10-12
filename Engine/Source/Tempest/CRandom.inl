// INLINED
inline CRndSeed::CRndSeed() {
}

// INLINED
inline float __stdcall CRandom::ufloat_(float range, CRndSeed& seed) {
    return CRandom::ufloat_(seed) * range;
}
