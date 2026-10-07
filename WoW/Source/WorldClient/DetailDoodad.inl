// INLINED
inline CDetailDoodadData::CDetailDoodadData() {
    this->m_model = NULL;
    this->m_texture = NULL;
    this->m_record = NULL;
    this->m_windScale = 0.0f;
}

// INLINED
inline CDetailDoodadInstBatch::CDetailDoodadInstBatch() {
    this->m_id = 0xFFFF;
    this->m_texture = NULL;
    this->m_count = 0;
    this->m_vertexCount = 0;
    this->m_indexCount = 0;
    this->m_vertexBuf = NULL;
    this->m_indexBuf = NULL;
    this->m_instanceBuf = NULL;
    this->m_chunk = NULL;
    this->m_material = NULL;
}

// INLINED
inline CDetailDoodadInst::CDetailDoodadInst() {
    this->unk_0010 = 0;
    this->m_chunk = NULL;
}
