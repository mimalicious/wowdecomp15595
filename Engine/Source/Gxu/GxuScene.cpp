#include "Engine/Source/Gxu/GxuScene.h"

// OFFSET: #0x009A8180
GxuScene::~GxuScene() {
}

// PROVEN: 30d1ff52 IDENTICAL
// OFFSET: 0x00612E10
void GxuScene::Clear() {
    for (uint32_t i = 0; i < this->m_objects.Count(); i++) {
        this->m_objects[i]->Release();
    }

    this->m_objects.SetCount(0);
    this->m_lastUpdateTime = 0;
}

// OFFSET: %0x00623950
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

// PROVEN: 6ba8406f IDENTICAL
// OFFSET: 0x00612970
void GxuScene::Render() {
    for (uint32_t i = 0; i < this->m_objects.Count(); i++) {
        this->m_objects[i]->Render();
    }
}

