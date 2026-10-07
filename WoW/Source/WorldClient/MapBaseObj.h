#ifndef WOW_SOURCE_WORLDCLIENT_MAPBASEOBJ_H
#define WOW_SOURCE_WORLDCLIENT_MAPBASEOBJ_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CM2Lighting;
class CMapBaseObj;
class CWorldMap;

enum MapBaseObjType {
    MapBaseObj_Type_Base = 0x1,
    MapBaseObj_Type_MapObjDef = 0x10,
    MapBaseObj_Type_MapObjDefGroup = 0x20,
    MapBaseObj_Type_Entity = 0x40,
    MapBaseObj_Type_DoodadDef = 0x80,
};

class CMapBaseObjLink {
    public:
    /* 0000 */ uint32_t m_handle;
    /* 0004 */ CMapBaseObj* m_owner;
    /* 0008 */ CMapBaseObj* m_ref;
    /* 000C */ TSLink<CMapBaseObjLink> m_refLink;
    /* 0014 */ TSLink<CMapBaseObjLink> m_ownerLink;
};

class CMapBaseObj {
    public:
    /* 0004 */ uint32_t m_handle;
    /* 0008 */ uint16_t m_type;
    /* 000A */ uint16_t m_refCount;
    /* 000C */ uint32_t m_flags;

    virtual ~CMapBaseObj();
    virtual void SelectLights(CM2Lighting* lighting);
    virtual void SelectUnderwater(CM2Lighting* lighting);

    CMapBaseObj();
};

#endif
