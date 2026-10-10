#include "ActionBarFrame.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Frame/CSimpleCheckbox.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include <cstring>
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/OverrideSpellDataRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellFlyoutRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellReagentsRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"
#include "WoW/Source/DB/Rec/SpellTotemsRec.h"
#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/Ui/CGCooldown.h"
#include "WoW/Source/Ui/EquipmentManager.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SpellActivationOverlay.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/UIBindings.h"
#include "WoW/Source/Ui/UIMacros.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

extern CVar* s_cvAutoFilledMultiCastSlots;

// STATIC: 0x00F43BC8
int32_t CGActionBar::m_slotNoMana[144];

// STATIC: 0x00F43E08
int32_t CGActionBar::m_slotUsable[144];

// STATIC: 0x00F44048
int32_t CGActionBar::m_slotUseCount[144];

// STATIC: 0x00F44288
uint32_t CGActionBar::m_slotActions[144];

// STATIC: 0x00F444C8
uint32_t CGActionBar::m_currentPage;

// STATIC: 0x00F444CC
uint32_t CGActionBar::m_bonusBarOffset;

// STATIC: 0x00F444D0
uint32_t CGActionBar::m_tempPageActiveFlags;

// STATIC: 0x00F444D8
TSCGrowableArray<UIButtons, 144> CGActionBar::m_uiButtons;

// STATIC: 0x00F33DC8
TSHashTable<OVERRIDE_SPELL_NODE, HASHKEY_NONE> CGActionBar::m_overrideSpells;

// PROVEN: 2e75b45b IDENTICAL/NONE
// OFFSET: 0x004644D0
int32_t CGActionBar::IsSpell(uint32_t slot) {
    uint32_t action = CGActionBar::m_slotActions[slot];
    return action && !(action & 0xF0000000);
}

// PROVEN: 9bcd55b7 IDENTICAL/NONE
// OFFSET: 0x00464500/0x140077ED0
int32_t CGActionBar::IsItem(uint32_t slot) {
    uint32_t action = CGActionBar::m_slotActions[slot];
    return action && (action & 0xF0000000) == 0x80000000;
}

// PROVEN: 19d316e3 IDENTICAL/NONE
// OFFSET: 0x00464530
int32_t CGActionBar::IsMacro(uint32_t slot) {
    uint32_t action = CGActionBar::m_slotActions[slot];
    return action && (action & 0xF0000000) == 0x40000000;
}

// OFFSET: 0x00464560
int32_t CGActionBar::IsPetAction(uint32_t slot) {
    uint32_t action = CGActionBar::m_slotActions[slot];
    return action && (action & 0xF0000000) == 0x10000000;
}

// OFFSET: 0x00464590/0x140077F60
int32_t CGActionBar::IsFlyout(uint32_t slot) {
    uint32_t action = CGActionBar::m_slotActions[slot];
    return action && (action & 0xF0000000) == 0x30000000;
}

// OFFSET: 0x004645C0
int32_t CGActionBar::IsEquipmentSet(uint32_t slot) {
    uint32_t action = CGActionBar::m_slotActions[slot];
    return action && (action & 0xF0000000) == 0x20000000;
}

// OFFSET: 0x004645F0/0x140077FC0
uint32_t CGActionBar::GetMacroID(uint32_t slot) {
    uint32_t action = CGActionBar::m_slotActions[slot];

    if (action && (action & 0xF0000000) == 0x40000000) {
        return action & 0xB1FFFFFF;
    }

    return 0;
}

// OFFSET: 0x00464620
uint32_t CGActionBar::GetSlotSpellAction(uint32_t slot) {
    uint32_t action = CGActionBar::m_slotActions[slot];

    if (action && (action & 0xF0000000) == 0x10000000) {
        return action & 0xE1FFFFFF;
    }

    return 0;
}

// OFFSET: 0x00464650
int32_t CGActionBar::GetEquipmentSetID(uint32_t slot) {
    uint32_t action = CGActionBar::m_slotActions[slot];

    if (action && (action & 0xF0000000) == 0x20000000) {
        return action & 0xD1FFFFFF;
    }

    return -1;
}

// PROVEN: 85398456 IDENTICAL/NONE
// OFFSET: 0x00464710/0x1400780A0
PetAction* GetPetAction(uint32_t index) {
    if (CGPetInfo::GetPet(0) && index < sizeof(((CGPetInfo*)0)->m_actions) / sizeof(CGPetInfo::m_actions[0])) {
        return &CGPetInfo::m_actions[index];
    }

    return NULL;
}

// PROVEN: 7e12b449 IDENTICAL/NONE
// OFFSET: 0x0094FEB0/0x1405E8860
uint32_t CGPetInfo::GetPetMode() {
    if (CGPetInfo::m_petMode & 0x8000000) {
        return 0;
    }

    return static_cast<uint8_t>(CGPetInfo::m_petMode);
}

// OFFSET: 0x009541B0/0x1406C5060
void CGActionBar::InitializeGame() {
    memset(CGActionBar::m_slotActions, 0, sizeof(CGActionBar::m_slotActions));
    memset(CGActionBar::m_slotUseCount, 0, sizeof(CGActionBar::m_slotUseCount));
    memset(CGActionBar::m_slotUsable, 0, sizeof(CGActionBar::m_slotUsable));
    memset(CGActionBar::m_slotNoMana, 0, sizeof(CGActionBar::m_slotNoMana));
    CGActionBar::m_currentPage = 0;
    CGActionBar::m_bonusBarOffset = 0;
    CGActionBar::m_tempPageActiveFlags = 0;
}

// OFFSET: 0x00954220/0x140750FD0
int32_t CGActionBar::HasMacro(uint32_t slot) {
    return CGUIMacros::GetMacroIndexByID(CGActionBar::m_slotActions[slot] & 0xB1FFFFFF) != -1;
}

// OFFSET: 0x00954250/0x1406C5110
void CGActionBar::ShowGrid() {
    FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SHOWGRID, NULL);
}

// OFFSET: 0x00954260
void CGActionBar::HideGrid() {
    FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_HIDEGRID, NULL);
}

// OFFSET: 0x00954270/0x1406C5130
int32_t CGActionBar::GetMacroItem(uint32_t slot) {
    MACRONODE* macro = CGUIMacros::GetMacroByID(CGActionBar::GetMacroID(slot));

    if (macro) {
        return macro->m_itemID;
    }

    return 0;
}

// OFFSET: 0x009542C0/0x1406C5180
uint32_t CGActionBar::GetFlyout(uint32_t slot) {
    uint32_t action;

    if (slot <= 143 && (action = CGActionBar::m_slotActions[slot]) && (action & 0xF0000000) == 0x30000000) {
        return action & 0xC1FFFFFF;
    }

    return 0;
}

// OFFSET: 0x00954300
static int32_t GetMultiCastTotemCategory(int32_t slot) {
    if (slot < 132 || static_cast<uint32_t>(slot) >= 144) {
        return 0;
    }

    switch ((slot - 132) % 4) {
        case 0:
            return 4;

        case 1:
            return 2;

        case 2:
            return 5;

        case 3:
            return 3;

        default:
            return 0;
    }
}

// OFFSET: 0x00954370
uint32_t CGActionBar::GetMultiCastSlotMaskForTotemCategory(int32_t category) {
    switch (category) {
        case 2:
            return 2;

        case 3:
            return 8;

        case 4:
            return 1;

        case 5:
            return 4;

        case 21:
            return 15;

        default:
            return 0;
    }
}

// PROVEN: 299cac77 IDENTICAL/NONE
// OFFSET: 0x009543E0
void CGActionBar::SetTemporaryActionBarFlags(uint32_t flags, int32_t enable) {
    if (enable) {
        CGActionBar::m_tempPageActiveFlags |= flags;
    } else {
        CGActionBar::m_tempPageActiveFlags &= ~flags;
    }
}

