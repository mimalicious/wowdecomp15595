// PROVEN: 140401a9 IDENTICAL
// OFFSET: 0x004C9270
inline C3Vector& C3Spline::Point(uint32_t index) {
    if (index < 25) {
        return this->m_points[index];
    }

    return this->m_extraPoints[index - 25];
}

// INLINED
inline float C3Spline::GetCachedSegLength(uint32_t index) {
    if (index < 25) {
        return this->m_segLength[index];
    }

    return this->m_extraSegLength[index - 25];
}

// INLINED
inline void C3Spline::SetCachedSegLength(float length, uint32_t index) {
    if (index < 25) {
        this->m_segLength[index] = length;
    } else {
        this->m_extraSegLength[index - 25] = length;
    }
}
