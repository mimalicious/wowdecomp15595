#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00D29914
static const uint8_t s_existsAll[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

// STATIC: 0x0100AAC4
static uint16_t s_defaultTexCoord[2];

// OFFSET: 0x00B3CD10
const SMLiquidChunkDesc* Liquid::GetChunkDesc(const void* data, uint32_t x, uint32_t y) {
    return static_cast<const SMLiquidChunkDesc*>(data) + (x + 16 * y);
}

// OFFSET: 0x00B3CD30
const SMLiquidInstance* Liquid::GetInstanceDesc(const void* data, const SMLiquidChunkDesc* desc, uint32_t index) {
    return reinterpret_cast<const SMLiquidInstance*>(static_cast<const uint8_t*>(data) + desc->m_offsetInstances) + index;
}

// OFFSET: 0x00B3CD50
const void* Liquid::GetInstanceVertices(const void* data, const SMLiquidInstance* instance) {
    if (instance->m_offsetVertexData) {
        return static_cast<const uint8_t*>(data) + instance->m_offsetVertexData;
    }

    return NULL;
}

// OFFSET: 0x00B3CD70
bool Liquid::HasData(const SMLiquidChunkDesc* desc, const SMLiquidInstance* instance) {
    return desc->m_layerCount && (instance->m_offsetExistsBitmap || desc->m_offsetAttributes);
}

// OFFSET: 0x00B3CDA0
const uint8_t* Liquid::GetExistsTable(const void* data, const SMLiquidInstance* instance) {
    if (instance->m_offsetExistsBitmap) {
        return static_cast<const uint8_t*>(data) + instance->m_offsetExistsBitmap;
    }

    return s_existsAll;
}

// OFFSET: 0x00B3CDC0
float Liquid::GetChunkHeight(uint32_t format, const void* vertices, uint32_t count, uint32_t index) {
    switch (format) {
        case 0:
        case 1:
        case 3:
            return static_cast<const float*>(vertices)[index];

        case 4:
        case 5:
            return *reinterpret_cast<const float*>(static_cast<const uint8_t*>(vertices) + 8 * index + 4);

        default:
            return 0.0f;
    }
}

// OFFSET: 0x00B3CE10
uint8_t Liquid::GetChunkDepth(uint32_t format, const void* vertices, uint32_t count, uint32_t index) {
    const uint8_t* bytes = static_cast<const uint8_t*>(vertices);

    switch (format) {
        case 0:
            return bytes[4 * count + index];

        case 2:
            if (bytes) {
                return bytes[index];
            }

            return 0xFF;

        case 3:
            return bytes[8 * count + index];

        case 4:
            return bytes[8 * index];

        default:
            return 0;
    }
}

// OFFSET: 0x00B3CE80
const uint16_t* Liquid::GetChunkTexCoord0(uint32_t format, const void* vertices, uint32_t count, uint32_t index) {
    if (format == 1 || format == 3) {
        return reinterpret_cast<const uint16_t*>(static_cast<const float*>(vertices) + (index + count));
    }

    if (format == 5) {
        return reinterpret_cast<const uint16_t*>(static_cast<const uint8_t*>(vertices) + 8 * index);
    }

    return s_defaultTexCoord;
}

// OFFSET: 0x00B3CEC0
int32_t Liquid::GetVertexFormat(uint32_t liquidType, uint32_t format) {
    if (!liquidType) {
        return -1;
    }

    if (format < 42) {
        return format;
    }

    if (liquidType == 2) {
        return 2;
    }

    LiquidTypeRec* rec = g_liquidTypeDB.GetRecord(liquidType);

    if (!rec) {
        return -1;
    }

    LiquidMaterialRec* materialRec = g_liquidMaterialDB.GetRecord(rec->m_materialID);

    if (!materialRec) {
        return -1;
    }

    return materialRec->m_lvf;
}
