// INLINED
inline Blizzard::Lock::EventObject::EventObject() {
    this->m_event = CreateEventW(NULL, FALSE, FALSE, NULL);
}

// OFFSET: 0x007B05B0
inline Blizzard::Lock::EventObject::~EventObject() {
    CloseHandle(this->m_event);
}
