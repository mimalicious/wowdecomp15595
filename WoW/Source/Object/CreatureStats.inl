// INLINED
inline CreatureStats::~CreatureStats() {
    for (uint32_t i = 0; i < 4; i++) {
        if (this->m_name[i]) {
            SMemFree(this->m_name[i], __FILE__, __LINE__, 0);
        }
    }

    if (this->m_subName) {
        SMemFree(this->m_subName, __FILE__, __LINE__, 0);
    }

    if (this->m_iconName) {
        SMemFree(this->m_iconName, __FILE__, __LINE__, 0);
    }
}
