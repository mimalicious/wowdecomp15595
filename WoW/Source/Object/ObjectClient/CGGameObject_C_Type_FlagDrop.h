#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_CGGAMEOBJECT_C_TYPE_FLAGDROP_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_CGGAMEOBJECT_C_TYPE_FLAGDROP_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"

class CGGameObject_C_Type_FlagDrop : public CGGameObject_C_TypeAnimated {
    public:
    CGGameObject_C_Type_FlagDrop(CGGameObject_C* owner, const CClientObjCreate& create);
};

#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_FlagDrop.inl"

#endif
