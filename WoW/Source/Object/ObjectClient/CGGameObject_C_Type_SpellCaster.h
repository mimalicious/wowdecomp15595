#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_CGGAMEOBJECT_C_TYPE_SPELLCASTER_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_CGGAMEOBJECT_C_TYPE_SPELLCASTER_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"

class CGGameObject_C_Type_SpellCaster : public CGGameObject_C_TypeAnimated {
    public:
    virtual bool CanUseNow(GAME_ERROR_TYPE* error, float* distance, const char** errorText) const;

    CGGameObject_C_Type_SpellCaster(CGGameObject_C* owner, const CClientObjCreate& create);
};

#endif
