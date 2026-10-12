#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_GAMEOBJECT_C_INL
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_GAMEOBJECT_C_INL

#include "WoW/Source/DB/Rec/DestructibleModelDataRec.h"

// INLINED
inline CGGameObject_C_TypeBase::CGGameObject_C_TypeBase(CGGameObject_C* owner, const CClientObjCreate& create) {
    this->m_owner = owner;
    this->m_useErrorType = 243;
    this->m_interactDistance = 5.0f;
}

// INLINED
inline CGGameObject_C_TypeAnimated::CGGameObject_C_TypeAnimated(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeBase(owner, create), m_animState(-1), m_pendingAnimState(-1), m_nextAnimTime(-1), m_sound(0), m_baseKit(NULL), m_stateKit(NULL), m_oneShotKit(NULL) {
    this->ResetAnimState();
}

// INLINED
inline CGGameObject_C_Type_WorldObj::CGGameObject_C_Type_WorldObj(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeBase(owner, create) {
    this->m_worldObject = NULL;
}

// INLINED
inline uint16_t CGGameObject_C_Type_DestructibleBuilding::GetDestructionDoodadSet(int32_t state) const {
    if (this->m_modelData) {
        switch (state >> 1) {
            case 1:
                return this->m_modelData->m_state1DestructionDoodadSet;

            case 2:
                return this->m_modelData->m_state2DestructionDoodadSet;

            case 3:
                return this->m_modelData->m_state3InitDoodadSet;
        }
    }

    return 0;
}

// INLINED
inline void CGGameObject_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGObject_C::SetStorage(storage, saved);
    this->m_gameObject = reinterpret_cast<CGGameObjectData*>(storage + OBJECT_END);
    this->m_gameObjectMirror = saved + CGObject::TotalFieldsSaved();
}

#endif
