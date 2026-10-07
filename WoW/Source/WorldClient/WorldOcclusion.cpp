#include "WoW/Source/WorldClient/WorldOcclusion.h"
#include <cstddef>
#include <new>
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00D02F20
static TSExplicitList<CWorldOccluder, offsetof(CWorldOccluder, m_link)> s_freeOccluders;

// STATIC: 0x00D02F2C
static TSExplicitList<CWorldAntiOccluder, offsetof(CWorldAntiOccluder, m_link)> s_freeAntiOccluders;

// PROVEN: f52cc1f6 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CWorldOccluder::Initialize() {
}

// OFFSET: 0x00780F20/0x14046E2F0
void CWorldOccluder::Free() {
    this->m_link.Unlink();
    s_freeOccluders.LinkToTail(this);
}

// OFFSET: 0x00780F70/0x14046E3C0
CWorldOccluder* CWorldOccluder::Alloc() {
    CWorldOccluder* occluder = s_freeOccluders.Head();

    if (!occluder) {
        void* mem = SMemAlloc(sizeof(CWorldOccluder), __FILE__, __LINE__, 0x8);
        occluder = mem ? new (mem) CWorldOccluder() : NULL;
        s_freeOccluders.LinkToTail(occluder);
    }

    occluder->m_link.Unlink();
    return occluder;
}

// OFFSET: 0x00781150
void CWorldOccluder::Destroy() {
    CWorldOccluder* occluder;

    while ((occluder = s_freeOccluders.Head()) != NULL) {
        occluder->m_link.Unlink();
        SMemFree(occluder, __FILE__, __LINE__, 0);
    }

    CWorldAntiOccluder* antiOccluder;

    while ((antiOccluder = s_freeAntiOccluders.Head()) != NULL) {
        antiOccluder->m_link.Unlink();
        SMemFree(antiOccluder, __FILE__, __LINE__, 0);
    }
}
