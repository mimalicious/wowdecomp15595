#include "Engine/Source/Base/Base.h"
#include "Engine/Source/Base/FileCache.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Prop.h"
#include "Engine/Source/Base/RCString.h"

void BaseDestroyContext() {
    HandleDestroy();
}

// PROVEN: 307ef98b IDENTICAL/NONE
// OFFSET: 0x00871FD0/0x1405A82D0
void BaseDestroyGlobal() {
    PropDestroy();
    CStringManager::DestroyManager();
}

// PROVEN: 8e59252c IDENTICAL/DIFFERENT
// OFFSET: 0x00871FE0/0x140581DE0
void BaseInitializeContext() {
    HandleInitialize();
}

// PROVEN: b5e0be3a IDENTICAL/NONE
// OFFSET: 0x00871FC0
void BaseInitializeGlobal() {
    PropInitialize();
}
