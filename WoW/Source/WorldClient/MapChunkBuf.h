#ifndef WOW_SOURCE_WORLDCLIENT_MAPCHUNKBUF_H
#define WOW_SOURCE_WORLDCLIENT_MAPCHUNKBUF_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"

struct CGxBuf;
struct CGxPool;

class CMapChunkBuf {
    public:
    /* 0000 */ uint32_t m_handle;
    /* 0004 */ uint32_t m_lastUsedFrame;
    /* 0008 */ uint32_t m_vertexBytes;
    /* 000C */ uint32_t m_indexBytes;
    /* 0010 */ CGxBuf* m_vertexBuf;
    /* 0014 */ CGxBuf* m_indexBuf;
    /* 0018 */ CGxPool* m_vertexPool;
    /* 001C */ CGxPool* m_indexPool;
    /* 0020 */ TSLink<CMapChunkBuf> m_link;

    static CMapChunkBuf* Alloc(int32_t vertexCount, int32_t indexCount, EGxVertexBufferFormat format);
    static CMapChunkBuf* CreateBuf(int32_t vertexCount, int32_t indexCount, int32_t vertexSize);
    static void Destroy();
    static void Free(CMapChunkBuf* buf);
    static void Initialize();
    static void Update();

    CMapChunkBuf();
    ~CMapChunkBuf();
    int32_t Valid() const;
};

class CMapChunkBufList {
    public:
    /* 0000 */ uint32_t m_vertexBytes;
    /* 0004 */ uint32_t m_indexBytes;
    /* 0008 */ TSExplicitList<CMapChunkBuf, offsetof(CMapChunkBuf, m_link)> m_bufs;
    /* 0014 */ TSLink<CMapChunkBufList> m_link;

    ~CMapChunkBufList();
};

#endif
