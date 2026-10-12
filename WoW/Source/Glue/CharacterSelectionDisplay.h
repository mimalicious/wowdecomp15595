#ifndef WOW_SOURCE_GLUE_CHARACTERSELECTIONDISPLAY_H
#define WOW_SOURCE_GLUE_CHARACTERSELECTIONDISPLAY_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Object/ObjectClient/AnimKitManager.h"
#include "WoW/Source/WowServices/RealmConnection.h"

class CCharacterComponent;
class CM2Model;

class CharacterSelectionDisplay {
    public:
    /* 0000 */ CHARACTER_INFO m_characterInfo;
    /* 0190 */ CCharacterComponent* m_component;
    /* 0198 */ CAnimKitManager m_animKitManager;
    /* 0288 */ CAnimKitInst* m_emoteKit;
    /* 028C */ CM2Model* m_petModel;
    /* 0290 */ float m_petAlpha;
    /* 0294 */ CGlueMgr::WeaponLoadOut m_weaponLoadOut;

    static void SetCharacterFacing(float rotation);

    CharacterSelectionDisplay();
    CharacterSelectionDisplay(const CharacterSelectionDisplay& source);
    ~CharacterSelectionDisplay();
};

#endif
