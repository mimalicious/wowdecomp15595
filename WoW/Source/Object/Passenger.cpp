#include "Passenger.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"

// PROVEN: 78fb98d5 IDENTICAL/NONE
// OFFSET: 0x0041F8E0/0x140023B50
CPassenger::CPassenger(const WOWGUID& transportGuid, const C3Vector& position, const WOWGUID& ownerGuid) : m_transportGuid(transportGuid), m_position(position) {
    this->m_facing = 0.0f;
    this->m_pitch = 0.0f;
    this->m_ownerGuid = &ownerGuid;
    this->unk_002C = 0;
}

// OFFSET: 0x0041EB90/0x140022AD0
void CPassenger::PostInit(const C3Vector& position) {
    if (this->m_transportGuid) {
        if (!MovementAddToTransport(this, this->m_transportGuid, 1, 1, 0)) {
            this->m_transportGuid = WOWGUID();
            this->m_position = position;
        }
    }
}

// PROVEN: 5ac83f0c IDENTICAL/NONE
// OFFSET: 0x0041EBE0/0x140022B30
void CPassenger::Reenable(const WOWGUID& transportGuid, const C3Vector& position) {
    this->m_transportGuid = transportGuid;
    this->m_position = position;
}

// OFFSET: 0x0041EC10/0x140022B50
float CPassenger::GetFacing(float facing) const {
    WOWGUID transport = this->m_transportGuid;

    if (!transport || transport == *this->m_ownerGuid) {
        return facing;
    }

    return CMath::normalizeangle0to2pi_(MovementGetTransportFacing(transport) + facing);
}

// OFFSET: 0x0041EC60/0x140022BA0
float CPassenger::GetRawFacing(float facing) const {
    if (!this->m_transportGuid) {
        return facing;
    }

    return CMath::normalizeangle0to2pi_(facing - MovementGetTransportFacing(this->m_transportGuid));
}

// OFFSET: 0x0041F090/0x140022E00
C3Vector CPassenger::GetPosition(const C3Vector& position) const {
    WOWGUID transport = this->m_transportGuid;

    if (!transport) {
        return position;
    }

    C44Matrix matrix;
    MovementGetTransportMtxX(transport, &matrix, *this->m_ownerGuid, __FILE__, __LINE__);
    return position * matrix;
}

// OFFSET: 0x0041F1D0/0x140022F80
C3Vector CPassenger::GetRawPosition(const C3Vector& position) const {
    WOWGUID transport = this->m_transportGuid;

    if (!transport) {
        return position;
    }

    C44Matrix matrix;
    MovementGetTransportMtxX(transport, &matrix, *this->m_ownerGuid, __FILE__, __LINE__);
    return position * matrix.AffineInverse();
}

// OFFSET: 0x0041ECA0
C4Quaternion CPassenger::GetRotation() const {
    C4Quaternion rotation(*reinterpret_cast<const C4QuaternionCompressed*>(&this->m_facing));

    if (this->m_transportGuid.m_guid) {
        C4Quaternion transport;
        transport.x = 0.0f;
        transport.y = 0.0f;
        transport.z = 0.0f;
        transport.w = 1.0f;
        MovementGetTransportQuat(this->m_transportGuid, &transport);
        return rotation * transport;
    }

    return rotation;
}

// OFFSET: 0x0041F330/0x140023110
float CPassenger::GetRawFacingFromRotation() const {
    C4Quaternion rotation = this->GetRotation();
    float x = 1.0f - (rotation.z * rotation.z + rotation.y * rotation.y) * 2.0f;
    float y = (rotation.w * rotation.z + rotation.y * rotation.x) * 2.0f;

    if (2.3841858e-7f <= fabsf(x)) {
        if (fabsf(y) < 2.3841858e-7f) {
            return 0.0f < x ? 0.0f : 3.1415927f;
        }

        return static_cast<float>(atan2(y, x));
    }

    return 0.0f <= y ? 0.5f * 3.1415927f : 1.5f * 3.1415927f;
}

// OFFSET: 0x0041F420/0x1400233C0
float CPassenger::RemoveFromTransportFinish(const C44Matrix& matrix) {
    float facing = MovementGetTransportFacing(this->m_transportGuid);
    this->m_position *= matrix;

    if (this->unk_002C & 0x2) {
        C4Quaternion rotation(*reinterpret_cast<const C4QuaternionCompressed*>(&this->m_facing));
        C4Quaternion transport;
        transport.x = 0.0f;
        transport.y = 0.0f;
        transport.z = 0.0f;
        transport.w = 1.0f;
        MovementGetTransportQuat(this->m_transportGuid, &transport);
        rotation = rotation * transport;
        *reinterpret_cast<C4QuaternionCompressed*>(&this->m_facing) = rotation;
    } else {
        this->m_facing = CMath::normalizeangle0to2pi_(this->m_facing + facing);
    }

    return facing;
}

// OFFSET: 0x0041F930/0x140023B90
float CPassenger::RemoveFromTransport(C44Matrix* matrix) {
    MovementGetTransportMtxX(this->m_transportGuid, matrix, *this->m_ownerGuid, __FILE__, __LINE__);
    return this->RemoveFromTransportFinish(*matrix);
}

// OFFSET: 0x0041F650/0x1400236D0
float CPassenger::AddToTransport(WOWGUID transport, C44Matrix* inverse, C44Matrix* transportMatrix) {
    float facing = -MovementGetTransportFacing(transport);

    if (transportMatrix) {
        MovementGetTransportMtxX(transport, transportMatrix, *this->m_ownerGuid, __FILE__, __LINE__);
        *inverse = transportMatrix->AffineInverse();
    } else {
        MovementGetTransportMtxX(transport, inverse, *this->m_ownerGuid, __FILE__, __LINE__);
        *inverse = inverse->AffineInverse();
    }

    this->m_position *= *inverse;

    if (this->unk_002C & 0x2) {
        C4Quaternion rotation;
        rotation.x = 0.0f;
        rotation.y = 0.0f;
        rotation.z = 0.0f;
        rotation.w = 0.0f;
        rotation.FromRotationMatrix(*inverse);
        C4Quaternion result = C4Quaternion(*reinterpret_cast<const C4QuaternionCompressed*>(&this->m_facing)) * rotation;
        *reinterpret_cast<C4QuaternionCompressed*>(&this->m_facing) = result;
    } else {
        this->m_facing = CMath::normalizeangle0to2pi_(this->m_facing + facing);
    }

    return facing;
}

// OFFSET: 0x0041F970/0x140023BE0
void CPassenger::ForceSetTransport(WOWGUID transport) {
    if (transport == this->m_transportGuid || (transport && !MovementObjIsTransport(transport))) {
        return;
    }

    C44Matrix matrix;

    if (this->m_transportGuid) {
        this->RemoveFromTransport(&matrix);
    }

    if (transport) {
        this->AddToTransport(transport, &matrix, NULL);
    }

    this->m_transportGuid = transport;
}
