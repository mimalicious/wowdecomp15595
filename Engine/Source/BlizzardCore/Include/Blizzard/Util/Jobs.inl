#ifndef ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_JOBS_INL
#define ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_JOBS_INL

// INLINED
template <class T>
inline Blizzard::Jobs::RefCounted<T>::RefCounted() {
    this->m_refCount = 0;
}

// OFFSET: %0x00979550<Blizzard::Jobs::Job>
template <class T>
inline Blizzard::Jobs::RefCounted<T>::~RefCounted() {
}

// INLINED
template <class T>
inline void Blizzard::Jobs::RefCounted<T>::AddRef() {
    ++this->m_refCount;
}

// INLINED
template <class T>
inline void Blizzard::Jobs::RefCounted<T>::Release() {
    if (!--this->m_refCount) {
        delete this;
    }
}

// INLINED
template <class T>
inline Blizzard::Jobs::RefPtr<T>::RefPtr() {
    this->m_ptr = NULL;
}

// INLINED
template <class T>
inline Blizzard::Jobs::RefPtr<T>::RefPtr(T* ptr) {
    this->m_ptr = ptr;

    if (ptr) {
        ptr->AddRef();
    }
}

// INLINED
template <class T>
inline Blizzard::Jobs::RefPtr<T>::RefPtr(const RefPtr& other) {
    this->m_ptr = other.m_ptr;

    if (this->m_ptr) {
        this->m_ptr->AddRef();
    }
}

// INLINED
template <class T>
inline Blizzard::Jobs::RefPtr<T>::~RefPtr() {
    if (this->m_ptr) {
        this->m_ptr->Release();
    }
}

// INLINED
template <class T>
inline Blizzard::Jobs::RefPtr<T>& Blizzard::Jobs::RefPtr<T>::operator=(const RefPtr& other) {
    T* ptr = other.m_ptr;

    if (ptr) {
        ptr->AddRef();
    }

    if (this->m_ptr) {
        this->m_ptr->Release();
    }

    this->m_ptr = ptr;

    return *this;
}

// INLINED
template <class T>
inline T* Blizzard::Jobs::RefPtr<T>::operator->() const {
    return this->m_ptr;
}

// INLINED
template <class T>
inline T* Blizzard::Jobs::RefPtr<T>::Get() const {
    return this->m_ptr;
}

#endif
