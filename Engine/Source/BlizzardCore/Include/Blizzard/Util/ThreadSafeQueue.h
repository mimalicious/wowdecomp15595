#ifndef ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_THREADSAFEQUEUE_H
#define ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_THREADSAFEQUEUE_H

#include <cstdint>
#include <deque>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

namespace Blizzard {
namespace Util {

template <class T>
class ThreadSafeQueue {
    public:
    /* 0004 */ Blizzard::Lock::Mutex m_mutex;
    /* 001C */ std::deque<T> m_queue;

    ThreadSafeQueue();
    virtual ~ThreadSafeQueue();
    virtual void Push(const T& value);
    virtual T& Front();
    virtual bool Pop(T& value);
    virtual bool Pop();
    bool PopFront(T& value);
    uint32_t Size() const;
};

}
}

// INLINED
template <class T>
Blizzard::Util::ThreadSafeQueue<T>::ThreadSafeQueue() {
    Blizzard::Lock::MutexCreate(this->m_mutex);
}

// OFFSET: [0x007CF880<Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job>>, 0x007CF920<Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job>>, 0x007C7780<Blizzard::Jobs::Job*>]
template <class T>
Blizzard::Util::ThreadSafeQueue<T>::~ThreadSafeQueue() {
    Blizzard::Lock::MutexDestroy(this->m_mutex);
}

// OFFSET: [0x007DB550<Blizzard::Jobs::Job*>, 0x007DB580<Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job>>]
template <class T>
void Blizzard::Util::ThreadSafeQueue<T>::Push(const T& value) {
    Blizzard::Lock::MutexEnter(this->m_mutex);
    this->m_queue.push_back(value);
    Blizzard::Lock::MutexLeave(this->m_mutex);
}

// OFFSET: [0x007CA890/0x1404CCB10<Blizzard::Jobs::Job*>, 0x007CA890/0x1404CCB10<Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job>>]
template <class T>
T& Blizzard::Util::ThreadSafeQueue<T>::Front() {
    Blizzard::Lock::MutexEnter(this->m_mutex);
    T& value = this->m_queue.front();
    Blizzard::Lock::MutexLeave(this->m_mutex);
    return value;
}

// OFFSET: [0x007C75F0<Blizzard::Jobs::Job*>, 0x007CF810<Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job>>]
template <class T>
bool Blizzard::Util::ThreadSafeQueue<T>::Pop() {
    Blizzard::Lock::MutexEnter(this->m_mutex);

    if (this->m_queue.size()) {
        this->m_queue.pop_front();
        Blizzard::Lock::MutexLeave(this->m_mutex);
        return true;
    }

    Blizzard::Lock::MutexLeave(this->m_mutex);
    return false;
}

// OFFSET: [0x007CA8E0<Blizzard::Jobs::Job*>, 0x007CF850<Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job>>]
template <class T>
bool Blizzard::Util::ThreadSafeQueue<T>::Pop(T& value) {
    Blizzard::Lock::MutexEnter(this->m_mutex);
    bool result = this->PopFront(value);
    Blizzard::Lock::MutexLeave(this->m_mutex);
    return result;
}

// OFFSET: [0x007C77D0<Blizzard::Jobs::Job*>, 0x007CF790<Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job>>]
template <class T>
bool Blizzard::Util::ThreadSafeQueue<T>::PopFront(T& value) {
    if (!this->m_queue.size()) {
        return false;
    }

    value = this->m_queue.front();
    this->m_queue.pop_front();

    return true;
}

// INLINED
template <class T>
uint32_t Blizzard::Util::ThreadSafeQueue<T>::Size() const {
    return static_cast<uint32_t>(this->m_queue.size());
}

#endif
