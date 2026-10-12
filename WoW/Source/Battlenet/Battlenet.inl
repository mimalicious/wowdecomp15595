// OFFSET: 0x00627E10/0x1402B9E50
inline Battlenet::Allocator::~Allocator() {
}

// OFFSET: 0x006271B0
inline void* Battlenet::Allocator::Alloc(uint32_t size, const char* name) {
    return this->Alloc(size);
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

// INLINED
inline Battlenet::SystemServer::~SystemServer() {
}

// OFFSET: 0x00627380/0x1402B93C0
inline Battlenet::EventSubscriber::~EventSubscriber() {
}

// OFFSET: 0x006615F0
inline Battlenet::EventInterface::~EventInterface() {
}
