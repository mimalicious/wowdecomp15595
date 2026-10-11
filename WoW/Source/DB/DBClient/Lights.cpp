#include <cstdint>
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LightRec.h"
#include "WoW/Source/WorldClient/World.h"

// INLINED
static void TransformMapCoord(LightRec* rec, bool transform) {
    if ((rec->m_gameCoords[0] != 0.0f || rec->m_gameCoords[1] != 0.0f || rec->m_gameCoords[2] != 0.0f) && transform) {
        rec->m_gameCoords[0] *= 1.0f / 36.0f;
        rec->m_gameCoords[1] *= 1.0f / 36.0f;
        rec->m_gameCoords[2] *= 1.0f / 36.0f;
        rec->m_gameFalloffStart *= 1.0f / 36.0f;
        rec->m_gameFalloffEnd *= 1.0f / 36.0f;

        float offsetX = 0.0f;
        float offsetY = 0.0f;

        if (!World::MapIsDungeon()) {
            offsetX = 51200.0f / 3.0f;
            offsetY = 51200.0f / 3.0f;
        }

        float x = -(rec->m_gameCoords[0] - offsetY);
        float z = rec->m_gameCoords[1];
        rec->m_gameCoords[0] = -(rec->m_gameCoords[2] - offsetX);
        rec->m_gameCoords[1] = x;
        rec->m_gameCoords[2] = z;
    }
}

// INLINED
static void TransformLights(bool transform) {
    for (int32_t i = 0; i < g_lightDB.m_numRecords; i++) {
        LightRec* rec = g_lightDB.GetRecordByIndex(i);
        TransformMapCoord(rec, transform);
    }
}

// INLINED
void InitLights(bool transform) {
    TransformLights(transform);
}