// OFFSET: 0x00954410
static int32_t FormHasActionBar(const SpellShapeshiftFormRec* form) {
    if (!form) {
        return 0;
    }

    for (uint32_t i = 0; i < 8; i++) {
        if (form->m_presetSpellID[i]) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00954440/0x1406C5270
static int32_t GetFormPresetSpell(uint32_t index, const void* data) {
    if (data && index < 8) {
        return static_cast<const SpellShapeshiftFormRec*>(data)->m_presetSpellID[index] | 0x2000000;
    }

    return 0;
}

// OFFSET: 0x00954470/0x1406C5290
static int32_t GetOverrideSpell(uint32_t index, const void* data) {
    if (data && index < 10) {
        return static_cast<const OverrideSpellDataRec*>(data)->m_spells[index] | 0x2000000;
    }

    return 0;
}

// OFFSET: 0x00954920/0x1406C5780
int32_t CGActionBar::GetItem(uint32_t slot) {
    if (slot > 143) {
        return 0;
    }

    uint32_t action = CGActionBar::m_slotActions[slot];

    if (action && (action & 0xF0000000) == 0x40000000 && CGActionBar::HasMacro(slot)) {
        return CGActionBar::GetMacroItem(slot);
    }

    action = CGActionBar::m_slotActions[slot];

    if (action && (action & 0xF0000000) == 0x80000000) {
        return action & 0x71FFFFFF;
    }

    return 0;
}

// OFFSET: 0x00954990/0x1406C5830
int32_t CGActionBar::GetPetActionSpell(uint32_t slot) {
    uint32_t action = CGActionBar::m_slotActions[slot];
    uint32_t index = action && (action & 0xF0000000) == 0x10000000 ? action & 0xE1FFFFFF : 0;
    PetAction* petAction = GetPetAction(index);
    uint32_t data;

    if (!petAction || !(data = petAction->m_data)) {
        return 0;
    }

    uint32_t type = (data >> 24) & 0x3F;

    if (type != 1 && type - 8 > 9) {
        return 0;
    }

    return data & 0xFFFFFF;
}

// OFFSET: 0x00954A10
int32_t IsMultiCastSpellForSlot(int32_t slot, const SpellRec* spell, int32_t* category) {
    int32_t multiCast = (spell->m_attributesExG & 0x20) != 0;
    int32_t totemCategory = GetMultiCastTotemCategory(slot);
    *category = totemCategory;

    if (!multiCast || !totemCategory || !spell->m_spellTotemsID) {
        return multiCast;
    }

    const SpellTotemsRec* totems = g_spellTotemsDB.GetRecord(spell->m_spellTotemsID);

    if (!totems) {
        return multiCast;
    }

    for (uint32_t i = 0; i < 2; i++) {
        if (totems->m_requiredTotemCategoryID[i] == totemCategory) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00954A90/0x1406C5990
int32_t CGActionBar::HasAction(uint32_t slot) {
    uint32_t action = CGActionBar::m_slotActions[slot];

    if (action && (action & 0xF0000000) == 0x40000000 && CGUIMacros::GetMacroIndexByID(action & 0xB1FFFFFF) == -1) {
        return 0;
    }

    action = CGActionBar::m_slotActions[slot];

    if (!action || (action & 0xF0000000) != 0x10000000) {
        return action != 0;
    }

    PetAction* petAction = GetPetAction((slot - 120) & 0xE1FFFFFF);

    if (!petAction || !petAction->m_data) {
        return 0;
    }

    return ((petAction->m_data >> 24) & 0x3F) - 6 <= 1 || CGActionBar::GetPetActionSpell(slot) != 0;
}

// OFFSET: 0x00954E00/0x1406C5E40
void CGActionBar::UpdateBonusBar() {
    uint32_t offset = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        if (!CGActionBar::m_tempPageActiveFlags || (CGActionBar::m_tempPageActiveFlags & 0x8)) {
            const SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(player->GetShapeshiftForm());

            if (form) {
                offset = 5;

                if (!FormHasActionBar(form)) {
                    offset = form->m_bonusActionBar;
                }
            }
        } else {
            offset = 5;
        }
    }

    if (offset != CGActionBar::m_bonusBarOffset) {
        bool wasOverride = CGActionBar::m_bonusBarOffset == 5;
        CGActionBar::m_bonusBarOffset = offset;

        if (wasOverride) {
            FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_PAGE_CHANGED, NULL);
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BONUS_ACTIONBAR, NULL);
    }
}

// OFFSET: 0x00954EB0/0x1406C5F10
const char* CGActionBar::GetAttackTexture() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return NULL;
    }

    uint32_t formId = player->GetShapeshiftForm();
    const SpellShapeshiftFormRec* form;
    const SpellIconRec* icon;

    if (formId && (form = g_spellShapeshiftFormDB.GetRecord(formId)) && form->m_attackIconID && (icon = g_spellIconDB.GetRecord(form->m_attackIconID))) {
        return icon->m_textureFilename;
    }

    if (player->IsDisarmed(0)) {
        return "Interface\\Buttons\\Spell-Reset";
    }

    CGBag_C* bag = player->GetBag();
    WOWGUID guid = static_cast<uint32_t>(bag->m_numSlots) > 15 ? bag->m_slots[15] : WOWGUID();
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item) {
        return "Interface\\Buttons\\Spell-Reset";
    }

    const char* path = ClientDBStringLookup(3);

    // STATIC: 0x00F44BA0
    static char s_texture[260];

    SStrPrintf(s_texture, sizeof(s_texture), "%s%s%s", path, *path ? "\\" : "", item->GetInventoryArt());
    return s_texture;
}

// OFFSET: 0x00954FC0/0x1406C60A0
const char* CGActionBar::GetRangedTexture() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || (player->m_unit->m_unitFieldFlags2 & 0x400)) {
        return NULL;
    }

    CGBag_C* bag = player->GetBag();
    WOWGUID guid = static_cast<uint32_t>(bag->m_numSlots) > 17 ? bag->m_slots[17] : WOWGUID();
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item || item->GetSubtypeID() == 16) {
        return NULL;
    }

    const char* path = ClientDBStringLookup(3);

    // STATIC: 0x00F44CA8
    static char s_texture[260];

    SStrPrintf(s_texture, sizeof(s_texture), "%s%s%s", path, *path ? "\\" : "", item->GetInventoryArt());
    return s_texture;
}

// OFFSET: 0x00955090/0x1406C61B0
CGItem_C* CGActionBar::FindUseEnchantItemPtr(int32_t itemId) {
    if (CGItem_C::GetUseSpell(itemId)) {
        return NULL;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return NULL;
    }

    CGItem_C* item = player->m_bag.FindItemOfType(itemId, 161);

    if (!item) {
        return player->m_bag.FindItemOfType(itemId, 129);
    }

    return item;
}

// OFFSET: 0x009552C0
int32_t CGActionBar::GetItemSpell(uint32_t slot) {
    int32_t itemId = CGActionBar::GetItem(slot);
    int32_t spellId = CGItem_C::GetUseSpell(itemId);

    if (spellId) {
        return spellId;
    }

    CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);

    if (!item) {
        return 0;
    }

    return item->GetEnchantmentSpell(0);
}

// OFFSET: 0x00955300
int32_t CGActionBar::GetMacroSpell(uint32_t slot, int32_t& petSpell) {
    MACRONODE* macro = CGUIMacros::GetMacroByID(CGActionBar::GetMacroID(slot));

    if (!macro) {
        return 0;
    }

    if (macro->m_itemID) {
        return CGActionBar::GetItemSpell(slot);
    }

    petSpell = macro->m_spellFlags;
    return macro->m_spellID;
}

// OFFSET: 0x009553F0/0x1406C6620
int32_t CGActionBar::GetSpell(uint32_t slot, int32_t& petSpell, int32_t* baseSpell, int32_t* overrideFlag1, int32_t* overrideFlag2, int32_t* ignoreKnown) {
    petSpell = 0;

    if (slot > 143) {
        return 0;
    }

    uint32_t action = CGActionBar::m_slotActions[slot];
    int32_t spellId = 0;

    if (action && (action & 0xF0000000) == 0x40000000 && CGActionBar::HasMacro(slot)) {
        spellId = CGActionBar::GetMacroSpell(slot, petSpell);
    } else {
        action = CGActionBar::m_slotActions[slot];

        if (action && !(action & 0xF0000000)) {
            spellId = action & 0xF1FFFFFF;

            if (ignoreKnown && (action & 0xE000000) == 0x2000000) {
                *ignoreKnown = 1;
            }
        } else if (CGActionBar::IsItem(slot)) {
            spellId = CGActionBar::GetItemSpell(slot);
        } else if (CGActionBar::IsPetAction(slot)) {
            spellId = CGActionBar::GetPetActionSpell(slot);
            const SpellRec* spell = g_spellDB.GetRecord(spellId);

            if (!spell || !(spell->m_attributesExF & 0x40000)) {
                petSpell = 1;
            }

            if (ignoreKnown) {
                *ignoreKnown = 1;
            }
        }
    }

    HASHKEY_NONE key;
    OVERRIDE_SPELL_NODE* node = CGActionBar::m_overrideSpells.Ptr(spellId, key);

    if (node) {
        if (baseSpell) {
            *baseSpell = spellId;
        }

        if (overrideFlag1) {
            *overrideFlag1 = node->m_flags & 0x1;
        }

        if (overrideFlag2) {
            *overrideFlag2 = (node->m_flags >> 1) & 0x1;
        }

        spellId = node->m_overrideSpellID;

        if (ignoreKnown) {
            *ignoreKnown = 1;
        }
    }

    return spellId;
}

