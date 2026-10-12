#include <cstdint>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Storm/H/storm.h"

// INLINED
CHandleObject::~CHandleObject() {
}

// INLINED
const char* CHandleObject::GetObjectName() {
    return NULL;
}

// OFFSET: 0x008722E0/0x1405A8700
void HandleClose(HOBJECT handle) {
    CHandleObject* object = HandleDereference(handle);

    if (object->m_refcount-- == 1) {
        delete object;
    }
}

// OFFSET: 0x00872300/0x1405A8720
HOBJECT HandleCreate(CHandleObject* ptr, const char* handleName) {
    if (!ptr) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    ptr->m_refcount++;
    return reinterpret_cast<HOBJECT>(ptr);
}

// INLINED
CHandleObject* HandleDereference(HOBJECT handle) {
    return reinterpret_cast<CHandleObject*>(handle);
}

// OFFSET: 0x00872320
HOBJECT HandleDuplicate(HOBJECT handle) {
    if (handle) {
        HandleDereference(handle)->m_refcount++;
    }

    return handle;
}

void HandleDestroy() {
}

void HandleInitialize() {
}
