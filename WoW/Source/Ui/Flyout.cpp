#include "Flyout.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellFlyoutItemRec.h"
#include "WoW/Source/DB/Rec/SpellFlyoutRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00F4B9D0
TSHashTable<CFlyout, HASHKEY_NONE> s_flyouts;

// STATIC: 0x00F4B9FC
TSHashTable<SpellToFlyoutMapItem, HASHKEY_NONE> s_spellToFlyout;

// OFFSET: 0x009A3EA0/0x140729600
void CFlyout::DelKnownSpellInternal(int32_t spellId) {
    bool changed = false;
    this->m_numKnownSpells = 0;

    if (!this->m_slots.m_count) {
        return;
    }

    for (uint32_t i = 0; i < this->m_slots.m_count; i++) {
        Slot& slot = this->m_slots.m_data[i];

        if (slot.m_rec->m_spellID == spellId && slot.m_known) {
            slot.m_known = 0;
            changed = true;
        }

        this->m_numKnownSpells |= slot.m_known;
    }

    if (changed) {
        FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_FLYOUT_UPDATE, "%d%d%b", this->m_rec ? this->m_rec->m_id : 0, spellId, false);
    }
}

// OFFSET: 0x009A3F10/0x140729690
void CFlyout::AddKnownSpellInternal(int32_t spellId) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    const SpellFlyoutRec* rec = this->m_rec;

    if (!((1 << (player->m_unit->m_unitFieldBytes0[0] - 1)) & rec->m_raceMask) || !((1 << (player->m_unit->m_unitFieldBytes0[1] - 1)) & rec->m_classMask)) {
        return;
    }

    bool changed = false;
    this->m_numKnownSpells = 0;

    if (!this->m_slots.m_count) {
        return;
    }

    for (uint32_t i = 0; i < this->m_slots.m_count; i++) {
        Slot& slot = this->m_slots.m_data[i];

        if (slot.m_rec->m_spellID == spellId && !slot.m_known) {
            changed = true;
            slot.m_known = 1;
        }

        this->m_numKnownSpells |= slot.m_known;
    }

    if (changed) {
        FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_FLYOUT_UPDATE, "%d%d%b", this->m_rec ? this->m_rec->m_id : 0, spellId, true);
    }
}

// OFFSET: 0x009A4040
void CFlyout::ShutdownGame() {
    s_spellToFlyout.Clear();
    s_flyouts.Clear();
}

// OFFSET: 0x009A40A0
CFlyout::~CFlyout() {
    this->m_slots.Clear();
}

// OFFSET: 0x009A4100
void CFlyout::AddSlot(const SpellFlyoutItemRec* rec) {
    if (!rec) {
        return;
    }

    Slot slot;
    slot.m_rec = rec;
    slot.m_known = 0;
    *this->m_slots.New() = slot;

    for (uint32_t i = this->m_slots.m_count - 1; i; i--) {
        Slot* current = &this->m_slots.m_data[i];

        if (current[-1].m_rec->m_slot <= current->m_rec->m_slot) {
            break;
        }

        Slot swap = current[-1];
        current[-1] = *current;
        *current = swap;
    }
}

// OFFSET: 0x009A4170
CFlyout* CFlyout::GetByID(int32_t id) {
    return s_flyouts.Ptr(id, HASHKEY_NONE());
}

// OFFSET: 0x009A4190
int32_t CFlyout::FindFlyoutForSpell(int32_t spellId) {
    SpellToFlyoutMapItem* item = s_spellToFlyout.Ptr(spellId, HASHKEY_NONE());

    if (!item) {
        return 0;
    }

    for (uint32_t i = 0; i < item->m_flyouts.m_count; i++) {
        CFlyout* flyout = item->m_flyouts.m_data[i];

        if (flyout && flyout->m_numKnownSpells) {
            return flyout->m_rec ? flyout->m_rec->m_id : 0;
        }
    }

    return 0;
}

// OFFSET: 0x009A41E0/0x140729C00
bool CFlyout::AddKnownSpell(int32_t spellId) {
    SpellToFlyoutMapItem* item = s_spellToFlyout.Ptr(spellId, HASHKEY_NONE());

    if (!item) {
        return false;
    }

    for (uint32_t i = 0; i < item->m_flyouts.m_count; i++) {
        CFlyout* flyout = item->m_flyouts.m_data[i];

        if (!flyout) {
            continue;
        }

        uint32_t known = flyout->m_numKnownSpells;
        flyout->AddKnownSpellInternal(spellId);

        if (flyout->m_numKnownSpells && !known) {
            CGSpellBook::AddFlyout(flyout->m_rec ? flyout->m_rec->m_id : 0, spellId);
        }
    }

    return true;
}

// OFFSET: 0x009A4270
void CFlyout::DelKnownSpell(int32_t spellId) {
    SpellToFlyoutMapItem* item = s_spellToFlyout.Ptr(spellId, HASHKEY_NONE());

    if (!item) {
        return;
    }

    for (uint32_t i = 0; i < item->m_flyouts.m_count; i++) {
        if (item->m_flyouts.m_data[i]) {
            item->m_flyouts.m_data[i]->DelKnownSpellInternal(spellId);
        }
    }
}

// OFFSET: 0x009A42C0
void CFlyout::ClearKnownSpells() {
    for (CFlyout* flyout = s_flyouts.Head(); flyout; flyout = s_flyouts.Next(flyout)) {
        for (uint32_t i = 0; i < flyout->m_slots.m_count; i++) {
            flyout->m_slots.m_data[i].m_known = 0;
        }

        flyout->m_numKnownSpells = 0;
    }
}

// OFFSET: 0x009A5670/0x14072B790
void CFlyout::InitializeGame() {
    for (int32_t i = 0; i < g_spellFlyoutItemDB.m_numRecords; i++) {
        SpellFlyoutRec* rec = g_spellFlyoutDB.GetRecordByIndex(i);

        if (!rec) {
            continue;
        }

        CFlyout* flyout = s_flyouts.New(rec->m_id, HASHKEY_NONE(), 0, 0);
        flyout->m_rec = rec;
    }

    for (int32_t i = 0; i < g_spellFlyoutItemDB.m_numRecords; i++) {
        SpellFlyoutItemRec* rec = g_spellFlyoutItemDB.GetRecordByIndex(i);

        if (!rec) {
            continue;
        }

        int32_t spellId = rec->m_spellID;
        SpellToFlyoutMapItem* item = s_spellToFlyout.Ptr(spellId, HASHKEY_NONE());

        if (!item) {
            item = s_spellToFlyout.New(spellId, HASHKEY_NONE(), 0, 0);
        }

        CFlyout* flyout = s_flyouts.Ptr(rec->m_spellFlyoutID, HASHKEY_NONE());

        if (!flyout) {
            continue;
        }

        *item->m_flyouts.New() = flyout;
        flyout->AddSlot(rec);
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (player && player->IsSpellKnown(rec->m_spellID)) {
            flyout->AddKnownSpellInternal(rec->m_spellID);
        }
    }
}
