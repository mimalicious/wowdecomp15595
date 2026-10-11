#include "CGGameObject_C_Type_Transport.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/TransportAnimationRec.h"
#include "WoW/Source/DB/Rec/TransportRotationRec.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/GameObject.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"
#include <cmath>
#include "WoW/Source/WorldClient/MapEntity.h"

// OFFSET: 0x0059AB90/0x140209300
int32_t FindTransportAnimationIndex(int32_t transportId) {
    int32_t high = g_transportAnimationDB.m_numRecords;
    int32_t low = 0;
    const TransportAnimationRec* first = g_transportAnimationDB.GetRecordByIndex(0);

    if (high > 0) {
        do {
            int32_t mid = (high + low) / 2;

            if (first[mid].m_transportID < transportId) {
                low = mid + 1;
            } else {
                high = mid;
            }
        } while (low < high);

        if (low < g_transportAnimationDB.m_numRecords && first[low].m_transportID == transportId) {
            return low;
        }
    }

    return -1;
}

// OFFSET: 0x0059AC00/0x140209380
int32_t FindTransportRotationIndex(int32_t transportId) {
    int32_t high = g_transportRotationDB.m_numRecords;
    int32_t low = 0;
    const TransportRotationRec* first = g_transportRotationDB.GetRecordByIndex(0);

    if (high > 0) {
        do {
            int32_t mid = (high + low) / 2;

            if (first[mid].m_gameObjectsID < transportId) {
                low = mid + 1;
            } else {
                high = mid;
            }
        } while (low < high);

        if (low < g_transportRotationDB.m_numRecords && first[low].m_gameObjectsID == transportId) {
            return low;
        }
    }

    return -1;
}