// OFFSET: 0x00955540/0x1406C67C0
void CGActionBar::UpdateUsablePower() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t changed = 0;

    for (uint32_t i = 0; i < 144; i++) {
        if (!CGActionBar::m_slotUsable[i]) {
            continue;
        }

        int32_t noMana = 0;
        int32_t petSpell;
        int32_t ignorePower = 0;
        int32_t spellId = CGActionBar::GetSpell(i, petSpell, NULL, NULL, &ignorePower, NULL);
        const SpellRec* spell = g_spellDB.GetRecord(spellId);

        if (spell && !ignorePower) {
            CGUnit_C* caster = petSpell ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : player;

            if (caster) {
                int32_t powerType;
                int32_t requirement;
                noMana = !Spell_C_HaveSpellPower(caster, spell, powerType, requirement);
            }
        }

        if (noMana != CGActionBar::m_slotNoMana[i]) {
            CGActionBar::m_slotNoMana[i] = noMana;
            changed = 1;
        }
    }

    if (changed) {
        FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_UPDATE_USABLE, NULL);
    }
}

// OFFSET: 0x00955720
int32_t CGActionBar::HasSpellAction(int32_t spellId) {
    for (uint32_t i = 0; i < 144; i++) {
        int32_t petSpell;
        int32_t slotSpell = CGActionBar::GetSpell(i, petSpell);

        if (slotSpell && spellId == slotSpell) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00955C80/0x1406C7110
void CGActionBar::LeaveWorld(bool reload) {
    if (!reload) {
        return;
    }

    if (CGActionBar::m_uiButtons.m_count == 0xFFFFFFFF) {
        CGActionBar::m_uiButtons.Dynamic().m_count = 0;
    } else {
        CGActionBar::m_uiButtons.m_count = 0;
    }
}

// OFFSET: 0x00955CB0
void CGActionBar::ShutdownGame() {
    if (CGActionBar::m_uiButtons.m_count == 0xFFFFFFFF) {
        CGActionBar::m_uiButtons.Dynamic().m_count = 0;
    } else {
        CGActionBar::m_uiButtons.m_count = 0;
    }
}

// OFFSET: 0x00955CD0
int32_t CGActionBar::IsAutoRepeatAction(uint32_t slot) {
    if (slot > 143) {
        return 0;
    }

    int32_t petSpell;
    int32_t spellId = CGActionBar::GetSpell(slot, petSpell);
    return spellId && !petSpell && (Spell_C_GetAutoRepeatingSpell() == spellId || Spell_C_GetAutoRangedCombatSpell() == spellId);
}

// OFFSET: 0x00955DE0/0x1406C72C0
int32_t CGActionBar::HasValidTarget(uint32_t slot, WOWGUID target, int32_t& inRange) {
    if (slot > 143) {
        return 0;
    }

    int32_t petSpell;
    const SpellRec* spell = g_spellDB.GetRecord(CGActionBar::GetSpell(slot, petSpell));

    if (!spell) {
        return 0;
    }

    if (!target && CGActionBar::IsMacro(slot)) {
        MACRONODE* macro = CGUIMacros::GetMacroByID(CGActionBar::GetMacroID(slot));

        if (macro) {
            target = macro->m_target;
        }
    }

    CGUnit_C* caster = petSpell ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : ClntObjMgrGetActivePlayerObj();
    uint8_t result;

    if (!caster || !Spell_C_RangeCheckSelected(caster, spell, target, &result, false, NULL)) {
        return 0;
    }

    inRange = result;
    return 1;
}

// OFFSET: 0x00955F90
int32_t CGActionBar::ActionHasRange(uint32_t slot) {
    if (slot > 143) {
        return 0;
    }

    int32_t petSpell;
    const SpellRec* spell = g_spellDB.GetRecord(CGActionBar::GetSpell(slot, petSpell));

    if (!spell) {
        return 0;
    }

    CGUnit_C* caster = petSpell ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : ClntObjMgrGetActivePlayerObj();

    if (!caster) {
        return 0;
    }

    float minRange;
    float maxRange;
    Spell_C_GetMinMaxRange(caster, spell, &minRange, &maxRange, NULL);
    return !fequal(minRange, 0.0f) || !fequal(maxRange, 0.0f);
}

// OFFSET: 0x00956130
int32_t CGActionBar::IsAttackAction(uint32_t slot) {
    if (slot > 143) {
        return 0;
    }

    int32_t petSpell;
    const SpellEffectRec* effect = SpellGetEffect(CGActionBar::GetSpell(slot, petSpell), 0);
    return effect && effect->m_effect == 78;
}

// OFFSET: 0x00956200
int32_t CGActionBar::IsRangedAttackAction(uint32_t slot) {
    if (slot > 143) {
        return 0;
    }

    int32_t petSpell;
    const SpellRec* spell = g_spellDB.GetRecord(CGActionBar::GetSpell(slot, petSpell));
    return spell && (spell->m_attributes & 0x2) && (spell->m_attributesExB & 0x20);
}

// OFFSET: 0x009562F0
void CGActionBar::SetActionUIButton(CSimpleCheckbox* checkbox, uint32_t slot, CGCooldown* cooldown) {
    if (!checkbox || slot >= 144) {
        return;
    }

    for (uint32_t i = 0; i < CGActionBar::m_uiButtons.Count(); i++) {
        if (CGActionBar::m_uiButtons[i].m_checkbox == checkbox) {
            CGActionBar::m_uiButtons[i].m_slot = slot;
            CGActionBar::m_uiButtons[i].m_cooldown = cooldown;
            return;
        }
    }

    UIButtons* button = CGActionBar::m_uiButtons.New();
    button->m_checkbox = checkbox;
    button->m_slot = slot;
    button->m_cooldown = cooldown;
}

// OFFSET: 0x00956760
int32_t CGActionBar::IsCurrentAction(uint32_t slot) {
    if (slot > 143 || !CGActionBar::m_slotActions[slot]) {
        return 0;
    }

    int32_t itemId = CGActionBar::GetItem(slot);

    if (itemId) {
        return Spell_C_IsCurrentItem(itemId);
    }

    if (CGActionBar::IsAttackAction(slot)) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
        return player && player->m_combat.m_attacking;
    }

    int32_t petSpell;

    if (CGActionBar::IsPetAction(slot)) {
        PetAction* petAction = GetPetAction((slot - 120) & 0xE1FFFFFF);
        uint32_t data;

        if (!petAction || !(data = petAction->m_data)) {
            return 0;
        }

        switch ((data >> 24) & 0x3F) {
            case 6:
                return CGPetInfo::GetPetMode() == (data & 0xFFFFFF);

            case 7: {
                uint32_t command = data & 0xFFFFFF;

                if ((CGPetInfo::m_petMode >> 8) == command || (command == 2 && CGPetInfo::m_attackingTarget)) {
                    return 1;
                }
            }

            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
                return Spell_C_IsCurrentSpell(CGActionBar::GetSpell(slot, petSpell));

            default:
                return 0;
        }
    }

    return Spell_C_IsCurrentSpell(CGActionBar::GetSpell(slot, petSpell));
}

// OFFSET: 0x009568C0/0x1406C8090
void CGActionBar::UpdateSelection() {
    uint32_t count = CGActionBar::m_uiButtons.Count();

    for (uint32_t i = 0; i < count; i++) {
        UIButtons& button = CGActionBar::m_uiButtons[i];

        if (!button.m_checkbox || !button.m_checkbox->m_visible) {
            continue;
        }

        if (CGActionBar::IsCurrentAction(button.m_slot) || CGActionBar::IsAutoRepeatAction(button.m_slot)) {
            button.m_checkbox->SetChecked(1, 0);
        } else {
            button.m_checkbox->SetChecked(0, 0);
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_UPDATE_STATE, NULL);
}

// OFFSET: 0x009569D0/0x1406C81F0
int32_t CGActionBar::UpdateUsableAction(uint32_t slot, int32_t& noMana) {
    noMana = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint16_t overrideId = *reinterpret_cast<const uint16_t*>(player->m_player->m_playerFieldBytes2);
    const OverrideSpellDataRec* overrideRec = NULL;

    if (overrideId) {
        overrideRec = g_overrideSpellDataDB.GetRecord(overrideId);
    }

    int32_t itemId = CGActionBar::GetItem(slot);

    if (itemId) {
        if (!CGActionBar::m_slotUseCount[slot] || Spell_C_NeedsCooldownEvent(itemId) || (overrideRec && !(overrideRec->m_flags & 0x9))) {
            return 0;
        }

        ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);
        bool arena = false;

        if (rec && CGGameUI::IsInArenaOrRatedBattleground(&arena) && (arena ? !(rec->m_flags[0] & 0x200000) : !(rec->m_flags[1] & 0x8000))) {
            CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);
            int32_t charges;
            uint32_t cooldown;
            uint32_t categoryCooldown;
            uint32_t category;

            if (item) {
                charges = item->GetUsableSpellCharges();
                item->GetSpellCooldownInfo(cooldown, categoryCooldown, category);
            } else {
                charges = CGItem_C::NumBaseCharges(itemId);
                CGItem_C::GetCooldowns(itemId, cooldown, categoryCooldown, category);
            }

            Spell_C_ModifyCategoryRecovery(reinterpret_cast<int32_t&>(categoryCooldown), category, 1);

            if (charges < 0 || static_cast<int32_t>(cooldown) > 900000 || static_cast<int32_t>(categoryCooldown) > 900000) {
                return 0;
            }
        }
    }

    int32_t attack;

    if (CGActionBar::IsAttackAction(slot) || CGActionBar::IsRangedAttackAction(slot)) {
        attack = 1;

        if (overrideRec && !(overrideRec->m_flags & 0xA)) {
            return 0;
        }
    } else {
        attack = 0;
    }

    uint32_t action = CGActionBar::m_slotActions[slot];

    if (action && (action & 0xF0000000) == 0x20000000) {
        if (!CGEquipmentManager::CanUseEquipmentSets(false)) {
            return 0;
        }

        const EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(CGActionBar::GetEquipmentSetID(slot));

        if (!set || CGEquipmentManager::SetContainsLockedItems(set) || CGEquipmentManager::SetUsesLockedSlots(set)) {
            return 0;
        }

        if (player->m_unit->m_unitFieldFlags & 0x100000) {
            return 1;
        }
    }

    int32_t petSpell;
    int32_t ignorePower = 0;
    int32_t ignoreKnown = 0;
    int32_t spellId = CGActionBar::GetSpell(slot, petSpell, NULL, NULL, &ignorePower, &ignoreKnown);

    if (spellId) {
        if (!itemId && !attack && overrideRec && !(overrideRec->m_flags & 0xC) && !Spell_C_IsOverride(spellId, overrideRec)) {
            return 0;
        }

        CGUnit_C* caster = petSpell ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : player;

        if (!caster) {
            return 0;
        }

        const SpellRec* spell = g_spellDB.GetRecord(spellId);

        if (!spell) {
            return 0;
        }

        WOWGUID target = WOWGUID();

        if (CGActionBar::IsMacro(slot)) {
            MACRONODE* macro = CGUIMacros::GetMacroByID(CGActionBar::GetMacroID(slot));

            if (macro) {
                target = macro->m_target;
            }
        }

        const ItemRecSparse* rec = NULL;

        if (itemId) {
            rec = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);
        } else if (!ignoreKnown && !player->IsSpellKnown(spellId)) {
            return 0;
        }

        if (!Spell_C_IsUsableAction(caster, spell, target, rec, false, false, ignorePower != 0)) {
            return 0;
        }

        if (!ignorePower) {
            int32_t powerType;
            int32_t requirement;
            noMana = !Spell_C_HaveSpellPower(caster, spell, powerType, requirement);
        }

        return 1;
    }

    if (!CGGameUI::m_hasControl) {
        return 0;
    }

    if (attack) {
        return !(player->m_unit->m_unitFieldFlags & 0x20000) && !(player->m_player->m_playerFlags & 0x1000000) && !(player->m_unit->m_unitFieldFlags2 & 0x800000);
    }

    if (CGActionBar::IsItem(slot) || (CGActionBar::IsMacro(slot) && CGActionBar::HasMacro(slot))) {
        return 1;
    }

    if (CGActionBar::IsPetAction(slot) || CGActionBar::IsEquipmentSet(slot) || CGActionBar::IsFlyout(slot)) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x00956E50
