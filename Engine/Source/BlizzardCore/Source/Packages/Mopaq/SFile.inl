#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILE_INL
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILE_INL

// INLINED
inline System_SFile::ListFile::ListFile() {
    this->m_blocks = NULL;
}

// INLINED
inline System_SFile::ListFile::~ListFile() {
    FreeBlocks(this->m_blocks);
}

#endif
