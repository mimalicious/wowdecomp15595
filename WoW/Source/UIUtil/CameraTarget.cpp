#include "WoW/Source/UIUtil/CameraTarget.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"

// OFFSET: 0x00485B40/0x1400A6810
CCameraTarget::CCameraTarget() {
    this->m_types[0] = TARGET_SELF;
    this->m_types[1] = TARGET_SELF;
    this->m_types[2] = TARGET_SELF;
    this->m_types[3] = TARGET_SELF;
    this->m_targetGUID = WOWGUID::Make(0);
    this->m_currentTargetGUID = WOWGUID::Make(0);
    this->m_valid = 0;
}

// OFFSET: 0x00485BC0
void CCameraTarget::UpdateInternal() {
    if (!this->m_valid) {
        return;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(this->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        this->m_valid = 0;
        return;
    }

    this->m_matrix.Identity();
    this->m_matrix.RotateAroundZ(object->GetDisplayFacing());
    C3Vector position = object->GetPosition();
    this->m_matrix.d0 = position.x;
    this->m_matrix.d1 = position.y;
    this->m_matrix.d2 = position.z;
}

// OFFSET: 0x00485CA0
WOWGUID CCameraTarget::CalcTargetGUID(WOWGUID guid) {
    for (int32_t i = 0; i < 4; i++) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            return WOWGUID::Make(0);
        }

        switch (this->m_types[i]) {
            case TARGET_SELF: {
                return guid;
            }

            case TARGET_VEHICLE: {
                CVehicle_C* vehicle = unit->m_vehicle;

                if (!vehicle) {
                    if (!unit->m_vehiclePassenger) {
                        return guid;
                    }

                    vehicle = unit->m_vehiclePassenger->GetVehicle();

                    if (!vehicle) {
                        return guid;
                    }
                }

                CGUnit_C* vehicleUnit = vehicle->m_unit;

                if (!vehicleUnit) {
                    return guid;
                }

                guid = vehicleUnit->m_obj->m_guid;
                break;
            }

            case TARGET_UNIT_TARGET: {
                guid = unit->m_unit->m_unitFieldTarget;

                if (unit->IsActivePlayer()) {
                    guid = CGGameUI::m_lockedTarget;
                }

                break;
            }

            case TARGET_CHANNEL_OBJECT: {
                guid = unit->m_unit->m_unitFieldChannelObject;
                break;
            }
        }
    }

    if (!ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__)) {
        return WOWGUID::Make(0);
    }

    return guid;
}

// OFFSET: 0x00485D90
void CCameraTarget::SetObject(const C44Matrix& matrix, WOWGUID guid, TARGET_TYPE* types, int32_t count) {
    this->m_matrix = matrix;
    this->m_types[0] = count > 0 ? types[0] : TARGET_SELF;
    this->m_types[1] = count > 1 ? types[1] : TARGET_SELF;
    this->m_types[2] = count > 2 ? types[2] : TARGET_SELF;
    this->m_types[3] = count > 3 ? types[3] : TARGET_SELF;
    this->m_targetGUID = this->CalcTargetGUID(guid);
    this->m_valid = 1;
    this->UpdateInternal();
    this->m_currentTargetGUID = this->m_targetGUID;
}

// OFFSET: 0x00485E20
void CCameraTarget::UpdateTarget(WOWGUID guid) {
    this->m_currentTargetGUID = this->CalcTargetGUID(guid);
    this->UpdateInternal();
}
