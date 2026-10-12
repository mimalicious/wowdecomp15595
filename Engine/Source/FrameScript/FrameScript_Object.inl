inline FrameScript_Object::ScriptIx::ScriptIx() : m_index(0xFF) {
}

// OFFSET: 0x00464E20
inline bool FrameScript_Object::HasScript(const FrameScript_Object::ScriptIx& script) {
    return script.m_index != 0xFF && this->m_scripts[script.m_index].m_luaRef;
}
