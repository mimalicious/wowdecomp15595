// INLINED
inline Battlenet::Service::Interface::Interface(TumorManager& manager) {
    this->m_manager = &manager;
    this->m_eventAdapter = NULL;
    this->m_tendrilId = 0;
}
