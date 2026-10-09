#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_AREATRIGGER_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_AREATRIGGER_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/AreaTrigger.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

class CClientObjCreate;

class CGAreaTrigger_C : public CGObject_C, public CGAreaTrigger {
    public:
    void UpdateDisplay(uint32_t time);
    /* 0100 */ uint8_t unk_0100[56];
    /* 0138 */ TSList<void> m_areaTriggerMirrorLists [6];
    /* 0180 */ uint8_t unk_0180[8];

    CGAreaTrigger_C(uint32_t time, const CClientObjCreate& create);
    ~CGAreaTrigger_C();
    void SetStorage(uint32_t* storage, uint32_t* saved);
};

#endif
