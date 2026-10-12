#include "WoW/Source/Ui/SpellActivationOverlay.h"
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/FileDataRec.h"
#include "WoW/Source/DB/Rec/ScreenLocationRec.h"
#include "WoW/Source/DB/Rec/SpellActivationOverlayRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// STATIC: 0x00F768C0/0x140E6C970
CGSpellActivationOverlay::ActiveMoverSpell CGSpellActivationOverlay::s_activeMoverSpells[10];

// STATIC: 0x00F76910/0x140BD1700
TSHashTable<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE> CGSpellActivationOverlay::s_counters;

// STATIC: 0x00F7693C/0x140E6C9C0
TSGrowableArray<CGSpellActivationOverlay::WatchedSpellInfo> CGSpellActivationOverlay::s_availableSpells;

// STATIC: 0x00F7694C/0x140E6C9D8
TSGrowableArray<int32_t> CGSpellActivationOverlay::s_glowableSpells;

// STATIC: 0x00F7695C/0x140BD1750
TSHashTable<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE> CGSpellActivationOverlay::s_petAuras;

// OFFSET: 0x008D71D0/0x14064AC30
void CGSpellActivationOverlay::ShutdownGame() {
    CGSpellActivationOverlay::s_counters.Clear();
    CGSpellActivationOverlay::s_availableSpells.Clear();
    CGSpellActivationOverlay::s_glowableSpells.Clear();
    CGSpellActivationOverlay::s_petAuras.Clear();
    memset(CGSpellActivationOverlay::s_activeMoverSpells, 0, sizeof(CGSpellActivationOverlay::s_activeMoverSpells));
}

