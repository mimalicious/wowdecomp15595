#ifndef ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_SCOPEGUARD_H
#define ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_SCOPEGUARD_H

namespace Blizzard {
namespace Util {

class ScopeGuardImplBase {
    public:
    ScopeGuardImplBase();
    void Dismiss() const;

    protected:
    template <typename J>
    static void SafeExecute(J& j);

    ScopeGuardImplBase(const ScopeGuardImplBase& other);
    ~ScopeGuardImplBase();

    /* 0000 */ mutable bool m_dismissed;

    private:
    ScopeGuardImplBase& operator=(const ScopeGuardImplBase&);
};

typedef const ScopeGuardImplBase& ScopeGuard;

template <class Obj, typename MemFun, typename P1>
class ObjScopeGuardImpl1 : public ScopeGuardImplBase {
    public:
    static ObjScopeGuardImpl1<Obj, MemFun, P1> MakeObjGuard(Obj& obj, MemFun memFun, P1 p1);

    ~ObjScopeGuardImpl1();
    void Execute();

    protected:
    ObjScopeGuardImpl1(Obj& obj, MemFun memFun, P1 p1);

    /* 0004 */ Obj& m_obj;
    /* 0008 */ MemFun m_memFun;
    /* 000C */ const P1 m_p1;
};

template <class Obj, typename MemFun, typename P1>
ObjScopeGuardImpl1<Obj, MemFun, P1> MakeObjGuard(Obj& obj, MemFun memFun, P1 p1);

}
}

#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/ScopeGuard.inl"

#endif
