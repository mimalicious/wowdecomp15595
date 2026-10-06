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

// PROVEN: a8266a2d IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CMapBaseObj::SelectLights(CM2Lighting* lighting) {
}

// PROVEN: e2a6c90a IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CMapBaseObj::SelectUnderwater(CM2Lighting* lighting) {
}
