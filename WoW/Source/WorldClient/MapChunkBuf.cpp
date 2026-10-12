#include "WoW/Source/WorldClient/MapChunkBuf.h"
#include <new>
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00D040A0/0x140BB42F0
static TSExplicitList<CMapChunkBufList, offsetof(CMapChunkBufList, m_link)> s_freeLists;

// OFFSET: 0x00796260/0x1404891F0
CMapChunkBuf* CMapChunkBuf::CreateBuf(int32_t vertexCount, int32_t indexCount, int32_t vertexSize) {
    uint32_t vertexBytes = vertexSize * vertexCount;
    CMapChunkBuf* buf = CMap::AllocChunkBuf();

    CGxPool* vertexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Vertex, static_cast<EGxPoolUsage>(World::GetPoolUsage()), vertexSize * vertexCount, GxPoolHintBit_Unk3, "CMapChunkBuf_Vertex");
    CGxPool* indexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Index, static_cast<EGxPoolUsage>(World::GetPoolUsage()), 2 * indexCount, GxPoolHintBit_Unk3, "CMapChunkBuf_Index");
    CGxBuf* vertexBuf = g_theGxDevicePtr->BufCreate(vertexPool, vertexSize, vertexCount, 0);
    CGxBuf* indexBuf = g_theGxDevicePtr->BufCreate(indexPool, 2, indexCount, 0);

    buf->m_vertexPool = vertexPool;
    buf->m_vertexBytes = vertexBytes;
    buf->m_indexBuf = indexBuf;
    buf->m_indexBytes = 2 * indexCount;
    buf->m_lastUsedFrame = 0;
    buf->m_indexPool = indexPool;
    buf->m_vertexBuf = vertexBuf;
    return buf;
}

// OFFSET: 0x00796320/0x140489310
int32_t CMapChunkBuf::Valid() const {
    if (!this->m_vertexBuf || !this->m_vertexBuf->unk_001C || !this->m_vertexBuf->unk_001D) {
        return 0;
    }

    if (!this->m_indexBuf || !this->m_indexBuf->unk_001C || !this->m_indexBuf->unk_001D) {
        return 0;
    }

    return 1;
}

// OFFSET: 0x00796350/0x140489350
CMapChunkBuf* CMapChunkBuf::Alloc(int32_t vertexCount, int32_t indexCount, EGxVertexBufferFormat format) {
    uint32_t vertexSize = GxVertexSize(format);
    CMapChunkBufList* list;

    for (list = s_freeLists.Head(); list; list = s_freeLists.Next(list)) {
        if (list->m_vertexBytes == vertexCount * vertexSize && list->m_indexBytes == 2 * indexCount && list->m_bufs.Head()) {
            break;
        }
    }

    if (!list) {
        return CMapChunkBuf::CreateBuf(vertexCount, indexCount, vertexSize);
    }

    CMapChunkBuf* buf = list->m_bufs.Head();
    buf->m_link.Unlink();
    buf->m_vertexBuf->unk_001C = 0;
    buf->m_indexBuf->unk_001C = 0;
    return buf;
}

// OFFSET: 0x00796410/0x140489440
CMapChunkBuf::CMapChunkBuf() {
    this->m_lastUsedFrame = 0;
    this->m_vertexBytes = 0;
    this->m_indexBytes = 0;
    this->m_vertexBuf = NULL;
    this->m_indexBuf = NULL;
    this->m_vertexPool = NULL;
    this->m_indexPool = NULL;
}

// OFFSET: 0x00796430/0x140489470
CMapChunkBuf::~CMapChunkBuf() {
    if (this->m_vertexBuf) {
        CGxBuf* buf = this->m_vertexBuf;

        if (buf != g_theGxDevicePtr->BufStream(buf->m_pool->m_target, 0, 0)) {
            g_theGxDevicePtr->BufDestroy(buf);
        }
    }

    if (this->m_indexBuf) {
        CGxBuf* buf = this->m_indexBuf;

        if (buf != g_theGxDevicePtr->BufStream(buf->m_pool->m_target, 0, 0)) {
            g_theGxDevicePtr->BufDestroy(buf);
        }
    }

    if (this->m_vertexPool) {
        g_theGxDevicePtr->PoolDestroy(this->m_vertexPool);
    }

    if (this->m_indexPool) {
        g_theGxDevicePtr->PoolDestroy(this->m_indexPool);
    }
}

// OFFSET: 0x00796510/0x140489580
CMapChunkBufList::~CMapChunkBufList() {
}

// OFFSET: 0x00796590/0x140489630
void CMapChunkBuf::Free(CMapChunkBuf* buf) {
    CMapChunkBufList* list;

    for (list = s_freeLists.Head(); list; list = s_freeLists.Next(list)) {
        if (list->m_vertexBytes == buf->m_vertexBytes && list->m_indexBytes == buf->m_indexBytes) {
            break;
        }
    }

    if (!list) {
        void* mem = SMemAlloc(sizeof(CMapChunkBufList), __FILE__, __LINE__, 0);
        list = mem ? new (mem) CMapChunkBufList() : NULL;
        list->m_vertexBytes = buf->m_vertexBytes;
        list->m_indexBytes = buf->m_indexBytes;
        s_freeLists.LinkToTail(list);
    }

    list->m_bufs.LinkToTail(buf);
    buf->m_lastUsedFrame = World::GetFrameCnt();
}

// OFFSET: 0x00796640/0x140489750
void CMapChunkBuf::Update() {
    for (CMapChunkBufList* list = s_freeLists.Head(), * nextList; list; list = nextList) {
        nextList = s_freeLists.Next(list);

        for (CMapChunkBuf* buf = list->m_bufs.Head(), * next; buf; buf = next) {
            next = list->m_bufs.Next(buf);

            if (World::GetFrameCnt() - buf->m_lastUsedFrame <= 120) {
                break;
            }

            buf->m_link.Unlink();
            CMap::FreeChunkBuf(buf);
        }

        if (!list->m_bufs.Head()) {
            list->m_link.Unlink();
            list->~CMapChunkBufList();
            SMemFree(list, "delete", -1, 0);
        }
    }
}

// OFFSET: 0x00796770/0x1404898E0
void CMapChunkBuf::Destroy() {
    for (CMapChunkBufList* list = s_freeLists.Head(), * nextList; list; list = nextList) {
        nextList = s_freeLists.Next(list);

        for (CMapChunkBuf* buf = list->m_bufs.Head(), * next; buf; buf = next) {
            next = list->m_bufs.Next(buf);
            buf->m_link.Unlink();
            CMap::FreeChunkBuf(buf);
        }

        list->m_link.Unlink();
        list->~CMapChunkBufList();
        SMemFree(list, "delete", -1, 0);
    }
}

// OFFSET: 0x00401000/0x1407D7E80
void CMapChunkBuf::Initialize() {
}
