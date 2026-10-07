#ifndef WOW_SOURCE_WORLDCLIENT_MAPDOODADDEF_H
#define WOW_SOURCE_WORLDCLIENT_MAPDOODADDEF_H

#include <cstdint>
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/WorldClient/MapStaticEntity.h"

class CMapChunk;

class CMapDoodadDef : public CMapStaticEntity, public TSHashObject<CMapDoodadDef, HASHKEY_DWORD> {
    public:
    /* 00A8 */ TSLink<CMapDoodadDef> m_updateLink;
    /* 00B0 */ uint32_t m_cullStamp;
    /* 00B4 */ CAaBox m_modelBox;
    /* 00CC */ C3Vector m_pos;
    /* 00D8 */ float m_scale;
    /* 00DC */ C4Quaternion m_rotation;
    /* 00EC */ uint16_t unk_00EC;
    /* 00EE */ uint16_t m_fade;

    virtual ~CMapDoodadDef();
    virtual void SelectLights(CM2Lighting* lighting);
    virtual void SelectUnderwater(CM2Lighting* lighting);

    static void StreamingCallback(void* param);

    CMapDoodadDef();
    void LinkToChunk(CMapBaseObj* owner);
    void Purge();
    void UpdateBounds();
    void UpdateLoaded();
    void UpdateOwners();
};

#endif
