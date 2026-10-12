#include "Engine/Source/Base/Base.h"
#include "Engine/Source/Base/FileCache.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Prop.h"
#include "Engine/Source/Base/RCString.h"

void BaseDestroyContext() {
    HandleDestroy();
}

// OFFSET: 0x00871FD0/0x1405A82D0
void BaseDestroyGlobal() {
    PropDestroy();
    CStringManager::DestroyManager();
}

// OFFSET: 0x00871FE0/0x140581DE0
void BaseInitializeContext() {
    HandleInitialize();
}

// OFFSET: 0x00871FC0/0x1405A82C0
void BaseInitializeGlobal() {
    PropInitialize();
}
