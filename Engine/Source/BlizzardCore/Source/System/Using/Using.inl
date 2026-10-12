#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_USING_USING_INL
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_USING_USING_INL

// INLINED
inline Blizzard::Using::GlobalUse::GlobalUse() {
    this->m_name = "Blizzard::Using::GlobalUse";
    Blizzard::Using::StartUsing("Blizzard::Using::GlobalUse");
}

// INLINED
inline Blizzard::Using::Use::Use(const char* name) {
    this->m_name = name;
    Blizzard::Using::StartUsing(name);
}

#endif
