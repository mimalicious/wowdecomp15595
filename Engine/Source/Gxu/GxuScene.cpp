#include "Engine/Source/Gxu/GxuScene.h"

// STATIC: 0x00CA8554/0x140B48590
GxuScene g_scene;

// PROVEN: 110effb7 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GxuSceneObject::Render() {
}

// PROVEN: 61f0e1f8 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void GxuSceneObject::Update(float elapsed) {
}

// PROVEN: 12e92145 IDENTICAL/IDENTICAL
// OFFSET: 0x009FCE20/0x14090EBB0
bool GxuSceneObject::IsAlive() {
    return true;
}

// PROVEN: d570d883 UNPROVEN/IDENTICAL
// OFFSET: 0x00612880/0x14029DB20
GxuSceneObject::GxuSceneObject() {
}

// OFFSET: [0x00612890/0x14029DB30, 0x006121C0/0x14029D160]
GxuSceneObject::~GxuSceneObject() {
}

// OFFSET: [0x00612BB0/0x14029DF10, 0x00612BB0/0x14029DF10]
GxuScene::~GxuScene() {
}

// PROVEN: c31d9bb2 IDENTICAL/IDENTICAL
// OFFSET: 0x00612E50/0x14029E370
void GxuScene::AddObject(GxuSceneObject* object) {
    this->m_objects.New(object);
}

// PROVEN: 30d1ff52 IDENTICAL/IDENTICAL
// OFFSET: 0x00612E10/0x14029E310
void GxuScene::Clear() {
    for (uint32_t i = 0; i < this->m_objects.Count(); i++) {
        this->m_objects[i]->Release();
    }

    this->m_objects.SetCount(0);
    this->m_lastUpdateTime = 0;
}

// OFFSET: 0x00612E70/0x14029E390
void GxuScene::Update(uint32_t time) {
    if (!this->m_lastUpdateTime) {
        this->m_lastUpdateTime = time;
    }

    uint32_t count = this->m_objects.Count();
    uint32_t removed = 0;
    uint32_t index = 0;
    float elapsed = static_cast<float>(time - this->m_lastUpdateTime) / 1000.0f;

    if (count) {
        while (true) {
            if (removed) {
                this->m_objects[index] = this->m_objects[index + removed];
            }

            GxuSceneObject* object = this->m_objects[index];

            if (object->IsAlive()) {
                object->Update(elapsed);
                index++;
            } else {
                object->Release();
                count--;
                removed++;
            }

            if (index >= count) {
                break;
            }
        }
    }

    this->m_objects.SetCount(count);
    this->m_lastUpdateTime = time;
}

// PROVEN: 6ba8406f IDENTICAL/IDENTICAL
// OFFSET: 0x00612970/0x14029DBF0
void GxuScene::Render() {
    for (uint32_t i = 0; i < this->m_objects.Count(); i++) {
        this->m_objects[i]->Render();
    }
}

