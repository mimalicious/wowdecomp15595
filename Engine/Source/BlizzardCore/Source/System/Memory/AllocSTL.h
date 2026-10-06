#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_MEMORY_ALLOCSTL_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_MEMORY_ALLOCSTL_H

#include <cstddef>
#include <new>
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"

namespace Blizzard {
namespace Memory {

template <class T>
class AllocSTL_raw {
    public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    template <class U>
    struct rebind {
        typedef AllocSTL_raw<U> other;
    };

    AllocSTL_raw();
    template <class U>
    AllocSTL_raw(const AllocSTL_raw<U>& other);

    T* address(T& value) const;
    const T* address(const T& value) const;
    T* allocate(size_t count);
    void deallocate(T* ptr, size_t count);
    size_t max_size() const;
    void construct(T* ptr, const T& value);
    void destroy(T* ptr);
};

template <class T, class U>
bool operator==(const AllocSTL_raw<T>& a, const AllocSTL_raw<U>& b);
template <class T, class U>
bool operator!=(const AllocSTL_raw<T>& a, const AllocSTL_raw<U>& b);

}
}

// INLINED
template <class T>
Blizzard::Memory::AllocSTL_raw<T>::AllocSTL_raw() {
}

// INLINED
template <class T>
template <class U>
Blizzard::Memory::AllocSTL_raw<T>::AllocSTL_raw(const AllocSTL_raw<U>& other) {
}

// INLINED
template <class T>
T* Blizzard::Memory::AllocSTL_raw<T>::address(T& value) const {
    return &value;
}

// INLINED
template <class T>
const T* Blizzard::Memory::AllocSTL_raw<T>::address(const T& value) const {
    return &value;
}

// INLINED
template <class T>
T* Blizzard::Memory::AllocSTL_raw<T>::allocate(size_t count) {
    return static_cast<T*>(Blizzard::Memory::s_allocHook(count * sizeof(T)));
}

// INLINED
template <class T>
void Blizzard::Memory::AllocSTL_raw<T>::deallocate(T* ptr, size_t count) {
    Blizzard::Memory::s_freeHook(ptr, count * sizeof(T));
}

// INLINED
template <class T>
size_t Blizzard::Memory::AllocSTL_raw<T>::max_size() const {
    size_t count = static_cast<size_t>(-1) / sizeof(T);
    return count ? count : 1;
}

// INLINED
template <class T>
void Blizzard::Memory::AllocSTL_raw<T>::construct(T* ptr, const T& value) {
    ::new (static_cast<void*>(ptr)) T(value);
}

// INLINED
template <class T>
void Blizzard::Memory::AllocSTL_raw<T>::destroy(T* ptr) {
    ptr->~T();
}

// INLINED
template <class T, class U>
bool Blizzard::Memory::operator==(const AllocSTL_raw<T>& a, const AllocSTL_raw<U>& b) {
    return true;
}

// INLINED
template <class T, class U>
bool Blizzard::Memory::operator!=(const AllocSTL_raw<T>& a, const AllocSTL_raw<U>& b) {
    return false;
}

#endif
