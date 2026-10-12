#include "WoW/Source/Ui/IncomingResurrection.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// STATIC: 0x00F76BC4/0x140BD21D0
TSHashTable<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID> CGIncomingResurrection::s_incoming;

// OFFSET: 0x009FCF80/0x14079ACD0
bool CGIncomingResurrection::HasIncomingResurrection(const WOWGUID& guid) {
    WOWGUID target = guid;
    INCOMINGRESLIST* list = CGIncomingResurrection::s_incoming.Ptr(static_cast<uint32_t>(target.m_guid), CHashKeyGUID(target.m_guid));

    if (list && list->m_resurrections.m_count) {
        return true;
    }

    CGCorpse_C* corpse = ClntObjMgrFindCorpseByOwner(guid);

    if (!corpse) {
        return false;
    }

    WOWGUID corpseGUID = corpse->m_obj->m_guid;
    list = CGIncomingResurrection::s_incoming.Ptr(static_cast<uint32_t>(corpseGUID.m_guid), CHashKeyGUID(corpseGUID.m_guid));

    return list && list->m_resurrections.m_count;
}

// OFFSET: 0x009FD1E0
void CGIncomingResurrection::Reset() {
    CGIncomingResurrection::s_incoming.Clear();
}

// OFFSET: 0x009FD1F0/0x14079B060
void CGIncomingResurrection::Remove(const WOWGUID& target, const WOWGUID& caster, int32_t spellID) {
    WOWGUID key = target;
    INCOMINGRESLIST* list = CGIncomingResurrection::s_incoming.Ptr(static_cast<uint32_t>(key.m_guid), CHashKeyGUID(key.m_guid));

    if (!list) {
        return;
    }

    for (uint32_t i = 0; i < list->m_resurrections.m_count; i++) {
        INCOMINGRESURRECTION& entry = list->m_resurrections.m_data[i];

        if (entry.m_caster == caster && entry.m_spellID == spellID) {
            if (i != list->m_resurrections.m_count - 1) {
                entry = list->m_resurrections.m_data[list->m_resurrections.m_count - 1];
            }

            list->m_resurrections.m_count--;
        }
    }

    if (!list->m_resurrections.m_count) {
        INCOMINGRESLIST* node = CGIncomingResurrection::s_incoming.Ptr(static_cast<uint32_t>(key.m_guid), CHashKeyGUID(key.m_guid));

        if (node) {
            CGIncomingResurrection::s_incoming.Delete(node);
        } else {
            SErrSetLastError(0x57);
        }
    }

    CGObject_C* object = ClntObjMgrObjectPtr(target, TYPEMASK_CORPSE, __FILE__, __LINE__);

    if (object) {
        WOWGUID owner = static_cast<CGCorpse_C*>(object)->m_corpse->m_corpseFieldOwner;
        Script_SendUnitSignal(owner, SCRIPT_EVENT_INCOMING_RESURRECT_CHANGED);
    } else {
        Script_SendUnitSignal(target, SCRIPT_EVENT_INCOMING_RESURRECT_CHANGED);
    }
}

// OFFSET: 0x009FDA00/0x14079BBB0
void CGIncomingResurrection::Add(const WOWGUID& target, const WOWGUID& caster, int32_t spellID) {
    WOWGUID key = target;
    INCOMINGRESLIST* list = CGIncomingResurrection::s_incoming.Ptr(static_cast<uint32_t>(key.m_guid), CHashKeyGUID(key.m_guid));

    if (!list) {
        list = CGIncomingResurrection::s_incoming.New(static_cast<uint32_t>(key.m_guid), CHashKeyGUID(key.m_guid), 0, 0);
    }

    for (uint32_t i = 0; i < list->m_resurrections.m_count; i++) {
        INCOMINGRESURRECTION& entry = list->m_resurrections.m_data[i];

        if (entry.m_caster == caster && entry.m_spellID == spellID) {
            return;
        }
    }

    INCOMINGRESURRECTION entry;
    entry.m_caster = caster;
    entry.m_spellID = spellID;
    list->m_resurrections.New(entry);
    CGObject_C* object = ClntObjMgrObjectPtr(target, TYPEMASK_CORPSE, __FILE__, __LINE__);

    if (object) {
        WOWGUID owner = static_cast<CGCorpse_C*>(object)->m_corpse->m_corpseFieldOwner;
        Script_SendUnitSignal(owner, SCRIPT_EVENT_INCOMING_RESURRECT_CHANGED);
    } else {
        Script_SendUnitSignal(target, SCRIPT_EVENT_INCOMING_RESURRECT_CHANGED);
    }
}
