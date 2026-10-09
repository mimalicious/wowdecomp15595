#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_CGGAMEOBJECT_C_TYPE_DOOR_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_CGGAMEOBJECT_C_TYPE_DOOR_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"

class CGGameObject_C_Type_Door : public CGGameObject_C_TypeAnimated {
    public:
    /* 002C */ uint16_t m_linkAll;

    virtual bool CanBeSolid() const;
    virtual void UpdateAnimState(uint32_t state);
    virtual void UpdateSolidity();
    virtual bool CanUseNow(GAME_ERROR_TYPE* error, float* distance) const;

    CGGameObject_C_Type_Door(CGGameObject_C* owner, const CClientObjCreate& create);
    bool CheckPropState() const;
};

#endif