// OFFSET: 0x009F2170/0x14078C770
void CGSpellActivationOverlay::ShowSpellOverlay(const SpellActivationOverlayRec* overlay) {
    const FileDataRec* fileData = g_fileDataDB.GetRecord(overlay->m_overlayFileDataID);
    const ScreenLocationRec* location = g_screenLocationDB.GetRecord(overlay->m_screenLocationID);

    if (!fileData || !location) {
        return;
    }

    char texture[260];
    SStrCopy(texture, fileData->m_filepath, sizeof(texture));
    SStrPack(texture, fileData->m_filename, sizeof(texture));
    uint32_t color = overlay->m_color;

    if (overlay->m_soundEntriesID) {
        SI3::PlayUISound(overlay->m_soundEntriesID, 0, 1);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_SHOW, "%d%s%s%f%d%d%d", overlay->m_spellID, texture, static_cast<const char*>(location->m_name), overlay->m_scale, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

// OFFSET: 0x009F22B0/0x14078C940
void CGSpellActivationOverlay::DelOverlayGlowableSpell(int32_t spellId) {
    for (uint32_t i = 0; i < CGSpellActivationOverlay::s_glowableSpells.m_count; i++) {
        if (CGSpellActivationOverlay::s_glowableSpells.m_data[i] == spellId) {
            if (i != CGSpellActivationOverlay::s_glowableSpells.m_count - 1) {
                CGSpellActivationOverlay::s_glowableSpells.m_data[i] = CGSpellActivationOverlay::s_glowableSpells.m_data[CGSpellActivationOverlay::s_glowableSpells.m_count - 1];
            }

            CGSpellActivationOverlay::s_glowableSpells.m_count--;
            return;
        }
    }
}

// OFFSET: 0x009F2300/0x1405EBD20
void CGSpellActivationOverlay::EnableActiveMoverGlow(int32_t classSet, const int32_t* classMask) {
    for (uint32_t i = 0; i < 10; i++) {
        int32_t spellId = CGSpellActivationOverlay::s_activeMoverSpells[i].m_spellId;

        if (spellId && SpellMatchesSpellClass(g_spellDB.GetRecord(spellId), classSet, classMask)) {
            CGSpellActivationOverlay::s_activeMoverSpells[i].m_glow = true;
        }
    }
}

// OFFSET: 0x009F2360/0x14078C990
void CGSpellActivationOverlay::AddOverlayOnAvailableSpell(int32_t spellId) {
    WatchedSpellInfo* info = CGSpellActivationOverlay::s_availableSpells.New();
    info->m_spellId = spellId;
    info->m_usable = false;
}

// OFFSET: 0x009F23D0/0x14078CA40
bool CGSpellActivationOverlay::IsSpellOverlayed(int32_t spellId) {
    HASHKEY_NONE key;
    COUNTERNODE* node = CGSpellActivationOverlay::s_counters.Ptr(spellId, key);

    if (node && node->m_count > 0) {
        return true;
    }

    for (uint32_t i = 0; i < 10; i++) {
        if (CGSpellActivationOverlay::s_activeMoverSpells[i].m_spellId == spellId && CGSpellActivationOverlay::s_activeMoverSpells[i].m_glow) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009F2420/0x14078CAC0
uint32_t CGSpellActivationOverlay::AddOverlayGlowableSpell(int32_t spellId) {
    for (uint32_t i = 0; i < CGSpellActivationOverlay::s_glowableSpells.m_count; i++) {
        if (CGSpellActivationOverlay::s_glowableSpells.m_data[i] == spellId) {
            return i;
        }
    }

    return CGSpellActivationOverlay::s_glowableSpells.Add(1, &spellId);
}

// OFFSET: 0x009F2790/0x14078CFC0
void CGSpellActivationOverlay::ClearActiveMoverSpells() {
    for (uint32_t i = 0; i < 10; i++) {
        ActiveMoverSpell& spell = CGSpellActivationOverlay::s_activeMoverSpells[i];
        int32_t spellId = spell.m_spellId;

        if (!spellId) {
            continue;
        }

        spell.m_glow = false;
        spell.m_usable = false;

        if (spell.m_prevGlow && !CGSpellActivationOverlay::IsSpellOverlayed(spellId)) {
            FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_GLOW_HIDE, "%d", spellId);
        }

        spell.m_prevGlow = false;

        if (spell.m_prevUsable) {
            const SpellActivationOverlayRec* overlay = GetSpellActivationOverlayRec(spell.m_spellId);

            if (overlay) {
                FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_HIDE, "%d", overlay->m_spellID);
            }
        }

        spell.m_prevUsable = false;
        spell.m_spellId = 0;
    }

    for (PETAURANODE* node = CGSpellActivationOverlay::s_petAuras.Head(); node; node = CGSpellActivationOverlay::s_petAuras.Next(node)) {
        if (node->m_wasActive) {
            const SpellActivationOverlayRec* overlay = GetSpellActivationOverlayRec(node->m_spellId);

            if (overlay) {
                FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_HIDE, "%d", overlay->m_spellID);
            }
        }
    }

    CGSpellActivationOverlay::s_petAuras.Clear();
}

// OFFSET: 0x009F3610/0x14078E3A0
void CGSpellActivationOverlay::ApplyChangeCheckSpell(int32_t spellId, int32_t classSet, const int32_t* classMask, int32_t change) {
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell || !SpellMatchesSpellClass(spell, classSet, classMask)) {
        return;
    }

    HASHKEY_NONE key;
    COUNTERNODE* node = CGSpellActivationOverlay::s_counters.Ptr(spellId, key);

    if (!node) {
        node = CGSpellActivationOverlay::s_counters.New(spellId, key, 0, 0);
        node->m_count = 0;
    }

    int32_t count = node->m_count;
    int32_t newCount = count + change <= 0 ? 0 : count + change;
    node->m_count = newCount;

    if (count > 0) {
        if (newCount <= 0) {
            FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_GLOW_HIDE, "%d", spellId);
        }
    } else if (newCount > 0) {
        FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_GLOW_SHOW, "%d", spellId);
    }
}

