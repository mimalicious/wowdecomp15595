#ifndef ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_SCOPEGUARD_INL
#define ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_SCOPEGUARD_INL

// INLINED
inline Blizzard::Util::ScopeGuardImplBase::ScopeGuardImplBase() : m_dismissed(false) {
}

// INLINED
inline Blizzard::Util::ScopeGuardImplBase::ScopeGuardImplBase(const ScopeGuardImplBase& other) : m_dismissed(other.m_dismissed) {
    other.Dismiss();
}

// INLINED
inline Blizzard::Util::ScopeGuardImplBase::~ScopeGuardImplBase() {
}

// INLINED
inline void Blizzard::Util::ScopeGuardImplBase::Dismiss() const {
    this->m_dismissed = true;
}

// OFFSET: 0x007B7510<Blizzard::Util::ObjScopeGuardImpl1<Blizzard::Storage::StorageSystem, void (Blizzard::Storage::StorageSystem::*)(Blizzard::Storage::BufferHandle), Blizzard::Storage::BufferHandle> >
template <typename J>
void Blizzard::Util::ScopeGuardImplBase::SafeExecute(J& j) {
    if (!j.m_dismissed) {
        try {
            j.Execute();
        } catch (...) {
        }
    }
}

// OFFSET: 0x007BE3C0<Blizzard::Storage::StorageSystem, void (Blizzard::Storage::StorageSystem::*)(Blizzard::Storage::BufferHandle), Blizzard::Storage::BufferHandle>
template <class Obj, typename MemFun, typename P1>
Blizzard::Util::ObjScopeGuardImpl1<Obj, MemFun, P1> Blizzard::Util::ObjScopeGuardImpl1<Obj, MemFun, P1>::MakeObjGuard(Obj& obj, MemFun memFun, P1 p1) {
    return ObjScopeGuardImpl1<Obj, MemFun, P1>(obj, memFun, p1);
}

// INLINED
template <class Obj, typename MemFun, typename P1>
Blizzard::Util::ObjScopeGuardImpl1<Obj, MemFun, P1>::ObjScopeGuardImpl1(Obj& obj, MemFun memFun, P1 p1) : m_obj(obj), m_memFun(memFun), m_p1(p1) {
}

// INLINED
template <class Obj, typename MemFun, typename P1>
Blizzard::Util::ObjScopeGuardImpl1<Obj, MemFun, P1>::~ObjScopeGuardImpl1() {
    SafeExecute(*this);
}

// INLINED
template <class Obj, typename MemFun, typename P1>
void Blizzard::Util::ObjScopeGuardImpl1<Obj, MemFun, P1>::Execute() {
    (this->m_obj.*this->m_memFun)(this->m_p1);
}

// OFFSET: 0x007C1850<Blizzard::Storage::StorageSystem, void (Blizzard::Storage::StorageSystem::*)(Blizzard::Storage::BufferHandle), Blizzard::Storage::BufferHandle>
template <class Obj, typename MemFun, typename P1>
Blizzard::Util::ObjScopeGuardImpl1<Obj, MemFun, P1> Blizzard::Util::MakeObjGuard(Obj& obj, MemFun memFun, P1 p1) {
    return ObjScopeGuardImpl1<Obj, MemFun, P1>::MakeObjGuard(obj, memFun, p1);
}

#endif
