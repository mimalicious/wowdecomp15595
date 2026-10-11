#include "InstanceEncounter_C.h"
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00D12A58
uint32_t CGInstanceEncounter_C::m_numUnits;

// STATIC: 0x00D12A60
ENCOUNTER_UNIT CGInstanceEncounter_C::m_units[16];

// STATIC: 0x00D12B60
TSList<ENCOUNTER_TIMER> CGInstanceEncounter_C::m_timers;

// STATIC: 0x00D12B6C
TSList<ENCOUNTER_OBJECTIVE> CGInstanceEncounter_C::m_objectives;

// STATIC: 0x00F43B50
static uint8_t s_maxResurrections;

// STATIC: 0x00F43B51
static uint8_t s_resurrectionsUsed;

// INLINED
bool ENCOUNTER_UNIT::operator<(const ENCOUNTER_UNIT& other) const {
    if (!this->m_guid.m_guid) {
        return false;
    }

    if (!other.m_guid.m_guid) {
        return true;
    }

    if (this->m_priority == other.m_priority) {
        return static_cast<int32_t>(this - CGInstanceEncounter_C::m_units) - static_cast<int32_t>(&other - CGInstanceEncounter_C::m_units) < 0;
    }

    return this->m_priority - other.m_priority < 0;
}

// OFFSET: 0x0094F4D0
static ENCOUNTER_UNIT* FindUnit(const WOWGUID& guid) {
    for (uint32_t i = 0; i < CGInstanceEncounter_C::m_numUnits; i++) {
        if (CGInstanceEncounter_C::m_units[i].m_guid == guid) {
            return &CGInstanceEncounter_C::m_units[i];
        }
    }

    return NULL;
}

// OFFSET: 0x0094F520/0x1406BF6F0
WOWGUID CGInstanceEncounter_C::GetBoss(uint32_t index) {
    if (static_cast<int32_t>(index) < 0 || static_cast<int32_t>(index) >= static_cast<int32_t>(CGInstanceEncounter_C::m_numUnits)) {
        return WOWGUID::Make(0);
    }

    return CGInstanceEncounter_C::m_units[index].m_guid;
}

// OFFSET: 0x0094F550/0x1406BF710
uint32_t CGInstanceEncounter_C::GetNumBossSlots() {
    return CGInstanceEncounter_C::m_numUnits;
}

// OFFSET: 0x0094F560/0x1406BF720
bool CGInstanceEncounter_C::CanResurrect(int32_t spellId) {
    bool limited = true;
    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (spell && !(spell->m_attributesExH & 0x800000)) {
        limited = false;
    }

    return !(s_maxResurrections > 0 && s_resurrectionsUsed >= s_maxResurrections && limited);
}

// OFFSET: 0x0094F5C0
static void SortUnits(ENCOUNTER_UNIT* units, uint32_t count) {
    if (count < 2) {
        return;
    }

    uint32_t start = (count >> 1) + 1;

    for (;;) {
        if (start > 1) {
            start--;
        } else {
            ENCOUNTER_UNIT temp = units[start - 1];
            units[start - 1] = units[count - 1];
            units[count - 1] = temp;
            count--;

            if (count == 1) {
                return;
            }
        }

        ENCOUNTER_UNIT* parent = &units[start - 1];
        uint32_t index = start;

        while (2 * index <= count) {
            uint32_t childIndex = 2 * index;
            ENCOUNTER_UNIT* child = &units[childIndex - 1];

            if (childIndex < count && *child < units[childIndex]) {
                childIndex++;
                child = &units[childIndex - 1];
            }

            if (!(*parent < *child)) {
                break;
            }

            ENCOUNTER_UNIT temp = *parent;
            *parent = *child;
            *child = temp;
            parent = child;
            index = childIndex;
        }
    }
}

// OFFSET: 0x0094F740
static void OnStateMessage(int32_t type) {
    if (type == 1) {
        s_resurrectionsUsed = 0;
    } else if (type == 9) {
        if (CGInstanceEncounter_C::m_numUnits) {
            SortUnits(CGInstanceEncounter_C::m_units, CGInstanceEncounter_C::m_numUnits);
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_INSTANCE_ENCOUNTER_ENGAGE_UNIT, NULL);
    } else if (type == 10) {
        s_resurrectionsUsed++;
    }
}

// OFFSET: 0x0094F790/0x1406BF9C0
void CGInstanceEncounter_C::EngageUnit(const WOWGUID& guid, uint8_t priority) {
    if (FindUnit(guid) || CGInstanceEncounter_C::m_numUnits == 16) {
        return;
    }

    ENCOUNTER_UNIT* unit = &CGInstanceEncounter_C::m_units[CGInstanceEncounter_C::m_numUnits++];
    unit->m_guid = guid;
    unit->m_priority = priority;

    if (CGInstanceEncounter_C::m_numUnits) {
        SortUnits(CGInstanceEncounter_C::m_units, CGInstanceEncounter_C::m_numUnits);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_INSTANCE_ENCOUNTER_ENGAGE_UNIT, NULL);
}

// OFFSET: 0x0094F800/0x1406BFA50
void CGInstanceEncounter_C::DisengageUnit(const WOWGUID& guid) {
    if (!FindUnit(guid)) {
        return;
    }

    bool found = false;

    for (uint32_t i = 0; i < CGInstanceEncounter_C::m_numUnits - 1; i++) {
        if (CGInstanceEncounter_C::m_units[i].m_guid == guid) {
            found = true;
        }

        if (found) {
            CGInstanceEncounter_C::m_units[i] = CGInstanceEncounter_C::m_units[i + 1];
        }
    }

    CGInstanceEncounter_C::m_numUnits--;

    if (CGInstanceEncounter_C::m_numUnits) {
        SortUnits(CGInstanceEncounter_C::m_units, CGInstanceEncounter_C::m_numUnits);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_INSTANCE_ENCOUNTER_ENGAGE_UNIT, NULL);
}

