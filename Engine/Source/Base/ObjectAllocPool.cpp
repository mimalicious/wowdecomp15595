#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SLock.h"

// STATIC: 0x00D1FD68
static TSExplicitList<ObjectAllocPool, offsetof(ObjectAllocPool, m_link)> s_pools;

// STATIC: 0x00F80494
static uint32_t s_poolCount;

// STATIC: 0x00F8049C
static SCritSect s_poolLock;

// OFFSET: 0x00A3BAE0/0x1407E8C10
void ObjectAllocRegisterPool(ObjectAllocPool* pool) {
    s_poolLock.Enter();
    s_pools.LinkToHead(pool);
    s_poolCount++;
    s_poolLock.Leave();
}

// OFFSET: 0x00A3BB10/0x1407E8C50
void ObjectAllocUnregisterPool(ObjectAllocPool* pool) {
    s_poolLock.Enter();
    pool->m_link.Unlink();
    s_poolCount--;
    s_poolLock.Leave();
}
