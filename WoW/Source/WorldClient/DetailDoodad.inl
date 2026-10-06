// INLINED
inline CDetailDoodadInstAdd::CDetailDoodadInstAdd() {
}

// INLINED
inline void CDetailDoodadInstBatch::FreeBuffers() {
    if (!this->m_vertexBuf) {
        return;
    }

    if (this->m_vertexBuf->m_pool == CDetailDoodad::s_gxVertexPool) {
        *CDetailDoodad::s_freeGxBufs.New() = this->m_vertexBuf;
        *CDetailDoodad::s_freeGxBufs.New() = this->m_indexBuf;
    }

    this->m_indexBuf = NULL;
    this->m_vertexBuf = NULL;
}

// INLINED
inline void CDetailDoodadInstBatch::Free() {
    this->m_texture = NULL;
    this->m_vertexCount = 0;
    this->m_indexCount = 0;
    this->m_adds.SetCount(0);
    this->m_adds.m_chunk = 64;
    this->FreeBuffers();
}

// INLINED
inline CDetailDoodadInstBatch::~CDetailDoodadInstBatch() {
    this->Free();
}
