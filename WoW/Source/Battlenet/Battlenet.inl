// INLINED
inline Battlenet::Allocator::~Allocator() {
}

// INLINED
inline Battlenet::Event::Header::Header() {
    this->m_sourceAddress.m_exists = false;
    this->m_sourceAddress.m_value = 0;
    this->m_sourceName.m_value = static_cast<uint32_t>(-1);
    this->m_destination.m_tag = 0;
}

// INLINED
inline Battlenet::EventSubscriber::EventSubscriber() {
    this->m_eventId.m_value = 0;
}

// OFFSET: 0x006605D0
inline Battlenet::SystemClient::~SystemClient() {
}

// OFFSET: #0x008C50E0
inline Battlenet::SystemServer::~SystemServer() {
}

// OFFSET: 0x00627380
inline Battlenet::EventSubscriber::~EventSubscriber() {
}

// OFFSET: 0x006615F0
inline Battlenet::EventInterface::~EventInterface() {
}