int32_t CGActionBar::IsConsumableAction(uint32_t slot) {
    if (slot > 143) {
        return 0;
    }

    int32_t itemId = CGActionBar::GetItem(slot);
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (rec) {
        if (rec->m_inventoryType == 25) {
            return 1;
        }

        CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);
        int32_t charges = item ? item->GetUsableSpellCharges() : CGItem_C::NumBaseCharges(itemId);

        if (charges < 0) {
            return 1;
        }
    }

    int32_t petSpell;
    int32_t ignorePower = 0;
    const SpellRec* spell = g_spellDB.GetRecord(CGActionBar::GetSpell(slot, petSpell, NULL, NULL, &ignorePower, NULL));

    if (!spell || petSpell || ignorePower || !spell->m_spellReagentsID) {
        return 0;
    }

    const SpellReagentsRec* reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID);

    if (!reagents) {
        return 0;
    }

    for (uint32_t i = 0; i < 8; i++) {
        if (reagents->m_reagent[i] && reagents->m_reagentCount[i] > 0) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00956F70
int32_t IsStackableAucItem(uint32_t slot) {
    if (slot > 143) {
        return 0;
    }

    ItemRecSparse* rec = g_itemSparseDB.GetRecord(CGActionBar::GetItem(slot), WOWGUID(), NULL, NULL, false);
    return rec && rec->m_stackable > 1;
}

// OFFSET: 0x00956FE0
int32_t CGActionBar::IsEquippedAction(uint32_t slot) {
    if (slot > 143) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t itemId = CGActionBar::GetItem(slot);
    ItemRec* rec = g_itemDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!rec || !rec->m_inventoryType) {
        return 0;
    }

    CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);
    int32_t charges = item ? item->GetUsableSpellCharges() : CGItem_C::NumBaseCharges(itemId);
    uint32_t flags = charges != -1 && charges ? 161 : 129;

    return player->m_bag.FindItemOfType(itemId, flags) || ((flags & 0x20) && !player->m_bag.FindItemOfType(itemId, 32) && player->m_bag.FindItemOfType(itemId, flags & ~0x20u));
}

// OFFSET: 0x009570E0/0x1406C8B20
void CGActionBar::SlotChanged(uint32_t slot, int32_t sendToServer, int32_t silent) {
    if (slot > 143) {
        return;
    }

    int32_t noMana;
    CGActionBar::m_slotUsable[slot] = CGActionBar::UpdateUsableAction(slot, noMana);
    CGActionBar::m_slotNoMana[slot] = noMana;

    if (sendToServer) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SET_ACTION_BUTTON));
        msg.Put(static_cast<uint8_t>(slot));
        msg.Put(CGActionBar::m_slotActions[slot]);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (!silent) {
        FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", slot + 1);
    }
}

// OFFSET: 0x009571C0/0x1406C8C40
void CGActionBar::UpdateUsable() {
    int32_t changed = 0;

    for (uint32_t i = 0; i < 144; i++) {
        int32_t noMana;
        int32_t usable = CGActionBar::UpdateUsableAction(i, noMana);

        if (usable != CGActionBar::m_slotUsable[i] || noMana != CGActionBar::m_slotNoMana[i]) {
            CGActionBar::m_slotUsable[i] = usable;
            CGActionBar::m_slotNoMana[i] = noMana;
            changed = 1;
        }
    }

    if (changed) {
        FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_UPDATE_USABLE, NULL);
    }
}

