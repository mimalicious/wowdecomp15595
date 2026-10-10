#ifndef WOW_SOURCE_UI_ACTIONBARFRAME_H
#define WOW_SOURCE_UI_ACTIONBARFRAME_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;
class SpellRec;
class CGCooldown;
class CGItem_C;
class CSimpleCheckbox;
class PetAction;

PetAction* GetPetAction(uint32_t index);

class OVERRIDE_SPELL_NODE : public TSHashObject<OVERRIDE_SPELL_NODE, HASHKEY_NONE> {
    public:
    /* 0018 */ int32_t m_overrideSpellID;
    /* 001C */ uint32_t m_flags;
};

struct UIButtons {
    /* 0000 */ uint32_t m_slot;
    /* 0004 */ CSimpleCheckbox* m_checkbox;
    /* 0008 */ CGCooldown* m_cooldown;
};

class CGActionBar {
    public:
    enum TEMP_PAGE_FLAGS {
        TEMP_PAGE_SHAPESHIFT = 0x2,
        TEMP_PAGE_OVERRIDE = 0x4,
        TEMP_PAGE_EXTRA = 0x8,
    };

    static uint32_t m_slotActions[144];
    static int32_t m_slotUseCount[144];
    static int32_t m_slotUsable[144];
    static int32_t m_slotNoMana[144];
    static uint32_t m_currentPage;
    static uint32_t m_bonusBarOffset;
    static uint32_t m_tempPageActiveFlags;
    static TSCGrowableArray<UIButtons, 144> m_uiButtons;
    static TSHashTable<OVERRIDE_SPELL_NODE, HASHKEY_NONE> m_overrideSpells;

    static void InitializeGame();
    static void ShutdownGame();
    static void EnterWorld();
    static void LeaveWorld(bool reload);
    static int32_t IsSpell(uint32_t slot);
    static int32_t IsItem(uint32_t slot);
    static int32_t IsMacro(uint32_t slot);
    static int32_t IsPetAction(uint32_t slot);
    static int32_t IsFlyout(uint32_t slot);
    static int32_t IsEquipmentSet(uint32_t slot);
    static uint32_t GetMacroID(uint32_t slot);
    static uint32_t GetSlotSpellAction(uint32_t slot);
    static int32_t GetEquipmentSetID(uint32_t slot);
    static uint32_t GetFlyout(uint32_t slot);
    static int32_t HasMacro(uint32_t slot);
    static int32_t GetMacroItem(uint32_t slot);
    static int32_t GetMacroSpell(uint32_t slot, int32_t& petSpell);
    static int32_t GetItem(uint32_t slot);
    static int32_t GetItemSpell(uint32_t slot);
    static int32_t GetPetActionSpell(uint32_t slot);
    static int32_t GetSpell(uint32_t slot, int32_t& petSpell, int32_t* baseSpell = NULL, int32_t* overrideFlag1 = NULL, int32_t* overrideFlag2 = NULL, int32_t* ignoreKnown = NULL);
    static int32_t HasAction(uint32_t slot);
    static int32_t HasSpellAction(int32_t spellId);
    static void ShowGrid();
    static void HideGrid();
    static void UpdateSelection();
    static void UpdateCooldowns();
    static uint32_t GetMultiCastSlotMaskForTotemCategory(int32_t category);
    static void SetTemporaryActionBarFlags(uint32_t flags, int32_t enable);
    static void UpdateBonusBar();
    static const char* GetAttackTexture();
    static const char* GetRangedTexture();
    static CGItem_C* FindUseEnchantItemPtr(int32_t itemId);
    static void UpdateUsablePower();
    static int32_t IsAutoRepeatAction(uint32_t slot);
    static int32_t HasValidTarget(uint32_t slot, WOWGUID target, int32_t& inRange);
    static int32_t ActionHasRange(uint32_t slot);
    static int32_t IsAttackAction(uint32_t slot);
    static int32_t IsRangedAttackAction(uint32_t slot);
    static void SetActionUIButton(CSimpleCheckbox* checkbox, uint32_t slot, CGCooldown* cooldown);
    static int32_t IsCurrentAction(uint32_t slot);
    static int32_t UpdateUsableAction(uint32_t slot, int32_t& noMana);
    static int32_t IsConsumableAction(uint32_t slot);
    static int32_t IsEquippedAction(uint32_t slot);
    static void SlotChanged(uint32_t slot, int32_t sendToServer, int32_t silent);
    static void UpdateUsable();
    static void UpdateAttackAction();
    static void UpdateRangedAttackAction();
    static void UpdateEquipmentSet(uint32_t id);
    static void RemoveAction(uint32_t slot, int32_t silent);
    static void UpdateSpell(int32_t spellId);
    static void UpdateCritter(int32_t critter);
    static void UpdateSpellEffect(int32_t effect);
    static void PickupActionInternal(uint32_t slot);
    static const char* GetTexture(uint32_t slot);
    static void GetCooldown(uint32_t slot, uint32_t& start, uint32_t& duration, uint32_t& enable);
    static int32_t CalculateItemCount(int32_t itemId);
    static void UpdateItem(int32_t itemId);
    static void UpdateMacro(uint32_t macroId);
    static void RemoveMacro(uint32_t macroId, int32_t silent);
    static void RemoveEquipmentSet(uint32_t id);
    static void SetAction(uint32_t slot, uint32_t action, int32_t sendToServer, int32_t silent);
    static void ReplaceSpell(int32_t oldSpell, int32_t newSpell);
    static void PutActionInSlot(uint32_t slot, int32_t a2);
    static void UpdateTemporaryActionBar(int32_t (*getSpell)(uint32_t index, const void* data), const void* data);
    static void UpdateMultiCastBar();
    static void UpdateShapeShiftBar();
    static void UpdateOverrideSpellBar();
    static void UseAction(uint32_t slot, const WOWGUID& target, const char* button);
    static void PickupAction(uint32_t slot);
    static void OnAuraChanged(int32_t spellId);
    static bool ClearOverrideSpellclass(int32_t spellId);
    static bool SetOverrideSpellclass(const SpellRec* spell, uint32_t effectIndex, int32_t spellId);
};

void ItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
int32_t IsStackableAucItem(uint32_t slot);
int32_t IsMultiCastSpellForSlot(int32_t slot, const SpellRec* spell, int32_t* category);
void ActionBarRegisterScriptFunctions();
void ActionBarUnregisterScriptFunctions();

#endif
