#include "WoW/Source/WorldClient/MapBaseObj.h"

// PROVEN: 2bc8c2d5 UNPROVEN/IDENTICAL
// OFFSET: 0x00797D70/0x14048B660
CMapBaseObj::CMapBaseObj() {
    this->m_refCount = 0;
    this->m_flags = 0;
    this->m_type = MapBaseObj_Type_Base;
}

// OFFSET: [0x00797D90/0x14048B680, 0x00797DA0/0x14048B690]
CMapBaseObj::~CMapBaseObj() {
}

// PROVEN: a8266a2d IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CMapBaseObj::SelectLights(CM2Lighting* lighting) {
}

// PROVEN: e2a6c90a IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CMapBaseObj::SelectUnderwater(CM2Lighting* lighting) {
}
