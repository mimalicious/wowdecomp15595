#include "WoW/Source/LiquidSystem/Liquid.h"
#include <new>
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00E4EBB8
uint32_t Liquid::CMaterialBank::s_refCount;

// STATIC: 0x00E4EBBC
TSGrowableArray<Liquid::IMaterial*> Liquid::CMaterialBank::s_materials;

// OFFSET: 0x0071C4D0
void Liquid::CMaterialBank::Create() {
    Liquid::CMaterialBank::s_refCount++;
}

// OFFSET: 0x0071C580
void Liquid::CMaterialBank::ResetMaterials() {
    for (uint32_t i = 0; i < Liquid::CMaterialBank::s_materials.Count(); i++) {
        if (Liquid::CMaterialBank::s_materials.m_data[i]) {
            Liquid::CMaterialBank::s_materials.m_data[i]->Reset();
        }
    }
}

// OFFSET: 0x0071C630
void Liquid::CMaterialBank::Purge() {
    for (uint32_t i = 0; i < Liquid::CMaterialBank::s_materials.Count(); i++) {
        if (Liquid::CMaterialBank::s_materials.m_data[i]) {
            Liquid::CMaterialBank::s_materials.m_data[i]->Release();
        }
    }

    Liquid::CMaterialBank::s_materials.m_count = 0;
}

// OFFSET: 0x0071C680
Liquid::IMaterial* Liquid::CMaterialBank::GetMaterial(uint32_t liquidType) {
    if (!Liquid::CMaterialBank::s_refCount) {
        return NULL;
    }

    LiquidTypeRec* rec;

    while (true) {
        rec = g_liquidTypeDB.GetRecord(liquidType);

        if (rec) {
            break;
        }

        SysMsgPrintf(STATUS_ERROR, 2, "Material Bank: Liquid type [%d] not found, defaulting to water!", liquidType);
        liquidType = 1;

        if (!Liquid::CMaterialBank::s_refCount) {
            return NULL;
        }
    }

    uint32_t materialId = rec->m_materialID;

    if (materialId < Liquid::CMaterialBank::s_materials.Count() && Liquid::CMaterialBank::s_materials.m_data[materialId]) {
        return Liquid::CMaterialBank::s_materials.m_data[materialId];
    }

    IMaterial* material = NULL;

    switch (materialId) {
        case 1: {
            void* mem = SMemAlloc(sizeof(CMaterialWater), __FILE__, __LINE__, 0);
            material = mem ? new (mem) CMaterialWater() : NULL;
            break;
        }

        case 2: {
            void* mem = SMemAlloc(sizeof(CMaterialMagma), __FILE__, __LINE__, 0);
            material = mem ? new (mem) CMaterialMagma() : NULL;
            break;
        }

        case 3: {
            void* mem = SMemAlloc(sizeof(CMaterialWater), __FILE__, __LINE__, 0);
            material = mem ? new (mem) CMaterialWater() : NULL;
            break;
        }

        case 4: {
            void* mem = SMemAlloc(sizeof(CMaterialMagma), __FILE__, __LINE__, 0);
            material = mem ? new (mem) CMaterialMagma() : NULL;
            break;
        }

        case 5: {
            void* mem = SMemAlloc(sizeof(CMaterialMercury), __FILE__, __LINE__, 0);
            material = mem ? new (mem) CMaterialMercury() : NULL;
            break;
        }

        case 8: {
            void* mem = SMemAlloc(sizeof(CMaterialDebug), __FILE__, __LINE__, 0);
            material = mem ? new (mem) CMaterialDebug() : NULL;
            break;
        }

        default:
            break;
    }

    Liquid::CMaterialBank::s_materials.GrowToFit(materialId, 1);
    Liquid::CMaterialBank::s_materials.m_data[materialId] = material;
    return material;
}

// OFFSET: 0x0071C810
void Liquid::CMaterialBank::Destroy() {
    if (!--Liquid::CMaterialBank::s_refCount) {
        Liquid::CMaterialBank::Purge();
    }
}