// OFFSET: 0x00957230
void CGActionBar::UpdateAttackAction() {
    for (uint32_t i = 0; i < 144; i++) {
        if (CGActionBar::IsAttackAction(i)) {
            int32_t noMana;
            CGActionBar::m_slotUsable[i] = CGActionBar::UpdateUsableAction(i, noMana);
            CGActionBar::m_slotNoMana[i] = noMana;
            FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", i + 1);
        }
    }
}

// OFFSET: 0x00957290
void CGActionBar::UpdateRangedAttackAction() {
    for (uint32_t i = 0; i < 144; i++) {
        if (CGActionBar::IsRangedAttackAction(i)) {
            int32_t noMana;
            CGActionBar::m_slotUsable[i] = CGActionBar::UpdateUsableAction(i, noMana);
            CGActionBar::m_slotNoMana[i] = noMana;
            FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", i + 1);
        }
    }
}

// OFFSET: 0x009572F0
void CGActionBar::UpdateEquipmentSet(uint32_t id) {
    if (id > 9) {
        return;
    }

    for (uint32_t i = 0; i < 144; i++) {
        if (CGActionBar::GetEquipmentSetID(i) == static_cast<int32_t>(id)) {
            int32_t noMana;
            CGActionBar::m_slotUsable[i] = CGActionBar::UpdateUsableAction(i, noMana);
            CGActionBar::m_slotNoMana[i] = noMana;
            FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", i + 1);
        }
    }
}

// OFFSET: 0x00957380
void CGActionBar::RemoveAction(uint32_t slot, int32_t silent) {
    if (slot > 143) {
        return;
    }

    if (!silent) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!player || player->unk_21FC) {
            return;
        }
    }

    if (CGActionBar::IsMacro(slot)) {
        MACRONODE* macro = CGUIMacros::GetMacroByID(CGActionBar::GetMacroID(slot));

        if (macro) {
            macro->DecrementActionBarCount();
        }
    }

    CGActionBar::m_slotActions[slot] = 0;
    CGActionBar::SlotChanged(slot, silent == 0, 0);
}

// OFFSET: 0x00957420
void CGActionBar::UpdateSpell(int32_t spellId) {
    if (!spellId) {
        return;
    }

    for (uint32_t i = 0; i < 144; i++) {
        int32_t petSpell;
        int32_t baseSpell = 0;
        int32_t slotSpell = CGActionBar::GetSpell(i, petSpell, &baseSpell, NULL, NULL, NULL);

        if (baseSpell) {
            slotSpell = baseSpell;
        }

        if (slotSpell == spellId && !petSpell) {
            int32_t noMana;
            CGActionBar::m_slotUsable[i] = CGActionBar::UpdateUsableAction(i, noMana);
            CGActionBar::m_slotNoMana[i] = noMana;
            FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", i + 1);
        }
    }
}

// OFFSET: 0x00957580
void CGActionBar::UpdateCritter(int32_t critter) {
    if (!critter) {
        return;
    }

    for (uint32_t slot = 0; slot < 144; slot++) {
        int32_t petSpell;
        const SpellRec* spell = g_spellDB.GetRecord(CGActionBar::GetSpell(slot, petSpell));

        if (!spell) {
            continue;
        }

        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

        for (uint32_t i = 0; i < count; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (effect && effect->m_effect == 28 && g_summonPropertiesDB.GetRecord(effect->m_effectMiscValueB) && effect->m_effectMiscValue == critter) {
                int32_t noMana;
                CGActionBar::m_slotUsable[slot] = CGActionBar::UpdateUsableAction(slot, noMana);
                CGActionBar::m_slotNoMana[slot] = noMana;
                FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", slot + 1);
                break;
            }
        }
    }
}

// OFFSET: 0x00957730
void CGActionBar::UpdateSpellEffect(int32_t effect) {
    for (uint32_t i = 0; i < 144; i++) {
        int32_t petSpell;
        const SpellRec* spell = g_spellDB.GetRecord(CGActionBar::GetSpell(i, petSpell));

        if (spell && SpellHasEffect(spell, effect)) {
            int32_t noMana;
            CGActionBar::m_slotUsable[i] = CGActionBar::UpdateUsableAction(i, noMana);
            CGActionBar::m_slotNoMana[i] = noMana;
            FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", i + 1);
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_SPELL_FLYOUT_UPDATE, NULL);
}

// OFFSET: 0x00957890/0x1406C9590
void CGActionBar::PickupActionInternal(uint32_t slot) {
    uint32_t action = CGActionBar::m_slotActions[slot];

    if (action) {
        if ((action & 0xF0000000) == 0x80000000) {
            ItemRec* rec = g_itemDB.GetRecord(CGActionBar::GetItem(slot), WOWGUID(), NULL, NULL, false);
            int32_t displayId = rec ? rec->m_displayInfoID : 0;
            CGGameUI::SetCursorVirtualItem(CGActionBar::GetItem(slot), displayId, slot, 6, 0);
            return;
        }

        if (!(action & 0xF0000000)) {
            int32_t petSpell;
            int32_t baseSpell = 0;
            int32_t spellId = CGActionBar::GetSpell(slot, petSpell, &baseSpell, NULL, NULL, NULL);

            if (baseSpell) {
                CGGameUI::SetCursorSpell(baseSpell, spellId);
            } else {
                CGGameUI::SetCursorSpell(spellId, 0);
            }

            return;
        }

        if ((action & 0xF0000000) == 0x40000000 && CGActionBar::HasMacro(slot)) {
            CGGameUI::SetCursorMacro(CGActionBar::GetMacroID(slot));
            return;
        }
    }

    action = CGActionBar::m_slotActions[slot];

    if (action && (action & 0xF0000000) == 0x20000000 && CGEquipmentManager::HasSet(CGActionBar::GetEquipmentSetID(slot))) {
        CGGameUI::SetCursorEquipmentSet(CGActionBar::GetEquipmentSetID(slot));
        return;
    }

    action = CGActionBar::m_slotActions[slot];

    if (action && (action & 0xF0000000) == 0x30000000) {
        CGGameUI::SetCursorFlyout(CGActionBar::GetFlyout(slot));
        return;
    }

    CGGameUI::ClearCursor(1, 1);
}

// OFFSET: 0x00957A10/0x1406C9730
const char* CGActionBar::GetTexture(uint32_t slot) {
    if (slot > 143) {
        return NULL;
    }

    // STATIC: 0x00F44EB4
    static const char* s_path = ClientDBStringLookup(3);

    // STATIC: 0x00F44DB0
    static char s_texture[260];

    if (!CGActionBar::m_slotActions[slot]) {
        return NULL;
    }

    if (CGActionBar::IsItem(slot)) {
        ItemRec* rec = g_itemDB.GetRecord(CGActionBar::GetItem(slot), WOWGUID(), NULL, NULL, false);
        SStrPrintf(s_texture, sizeof(s_texture), "%s%s%s", s_path, *s_path ? "\\" : "", CGItem_C::GetInventoryArt(rec ? rec->m_displayInfoID : 0));
        return s_texture;
    }

    if (CGActionBar::IsSpell(slot)) {
        int32_t petSpell;
        const SpellRec* spell = g_spellDB.GetRecord(CGActionBar::GetSpell(slot, petSpell));

        if (!spell) {
            return NULL;
        }

        if (!spell->m_activeIconID) {
            return CGSpellBook::GetSpellTexture(spell->m_id);
        }

        CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(petSpell ? CGPetInfo::GetPet(0) : ClntObjMgrGetActivePlayer(), TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!Spell_C_IsActiveSpell(spell, caster)) {
            return CGSpellBook::GetSpellTexture(spell->m_id);
        }

        const SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_activeIconID);

        if (!icon) {
            return CGSpellBook::GetSpellTexture(spell->m_id);
        }

        return icon->m_textureFilename;
    }

    if (CGActionBar::IsMacro(slot) && CGActionBar::HasMacro(slot)) {
        CGUIMacros::GetFullIconPath(CGActionBar::GetMacroID(slot), s_texture, sizeof(s_texture));
        return s_texture;
    }

    if (CGActionBar::IsEquipmentSet(slot)) {
        CGEquipmentManager::GetFullIconPath(CGActionBar::GetEquipmentSetID(slot), s_texture, sizeof(s_texture));
        return s_texture;
    }

    if (CGActionBar::IsPetAction(slot)) {
        PetAction* petAction = GetPetAction((slot - 120) & 0xE1FFFFFF);

        if (petAction && petAction->m_data) {
            return CGPetInfo::GetPetActionTexture(*petAction);
        }

        return NULL;
    }

    if (CGActionBar::IsFlyout(slot)) {
        const SpellFlyoutRec* flyout = g_spellFlyoutDB.GetRecord(CGActionBar::GetFlyout(slot));

        if (flyout) {
            const SpellIconRec* icon = g_spellIconDB.GetRecord(flyout->m_spellIconID);

            if (icon) {
                return icon->m_textureFilename;
            }
        }
    }

    return NULL;
}

