#ifndef ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_JOBS_H
#define ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_JOBS_H

#include <cstddef>
#include <cstdint>

namespace Blizzard {
namespace Jobs {

template <class T>
class RefCounted {
    public:
    /* 0004 */ int32_t m_refCount;

    RefCounted();
    virtual ~RefCounted();
    void AddRef();
    void Release();
};

template <class T>
class RefPtr {
    public:
    /* 0000 */ T* m_ptr;

    RefPtr();
    RefPtr(T* ptr);
    RefPtr(const RefPtr& other);
    ~RefPtr();
    RefPtr& operator=(const RefPtr& other);
    T* operator->() const;
    T* Get() const;
};

class Job : public RefCounted<Job> {
    public:
    /* 0008 */ int32_t m_dependencyCount;
    /* 000C */ RefPtr<Job> m_dependencies[4];
    /* 001C */ RefPtr<Job> unk_001C;
    /* 0020 */ RefPtr<Job> unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ bool m_result;
    /* 0029 */ bool m_cancelled;

    Job(int32_t dependencyCount);
    virtual ~Job();
    virtual bool Execute() = 0;
    virtual uint32_t GetClassId() const = 0;
    virtual bool IsReady() const;
};

}
}

#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Jobs.inl"

#endif
