// INLINED
inline SockAddr::SockAddr() {
    memset(this->m_ipv6, 0, sizeof(this->m_ipv6));
    this->m_type = SOCKADDR_NONE;
}

// INLINED
inline SockAddr::~SockAddr() {
    if (this->m_type == SOCKADDR_HOSTNAME && this->m_hostname) {
        SMemFree(this->m_hostname, __FILE__, __LINE__, 0);
    }
}

// OFFSET: 0x004031A0
inline void SockAddr::Clear() {
    if (this->m_type == SOCKADDR_HOSTNAME && this->m_hostname) {
        SMemFree(this->m_hostname, __FILE__, __LINE__, 0);
    }

    memset(this->m_ipv6, 0, sizeof(this->m_ipv6));
    this->m_type = SOCKADDR_NONE;
}
