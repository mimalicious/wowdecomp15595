#include <cstdint>
#include "Engine/Source/Base/FileCache.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SLock.h"

static TSHashTable<PrefetchNode, HASHKEY_STRI> s_activeFiles;

static TSHashTable<UncachableNode, HASHKEY_STRI> s_uncachableFiles;

static SCritSect s_critSect;

static void STORMAPI IBaseFileWaitForLoad(SOVERLAPPED* overlapped) {
}

// INLINED
PrefetchNode::PrefetchNode() {
    this->buffer = NULL;
    this->refCount = 0;
    SFile::CreateOverlapped(&this->overlapped);
}

// INLINED
PrefetchNode::~PrefetchNode() {
    if (this->buffer) {
        IBaseFileWaitForLoad(&this->overlapped);
        SMemFree(this->buffer, __FILE__, __LINE__, 0);
    }

    SFile::DestroyOverlapped(&this->overlapped);
}

void BaseFileInitialize() {
}

// INLINED
void BaseFileDestroy() {
    s_critSect.Enter();
    s_activeFiles.Clear();
    s_uncachableFiles.Clear();
    s_critSect.Leave();
}