// OFFSET: 0x00957C80/0x1406C9A80
void CGActionBar::GetCooldown(uint32_t slot, uint32_t& start, uint32_t& duration, uint32_t& enable) {
    start = 0;
    duration = 0;

    if (slot > 143) {
        return;
    }

    int32_t itemId = CGActionBar::GetItem(slot);

    if (itemId) {
        if (g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false)) {
            CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);

            if (item) {
                Spell_C_GetItemCooldown(item, &duration, &start, &enable);
            } else {
                Spell_C_GetItemCooldown(itemId, &duration, &start, &enable);
            }
        }

        return;
    }

    int32_t petSpell;
    int32_t spellId = CGActionBar::GetSpell(slot, petSpell);

    if (spellId) {
        Spell_C_GetSpellCooldown(spellId, petSpell, &duration, &start, &enable, NULL);
    }
}

// OFFSET: 0x00957E00/0x1406C9C60
int32_t CGActionBar::CalculateItemCount(int32_t itemId) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t count = player->GetBag()->GetItemTypeCount(itemId, 128);

    if (!count) {
        return 0;
    }

    ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!rec || rec->m_stackable > 1) {
        return count;
    }

    CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);
    int32_t charges = item ? item->GetUsableSpellCharges() : CGItem_C::NumBaseCharges(itemId);

    if (charges < 0) {
        return player->GetBag()->GetItemTypeCharges(itemId, 0);
    }

    int32_t spellId = item ? item->GetEnchantmentSpell(0) : CGItem_C::GetUseSpell(itemId);
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell || !spell->m_spellReagentsID) {
        return count;
    }

    const SpellReagentsRec* reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID);

    if (!reagents) {
        return count;
    }

    for (uint32_t i = 0; i < 8; i++) {
        if (reagents->m_reagent[i] && reagents->m_reagentCount[i] > 0) {
            return Spell_C_GetSpellReagentUses(spell);
        }
    }

    return count;
}

// OFFSET: 0x009581D0/0x1406CA0C0
void CGActionBar::UpdateCooldowns() {
    uint32_t count = CGActionBar::m_uiButtons.Count();

    for (uint32_t i = 0; i < count; i++) {
        UIButtons& button = CGActionBar::m_uiButtons[i];

        if (!button.m_cooldown) {
            continue;
        }

        uint32_t start;
        uint32_t duration;
        uint32_t enable = 0;
        CGActionBar::GetCooldown(button.m_slot, start, duration, enable);

        if (enable && start && duration) {
            button.m_cooldown->SetCooldown(start, duration);
        } else {
            button.m_cooldown->Hide();
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_UPDATE_COOLDOWN, NULL);
}

// OFFSET: 0x00958280/0x1406CA1A0
void CGActionBar::UpdateItem(int32_t itemId) {
    if (!itemId) {
        return;
    }

    int32_t count = CGActionBar::CalculateItemCount(itemId);

    for (uint32_t i = 0; i < 144; i++) {
        uint32_t action = CGActionBar::m_slotActions[i];

        if (action && (action & 0xF0000000) == 0x40000000) {
            MACRONODE* macro = CGUIMacros::GetMacroByID(action & 0xB1FFFFFF);

            if (macro && macro->m_spellID < 0) {
                macro->FindSpellID();
            }
        }

        int32_t slotItem = CGActionBar::GetItem(i);

        if (!slotItem) {
            int32_t petSpell;
            int32_t spellId = CGActionBar::GetSpell(i, petSpell);
            const SpellRec* spell;

            if (spellId && !petSpell && (spell = g_spellDB.GetRecord(spellId)) && Spell_C_SpellUsesReagent(spell, itemId)) {
                CGActionBar::m_slotUseCount[i] = Spell_C_GetSpellReagentUses(spell);
                int32_t noMana;
                CGActionBar::m_slotUsable[i] = CGActionBar::UpdateUsableAction(i, noMana);
                CGActionBar::m_slotNoMana[i] = noMana;
                FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", i + 1);
            }

            continue;
        }

        if (slotItem == itemId) {
            if (!count) {
                ItemRecSparse* rec = g_itemSparseDB.GetRecord(slotItem, WOWGUID(), NULL, NULL, false);

                if (rec) {
                    uint32_t index = 0;

                    while (rec->m_spellTrigger[index] != 6) {
                        if (++index >= 5) {
                            break;
                        }
                    }

                    if (index < 5) {
                        uint32_t spellId = rec->m_spellID[index];

                        if (spellId && spellId <= static_cast<uint32_t>(g_spellDB.m_maxID) && (s_knownSpellBits.m_bits.m_data[spellId >> 5] & (1 << (spellId & 0x1F)))) {
                            CGActionBar::SetAction(i, spellId, 1, 0);
                            continue;
                        }
                    }
                }
            }

            CGActionBar::m_slotUseCount[i] = count;
            int32_t noMana;
            CGActionBar::m_slotUsable[i] = CGActionBar::UpdateUsableAction(i, noMana);
            CGActionBar::m_slotNoMana[i] = noMana;
            FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", i + 1);
            continue;
        }

        ItemRecSparse* rec = g_itemSparseDB.GetRecord(slotItem, WOWGUID(), NULL, NULL, false);

        if (!rec || rec->m_stackable > 1) {
            continue;
        }

        CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(slotItem);
        int32_t charges = item ? item->GetUsableSpellCharges() : CGItem_C::NumBaseCharges(slotItem);

        if (charges < 0) {
            continue;
        }

        int32_t spellId = item ? item->GetEnchantmentSpell(0) : CGItem_C::GetUseSpell(slotItem);
        const SpellRec* spell;

        if (!spellId || !(spell = g_spellDB.GetRecord(spellId)) || !spell->m_spellReagentsID) {
            continue;
        }

        const SpellReagentsRec* reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID);

        if (!reagents) {
            continue;
        }

        uint32_t reagent = 0;

        while (!reagents->m_reagent[reagent] || reagents->m_reagentCount[reagent] <= 0) {
            if (++reagent >= 8) {
                break;
            }
        }

        if (reagent < 8 && Spell_C_SpellUsesReagent(spell, itemId)) {
            CGActionBar::m_slotUseCount[i] = Spell_C_GetSpellReagentUses(spell);
            int32_t noMana;
            CGActionBar::m_slotUsable[i] = CGActionBar::UpdateUsableAction(i, noMana);
            CGActionBar::m_slotNoMana[i] = noMana;
            FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", i + 1);
        }
    }
}

// OFFSET: 0x00958670/0x1406CA6F0
void CGActionBar::UpdateMacro(uint32_t macroId) {
    if (!macroId) {
        return;
    }

    for (uint32_t i = 0; i < 144; i++) {
        if (CGActionBar::GetMacroID(i) != macroId) {
            continue;
        }

        int32_t count = 0;

        if (CGActionBar::GetItem(i)) {
            count = CGActionBar::CalculateItemCount(CGActionBar::GetItem(i));
        } else {
            int32_t petSpell;
            int32_t spellId = CGActionBar::GetSpell(i, petSpell);

            if (spellId && !petSpell) {
                count = Spell_C_GetSpellReagentUses(spellId);
            }
        }

        CGActionBar::m_slotUseCount[i] = count;
        int32_t noMana;
        CGActionBar::m_slotUsable[i] = CGActionBar::UpdateUsableAction(i, noMana);
        CGActionBar::m_slotNoMana[i] = noMana;
        FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", i + 1);
    }
}

// OFFSET: 0x00958820
void CGActionBar::RemoveMacro(uint32_t macroId, int32_t silent) {
    if (!macroId) {
        return;
    }

    for (uint32_t i = 0; i < 144; i++) {
        if (CGActionBar::GetMacroID(i) == macroId) {
            CGActionBar::RemoveAction(i, silent);
        }
    }
}

