#ifndef WOW_SOURCE_GLUE_CHARACTERCREATION_H
#define WOW_SOURCE_GLUE_CHARACTERCREATION_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Game/NameGen.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Glue/Types.h"

class CAnimKitInst;
class CAnimKitManager;
class CAsyncObject;
class CCharacterComponent;
class CM2Model;
class ChrClassesRec;
class ComponentData;
class CSimpleModelFFX;
struct CharacterPreferences;

class CCharacterCreation {
    public:
    static int32_t m_selectedClassID;
    static int32_t m_existingCharacterIndex;
    static CharacterPreferences* m_charPreferences[24][2];
    static uint32_t m_raceIndex;
    static CSimpleModelFFX* m_charCustomizeFrame;
    static float m_charFacing;
    static int32_t m_alteredFormAtFront;
    static uint32_t m_prevSkinIndex;
    static uint32_t m_prevFaceIndex;
    static uint32_t m_prevHairColorIndex;
    static uint32_t m_prevHairStyleIndex;
    static uint32_t m_prevFacialFeatureIndex;
    static CCharacterComponent* m_character;
    static CCharacterComponent* m_alteredCharacter;
    static CAnimKitManager* m_animKitManager;
    static CAnimKitInst* m_standAnimKit;
    static TSFixedArray<const ChrClassesRec*> m_classes;
    static CM2Model* m_petModel;
    static TSGrowableArray<int32_t> m_races;
    static CNameGen m_nameGen;
    static TSGrowableArray<CAsyncObject*> m_racialTexturesLoading;
    static CGlueMgr::WeaponLoadOut m_weaponLoadOut;

    static void BowModelLoadedCallback(CM2Model* model, void* arg);
    static void CalcClasses(int32_t raceID);
    static void CancelRaceSexComponentTexturesPreload();
    static void CreateCharacter(const char* name);
    static void CycleCharCustomization(CHAR_CUSTOMIZATION_TYPE type, int32_t delta);
    static CGlueMgr::WeaponLoadOut Dress();
    static int32_t GetDefaultSlotByType(int32_t inventoryType);
    static int32_t GetExistingClassIndex();
    static int32_t GetExistingRaceIndex();
    static int32_t GetRandomClassID();
    static void GetRandomRaceAndSex(ComponentData* data);
    static int32_t GetSelectedSex();
    static int32_t Idle(const void* data, void* param);
    static void InitCharacterComponent(CCharacterComponent*& component, ComponentData* data);
    static void InitCharacterComponents(ComponentData* data, bool randomize);
    static void Initialize();
    static bool IsClassAvailable(int32_t classID);
    static void OnHide();
    static void OnShow();
    static void PreloadRaceSexComponentTextures();
    static void RandomizeCharCustomization();
    static void RandomizeCharFeatures();
    static void ResetCharCustomizeInfo();
    static void SetCharacterFacing(float facing);
    static void SetCharCustomizeFrame(CSimpleModelFFX* modelFrame);
    static void SetCharCustomizeModel(const char* modelName);
    static void SetPet();
    static bool SetSelectedClass(int32_t classID);
    static void SetSelectedRace(uint32_t raceIndex);
    static void SetSelectedSex(uint32_t sexID);
    static void SetToExistingCharacter(uint32_t index);
    static void SetViewingAlteredForm(int32_t alteredForm);
    static void Shutdown();
    static void StartAnimation(uint64_t param);
    static void TextureMakeLocalFinishedCallback(CAsyncObject* object);
    static void UndressComponent(CCharacterComponent* component);
    static void UninitCharacterComponent(CCharacterComponent*& component);
    static void UninitCharacterComponents();
};

bool IsRaceClassValid(int32_t raceID, int32_t classID);
void CharacterCreateRegisterScriptFunctions();
void CharacterCreateUnregisterScriptFunctions();

#endif
