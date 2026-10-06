// INLINED
inline EvtContextQueue::EvtContextQueue() : TSPriorityQueue<EvtContext>(offsetof(EvtContext, m_schedNextWakeTime)) {
}
