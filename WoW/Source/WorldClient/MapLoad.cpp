#include "WoW/Source/WorldClient/Map.h"
#include <cstring>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"

// STATIC: 0x00E74368
TSHashTable<CMapDynamicTexture, HASHKEY_DWORD> CMap::s_dynamicTextures;

// OFFSET: 0x00773270/0x14045D800
void CMap::UpdateDynamicTextures() {
    for (CMapDynamicTexture* entry = CMap::s_dynamicTextures.Head(), * next; entry; entry = next) {
        next = CMap::s_dynamicTextures.Next(entry);

        if (entry->m_texture && entry->m_texture->m_refcount == 1) {
            TextureRelease(entry->m_texture);
            entry->m_texture = NULL;
        }
    }
}

// OFFSET: 0x007732E0
void CMap::ClearDynamicTextures() {
    for (CMapDynamicTexture* entry = CMap::s_dynamicTextures.Head(), * next; entry; entry = next) {
        next = CMap::s_dynamicTextures.Next(entry);

        if (entry->m_texture) {
            TextureRelease(entry->m_texture);
        }
    }

    CMap::s_dynamicTextures.Clear();
}

// OFFSET: 0x00773390/0x14045D010
CTexture* CMap::LoadTexture(const char* fileName) {
    CStatus status;
    CGxTexFlags texFlags;
    texFlags.m_filter = 4;
    texFlags.m_wrapU = 1;
    texFlags.m_wrapV = 1;
    CTexture* texture = TextureCreate(fileName, texFlags, &status, 0, NULL, 3);
    SysMsgAdd(&status);
    return texture;
}

// OFFSET: 0x007734F0
CTexture* CMap::GetDynamicTexture(uint32_t textureId) {
    HASHKEY_DWORD key;
    key.m_key = textureId;
    CMapDynamicTexture* entry = CMap::s_dynamicTextures.Ptr(textureId, key);

    if (!entry->m_texture) {
        entry->m_texture = CMap::LoadTerrainTexture(entry->m_fileName, entry->m_flags);
    }

    TextureAddRef(entry->m_texture);
    return entry->m_texture;
}

// PROVEN: c1e3118d IDENTICAL/NONE
// OFFSET: 0x00784990/0x140473590
int32_t CMap::SafeOpen(const char* fileName, SFileImpl** file, int32_t fatal) {
    if (SFileOpenFile(fileName, file)) {
        return 1;
    }

    if (fatal) {
        SErrDisplayAppFatal("CMap::SafeOpen() failed %s", fileName);
    }

    return 0;
}

// OFFSET: 0x007849D0/0x1404735E0
void CMap::LoadTextureBlob() {
    char blobName[260];
    strcpy(blobName, this->m_wdtFileName);
    strcpy(strrchr(blobName, '.'), ".tex");

    SFileImpl* file = NULL;

    if (!SFileOpenFile(blobName, &file)) {
        return;
    }

    uint32_t size = SFileGetFileSize(file, NULL);
    void* data = SMemAlloc(size, __FILE__, __LINE__, 0);

    CAsyncObject* object;
    AsyncFileReadCreateObject(file, 3, 1, &object, data, NULL, NULL, NULL, 0, 0, 0);

    while (!object->m_isProcessed) {
        if (World::s_loadProgressCallback) {
            World::s_loadProgressCallback(0.0f, World::s_loadProgressParam);
        }

        AsyncFileReadPollHandler(NULL, NULL);
        OsSleep(10);
    }

    int32_t succeeded = object->m_readSucceeded;
    AsyncFileReadDestroyObject(object);

    if (succeeded) {
        TextureLoadBlob(blobName, data, size);
        return;
    }

    if (data) {
        SMemFree(data, __FILE__, __LINE__, 0);
    }

    World::SetLoadFailed(1, blobName);
}

