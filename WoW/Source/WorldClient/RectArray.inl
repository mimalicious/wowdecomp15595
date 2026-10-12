#ifndef WOW_SOURCE_WORLDCLIENT_RECTARRAY_INL
#define WOW_SOURCE_WORLDCLIENT_RECTARRAY_INL

// INLINED
template <class T>
TSRectArray<T>::TSRectArray() {
    this->m_width = 0;
    this->m_height = 0;
}

// OFFSET: [0x0072CFA0<CWorldMap::CMapTableEntry>, 0x0072CF40/0x140475850<CMap::CMapAreaTableEntry>, 0x00797E70<CMapLowDetail::CMapAreaLowTableEntry>]
template <class T>
T* TSRectArray<T>::Get(int32_t x, int32_t y) {
    if (!this->m_width || !this->m_height) {
        return NULL;
    }

    if (x < this->m_rect.minX || x > this->m_rect.maxX || y < this->m_rect.minY || y > this->m_rect.maxY) {
        return NULL;
    }

    return &this->m_data.m_data[x + this->m_width * (y - this->m_rect.minY) - this->m_rect.minX];
}

// OFFSET: [0x0072D880<CWorldMap::CMapTableEntry>, 0x00798200<CMapLowDetail::CMapAreaLowTableEntry>]
template <class T>
void TSRectArray<T>::SetRect(const CiRect& rect) {
    this->m_rect = rect;

    if (this->m_rect.minX <= this->m_rect.maxX && this->m_rect.minY <= this->m_rect.maxY) {
        int32_t width = this->m_rect.maxX - this->m_rect.minX + 1;
        int32_t height = this->m_rect.maxY - this->m_rect.minY + 1;
        this->m_height = height;
        this->m_width = width;
        this->m_data.SetCount(width * height);
    }
}

#endif
