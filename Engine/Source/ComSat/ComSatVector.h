#ifndef ENGINE_SOURCE_COMSAT_COMSATVECTOR_H
#define ENGINE_SOURCE_COMSAT_COMSATVECTOR_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include "Engine/Source/ComSat/ComSatInterface.h"

namespace ComSat {

template <class T>
class Vector {
    public:
    /* 0000 */ T* m_data;
    /* 0004 */ size_t m_used;
    /* 0008 */ size_t m_capacity;
    /* 000C */ Allocator* m_allocator;

    Vector(Allocator& allocator);
    ~Vector();
    bool Find(const T& value, uint32_t& index) const;
    bool Reserve(uint32_t size);
    T* PushBack(const T& value);
    bool Remove(const T& value);
    void RemoveAt(uint32_t offset);
    uint32_t Count() const;
    T& operator[](uint32_t index);
};

}

// INLINED
template <class T>
ComSat::Vector<T>::Vector(ComSat::Allocator& allocator) {
    this->m_data = NULL;
    this->m_used = 0;
    this->m_capacity = 0;
    this->m_allocator = &allocator;
}

// INLINED
template <class T>
ComSat::Vector<T>::~Vector() {
    T* data = this->m_data;
    this->m_used = 0;
    if (data) {
        this->m_allocator->Free(data);
    }
}

// OFFSET: [0x00A33BF0<unsigned __int64>, 0x00A39590<ComSat::Network::Session::LocalTalker>, 0x00A38950<ComSat::Network::Endpoint*>]
template <class T>
bool ComSat::Vector<T>::Find(const T& value, uint32_t& index) const {
    index = 0;
    if (!this->m_used) {
        return false;
    }
    while (!(this->m_data[index] == value)) {
        if (++index >= this->m_used) {
            return false;
        }
    }
    return true;
}

// OFFSET: [0x00A33C40<unsigned __int64>, 0x00A38990/0x1407DE1E0<ComSat::Network::Endpoint*>, 0x00A395F0/0x1407E5950<ComSat::Network::Session::LocalTalker>]
template <class T>
bool ComSat::Vector<T>::Reserve(uint32_t size) {
    if (size < this->m_used) {
        return false;
    }
    T* data = static_cast<T*>(this->m_allocator->Alloc(sizeof(T) * size));
    if (this->m_data) {
        memcpy(data, this->m_data, sizeof(T) * this->m_used);
        this->m_allocator->Free(this->m_data);
    }
    this->m_data = data;
    this->m_capacity = size;
    return true;
}

// OFFSET: [0x00A34430<unsigned __int64>, 0x00A34470<ComSat::Session*>]
template <class T>
T* ComSat::Vector<T>::PushBack(const T& value) {
    if (this->m_used == this->m_capacity) {
        this->Reserve(this->m_capacity + 10);
    }
    T* element = new (&this->m_data[this->m_used]) T(value);
    this->m_used++;
    return element;
}

// OFFSET: [0x00A34060<unsigned __int64>, 0x00A39880<ComSat::Network::Session::LocalTalker>, 0x00A398D0<ComSat::Network::Endpoint*>]
template <class T>
bool ComSat::Vector<T>::Remove(const T& value) {
    uint32_t index;
    if (!this->Find(value, index)) {
        return false;
    }
    this->RemoveAt(index);
    return true;
}

// INLINED
template <class T>
void ComSat::Vector<T>::RemoveAt(uint32_t offset) {
    uint32_t remaining = this->m_used - offset;
    if (remaining > 1) {
        memmove(&this->m_data[offset], &this->m_data[offset + 1], sizeof(T) * (remaining - 1));
    }
    this->m_used--;
}

// INLINED
template <class T>
uint32_t ComSat::Vector<T>::Count() const {
    return this->m_used;
}

// INLINED
template <class T>
T& ComSat::Vector<T>::operator[](uint32_t index) {
    return this->m_data[index];
}

#endif
