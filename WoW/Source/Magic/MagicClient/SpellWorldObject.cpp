#include "SpellWorldObject.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/DB/Rec/GameObjectDisplayInfoRec.h"
#include "WoW/Source/WorldClient/World.h"

// OFFSET: 0x008C1200/0x14060B5F0
void SpellWorldObject::Release() {
    if (this->m_entity) {
        World::ObjectDestroy(this->m_entity);
        this->m_entity = 0;
    }

    if (this->m_model) {
        this->m_model->Release();
        this->m_model = NULL;
    }
}

// OFFSET: 0x008C1230/0x14060B5D0
SpellWorldObject::SpellWorldObject() {
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->m_facing = 0.0f;
    this->m_scale = 1.0f;
    this->m_entity = 0;
    this->m_model = NULL;
}

// OFFSET: 0x008C1270/0x14060B630
int32_t SpellWorldObject::Update(uintptr_t object, uint64_t param64, uintptr_t param32) {
    if (this->m_model) {
        C44Matrix matrix(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, this->m_position.x, this->m_position.y, this->m_position.z, 1.0f);
        matrix.RotateAroundZ(this->m_facing);
        matrix.Scale(this->m_scale);
        this->m_model->m_fFlags |= 0x4000;
        this->m_model->m_worldTransform = matrix;
    }

    return 1;
}

// OFFSET: 0x008C1330/0x14060B7D0
int32_t SpellWorldObject::UpdateCallback(void* param, uintptr_t object, uint64_t param64, uintptr_t param32) {
    return static_cast<SpellWorldObject*>(param)->Update(object, param64, param32);
}

// OFFSET: 0x008C1350/0x14060B7E0
void SpellWorldObject::Create(const GameObjectDisplayInfoRec* display) {
    this->Release();
    const char* name = display->m_modelName;
    this->m_model = World::GetScene()->CreateModel(name, 0, 3);

    if (this->m_model) {
        this->m_entity = World::ObjectCreate(this->m_model, SpellWorldObject::UpdateCallback, this, 0, 0, 10, __FILE__, __LINE__);
    }
}
