#ifndef WOW_SOURCE_MAGIC_MAGICCLIENT_SPELLWORLDOBJECT_H
#define WOW_SOURCE_MAGIC_MAGICCLIENT_SPELLWORLDOBJECT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class CM2Model;
class GameObjectDisplayInfoRec;

class SpellWorldObject {
    public:
    /* 0000 */ uintptr_t m_entity;
    /* 0004 */ CM2Model* m_model;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ float m_facing;
    /* 0018 */ float m_scale;

    static int32_t UpdateCallback(void* param, uintptr_t object, uint64_t param64, uintptr_t param32);

    SpellWorldObject();
    ~SpellWorldObject();
    void Release();
    int32_t Update(uintptr_t object, uint64_t param64, uintptr_t param32);
    void Create(const GameObjectDisplayInfoRec* display);
    void SetFacing(const float& facing);
};

#include "SpellWorldObject.inl"

#endif
