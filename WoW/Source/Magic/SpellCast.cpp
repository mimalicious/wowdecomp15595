#include "SpellCast.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/WGUID.h"

// OFFSET: 0x00B3AE10/0x140907B00
void SpellPutCastTargets(const SpellCast* data, CDataStore* msg) {
    msg->Put(data->m_targetFlags);

    if (data->m_targetFlags & 0x18A02) {
        SmartGUID guid;
        guid.m_guid = data->m_objectTarget.m_guid;
        *msg << guid;
    }

    if (data->m_targetFlags & 0x1010) {
        SmartGUID guid;
        guid.m_guid = data->m_itemTarget.m_guid;
        *msg << guid;
    }

    if (data->m_targetFlags & 0x20) {
        SmartGUID guid;
        guid.m_guid = data->m_sourceTarget.m_guid;
        *msg << guid;
        msg->Put(data->m_sourcePos.x);
        msg->Put(data->m_sourcePos.y);
        msg->Put(data->m_sourcePos.z);
    }

    if (data->m_targetFlags & 0x40) {
        SmartGUID guid;
        guid.m_guid = data->m_destTarget.m_guid;
        *msg << guid;
        msg->Put(data->m_destPos.x);
        msg->Put(data->m_destPos.y);
        msg->Put(data->m_destPos.z);
    }

    if (data->m_targetFlags & 0x2000) {
        msg->PutArray(reinterpret_cast<const uint8_t*>(data->m_targetString), sizeof(data->m_targetString));
    }
}

// OFFSET: 0x00B3AF20/0x140907C10
void SpellGetCastTargets(SpellCast* data, CDataStore* msg) {
    msg->Get(data->m_targetFlags);
    SmartGUID guid;

    if (data->m_targetFlags & 0x18A02) {
        *msg >> guid;
        data->m_objectTarget = guid;
    }

    if (data->m_targetFlags & 0x1010) {
        *msg >> guid;
        data->m_itemTarget = guid;
    }

    if (data->m_targetFlags & 0x20) {
        *msg >> guid;
        data->m_sourceTarget = guid;
        msg->Get(data->m_sourcePos.x);
        msg->Get(data->m_sourcePos.y);
        msg->Get(data->m_sourcePos.z);
    }

    if (data->m_targetFlags & 0x40) {
        *msg >> guid;
        data->m_destTarget = guid;
        msg->Get(data->m_destPos.x);
        msg->Get(data->m_destPos.y);
        msg->Get(data->m_destPos.z);
    }

    if (data->m_targetFlags & 0x2000) {
        msg->GetArray(reinterpret_cast<uint8_t*>(data->m_targetString), sizeof(data->m_targetString));
    }
}

// OFFSET: 0x00B3B020/0x140907D30
SpellCast::SpellCast() {
    this->m_castItem = WOWGUID::Make(0);
    this->unk_0010 = 0;
    this->unk_0014 = 0;
    this->m_spellId = 0;
    this->m_castCount = 0;
    this->m_targetFlags = 0;
    this->m_objectTarget = WOWGUID::Make(0);
    this->m_itemTarget = WOWGUID::Make(0);
    this->unk_0038 = WOWGUID::Make(0);
    this->m_sourceTarget = WOWGUID::Make(0);
    this->m_destTarget = WOWGUID::Make(0);
    this->m_sourcePos = C3Vector();
    this->m_destPos = C3Vector();
    this->unk_0068 = 0.0f;
    this->unk_006C = -1;
    this->unk_0070 = 0;
    this->unk_0074 = 0;
    this->unk_0078 = 0;
    this->unk_007C = 0;
    this->unk_0080 = 0;
    this->unk_0084 = 0;
    this->unk_0088 = 0;
    this->unk_008C = 0;
    this->unk_0110 = 0;
    this->m_castFlags = 0;
    this->m_castFlagsEx = 0;
    this->unk_011C = 0;
    this->m_elevation = 0.0f;
    this->m_speed = 0.0f;
    this->m_glyphIndex = 0;
}

// OFFSET: 0x00B3B0E0/0x140907DC0
C3Vector SpellCast::GetDestLocation() const {
    if (!this->m_destTarget.m_guid) {
        return this->m_destPos;
    }

    C44Matrix matrix;
    MovementGetTransportMtxX(this->m_destTarget, &matrix, WOWGUID::Make(0), __FILE__, __LINE__);
    return this->m_destPos * matrix;
}