// OFFSET: 0x009F3730/0x14078E590
void CGSpellActivationOverlay::EnterWorld() {
    FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_HIDE, NULL);
    TSHashTable<COUNTERNODE, HASHKEY_NONE> counters;
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    for (uint32_t i = 0; i < player->GetAuraCount(); i++) {
        int32_t auraSpellId = player->m_auras[i].m_spellId;

        if (!auraSpellId) {
            continue;
        }

        const SpellRec* auraSpell = g_spellDB.GetRecord(auraSpellId);

        if (!auraSpell) {
            continue;
        }

        const SpellActivationOverlayRec* overlay = GetSpellActivationOverlayRec(auraSpell->m_id);

        if (!overlay || overlay->m_triggerType) {
            continue;
        }

        CGSpellActivationOverlay::ShowSpellOverlay(overlay);

        for (uint32_t j = 0; j < CGSpellActivationOverlay::s_glowableSpells.m_count; j++) {
            const SpellRec* spell = g_spellDB.GetRecord(CGSpellActivationOverlay::s_glowableSpells.m_data[j]);

            if (spell && SpellMatchesSpellClass(spell, Spell_C_GetSpellClassSet(auraSpell), overlay->m_spellClassMask)) {
                int32_t spellId = spell->m_id;
                HASHKEY_NONE key;
                COUNTERNODE* node = counters.Ptr(spellId, key);

                if (!node) {
                    node = counters.New(spellId, key, 0, 0);
                    node->m_count = 0;
                }

                node->m_count++;
            }
        }
    }

    uint32_t count = CGSpellActivationOverlay::s_availableSpells.m_count;

    for (uint32_t i = 0; i < count; i++) {
        if (!CGSpellActivationOverlay::s_availableSpells.m_data[i].m_usable) {
            continue;
        }

        const SpellRec* availableSpell = g_spellDB.GetRecord(CGSpellActivationOverlay::s_availableSpells.m_data[i].m_spellId);

        if (!availableSpell) {
            continue;
        }

        const SpellActivationOverlayRec* overlay = GetSpellActivationOverlayRec(availableSpell->m_id);
        CGSpellActivationOverlay::ShowSpellOverlay(overlay);

        for (uint32_t j = 0; j < CGSpellActivationOverlay::s_glowableSpells.m_count; j++) {
            const SpellRec* spell = g_spellDB.GetRecord(CGSpellActivationOverlay::s_glowableSpells.m_data[j]);

            if (spell && SpellMatchesSpellClass(spell, Spell_C_GetSpellClassSet(availableSpell), overlay->m_spellClassMask)) {
                int32_t spellId = spell->m_id;
                HASHKEY_NONE key;
                COUNTERNODE* node = counters.Ptr(spellId, key);

                if (!node) {
                    node = counters.New(spellId, key, 0, 0);
                    node->m_count = 0;
                }

                node->m_count++;
            }
        }
    }

    for (COUNTERNODE* node = CGSpellActivationOverlay::s_counters.Head(); node; node = CGSpellActivationOverlay::s_counters.Next(node)) {
        int32_t oldCount = node->m_count;
        HASHKEY_NONE key;
        COUNTERNODE* newNode = counters.Ptr(node->m_hashval, key);
        int32_t newCount = newNode ? newNode->m_count : 0;
        node->m_count = newCount;

        if (oldCount) {
            if (!newCount) {
                FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_GLOW_HIDE, "%d", node->m_hashval);
            }
        } else if (newCount) {
            FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_GLOW_SHOW, "%d", node->m_hashval);
        }
    }

    for (COUNTERNODE* node = counters.Head(); node; node = counters.Next(node)) {
        uint32_t spellId = node->m_hashval;
        HASHKEY_NONE key;

        if (CGSpellActivationOverlay::s_counters.Ptr(spellId, key)) {
            continue;
        }

        COUNTERNODE* newNode = CGSpellActivationOverlay::s_counters.New(spellId, key, 0, 0);
        newNode->m_count = node->m_count;

        if (newNode->m_count) {
            FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_GLOW_SHOW, "%d", spellId);
        }
    }

    counters.Clear();
}

