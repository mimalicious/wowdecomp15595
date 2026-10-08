// INLINED
inline ThrottleObject::ThrottleObject(uint32_t limit, uint32_t period) {
    this->m_count = limit;
    this->m_startTime = 0;
    this->m_limit = limit;
    this->m_period = period;
}