// OFFSET: 0x00958880/0x1406CA980
void CGActionBar::RemoveEquipmentSet(uint32_t id) {
    if (id > 9) {
        return;
    }

    for (uint32_t i = 0; i < 144; i++) {
        if (CGActionBar::GetEquipmentSetID(i) != static_cast<int32_t>(id)) {
            continue;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!player || player->unk_21FC) {
            continue;
        }

        if (CGActionBar::IsMacro(i)) {
            MACRONODE* macro = CGUIMacros::GetMacroByID(CGActionBar::GetMacroID(i));

            if (macro) {
                macro->DecrementActionBarCount();
            }
        }

        CGActionBar::m_slotActions[i] = 0;
        CGActionBar::SlotChanged(i, 1, 0);
    }
}

// PROVEN: e2c92266 IDENTICAL/NONE
// OFFSET: 0x00958950/0x1406CAA70
void ItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        CGActionBar::UpdateItem(id);
    }
}

// OFFSET: 0x00958970/0x1406CAA90
void CGActionBar::SetAction(uint32_t slot, uint32_t action, int32_t sendToServer, int32_t silent) {
    if (slot > 143) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->unk_21FC) {
        return;
    }

    if (silent && (action & 0xF0000000) == 0x80000000) {
        g_itemSparseDB.GetRecord(action & 0x71FFFFFF, WOWGUID(), ItemStatsCallback, NULL, true);
    }

    if (CGActionBar::IsMacro(slot)) {
        MACRONODE* macro = CGUIMacros::GetMacroByID(CGActionBar::GetMacroID(slot));

        if (macro) {
            macro->DecrementActionBarCount();
        }
    }

    if ((action & 0xF0000000) == 0x40000000) {
        MACRONODE* macro = CGUIMacros::GetMacroByID(action & 0xB1FFFFFF);

        if (macro) {
            macro->IncrementActionBarCount();
        }
    }

    CGActionBar::m_slotActions[slot] = action;
    int32_t count = 0;

    if (CGActionBar::GetItem(slot)) {
        count = CGActionBar::CalculateItemCount(CGActionBar::GetItem(slot));
    } else {
        int32_t petSpell;
        int32_t spellId = CGActionBar::GetSpell(slot, petSpell);

        if (spellId && !petSpell) {
            count = Spell_C_GetSpellReagentUses(spellId);
        }
    }

    CGActionBar::m_slotUseCount[slot] = count;
    CGActionBar::SlotChanged(slot, sendToServer, silent);
}

// OFFSET: 0x00958AD0
void CGActionBar::ReplaceSpell(int32_t oldSpell, int32_t newSpell) {
    const SpellRec* newRec = g_spellDB.GetRecord(newSpell);
    int32_t newRank = newRec ? CGSpellBook::GetSpellProgressionRank(newRec) : 0;

    for (uint32_t slot = 0; slot < 144; slot++) {
        int32_t petSpell;
        int32_t spellId = CGActionBar::GetSpell(slot, petSpell);

        if (!spellId) {
            continue;
        }

        if (spellId != oldSpell) {
            const SpellRec* rec = g_spellDB.GetRecord(spellId);

            if (!newRec || !rec || strcmp(rec->m_name, newRec->m_name) || newRank != CGSpellBook::GetSpellProgressionRank(rec) + 1) {
                continue;
            }
        }

        uint32_t action = CGActionBar::m_slotActions[slot];

        if (!action) {
            continue;
        }

        if (!(action & 0xF0000000)) {
            CGActionBar::SetAction(slot, newSpell, 1, 0);
        } else if ((action & 0xF0000000) == 0x40000000) {
            CGUIMacros::GetMacroByID(action & 0xB1FFFFFF)->m_spellID = newSpell;
        }
    }
}

// OFFSET: 0x00958CC0/0x1406CAFA0
void CGActionBar::PutActionInSlot(uint32_t slot, int32_t a2) {
    if (slot > 143) {
        return;
    }

    if (!CGGameUI::CanPerformAction(14)) {
        CGGameUI::ClearCursor(1, 1);
        return;
    }

    int32_t spellId = CGGameUI::GetCursorSpell();
    int32_t itemId = CGGameUI::GetCursorVirtualItemOfType(6);
    int32_t macroId = CGGameUI::m_cursorMacro;
    CGItem_C* item = NULL;

    if (!itemId && CGGameUI::GetCursorItem()) {
        item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::GetCursorItem(), TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item) {
            itemId = item->m_obj->m_entryID;
        }
    }

    int32_t setId = CGGameUI::m_cursorEquipmentSet;
    int32_t flyoutId = CGGameUI::m_cursorFlyout;
    const SpellRec* spell = NULL;

    if (slot >= 132) {
        if (spellId) {
            spell = g_spellDB.GetRecord(spellId);

            if (!spell) {
                return;
            }
        }

        int32_t category = GetMultiCastTotemCategory(slot);

        if (!spell || !IsMultiCastSpellForSlot(slot, spell, &category)) {
            const TotemCategoryRec* rec = g_totemCategoryDB.GetRecord(category);

            if (rec) {
                CGGameUI::DisplayError(ERR_MULTI_CAST_ACTION_TOTEM_S, rec->m_name);
            }

            return;
        }
    }

    if (spellId > 0) {
        if (spell || (spell = g_spellDB.GetRecord(spellId))) {
            int32_t petSpell;

            if (spell->m_attributes & 0x40) {
                CGGameUI::DisplayError(ERR_PASSIVE_ABILITY);
            } else if (CGActionBar::IsSpell(slot) && CGActionBar::GetSpell(slot, petSpell) == spellId) {
                CGGameUI::DropCursorSpell();
            } else {
                CGActionBar::PickupActionInternal(slot);
                CGActionBar::SetAction(slot, spellId, 1, 0);
            }
        }

        return;
    }

    if (itemId <= 0) {
        if (macroId) {
            if (CGActionBar::IsMacro(slot) && CGActionBar::HasMacro(slot) && static_cast<uint32_t>(macroId) == CGActionBar::GetMacroID(slot)) {
                CGGameUI::ClearCursor(1, 1);
                return;
            }

            CGActionBar::PickupActionInternal(slot);
            CGActionBar::SetAction(slot, macroId | 0x40000000, a2 == 0, 0);
            return;
        }

        if (setId < 0) {
            if (!flyoutId) {
                return;
            }

            if (CGActionBar::IsFlyout(slot) && CGActionBar::GetFlyout(slot) == static_cast<uint32_t>(flyoutId)) {
                CGGameUI::ClearCursor(1, 1);
                return;
            }

            CGActionBar::PickupActionInternal(slot);
            CGActionBar::SetAction(slot, flyoutId | 0x30000000, 1, 0);
            return;
        }

        if (CGActionBar::IsEquipmentSet(slot) && CGEquipmentManager::HasSet(CGActionBar::GetEquipmentSetID(slot)) && setId == CGActionBar::GetEquipmentSetID(slot)) {
            CGGameUI::ClearCursor(1, 1);
            return;
        }

        CGActionBar::PickupActionInternal(slot);
        CGActionBar::SetAction(slot, setId | 0x20000000, 1, 0);
        return;
    }

    if (!ClntObjMgrGetActivePlayerObj()) {
        return;
    }

    ItemRec* rec = g_itemDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!rec) {
        return;
    }

    if (!CGItem_C::GetUseSpell(itemId) && !((item || (item = CGActionBar::FindUseEnchantItemPtr(itemId))) && item->GetEnchantmentSpell(0)) && !(rec->m_inventoryType && rec->m_inventoryType != 18)) {
        return;
    }

    if (CGActionBar::IsItem(slot) && itemId == CGActionBar::GetItem(slot)) {
        CGGameUI::ClearCursor(1, 1);
        return;
    }

    CGActionBar::PickupActionInternal(slot);
    CGActionBar::SetAction(slot, itemId | 0x80000000, 1, 0);
}

// PROVEN: 7fc56e8b IDENTICAL/NONE
// OFFSET: 0x00959050
void CGActionBar::UpdateTemporaryActionBar(int32_t (*getSpell)(uint32_t index, const void* data), const void* data) {
    if (!getSpell) {
        return;
    }

    for (uint32_t i = 0; i < 12; i++) {
        CGActionBar::SetAction(i + 120, getSpell(i, data), 0, 0);
    }
}