// OFFSET: 0x009F3D40/0x14078F340
void CGSpellActivationOverlay::UpdateActiveMoverOverlays() {
    if (ClntObjMgrGetActivePlayer() == CGUnit_C::s_activeMover) {
        return;
    }

    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return;
    }

    for (uint32_t i = 0; i < 10; i++) {
        CGSpellActivationOverlay::s_activeMoverSpells[i].m_glow = false;
        CGSpellActivationOverlay::s_activeMoverSpells[i].m_usable = false;
    }

    for (PETAURANODE* node = CGSpellActivationOverlay::s_petAuras.Head(); node; node = CGSpellActivationOverlay::s_petAuras.Next(node)) {
        node->m_active = false;
    }

    for (uint32_t i = 0; i < mover->GetAuraCount(); i++) {
        int32_t spellId = mover->m_auras[i].m_spellId;
        const SpellActivationOverlayRec* overlay = GetSpellActivationOverlayRec(spellId);

        if (!overlay || overlay->m_triggerType) {
            continue;
        }

        HASHKEY_NONE key;
        PETAURANODE* node = CGSpellActivationOverlay::s_petAuras.Ptr(spellId, key);

        if (!node) {
            node = CGSpellActivationOverlay::s_petAuras.New(spellId, key, 0, 0);
            node->m_spellId = spellId;
            node->m_wasActive = false;
        }

        node->m_active = true;
    }

    for (PETAURANODE* node = CGSpellActivationOverlay::s_petAuras.Head(); node; node = CGSpellActivationOverlay::s_petAuras.Next(node)) {
        int32_t spellId = node->m_spellId;
        const SpellActivationOverlayRec* overlay = GetSpellActivationOverlayRec(spellId);

        if (!overlay) {
            continue;
        }

        if (node->m_active) {
            const SpellRec* spell = g_spellDB.GetRecord(spellId);

            if (!node->m_wasActive) {
                CGSpellActivationOverlay::ShowSpellOverlay(overlay);
            }

            CGSpellActivationOverlay::EnableActiveMoverGlow(Spell_C_GetSpellClassSet(spell), overlay->m_spellClassMask);
        } else if (node->m_wasActive) {
            FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_HIDE, "%d", overlay->m_spellID);
        }
    }

    WOWGUID target = CGGameUI::m_lockedTarget;

    for (uint32_t i = 0; i < 10; i++) {
        ActiveMoverSpell& moverSpell = CGSpellActivationOverlay::s_activeMoverSpells[i];
        int32_t spellId = moverSpell.m_spellId;

        if (!spellId) {
            continue;
        }

        const SpellActivationOverlayRec* overlay = GetSpellActivationOverlayRec(spellId);

        if (!overlay || overlay->m_triggerType != 1) {
            continue;
        }

        const SpellRec* spell = g_spellDB.GetRecord(spellId);
        CGUnit_C* spellTarget = Spell_C_GetImplicitSpellTarget(mover, spell, target);
        bool usable = Spell_C_AuraStateValid(mover, spell, spellTarget);

        if (spell->m_attributesEx & 0x500000) {
            bool comboPoints = CGPetInfo::GetComboPoints(target, (spell->m_attributesExH & 0x80000) != 0) || mover->HasIgnoreAuraState(spell);
            usable = usable && comboPoints;
        }

        moverSpell.m_usable = usable;

        if (usable) {
            CGSpellActivationOverlay::EnableActiveMoverGlow(Spell_C_GetSpellClassSet(spell), overlay->m_spellClassMask);
        }

        if (moverSpell.m_usable != moverSpell.m_prevUsable) {
            if (moverSpell.m_usable) {
                CGSpellActivationOverlay::ShowSpellOverlay(overlay);
            } else {
                FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_HIDE, "%d", overlay->m_spellID);
            }
        }
    }

    for (uint32_t i = 0; i < 10; i++) {
        ActiveMoverSpell& moverSpell = CGSpellActivationOverlay::s_activeMoverSpells[i];

        if (!moverSpell.m_spellId) {
            continue;
        }

        if (moverSpell.m_glow != moverSpell.m_prevGlow) {
            if (moverSpell.m_glow) {
                FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_GLOW_SHOW, "%d", moverSpell.m_spellId);
            } else {
                FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_GLOW_HIDE, "%d", moverSpell.m_spellId);
            }
        }

        moverSpell.m_prevGlow = moverSpell.m_glow;
        moverSpell.m_prevUsable = moverSpell.m_usable;
    }

    for (PETAURANODE* node = CGSpellActivationOverlay::s_petAuras.Head(); node; node = CGSpellActivationOverlay::s_petAuras.Next(node)) {
        node->m_wasActive = node->m_active;
    }
}

// OFFSET: 0x009F41E0/0x14078FA00
void CGSpellActivationOverlay::ApplyChangeCheckSpells(int32_t classSet, const int32_t* classMask, int32_t change) {
    if (!classSet) {
        return;
    }

    for (uint32_t i = 0; i < CGSpellActivationOverlay::s_glowableSpells.m_count; i++) {
        CGSpellActivationOverlay::ApplyChangeCheckSpell(CGSpellActivationOverlay::s_glowableSpells.m_data[i], classSet, classMask, change);
    }
}

// OFFSET: 0x009F4230/0x14078FA90
void CGSpellActivationOverlay::AddActiveMoverSpells() {
    for (uint32_t i = 0; i < 10; i++) {
        ActiveMoverSpell& moverSpell = CGSpellActivationOverlay::s_activeMoverSpells[i];
        moverSpell.m_prevGlow = false;
        moverSpell.m_glow = false;
        moverSpell.m_prevUsable = false;
        moverSpell.m_usable = false;
        PetAction* action = GetPetAction(i);
        uint32_t data;
        uint32_t type;

        if (action && (data = action->m_data) != 0 && ((type = (data >> 24) & 0x3F) == 1 || type - 8 <= 9)) {
            moverSpell.m_spellId = data & 0xFFFFFF;
        } else {
            moverSpell.m_spellId = 0;
        }
    }

    CGSpellActivationOverlay::UpdateActiveMoverOverlays();
}

