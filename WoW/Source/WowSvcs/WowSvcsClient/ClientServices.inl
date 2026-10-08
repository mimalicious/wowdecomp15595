// INLINED
inline void ClientServices::SetMessageHandler(NETMESSAGE msgId, MESSAGE_HANDLER handler, void* param) {
    ClientServices::s_currentConnection->SetMessageHandler(msgId, handler, param);
}

// INLINED
inline void ClientServices::ClearMessageHandler(NETMESSAGE msgId) {
    ClientServices::s_currentConnection->ResetHandler(msgId);
}
