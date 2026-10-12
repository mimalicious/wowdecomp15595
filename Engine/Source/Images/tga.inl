#ifndef ENGINE_SOURCE_IMAGES_TGA_INL
#define ENGINE_SOURCE_IMAGES_TGA_INL

// INLINED
inline CTgaFile::CTgaFile() {
    this->m_file = NULL;
    this->m_image = NULL;
    this->m_header = NULL;
    this->m_addlHeaderData = NULL;
    this->m_imageBytes = 0;
    this->m_colorMap = NULL;
    this->m_fileData = NULL;
    this->m_data = NULL;
}

#endif
