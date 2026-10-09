#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITDISPLAYTRANSITION_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITDISPLAYTRANSITION_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"

class CGUnit_C;
class CM2Model;
class CreatureDisplayInfoRec;
class CreatureDisplayInfoExtraRec;
class CreatureModelDataRec;
class CreatureSoundDataRec;

class CUnitDisplayTransition_C {
    public:
    /* 0000 */ uint32_t m_memHandle;
    /* 0004 */ CGUnit_C* m_unit;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ int32_t m_state;
    /* 0010 */ uint32_t m_race;
    /* 0014 */ CreatureDisplayInfoRec* m_displayInfo;
    /* 0018 */ CreatureDisplayInfoExtraRec* m_displayInfoExtra;
    /* 001C */ CreatureModelDataRec* m_modelData;
    /* 0020 */ CreatureSoundDataRec* m_soundData;
    /* 0024 */ CM2Model* m_model;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uintptr_t m_worldObject;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034 [2];
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ CM2Model* m_effectModel;
    /* 0048 */ uint32_t unk_0048 [3];

    int32_t GetTargetDisplayID() const;
    int32_t UpdateState(int32_t a2);
    void Expedite(int32_t a2);

    static CUnitDisplayTransition_C* Create(CGUnit_C* unit, int32_t race, int32_t displayId, int32_t a4, uint32_t anim, uint32_t endAnim, const char* modelName, int32_t attachment, int32_t fadeTime);
    void Complete(int32_t swap);
    void Destroy();
    void UpdateTransform(C44Matrix matrix);
    void UpdateWorldObject(const C44Matrix& matrix, const CAaBox& box, const CAaSphere& sphere, const C3Vector& position, int32_t a6, int32_t keyBone);
};

#endif
