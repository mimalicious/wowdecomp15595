// INLINED
inline CModelRecord::CModelRecord() {
    this->m_model = NULL;
    this->m_distance = std::numeric_limits<float>::infinity();
    this->m_guid = WOWGUID();
    this->m_object = NULL;
}

// INLINED
inline CModelRecord::~CModelRecord() {
    if (this->m_model) {
        this->m_model->Release();
    }
}
