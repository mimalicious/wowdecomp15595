// INLINED
inline Battlenet::Tumor::EventBinding::EventBinding() {
    this->m_subscriber = NULL;
    this->m_active = false;
    this->m_tendrilId = 0;
}

// INLINED
inline Battlenet::Tumor::ConnectionBinding::ConnectionBinding() {
    this->m_connection = 0;
    this->m_client = NULL;
    this->m_active = false;
    this->m_tendrilId = 0;
}

// INLINED
inline Battlenet::Tumor::TokenBinding::TokenBinding() {
    this->m_token = 0;
    this->m_request = 0;
    this->m_client = NULL;
    this->m_active = false;
}
