#ifndef ENGINE_SOURCE_IMAGES_STATICIMAGE_INL
#define ENGINE_SOURCE_IMAGES_STATICIMAGE_INL

// INLINED
inline uint32_t StaticImage::Width(uint32_t mipLevel) const {
    uint32_t width = this->m_width >> mipLevel;
    return width <= 1 ? 1 : width;
}

// INLINED
inline uint32_t StaticImage::Height(uint32_t mipLevel) const {
    uint32_t height = this->m_height >> mipLevel;
    return height <= 1 ? 1 : height;
}

#endif
