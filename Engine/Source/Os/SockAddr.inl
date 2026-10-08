// INLINED
inline SockAddr::~SockAddr() {
    if (this->m_type == SOCKADDR_HOSTNAME && this->m_hostname) {
        SMemFree(this->m_hostname, __FILE__, __LINE__, 0);
    }
}