// OFFSET: 0x005A3330/0x140213940
CGGameObject_C_Type_Transport::CGGameObject_C_Type_Transport(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_Type_TransportBase(owner, create), m_frames(NULL), m_frameCount(0), m_currentFrame(0), m_rotations(NULL), m_rotationCount(0), m_currentRotation(0), m_sequence(660), m_stateChangeStartProgress(1.0f), m_stopFrameCount(9) {
    uint32_t stopFrameCount = create.m_stopFrames.m_count;
    const uint32_t* stopFrames = create.m_stopFrames.m_data;

    for (uint32_t i = 0; i < stopFrameCount; i++) {
        this->m_stopFrames[i] = stopFrames[i];
    }

    this->m_stopFrameCount = stopFrameCount;
    int32_t index = FindTransportAnimationIndex(owner->m_obj->m_entryID);

    if (index != -1) {
        this->m_frames = reinterpret_cast<TransportFrame*>(g_transportAnimationDB.GetRecordByIndex(index));
        this->m_frameCount = 1;
        int32_t numRecords = g_transportAnimationDB.m_numRecords;
        C33Matrix rotation(reinterpret_cast<const C4Quaternion&>(this->m_owner->m_gameObject->m_parentRotation));
        C3Vector position = this->m_owner->m_passenger.GetPosition(this->m_owner->m_passenger.m_position);

        for (int32_t i = index + 1; i < numRecords; i++) {
            const TransportAnimationRec* rec = g_transportAnimationDB.GetRecordByIndex(i);

            if (rec->m_transportID != static_cast<int32_t>(owner->m_obj->m_entryID)) {
                break;
            }

            this->m_frameCount++;

            if (!this->m_inLiquid) {
                float x = rec->m_pos[0];
                float y = rec->m_pos[1];
                float z = rec->m_pos[2];
                C3Vector point;
                point.x = position.x + (rotation.c0 * z + rotation.b0 * y + x * rotation.a0);
                point.y = position.y + (rotation.c1 * z + rotation.b1 * y + rotation.a1 * x);
                point.z = position.z + (z * rotation.c2 + y * rotation.b2 + x * rotation.a2);
                uint32_t liquidType;
                float height;

                if (World::QueryLiquid(point, liquidType, height)) {
                    this->m_inLiquid = 1;
                }
            }
        }
    }

    index = FindTransportRotationIndex(owner->m_obj->m_entryID);

    if (index != -1) {
        this->m_rotations = g_transportRotationDB.GetRecordByIndex(index);
        this->m_rotationCount = 1;
        int32_t numRecords = g_transportRotationDB.m_numRecords;

        for (int32_t i = index + 1; i < numRecords; i++) {
            const TransportRotationRec* rec = g_transportRotationDB.GetRecordByIndex(i);

            if (rec->m_gameObjectsID != static_cast<int32_t>(owner->m_obj->m_entryID)) {
                break;
            }

            this->m_rotationCount++;
        }
    }

    this->InitializeMovement();
}

// OFFSET: 0x0059AC70
uint32_t CGGameObject_C_Type_Transport::GetSequenceTimeFromRealTime(int32_t state, uint32_t time) const {
    uint32_t count = this->m_frameCount;

    if (!count) {
        return time;
    }

    uint32_t period = this->m_frames[count - 1].m_time;

    if (!this->m_stopFrameCount) {
        return time % period;
    }

    uint32_t target = state == 24 ? 0 : this->m_stopFrames[state - 25];
    uint32_t endTime = this->m_stateChangeEndTime;

    if (static_cast<int32_t>(time - endTime) >= 0) {
        return target;
    }

    double duration = static_cast<double>(period);
    double targetTime = static_cast<double>(target);
    float targetProgress = static_cast<float>(targetTime / duration);
    float startProgress = this->m_stateChangeStartProgress;
    float t = static_cast<float>(static_cast<double>(time - this->m_stateChangeStartTime) / static_cast<double>(endTime - this->m_stateChangeStartTime));
    float progress;

    if (startProgress >= 0.0f) {
        float endProgress = state == 24 ? 1.0f : static_cast<float>(targetTime / duration);
        float delta = endProgress - startProgress;

        if (delta < 0.0f) {
            delta = delta + 1.0f;
        }

        progress = delta * t + startProgress;

        if (progress > 1.0f) {
            progress = progress - 1.0f;
        }
    } else {
        float start = fabs(startProgress);
        float delta = start - targetProgress;

        if (start - targetProgress < 0.0f) {
            delta = delta + 1.0f;
        }

        progress = start - delta * t;

        if (progress < 0.0f) {
            progress = progress + 1.0f;
        }
    }

    return static_cast<uint32_t>(static_cast<float>(duration) * progress);
}

// OFFSET: 0x005A35D0
uint32_t CGGameObject_C_Type_Transport::GetSequenceTimeFromRealTime(uint32_t time) const {
    return this->GetSequenceTimeFromRealTime(this->m_owner->m_gameObject->m_state, time);
}

// OFFSET: 0x005A0C80
void CGGameObject_C_Type_Transport::Reenable() {
    CGGameObject_C_Type_TransportBase::Reenable();
    this->InitializeMovement();
}

// OFFSET: 0x00599560/0x140207950
void CGGameObject_C_Type_Transport::PostInitWithStats() {
    this->CreateWorldObj();

    if (this->m_worldObject) {
        MovementAddTransport(this->m_owner);
    }

    if (this->m_worldObject) {
        World::ObjectSetSequenceFinishedCallback(this->m_worldObject, &CGGameObject_C_Type_WorldObj::SequenceFinishedHandler, 0, 0);
        World::ObjectUpdate(this->m_worldObject, this->m_owner->GetMatrix());
    }
}

// OFFSET: 0x005995D0
void CGGameObject_C_Type_Transport::PostReenable() {
    this->m_sequence = 660;
    this->CreateWorldObj();

    if (this->m_worldObject) {
        MovementAddTransport(this->m_owner);
        World::ObjectUpdate(this->m_worldObject, this->m_owner->GetMatrix());
    }

    if (this->m_worldObject) {
        World::ObjectSetSequenceFinishedCallback(this->m_worldObject, &CGGameObject_C_Type_WorldObj::SequenceFinishedHandler, 0, 0);
    }
}

// OFFSET: 0x005A0B30/0x140210670
void CGGameObject_C_Type_Transport::InitializeMovement() {
    CGGameObject_C* owner = this->m_owner;
    this->m_transportState = owner->m_gameObject->m_state;
    float progress = owner->m_gameObject->m_pathProgress * 0.000015259022f;
    this->m_stateChangeStartProgress = progress;

    if (owner->m_gameObject->m_flags & 0x40) {
        this->m_stateChangeStartProgress = progress * -1.0f;
    }

    uint32_t time = owner->m_transportTimeOffset + FrameTime::GetCurTimeMs();
    this->m_pathTime = time;
    this->m_stateChangeStartTime = time;
    this->m_stateChangeEndTime = this->m_owner->m_gameObject->m_level;
    C3Vector offset(0.0f, 0.0f, 0.0f);
    C4Quaternion rotation;
    rotation.x = 0.0f;
    rotation.y = 0.0f;
    rotation.z = 0.0f;
    rotation.w = 1.0f;
    this->Interpolate(time, &offset, &rotation);
    C3Vector position = this->m_owner->m_passenger.GetPosition(this->m_owner->m_passenger.m_position);
    CGGameObject_C* object = this->m_owner;
    this->m_position.x = position.x + offset.x;
    this->m_position.y = position.y + offset.y;
    this->m_position.z = position.z + offset.z;
    reinterpret_cast<C4QuaternionCompressed&>(object->m_passenger.m_facing) = rotation;
    object->m_passenger.unk_002C |= 0x2;
    C44Matrix& matrix = this->m_owner->m_matrix;
    matrix = this->m_owner->GetRotation();
    matrix.d0 = this->m_position.x;
    matrix.d1 = this->m_position.y;
    matrix.d2 = this->m_position.z;
}

// OFFSET: 0x0059C170
C3Vector CGGameObject_C_Type_Transport::GetMovement(uint32_t time) {
    if (this->m_frameCount <= 1) {
        return C3Vector(0.0f, 0.0f, 0.0f);
    }

    uint32_t next = this->m_frameCount != this->m_currentFrame + 1 ? this->m_currentFrame + 1 : 0;
    TransportFrame* current;
    TransportFrame* following;

    while (true) {
        following = &this->m_frames[next];
        current = &this->m_frames[this->m_currentFrame];

        if (time >= current->m_time && time < following->m_time) {
            break;
        }

        this->m_currentFrame = next;
        next = this->m_frameCount != next + 1 ? next + 1 : 0;
    }

    float t = static_cast<float>(static_cast<double>(time - current->m_time) / static_cast<double>(following->m_time - current->m_time));

    if (this->m_worldObject) {
        uint32_t sequence = current->m_sequenceId;

        if (sequence != this->m_sequence) {
            CGGameObject_C* owner = this->m_owner;
            this->m_sequence = sequence;
            World::ObjectSetSequence(this->m_worldObject, sequence, 0, 0);
            owner->UpdateObjectEffectAnimationStates(sequence);
        }
    }

    C33Matrix rotation(reinterpret_cast<const C4Quaternion&>(this->m_owner->m_gameObject->m_parentRotation));
    float inverse = 1.0f - t;
    float x = current->m_pos[0] * inverse + following->m_pos[0] * t;
    float y = current->m_pos[1] * inverse + following->m_pos[1] * t;
    float z = current->m_pos[2] * inverse + following->m_pos[2] * t;
    C3Vector result;
    result.x = rotation.c0 * z + rotation.b0 * y + x * rotation.a0;
    result.y = rotation.c1 * z + rotation.b1 * y + rotation.a1 * x;
    result.z = rotation.c2 * z + rotation.b2 * y + rotation.a2 * x;
    return result;
}

// OFFSET: 0x0059C340
C4Quaternion CGGameObject_C_Type_Transport::GetRotation(uint32_t time) {
    uint32_t count = this->m_rotationCount;

    if (count <= 1) {
        return this->m_owner->GetRotation();
    }

    uint32_t next = this->m_currentRotation + 1 != count ? this->m_currentRotation + 1 : 0;
    TransportRotationRec* current = &this->m_rotations[this->m_currentRotation];
    TransportRotationRec* following = &this->m_rotations[next];
    uint32_t endTime = following->m_timeIndex;

    if (!endTime && this->m_frames) {
        endTime = this->m_frames[this->m_frameCount - 1].m_time;
    }

    while (!(time >= static_cast<uint32_t>(current->m_timeIndex) && time < endTime)) {
        this->m_currentRotation = next;
        next = this->m_rotationCount != next + 1 ? next + 1 : 0;
        current = &this->m_rotations[this->m_currentRotation];
        following = &this->m_rotations[next];
        endTime = following->m_timeIndex;

        if (!endTime && this->m_frames) {
            endTime = this->m_frames[this->m_frameCount - 1].m_time;
        }
    }

    float t = static_cast<float>(static_cast<double>(time - current->m_timeIndex) / static_cast<double>(static_cast<uint32_t>(following->m_timeIndex - current->m_timeIndex)));
    C4Quaternion rotation = C4Quaternion::Slerp(t, reinterpret_cast<const C4Quaternion&>(current->m_rot), reinterpret_cast<const C4Quaternion&>(following->m_rot));
    return rotation * reinterpret_cast<const C4Quaternion&>(this->m_owner->m_gameObject->m_parentRotation);
}

// OFFSET: 0x0059C570
void CGGameObject_C_Type_Transport::Interpolate(uint32_t time, C3Vector* offset, C4Quaternion* rotation) {
    uint32_t sequenceTime = this->GetSequenceTimeFromRealTime(this->m_owner->m_gameObject->m_state, time);
    *offset = this->GetMovement(sequenceTime);
    *rotation = this->GetRotation(sequenceTime);
}

// OFFSET: 0x0059A8C0
C44Matrix& CGGameObject_C_Type_Transport::UpdateOwnerMatrix() {
    C44Matrix& matrix = this->m_owner->m_matrix;
    matrix = this->m_owner->GetRotation();
    matrix.d0 = this->m_position.x;
    matrix.d1 = this->m_position.y;
    matrix.d2 = this->m_position.z;
    return matrix;
}

// OFFSET: 0x005A0C90
void CGGameObject_C_Type_Transport::UpdateMovement(uint32_t time, uint32_t elapsed) {
    if (!this->m_isLoaded && this->m_worldObject && World::ObjectIsLoaded(this->m_worldObject)) {
        this->MarkTransportLoaded();

        if (!this->m_frameCount && !this->m_rotationCount) {
            World::ObjectUpdate(this->m_worldObject, this->m_owner->GetMatrix());
            return;
        }
    }

    uint32_t step = elapsed;

    if (elapsed > 250) {
        uint32_t pathTime = this->m_owner->m_transportTimeOffset + time - 250;
        this->m_pathTime = pathTime;

        if (this->m_stopFrameCount && static_cast<int32_t>(this->m_stateChangeStartTime - pathTime) >= 0) {
            this->m_pathTime = this->m_stateChangeStartTime;
        }

        C3Vector offset(0.0f, 0.0f, 0.0f);
        C4Quaternion rotation;
        rotation.x = 0.0f;
        rotation.y = 0.0f;
        rotation.z = 0.0f;
        rotation.w = 1.0f;
        this->Interpolate(this->m_pathTime, &offset, &rotation);
        CPassenger& passenger = this->m_owner->m_passenger;
        reinterpret_cast<C4QuaternionCompressed&>(passenger.m_facing) = rotation;
        passenger.unk_002C |= 0x2;
        C3Vector position = this->m_owner->m_passenger.GetPosition(this->m_owner->m_passenger.m_position);
        this->m_position.x = position.x + offset.x;
        this->m_position.y = position.y + offset.y;
        this->m_position.z = position.z + offset.z;
        step = 250;
    }

    uint32_t pathTime = this->m_owner->m_transportTimeOffset + time;
    int32_t state = this->m_transportState;
    this->m_pathTime = pathTime;
    uint32_t sequenceTime = this->GetSequenceTimeFromRealTime(state, pathTime);

    if (sequenceTime == this->m_transportUpdateTime) {
        return;
    }

    this->m_transportUpdateTime = sequenceTime;
    C3Vector offset(0.0f, 0.0f, 0.0f);
    C4Quaternion rotation;
    rotation.x = 0.0f;
    rotation.y = 0.0f;
    rotation.z = 0.0f;
    rotation.w = 1.0f;
    this->Interpolate(this->m_pathTime, &offset, &rotation);
    C3Vector base = this->m_owner->m_passenger.GetPosition(this->m_owner->m_passenger.m_position);
    C3Vector position(base.x + offset.x, base.y + offset.y, base.z + offset.z);
    C3Vector direction(0.0f, 0.0f, 0.0f);
    float speed = this->UpdateCurrentVelocity(step, position, &direction);

    if (this->m_passengers.Head()) {
        bool moving = !(fabs(speed) < 0.00000095367432f);
        int32_t moved;

        if (moving) {
            C44Matrix* matrix = &this->UpdateOwnerMatrix();

            for (CPassenger* passenger = this->m_passengers.Head(); passenger; passenger = this->m_passengers.Next(passenger)) {
                if (passenger->unk_002C & 0x1) {
                    static_cast<CMovement_C*>(passenger)->PassengerMove(time, step, speed, direction, *matrix);
                }
            }

            this->m_position = position;
        }

        CGGameObject_C* owner = this->m_owner;
        reinterpret_cast<C4QuaternionCompressed&>(owner->m_passenger.m_facing) = rotation;
        owner->m_passenger.unk_002C |= 0x2;
        C44Matrix& matrix = this->m_owner->m_matrix;
        matrix = this->m_owner->GetRotation();
        matrix.d0 = this->m_position.x;
        matrix.d1 = this->m_position.y;
        matrix.d2 = this->m_position.z;
        this->MovePassengers(&moved);

        if (moving && this->m_inLiquid) {
            for (CPassenger* passenger = this->m_passengers.Head(); passenger; passenger = this->m_passengers.Next(passenger)) {
                if (passenger->unk_002C & 0x1) {
                    static_cast<CMovement_C*>(passenger)->m_unit->UpdateSwimmingStatus(time, 0);
                }
            }
        }
    } else {
        CGGameObject_C* owner = this->m_owner;
        this->m_position = position;
        reinterpret_cast<C4QuaternionCompressed&>(owner->m_passenger.m_facing) = rotation;
        owner->m_passenger.unk_002C |= 0x2;
        C44Matrix& matrix = this->UpdateOwnerMatrix();

        if (this->m_worldObject) {
            World::ObjectUpdate(this->m_worldObject, matrix);
        }
    }
}

// OFFSET: 0x0059C5F0
void CGGameObject_C_Type_Transport::UpdateState(int32_t oldState, int32_t newState) {
    if (!this->m_stopFrameCount || !this->m_frameCount || this->m_transportState == newState) {
        return;
    }

    uint32_t time = this->m_owner->m_transportTimeOffset + FrameTime::GetCurTimeMs();
    uint32_t sequenceTime = this->GetSequenceTimeFromRealTime(oldState, time);
    uint32_t period = this->m_frames[this->m_frameCount - 1].m_time;
    CGGameObject_C* owner = this->m_owner;
    this->m_stateChangeStartTime = time;
    this->m_stateChangeStartProgress = static_cast<float>(static_cast<double>(sequenceTime) / static_cast<double>(period));
    this->m_stateChangeEndTime = owner->m_gameObject->m_level;
    int32_t forward = newState - oldState;

    if (newState - oldState < 0) {
        forward += this->m_stopFrameCount + 1;
    }

    uint32_t count = this->m_stopFrameCount;
    uint32_t backward = count - newState + oldState + 1;

    if (backward > count) {
        backward += -1 - count;
    }

    if (static_cast<int32_t>(backward) < forward) {
        this->m_stateChangeStartProgress = this->m_stateChangeStartProgress * -1.0f;
    }

    this->m_transportState = newState;
}
