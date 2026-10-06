#ifndef ENGINE_SOURCE_STORM_H_TSCARRAY_H
#define ENGINE_SOURCE_STORM_H_TSCARRAY_H

#include <cstddef>
#include <cstdint>

template <class T, uint32_t N>
class TSCArray {
    public:
    /* 0000 */ uint32_t m_count;
    /* 0004 */ T m_data[N];
};

#endif
