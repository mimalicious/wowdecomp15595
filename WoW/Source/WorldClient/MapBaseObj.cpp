#include "WoW/Source/WorldClient/MapBaseObj.h"

// OFFSET: 0x00797D70
CMapBaseObj::CMapBaseObj() {
    this->m_refCount = 0;
    this->m_flags = 0;
    this->m_type = MapBaseObj_Type_Base;
}

// OFFSET: 0x00797DA0
CMapBaseObj::~CMapBaseObj() {
}

// OFFSET: 0x00847950
void CMapBaseObj::SelectLights(CM2Lighting* lighting) {
}

// OFFSET: 0x00847950
void CMapBaseObj::SelectUnderwater(CM2Lighting* lighting) {
}