// OFFSET: 0x009591F0/0x1406CB5F0
void CGActionBar::UpdateMultiCastBar() {
    if (s_cvAutoFilledMultiCastSlots) {
        int32_t filled = s_cvAutoFilledMultiCastSlots->m_intValue;
        int32_t newFilled = filled;

        for (uint32_t i = 0; i < 4; i++) {
            int32_t bit = 1 << i;
            TSGrowableArray<int32_t>& spells = CGSpellBook::m_totemSpells[i];

            if (filled & bit) {
                if (!spells.m_count) {
                    newFilled &= ~bit;
                }
            } else if (spells.m_count) {
                int32_t spellId = spells.m_data[0];

                for (uint32_t slot = i + 132; slot < 144; slot += 4) {
                    if (!CGActionBar::HasAction(slot)) {
                        CGActionBar::SetAction(slot, spellId, 1, 1);
                    }
                }

                newFilled |= bit;
            }
        }

        if (newFilled != filled) {
            s_cvAutoFilledMultiCastSlots->Set(newFilled, true, false, false, true);
        }
    }

    Script_SendUniqueSignal(SCRIPT_EVENT_UPDATE_MULTI_CAST_ACTIONBAR);
}

// OFFSET: 0x009592E0/0x1406CB710
void CGActionBar::UpdateShapeShiftBar() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    const SpellShapeshiftFormRec* form;

    if (player && (form = g_spellShapeshiftFormDB.GetRecord(player->GetShapeshiftForm())) && FormHasActionBar(form)) {
        CGActionBar::UpdateTemporaryActionBar(&GetFormPresetSpell, form);
        CGActionBar::m_tempPageActiveFlags |= TEMP_PAGE_SHAPESHIFT;
    } else {
        CGActionBar::m_tempPageActiveFlags &= ~TEMP_PAGE_SHAPESHIFT;
    }
}

// OFFSET: 0x00959360/0x1406CB7E0
void CGActionBar::UpdateOverrideSpellBar() {
    int32_t extra = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    const OverrideSpellDataRec* rec = NULL;
    bool active = false;

    if (player && !(CGActionBar::m_tempPageActiveFlags & (TEMP_PAGE_SHAPESHIFT | 0x1))) {
        rec = g_overrideSpellDataDB.GetRecord(*reinterpret_cast<const int16_t*>(player->m_player->m_playerFieldBytes2));
    }

    if (rec) {
        for (uint32_t i = 0; i < 10; i++) {
            if (rec->m_spells[i]) {
                CGSpellActivationOverlay::AddOverlayGlowableSpell(rec->m_spells[i]);
            }
        }

        for (uint32_t i = 0; i < 10; i++) {
            if (rec->m_spells[i]) {
                active = true;
                break;
            }
        }
    }

    if (active) {
        bool overrideBar;

        if (rec->m_flags & 0x8) {
            overrideBar = false;
            extra = 1;
        } else {
            overrideBar = true;
        }

        CGActionBar::UpdateTemporaryActionBar(&GetOverrideSpell, rec);

        if (overrideBar) {
            CGActionBar::m_tempPageActiveFlags |= TEMP_PAGE_OVERRIDE;
        } else {
            CGActionBar::m_tempPageActiveFlags &= ~TEMP_PAGE_OVERRIDE;
        }
    } else {
        CGActionBar::m_tempPageActiveFlags &= ~TEMP_PAGE_OVERRIDE;
    }

    if (extra) {
        CGActionBar::m_tempPageActiveFlags |= TEMP_PAGE_EXTRA;
    } else {
        CGActionBar::m_tempPageActiveFlags &= ~TEMP_PAGE_EXTRA;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_EXTRA_ACTIONBAR, NULL);
}

// OFFSET: 0x00959470/0x1406CB970
void CGActionBar::UseAction(uint32_t slot, const WOWGUID& target, const char* button) {
    if (slot > 143) {
        return;
    }

    if (!CGUIBindings::s_bindings->m_keyStateOverride && (slot < 120 || slot >= 132) && (CGGameUI::GetCursorSpell() || CGGameUI::GetCursorItem() || CGGameUI::m_cursorMacro || CGGameUI::GetCursorVirtualItemOfType(6) || CGGameUI::m_cursorEquipmentSet >= 0 || CGGameUI::m_cursorFlyout)) {
        CGActionBar::PutActionInSlot(slot, 0);
        return;
    }

    if (!CGActionBar::m_slotActions[slot]) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (CGActionBar::IsItem(slot)) {
        if (!player) {
            return;
        }

        int32_t itemId = CGActionBar::GetItem(slot);
        ItemRec* rec = g_itemDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

        if (!rec) {
            return;
        }

        CGItem_C* enchantItem = CGActionBar::FindUseEnchantItemPtr(itemId);
        int32_t charges = enchantItem ? enchantItem->GetUsableSpellCharges() : CGItem_C::NumBaseCharges(itemId);
        CGItem_C* item;

        if (rec->m_inventoryType) {
            uint32_t flags = charges != -1 && charges ? 161 : 129;
            item = player->m_bag.FindItemOfType(itemId, flags);

            if (!item) {
                WOWGUID container = WOWGUID();
                uint32_t containerSlot = 0;
                CGItem_C* equip;

                if (((flags & 0x20) && (equip = player->m_bag.FindItemOfType(itemId, container, containerSlot, 32))) || (equip = player->m_bag.FindItemOfType(itemId, container, containerSlot, 128))) {
                    if (!(equip->m_itemFlags & 0x1)) {
                        player->AutoEquipItem(container, containerSlot, 0);
                    }
                }

                return;
            }
        } else {
            uint32_t flags = charges != -1 && charges ? 288 : 256;
            item = player->m_bag.FindItemOfType(itemId, flags);

            if (!item && (!(flags & 0x20) || !(item = player->m_bag.FindItemOfType(itemId, flags & ~0x20u)))) {
                return;
            }
        }

        item->UseItem(const_cast<WOWGUID*>(&target), 0);
        return;
    }

    if (CGActionBar::IsMacro(slot)) {
        CGUIMacros::RunMacro(CGActionBar::GetMacroID(slot), button);
        return;
    }

    if (CGActionBar::IsEquipmentSet(slot)) {
        CGEquipmentManager::EquipSet(CGActionBar::GetEquipmentSetID(slot));
        return;
    }

    if (CGActionBar::IsPetAction(slot)) {
        PetAction* petAction = GetPetAction(CGActionBar::m_slotActions[slot] & 0xE1FFFFFF);

        if (petAction && petAction->m_data) {
            CGPetInfo::SendPetAction(*petAction, WOWGUID(), 0);
            SE3SoundKitProperties properties;
            SE3::PlaySoundKit("GAMEABILITYACTIVATE", NULL, &properties, NULL, __FILE__, __LINE__, "UseAction()");
            CGActionBar::UpdateSelection();
        }

        return;
    }

    int32_t petSpell;
    int32_t baseSpell = 0;
    int32_t ignorePower = 0;
    int32_t ignoreKnown = 0;
    int32_t spellId = CGActionBar::GetSpell(slot, petSpell, &baseSpell, NULL, &ignorePower, &ignoreKnown);

    if (!player || ignoreKnown || player->IsSpellKnown(spellId)) {
        Spell_C_ClickSpell(spellId, NULL, target, baseSpell, ignorePower);
    }
}

// OFFSET: 0x009597B0
void CGActionBar::PickupAction(uint32_t slot) {
    if (slot > 143 || !CGGameUI::CanPerformAction(14) || (slot >= 120 && slot < 132)) {
        return;
    }

    if (CGGameUI::GetCursorSpell() || CGGameUI::GetCursorItem() || CGGameUI::m_cursorMacro || CGGameUI::GetCursorVirtualItemOfType(6) || CGGameUI::m_cursorEquipmentSet >= 0 || CGGameUI::m_cursorFlyout) {
        CGActionBar::PutActionInSlot(slot, 0);
        return;
    }

    if (CGActionBar::m_slotActions[slot]) {
        CGActionBar::PickupActionInternal(slot);
        CGActionBar::RemoveAction(slot, 0);
    }
}

// PROVEN: 245edc1b IDENTICAL/NONE
// OFFSET: 0x00959930/0x1406CBF50
void CGActionBar::EnterWorld() {
    CGActionBar::UpdateUsable();
    CGActionBar::UpdateShapeShiftBar();
    CGActionBar::UpdateOverrideSpellBar();
    CGActionBar::UpdateBonusBar();
}
