#ifndef WOW_SOURCE_WORLDCLIENT_MAPAREA_H
#define WOW_SOURCE_WORLDCLIENT_MAPAREA_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"
#include "WoW/Source/WorldClient/MapChunk.h"

class CAsyncObject;
struct CGxBuf;
class CMap;
class CMapChunkBuf;
class CTexture;
class CWorldMap;
class SFileImpl;
struct SMDoodadDef;
struct SMMapObjDef;
namespace Liquid { class CInstance; }

struct SMAreaHeader {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t unk_0004[11];
    /* 0030 */ uint8_t m_mamp;
    /* 0031 */ uint8_t unk_0031[15];
};

struct SMChunkInfo {
    /* 0000 */ uint32_t m_offset;
    /* 0004 */ uint32_t m_size;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ uint32_t m_asyncId;
};

class CMapChunkInfo {
    public:
    /* 0000 */ uint32_t m_handle;
    /* 0004 */ SMChunkInfo m_chunks[256];
};

class CMapAreaTexture {
    public:
    /* 0000 */ const char* m_name;
    /* 0004 */ CTexture* m_texture;

    CMapAreaTexture();
};

class CMapArea : public CMapBaseObj {
    public:
    static void AsyncFailedCallback(CAsyncObject* object);
    static void AsyncLoadCallback(CAsyncObject* object);
    static void AsyncPriorityCallback(void* param);
    static void Destroy();
    static void Initialize();
    static void LinkToAsyncHead(void* param);

    /* 0010 */ CMap* m_map;
    /* 0014 */ CWorldMap* m_worldMap;
    /* 0018 */ float m_distSq;
    /* 001C */ TSLink<CMapArea> m_mapLink;
    /* 0024 */ TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)> m_linkList;
    /* 0030 */ CAaBox m_bbox;
    /* 0048 */ float m_posX;
    /* 004C */ float m_posY;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ int32_t m_indexX;
    /* 0058 */ int32_t m_indexY;
    /* 005C */ int32_t m_chunkOffsetX;
    /* 0060 */ int32_t m_chunkOffsetY;
    /* 0064 */ TSGrowableArray<CMapAreaTexture> m_textures;
    /* 0074 */ SMAreaHeader* m_header;
    /* 0078 */ uint32_t m_areaFlags;
    /* 007C */ int32_t m_numFiles;
    /* 0080 */ SFileImpl* m_files[3];
    /* 008C */ CAsyncObject* m_asyncObjects[3];
    /* 0098 */ uint8_t* m_buffers[3];
    /* 00A4 */ uint32_t m_bufferSizes[3];
    /* 00B0 */ CMapChunkInfo* m_chunkInfo[3];
    /* 00BC */ const char* m_textureNames;
    /* 00C0 */ uint32_t m_textureNamesSize;
    /* 00C4 */ SMDoodadDef* m_doodadDefs;
    /* 00C8 */ SMMapObjDef* m_mapObjDefs;
    /* 00CC */ uint32_t m_doodadDefCount;
    /* 00D0 */ uint32_t m_mapObjDefCount;
    /* 00D4 */ const char* m_doodadNames;
    /* 00D8 */ const char* m_mapObjNames;
    /* 00DC */ const uint32_t* m_doodadNameOffsets;
    /* 00E0 */ const uint32_t* m_mapObjNameOffsets;
    /* 00E4 */ const void* m_flightBounds;
    /* 00E8 */ const uint32_t* m_textureFlags;
    /* 00EC */ const void* m_liquid;
    /* 00F0 */ CMapChunk* m_chunks[256];
    /* 04F0 */ CAaBox m_chunkBounds[256];
    /* 1CF0 */ TSExplicitList<CMapChunk, offsetof(CMapChunk, m_areaLink)> m_chunkList;

    virtual ~CMapArea();

    CMapArea();
    void Create();
    void CreateMapObjDefs();
    void Enable(int32_t enable);
    void InitBounds();
    CTexture* GetTexture(uint32_t index);
    int32_t IsLoading();
    void Load(const char* fileName);
    CTexture* LoadTexture(uint32_t index);
    void LoadTextures();
    void PrepareChunk(int32_t x, int32_t y);
    void ProcessIffChunks(int32_t fileIndex, uint32_t* data, int32_t size);
    void BatchChunks(const C2iVector& index);
    void PurgeChunk(CMapChunk* chunk);
    void PurgeChunks(const CiRect& rect);
    void PurgeXXX();
    void Update(const CiRect& rect);
};

class CMapAreaMed {
    public:
    /* 0000 */ uint32_t m_handle;
    /* 0004 */ uint8_t unk_0004[33380];
};

class CMapAreaLow : public CMapBaseObj {
    public:
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ CAaBox m_bbox;
    /* 0030 */ C3Vector m_center;
    /* 003C */ float m_radius;
    /* 0040 */ float m_posX;
    /* 0044 */ float m_posY;
    /* 0048 */ float m_oceanHeight;
    /* 004C */ int32_t m_indexX;
    /* 0050 */ int32_t m_indexY;
    /* 0054 */ uint32_t m_indexCount;
    /* 0058 */ const int16_t* m_heights;
    /* 005C */ const uint16_t* m_holes;
    /* 0060 */ CMapChunkBuf* m_buf;
    /* 0064 */ uint32_t m_lastUsedFrame;
    /* 0068 */ Liquid::CInstance* m_oceanInstance;
    /* 006C */ TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)> m_linkList;
    /* 0078 */ TSLink<CMapAreaLow> m_renderLink;
    /* 0080 */ TSLink<CMapAreaLow> m_lowDetailLink;

    static void Destroy();
    static void Initialize();

    virtual ~CMapAreaLow();

    CMapAreaLow();
    void Create(int32_t x, int32_t y, const int16_t* heights, const uint16_t* holes);
    void CreateOceanInstance();
    void CreateVertices(CGxBuf* buf);
    void FreeBuf();
    void LiquidReset();
    void Render();
    void RenderPrep();
};

#endif
