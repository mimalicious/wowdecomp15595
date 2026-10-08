// INLINED
inline void NetClient::SetMessageHandler(NETMESSAGE msgId, MESSAGE_HANDLER handler, void* param) {
    uint32_t index = (msgId & 3) | (((msgId & 0x30) | (((msgId & 0xF00) | ((msgId >> 1) & 0x3000)) >> 2)) >> 2);
    uint32_t key = (msgId | (msgId << 16)) ^ 0x62A3A31D;
    this->m_handlers[index] = reinterpret_cast<MESSAGE_HANDLER>(reinterpret_cast<uintptr_t>(handler) + key);
    this->m_handlerParams[index] = param;
}

// INLINED
inline void NetClient::ResetHandler(NETMESSAGE msgId) {
    uint32_t index = (msgId & 3) | (((msgId & 0x30) | (((msgId & 0xF00) | ((msgId >> 1) & 0x3000)) >> 2)) >> 2);
    this->m_handlers[index] = NULL;
    this->m_handlerParams[index] = NULL;
}
