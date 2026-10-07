#include "WoW/Source/WorldClient/MapBaseObj.h"

// OFFSET: 0x00797D70/0x14048B660
CMapBaseObj::CMapBaseObj() {
    this->m_refCount = 0;
    this->m_flags = 0;
    this->m_type = MapBaseObj_Type_Base;
}

// OFFSET: 0x00797DA0/0x14048B690
CMapBaseObj::~CMapBaseObj() {
}

// PROVEN: a8266a2d IDENTICAL/NONE
// OFFSET: 0x00847950
void CMapBaseObj::SelectLights(CM2Lighting* lighting) {
}

// PROVEN: e2a6c90a IDENTICAL/NONE
// OFFSET: 0x00847950
void CMapBaseObj::SelectUnderwater(CM2Lighting* lighting) {
}
