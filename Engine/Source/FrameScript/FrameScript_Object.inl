inline FrameScript_Object::ScriptIx::ScriptIx() : m_index(0xFF) {
}

// OFFSET: %0x0046E7E0
inline bool FrameScript_Object::HasScript(const FrameScript_Object::ScriptIx& script) {
    return script.m_index != 0xFF && this->m_scripts[script.m_index].m_luaRef;
}