// OFFSET: 0x00784B20/0x1404737A0
int32_t CMap::SafeRead(const char* fileName, SFileImpl* file, void* buffer, uint32_t bytes) {
    int32_t tries = 10;

    while (!SFile::Read(file, buffer, bytes, NULL)) {
        if (!--tries) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00784CA0/0x1404739F0
void CMap::CreateAreaTable(const SMAreaInfo* areaInfo) {
    this->m_areaExtents.minX = 0xFFFF;
    this->m_areaExtents.minY = 0xFFFF;
    this->m_areaExtents.maxX = 0;
    this->m_areaExtents.maxY = 0;

    const SMAreaInfo* info = areaInfo;

    for (int32_t y = 0; y < 64; y++) {
        for (int32_t x = 0; x < 64; x++) {
            if (info->m_flags & 0x1) {
                if (x < this->m_areaExtents.minX) {
                    this->m_areaExtents.minX = x;
                }

                if (x > this->m_areaExtents.maxX) {
                    this->m_areaExtents.maxX = x;
                }

                if (y < this->m_areaExtents.minY) {
                    this->m_areaExtents.minY = y;
                }

                if (y > this->m_areaExtents.maxY) {
                    this->m_areaExtents.maxY = y;
                }
            }

            if (info->m_flags & 0x2) {
                this->m_mapFlags |= 0x20;
            }

            info++;
        }
    }

    this->m_areaTable.SetRect(this->m_areaExtents);

    info = areaInfo;

    for (int32_t y = 0; y < 64; y++) {
        for (int32_t x = 0; x < 64; x++) {
            CMapAreaTableEntry* entry = this->m_areaTable.Get(x, y);

            if (entry && (info->m_flags & 0x1)) {
                entry->m_flags |= 0x1;
            }

            info++;
        }
    }
}

// OFFSET: 0x00785060/0x140473BA0
int32_t CMap::LoadWdt() {
    char globalWmoName[256];
    SMMapObjDef globalWmoDef;
    globalWmoName[0] = '\0';
    memset(&globalWmoDef.m_position, 0, 48);

    SFileImpl* file = NULL;

    if (!SFileOpenFile(this->m_wdtFileName, &file)) {
        World::SetLoadFailed(1, this->m_wdtFileName);
        return 0;
    }

    int32_t size = SFileGetFileSize(file, NULL);
    uint8_t* data = static_cast<uint8_t*>(alloca(size));

    CAsyncObject* object;
    AsyncFileReadCreateObject(file, 3, 1, &object, data, NULL, NULL, NULL, 0, 0, 0);

    while (!object->m_isProcessed) {
        if (World::s_loadProgressCallback) {
            World::s_loadProgressCallback(0.0f, World::s_loadProgressParam);
        }

        AsyncFileReadPollHandler(NULL, NULL);
        OsSleep(10);
    }

    int32_t succeeded = object->m_readSucceeded;
    int32_t cancelled = object->m_cancelled;
    AsyncFileReadDestroyObject(object);

    if (!succeeded) {
        if (!cancelled) {
            AsyncFileReadSetStreamingError();
        }

        World::SetLoadFailed(1, this->m_wdtFileName);
        return 0;
    }

    uint32_t* chunk = reinterpret_cast<uint32_t*>(data);

    for (int32_t remaining = size; remaining > 0;) {
        uint32_t token = chunk[0];
        uint8_t* chunkData = reinterpret_cast<uint8_t*>(&chunk[2]);
        remaining -= 8;

        if (token == 'MPHD') {
            memcpy(&this->m_header, chunkData, sizeof(this->m_header));
        } else if (token == 'MAIN') {
            this->CreateAreaTable(reinterpret_cast<const SMAreaInfo*>(chunkData));
        } else if (token == 'MODF') {
            memcpy(&globalWmoDef, chunkData, sizeof(globalWmoDef));
        } else if (token == 'MWMO' && chunk[1]) {
            memcpy(globalWmoName, chunkData, chunk[1]);
        }

        remaining -= chunk[1];
        chunk = reinterpret_cast<uint32_t*>(&chunkData[chunk[1]]);
    }

    if (this->m_header.m_flags & 0x1) {
        globalWmoDef.m_uniqueId = CMap::s_globalUniqueId--;
        C3Vector offset;
        memset(&offset, 0, sizeof(offset));
        this->m_globalMapObjDef = this->m_worldMap->CreateMapObjDef(globalWmoName, globalWmoDef, offset, 1);
        this->m_globalMapObjDef->m_refCount++;
        this->m_worldMap->m_mapObjDefList.LinkToTail(this->m_globalMapObjDef);
        this->m_mapFlags |= Flag_GlobalMapObj;
    }

    int32_t shaderModel = g_theGxDevicePtr->m_caps.m_shaderModel;
    CMap::enableChunkBatching = shaderModel >= 4;

    if (shaderModel >= 4 || (CMap::enableTerrainShaderPixel = 0, shaderModel == 2)) {
        CMap::enableTerrainShaderPixel = 1;
    }

    CMap::enableTerrainShaderVertex = shaderModel >= 4;

    if (this->m_header.m_flags & 0x2) {
        CMapRenderChunk::SetVertexFormat(2);

        if (CMap::enableTerrainShaderVertex && (this->m_header.m_flags & 0x10)) {
            CMapRenderChunk::SetVertexFormat(16);
        }
    } else {
        CMapRenderChunk::SetVertexFormat(1);
    }

    return 1;
}

// OFFSET: 0x00785330/0x140473F00
int32_t CMap::Load(const char* mapName, CWorldMap* worldMap, int32_t mapId, int32_t preload) {
    LoadingScreenEnableFileBar(true);

    strcpy(this->m_mapPath, "World\\Maps\\");
    strcpy(&this->m_mapPath[11], mapName);
    strcpy(this->m_mapName, mapName);
    SStrPrintf(this->m_wdtFileName, sizeof(this->m_wdtFileName), "%s\\%s.wdt", this->m_mapPath, this->m_mapName);

    this->m_mapFlags &= ~(Flag_GlobalMapObj | Flag_Preload);
    this->m_worldMap = worldMap;
    this->m_mapId = mapId;

    MapRec* rec = g_mapDB.GetRecord(mapId);

    if (rec->m_flags & 0x20000) {
        this->m_mapFlags |= 0x40;
    }

    if (rec->m_flags & 0x200000) {
        this->m_mapFlags |= 0x80;
    }

    this->LoadWdt();
    this->m_lowDetail.Load(this, this->m_mapPath, this->m_mapName);
    this->LoadTextureBlob();
    this->UpdateMap();
    this->m_mapFlags ^= (this->m_mapFlags ^ (2 * preload)) & Flag_Preload;

    LoadingScreenEnableFileBar(false);

    return 1;
}

// OFFSET: 0x0079D2D0/0x140491FF0
void CMap::LoadArea(CMapArea* area) {
    char fileName[256];
    area->m_areaFlags &= ~0x2u;
    SStrPrintf(fileName, sizeof(fileName), "%s\\%s_%d_%d.adt", this->m_mapPath, this->m_mapName, area->m_indexX, area->m_indexY);
    area->Load(fileName);

    char* ext = strrchr(fileName, '.');
    strcpy(ext, "_obj0.adt");

    if (SFile::FileExists(fileName)) {
        area->Load(fileName);
    }

    strcpy(ext, "_tex0.adt");

    if (SFile::FileExists(fileName)) {
        int32_t local = SFile::FileIsLocal(fileName);

        if (local != 1) {
            if (!local) {
                AsyncFileReadCreateObject(fileName, 6, 0, 0, NULL, NULL, NULL, AsyncFileReadDestroyObject, AsyncFileReadDestroyObject, 0, 0, 0);
            }

            strcpy(ext, "_tex1.adt");

            if (!SFile::FileExists(fileName)) {
                strcpy(ext, "_tex0.adt");
            }
        }
    } else {
        strcpy(ext, "_tex1.adt");

        if (!SFile::FileExists(fileName)) {
            return;
        }
    }

    area->Load(fileName);
}

// OFFSET: 0x00773420/0x14045D0C0
CTexture* CMap::LoadTerrainTexture(const char* fileName, uint32_t flags) {
    int32_t shaderModel = g_theGxDevicePtr->m_caps.m_shaderModel;
    int32_t solid = g_theGxDevicePtr->m_caps.m_texTarget[1] && (shaderModel >= 4 || shaderModel == 2);

    if (!(flags & 0x1) && CMap::enableTerrainShaderPixel) {
        char name[260];
        strcpy(name, fileName);
        strcpy(strrchr(name, '.'), "_s.blp");
        return CMap::LoadTexture(name);
    }

    if (!(flags & 0x1) || solid) {
        return CMap::LoadTexture(fileName);
    }

    CImVector black(0xFF000000);
    return TextureCreateSolid(black);
}
