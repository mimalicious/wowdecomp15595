// INLINED
inline JamHandlerKeyOp::JamHandlerKeyOp(JamHandlerKey* left, JamHandlerKey* right) {
    this->m_left = left;
    this->m_right = right;
}

// INLINED
inline JamHandlerKeyMulXor::JamHandlerKeyMulXor(JamHandlerKey* left, JamHandlerKey* right) : JamHandlerKeyOp(left, right) {
}

// INLINED
inline JamHandlerKeySub::JamHandlerKeySub(JamHandlerKey* left, JamHandlerKey* right) : JamHandlerKeyOp(left, right) {
}

// INLINED
inline JamHandlerKeyRol::JamHandlerKeyRol(JamHandlerKey* left, JamHandlerKey* right) : JamHandlerKeyOp(left, right) {
}

// INLINED
inline JamHandlerKeyAdd::JamHandlerKeyAdd(JamHandlerKey* left, JamHandlerKey* right) : JamHandlerKeyOp(left, right) {
}

// INLINED
inline JamHandlerKeyXor::JamHandlerKeyXor(JamHandlerKey* left, JamHandlerKey* right) : JamHandlerKeyOp(left, right) {
}

// PROVEN: 50fb0c01 IDENTICAL/IDENTICAL
// OFFSET: 0x0066A4C0/0x1403125B0
inline uint32_t JamHandlerKeyMulXor::Get() {
    uint32_t right = this->m_right->Get();
    return right ^ ((right & 0xF) * this->m_left->Get());
}

// PROVEN: c9899827 IDENTICAL/IDENTICAL
// OFFSET: 0x0066A520/0x140312620
inline uint32_t JamHandlerKeySub::Get() {
    uint32_t right = this->m_right->Get();
    return this->m_left->Get() - right;
}

// PROVEN: 14aef7f2 IDENTICAL/NONE
// OFFSET: 0x0066A540
inline uint32_t JamHandlerKeyRol::Get() {
    uint32_t right = this->m_right->Get() & 0xF;
    uint32_t left = this->m_left->Get();
    return (left << right) | (left >> (32 - right));
}

// PROVEN: 074c6f01 IDENTICAL/IDENTICAL
// OFFSET: 0x0066A580/0x1403126A0
inline uint32_t JamHandlerKeyAdd::Get() {
    uint32_t right = this->m_right->Get();
    return right + this->m_left->Get();
}

// PROVEN: d036585f IDENTICAL/IDENTICAL
// OFFSET: 0x0066A6A0/0x1403127D0
inline uint32_t JamHandlerKeyXor::Get() {
    uint32_t right = this->m_right->Get();
    return right ^ this->m_left->Get();
}

// INLINED
template <class F, class K>
inline F JamHandlerEncode(F handler, K& key) {
    return reinterpret_cast<F>(reinterpret_cast<uintptr_t>(handler) + key.Get());
}