// OFFSET: 0x009F42C0/0x14078FB40
void CGSpellActivationOverlay::OnAuraAdded(const SpellRec* spell) {
    const SpellActivationOverlayRec* overlay = GetSpellActivationOverlayRec(spell->m_id);

    if (overlay && !overlay->m_triggerType) {
        CGSpellActivationOverlay::ShowSpellOverlay(overlay);
        CGSpellActivationOverlay::ApplyChangeCheckSpells(Spell_C_GetSpellClassSet(spell), overlay->m_spellClassMask, 1);
    }
}

// OFFSET: 0x009F4300/0x14078FB90
void CGSpellActivationOverlay::OnAuraRemoved(const SpellRec* spell) {
    const SpellActivationOverlayRec* overlay = GetSpellActivationOverlayRec(spell->m_id);

    if (overlay && !overlay->m_triggerType) {
        FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_HIDE, "%d", overlay->m_spellID);
        CGSpellActivationOverlay::ApplyChangeCheckSpells(Spell_C_GetSpellClassSet(spell), overlay->m_spellClassMask, -1);
    }
}

// OFFSET: 0x009F4350/0x14078FBF0
void CGSpellActivationOverlay::DelOverlayOnAvailableSpell(int32_t spellId) {
    int32_t index;

    for (index = 0; index < static_cast<int32_t>(CGSpellActivationOverlay::s_availableSpells.m_count); index++) {
        if (CGSpellActivationOverlay::s_availableSpells.m_data[index].m_spellId == spellId) {
            break;
        }
    }

    if (index >= static_cast<int32_t>(CGSpellActivationOverlay::s_availableSpells.m_count)) {
        return;
    }

    if (CGSpellActivationOverlay::s_availableSpells.m_data[index].m_usable) {
        const SpellActivationOverlayRec* overlay = GetSpellActivationOverlayRec(spellId);

        if (overlay) {
            const SpellRec* spell = g_spellDB.GetRecord(spellId);
            FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_HIDE, "%d", overlay->m_spellID);
            CGSpellActivationOverlay::ApplyChangeCheckSpells(Spell_C_GetSpellClassSet(spell), overlay->m_spellClassMask, -1);
        }
    }

    uint32_t last = CGSpellActivationOverlay::s_availableSpells.m_count - 1;

    if (index != last) {
        CGSpellActivationOverlay::s_availableSpells.m_data[index] = CGSpellActivationOverlay::s_availableSpells.m_data[last];
    }

    CGSpellActivationOverlay::s_availableSpells.m_count--;
}

// OFFSET: 0x009F4420/0x14078FD00
void CGSpellActivationOverlay::UpdateUsable() {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    WOWGUID target = CGGameUI::m_lockedTarget;
    int32_t count = CGSpellActivationOverlay::s_availableSpells.m_count;

    for (int32_t i = 0; i < count; i++) {
        WatchedSpellInfo* info = &CGSpellActivationOverlay::s_availableSpells.m_data[i];
        const SpellRec* spell = g_spellDB.GetRecord(info->m_spellId);
        CGUnit_C* spellTarget = Spell_C_GetImplicitSpellTarget(player, spell, target);
        bool usable = Spell_C_AuraStateValid(player, spell, spellTarget) && Spell_C_IsValidTarget(player, spell, spellTarget);

        if (spell->m_attributesEx & 0x500000) {
            bool comboPoints = CGGameUI::GetComboPoints(target, (spell->m_attributesExH & 0x80000) != 0) || player->HasIgnoreAuraState(spell);
            usable = usable && comboPoints;
        }

        if (usable == info->m_usable) {
            continue;
        }

        const SpellActivationOverlayRec* overlay = GetSpellActivationOverlayRec(spell->m_id);
        int32_t classSet = Spell_C_GetSpellClassSet(spell);

        if (usable) {
            CGSpellActivationOverlay::ShowSpellOverlay(overlay);
            CGSpellActivationOverlay::ApplyChangeCheckSpells(classSet, overlay->m_spellClassMask, 1);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_ACTIVATION_OVERLAY_HIDE, "%d", overlay->m_spellID);
            CGSpellActivationOverlay::ApplyChangeCheckSpells(classSet, overlay->m_spellClassMask, -1);
        }

        info->m_usable = usable;
    }
}
