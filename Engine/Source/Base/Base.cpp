#include "Engine/Source/Base/Base.h"
#include "Engine/Source/Base/FileCache.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Prop.h"
#include "Engine/Source/Base/RCString.h"

void BaseDestroyContext() {
    HandleDestroy();
}

// OFFSET: #0x0047B750
void BaseDestroyGlobal() {
    BaseFileDestroy();
    PropDestroy();
    CStringManager::DestroyManager();
}

// OFFSET: 0x00871FE0/0x140581DE0
void BaseInitializeContext() {
    HandleInitialize();
}

// OFFSET: %0x007D6970
void BaseInitializeGlobal() {
    PropInitialize();
    BaseFileInitialize();
}
