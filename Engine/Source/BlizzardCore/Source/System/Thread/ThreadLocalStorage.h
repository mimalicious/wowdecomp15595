#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_THREAD_THREADLOCALSTORAGE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_THREAD_THREADLOCALSTORAGE_H

#include <cstdint>

namespace Blizzard {
namespace Thread {

typedef void (*TLSDestructor)(void*);

struct TLSSlot {
    /* 0000 */ uint32_t m_key;
    /* 0004 */ TLSDestructor m_destructor;
};

template <class T>
class LocalObject {
    public:
    static void Delete(void* ptr);
    static void* New(void* param);

    /* 0000 */ TLSSlot m_slot;

    operator T&();
};

void* RegisterLocalStorage(TLSSlot* slot, void* (*constructor)(void*), void* param, TLSDestructor destructor);

}
}

namespace System_Thread {

extern Blizzard::Thread::TLSSlot* s_slotList[128];
extern uint32_t s_slotListUsed;

bool AllocateLocalStorage(Blizzard::Thread::TLSSlot* slot, Blizzard::Thread::TLSDestructor destructor);
bool AllocateTLSSlot(Blizzard::Thread::TLSSlot* slot, Blizzard::Thread::TLSDestructor destructor);
void InternalSetLocalStorage(const Blizzard::Thread::TLSSlot* slot, const void* value);
void RunTLSDestructors();

}

// OFFSET: [0x007AEBE0/0x14049AC10<System_SFile::RawReadInfo>, 0x007C0ED0<System_File::FileError>, 0x007C58D0<std::string>]
template <class T>
void Blizzard::Thread::LocalObject<T>::Delete(void* ptr) {
    delete static_cast<T*>(ptr);
}

// OFFSET: 0x007A5DB0/0x14049ABE0<System_SFile2::SFileFatalInfo>
template <class T>
void* Blizzard::Thread::LocalObject<T>::New(void* param) {
    return new T;
}

// OFFSET: [0x007A6210<System_SFile2::SFileFatalInfo>, 0x007E3990<System_SFile::RawReadInfo>, 0x007E3A00<std::string>, 0x007E3B80<System_File::FileError>, 0x007E4150<System_SComp::ZlibAllocBuffer>]
template <class T>
Blizzard::Thread::LocalObject<T>::operator T&() {
    return *static_cast<T*>(RegisterLocalStorage(&this->m_slot, New, NULL, Delete));
}

#endif
