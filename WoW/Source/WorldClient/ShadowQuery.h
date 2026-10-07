#ifndef WOW_SOURCE_WORLDCLIENT_SHADOWQUERY_H
#define WOW_SOURCE_WORLDCLIENT_SHADOWQUERY_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class CWorldView;

class CShadowQuery {
    public:
    static CShadowQuery* s_activeShadowQuery;

    static void Destroy();
    static void Initialize();

    /* 0000 */ uint8_t unk_0000[172];
    /* 00AC */ CWorldView* m_worldView;
    /* 00B0 */ uint32_t unk_00B0[2];

    CShadowQuery();
    ~CShadowQuery();
    void SetEntityCutoff(const C3Vector& pos, const C3Vector& camPos);
    void Update();
};

#endif
