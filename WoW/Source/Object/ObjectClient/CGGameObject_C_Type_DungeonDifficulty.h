#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_CGGAMEOBJECT_C_TYPE_DUNGEONDIFFICULTY_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_CGGAMEOBJECT_C_TYPE_DUNGEONDIFFICULTY_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"

class CGGameObject_C_Type_DungeonDifficulty : public CGGameObject_C_TypeBase {
    public:
    /* 0010 */ uint32_t m_sound;
    /* 0014 */ int32_t m_disabled;

    virtual ~CGGameObject_C_Type_DungeonDifficulty();
    virtual bool CanUse() const;
    virtual void ShouldRender(uint32_t flags, int32_t& render, int32_t& a3);
    virtual float GetBaseAlpha() const;
    virtual void HandleAnimEvent(uint32_t eventId, uint32_t data, const C3Vector& position, uint32_t time);
    virtual void PostInitWithStats();
    virtual void UpdateDisabled();

    CGGameObject_C_Type_DungeonDifficulty(CGGameObject_C* owner, const CClientObjCreate& create);
    int32_t GetMapID() const;
    int32_t GetDifficulty() const;
};

#endif
