#include "WoW/Source/WorldClient/MapObj.h"
#include <cstring>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WorldClient/Map.h"

// OFFSET: 0x00795480
int32_t CMapObj::Load(const char* name) {
    strcpy(this->m_name, name);

    SFileImpl* file = NULL;

    if (CMap::SafeOpen(name, &file, 0)) {
        strcpy(this->m_name, name);
    } else {
        CMap::SafeOpen("world\\wmo\\Dungeon\\test\\missingwmo.wmo", &file, 1);
        strcpy(this->m_name, "world\\wmo\\Dungeon\\test\\missingwmo.wmo");
    }

    this->m_dataSize = SFileGetFileSize(file, NULL);
    this->m_data = static_cast<uint8_t*>(SMemAlloc(this->m_dataSize, __FILE__, __LINE__, 0));
    AsyncFileReadCreateObject(file, 3, 0, &this->m_asyncObject, this->m_data, this, CMapObj::AsyncCallback, CMapObj::AsyncFailedCallback, 0, 0, 0);

    return 1;
}