// OFFSET: 0x0094F8A0
void CGInstanceEncounter_C::SetUnitPriority(const WOWGUID& guid, uint8_t priority) {
    ENCOUNTER_UNIT* unit = FindUnit(guid);

    if (!unit) {
        return;
    }

    unit->m_priority = priority;

    if (CGInstanceEncounter_C::m_numUnits) {
        SortUnits(CGInstanceEncounter_C::m_units, CGInstanceEncounter_C::m_numUnits);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_INSTANCE_ENCOUNTER_ENGAGE_UNIT, NULL);
}

// OFFSET: 0x0094F8F0
static void UpdateObjective(int32_t id, int32_t delta) {
    for (ENCOUNTER_OBJECTIVE* objective = CGInstanceEncounter_C::m_objectives.Head(); objective; objective = CGInstanceEncounter_C::m_objectives.Next(objective)) {
        if (objective->m_id == id) {
            objective->m_progress += delta;
            return;
        }
    }
}

// OFFSET: 0x0094F930
static void OnUnitMessage(int32_t type, WOWGUID guid, uint8_t priority) {
    if (type == 2) {
        CGInstanceEncounter_C::EngageUnit(guid, priority);
    } else if (type == 3) {
        CGInstanceEncounter_C::DisengageUnit(guid);
    } else if (type == 4) {
        CGInstanceEncounter_C::SetUnitPriority(guid, priority);
    }
}

// OFFSET: 0x0094F980
void CGInstanceEncounter_C::EnableObjective(int32_t id) {
    for (ENCOUNTER_OBJECTIVE* objective = CGInstanceEncounter_C::m_objectives.Head(); objective; objective = CGInstanceEncounter_C::m_objectives.Next(objective)) {
        if (objective->m_id == id) {
            return;
        }
    }

    ENCOUNTER_OBJECTIVE* objective = CGInstanceEncounter_C::m_objectives.NewNode(2, 0, 0);
    objective->m_id = static_cast<uint8_t>(id);
    objective->m_progress = 0;
}

// OFFSET: 0x0094FBB0
void CGInstanceEncounter_C::LeaveWorld() {
    if (CGGameUI::m_currentlyReloadingUI) {
        return;
    }

    ClientServices::ClearMessageHandler(SMSG_UPDATE_INSTANCE_ENCOUNTER_UNIT);
    CGInstanceEncounter_C::m_timers.Clear();
}

// OFFSET: 0x0094FBE0
void CGInstanceEncounter_C::ClearExpiredTimers() {
    if (!CGInstanceEncounter_C::m_timers.Head()) {
        return;
    }

    uint32_t now = FrameTime::GetCurTimeMs();
    ENCOUNTER_TIMER* timer = CGInstanceEncounter_C::m_timers.Head();

    while (timer) {
        if (static_cast<int32_t>(now - timer->m_expireTime) < 0) {
            timer = CGInstanceEncounter_C::m_timers.Next(timer);
        } else {
            timer = CGInstanceEncounter_C::m_timers.DeleteNode(timer);
        }
    }
}

// OFFSET: 0x0094FC30
void CGInstanceEncounter_C::DisableObjective(int32_t id) {
    for (ENCOUNTER_OBJECTIVE* objective = CGInstanceEncounter_C::m_objectives.Head(); objective; objective = CGInstanceEncounter_C::m_objectives.Next(objective)) {
        if (objective->m_id == id) {
            CGInstanceEncounter_C::m_objectives.DeleteNode(objective);
            return;
        }
    }
}

// OFFSET: 0x0094FC70
void CGInstanceEncounter_C::AddTimer(uint32_t duration) {
    CGInstanceEncounter_C::ClearExpiredTimers();
    ENCOUNTER_TIMER* timer = CGInstanceEncounter_C::m_timers.NewNode(2, 0, 0);
    timer->m_expireTime = duration + FrameTime::GetCurTimeMs();
}

// OFFSET: 0x0094FD10
static void OnObjectiveMessage(int32_t type, uint8_t value) {
    switch (type) {
        case 0:
            s_maxResurrections = value;
            break;

        case 5:
            CGInstanceEncounter_C::AddTimer(value);
            break;

        case 6:
            CGInstanceEncounter_C::EnableObjective(value);
            break;

        case 8:
            CGInstanceEncounter_C::DisableObjective(value);
            break;
    }
}

// OFFSET: 0x0094FD80
int32_t Packet_SMSG_UPDATE_INSTANCE_ENCOUNTER_UNIT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t type;
    SmartGUID guid;
    uint8_t value;
    uint8_t delta;
    msg->Get(type);

    switch (type) {
        case 0:
        case 5:
        case 6:
        case 8:
            msg->Get(value);
            OnObjectiveMessage(type, value);
            break;

        case 1:
        case 9:
        case 10:
            OnStateMessage(type);
            break;

        case 2:
        case 3:
        case 4:
            *msg >> guid;
            msg->Get(value);
            OnUnitMessage(type, guid, value);
            break;

        case 7:
            msg->Get(value);
            msg->Get(delta);
            UpdateObjective(value, delta);
            break;
    }

    return 1;
}

// OFFSET: 0x0094FE80/0x1406C0230
void CGInstanceEncounter_C::EnterWorld() {
    if (!CGGameUI::m_currentlyReloadingUI) {
        ClientServices::SetMessageHandler(SMSG_UPDATE_INSTANCE_ENCOUNTER_UNIT, Packet_SMSG_UPDATE_INSTANCE_ENCOUNTER_UNIT, NULL);
        CGInstanceEncounter_C::m_numUnits = 0;
    }
}
