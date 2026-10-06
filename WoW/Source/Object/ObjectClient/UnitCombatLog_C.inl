// INLINED
inline CombatLogFilter::CombatLogFilter() {
    this->m_events.m_count = 50;
    this->m_events.m_bits.SetCount(((50 - 1) >> 5) + 1);
}
