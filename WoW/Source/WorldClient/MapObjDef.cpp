#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/LiquidSystem/Liquid.h"

// OFFSET: 0x0075EF10
void CMapObjDefGroup::ExtendBounds(const CAaBox& box) {
    this->m_bbox |= box;
    this->m_mapObjDef->m_bbox |= box;
}

// OFFSET: 0x00761060
CMapObjDef::CMapObjDef() {
    this->m_bbox.b.x = 0.0f;
    this->m_bbox.b.y = 0.0f;
    this->m_bbox.b.z = 0.0f;
    this->m_bbox.t.x = 0.0f;
    this->m_bbox.t.y = 0.0f;
    this->m_bbox.t.z = 0.0f;
    this->unk_0140[0] = 0;
    this->unk_0140[1] = 0;
    this->m_worldMap = NULL;
    this->m_mapObj = NULL;
    this->m_doodadSet = 0;
    this->m_nameSet = 0;
    this->unk_0100 = 0;
    this->m_param64 = 0;
    this->m_type |= 0x10;
    this->m_perturbatorMesh = NULL;
    this->m_gatherStamp = 0;
    this->unk_004C = 0;
    this->m_skipPortal = -1;
}

// OFFSET: 0x00761260
CMapObjDef::~CMapObjDef() {
    if (this->m_perturbatorMesh) {
        Liquid::DestroyPerturbatorMesh(Liquid::PertSurface_Ocean, this->m_perturbatorMesh);
        this->m_perturbatorMesh = NULL;
    }
}
