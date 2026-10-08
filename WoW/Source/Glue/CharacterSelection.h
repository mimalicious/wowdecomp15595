#ifndef WOW_SOURCE_GLUE_CHARACTERSELECTION_H
#define WOW_SOURCE_GLUE_CHARACTERSELECTION_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Glue/CharacterSelectionDisplay.h"

class CHARACTER_INFO;
class CharStartOutfitRec;
class CM2Lighting;
class CM2Model;
class CSimpleModelFFX;
class PreloadList;

class CCharacterSelection {
    public:
    static int32_t m_selectionIndex;
    static int32_t m_enterWorldIndex;
    static CSimpleModelFFX* m_modelFrame;
    static uint32_t m_emoteTimerEvent;
    static int32_t m_characterCount;
    static float m_charFacing;
    static int32_t m_characterListRcvd;
    static PreloadList* m_startRacePreloadList;
    static TSGrowableArray<CharacterSelectionDisplay> m_characterList;
    static uint32_t s_raceRestrictions[10];

    static void ClearCharacterList();
    static void DeleteCharacter(int32_t index);
    static void EnumerateCharactersCallback(const CHARACTER_INFO& info, void* param);
    static const CharStartOutfitRec* FindStartOutfit(uint8_t raceID, uint8_t classID, uint8_t sexID);
    static void GenericLightingCallback(CM2Model* model, CM2Lighting* lighting, void* arg);
    static CharacterSelectionDisplay* GetCharacterDisplay(uint32_t index);
    static uint32_t GetEquippedGuildTabardFlags(int32_t slot, int32_t index);
    static void GuildCallback(const WOWGUID& guildGUID, const WOWGUID& guid, void* param, bool success);
    static int32_t Idle(const void* data, void* param);
    static void Initialize();
    static bool IsStreamingDataReady();
    static void ModelLoadedCallback(CM2Model* model, void* arg);
    static void OnGetCharacterList();
    static void OnHide();
    static void OnShow();
    static void RenderPrep();
    static void SelectCharacter(int32_t index);
    static void SetBackgroundModel(const char* modelName);
    static void SetModelFrame(CSimpleModelFFX* modelFrame);
    static void SetNextEmoteTimer();
    static void ShowCharacter(uint8_t raceOverride, int32_t noEffects);
    static void Shutdown();
    static void StartAnimation(uint64_t param);
    static void StartLoadAnimation(int32_t param);
    static void TransferComponentToWorld();
    static int32_t UpdateEmote(const void* data, void* param);
    static void UpdateCharacterList(const TSFixedArray<CHARACTER_INFO>& characters);
    static void UpdateCharacterName(WOWGUID guid, const char* name);
    static void UpdateCharacterInfo(WOWGUID guid, const char* name, uint8_t sexID, uint8_t skinColorID, uint8_t hairColorID, uint8_t hairStyleID, uint8_t facialHairStyleID, uint8_t faceID, uint8_t raceID);
    static uint32_t* UpdateCharacterNameDeclension(WOWGUID guid);
};

extern const int32_t g_scriptSexIndex[3];

int32_t CharSelectIsStartRaceComplete();
void CharSelectStartRaceShutdown();
void CharSelectSwitchStartRaceDownload(int32_t raceID, int32_t classID);
void CharSelectGetBGModelPathFromRaceStr(const char* raceString, char* path);
void CharSelectAttachDeathVisuals(CM2Model* model);
void LinkModelAndTexturesToAsyncHead(CM2Model* model);

void CharSelectRegisterScriptFunctions();
void CharSelectUnregisterScriptFunctions();

#endif
