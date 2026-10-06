#include "MapBaseObj.h"

// OFFSET: [#0x007D71D0, #0x007D72A0]
CMapBaseObj::~CMapBaseObj() {
}

// OFFSET: #0x007D7260
CMapBaseObj::CMapBaseObj() {
    this->m_type = 1;
    this->m_refCount = 0;
    this->m_flags = static_cast<MapObjFlag>(0);
}

// OFFSET: %0x00A42EC0
void CMapBaseObj::SelectLights(CM2Lighting* lighting) {
}

// OFFSET: %0x00A42EC0
void CMapBaseObj::SelectUnderwater(CM2Lighting* lighting) {
}
