#ifndef WOW_SOURCE_UI_CHARACTERMODELBASE_H
#define WOW_SOURCE_UI_CHARACTERMODELBASE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleModel.h"
#include "WoW/Source/Object/WGUID.h"

class CCharacterComponent;
class CGPlayer_C;
class CGUnit_C;
class CreatureStats_C;
class CM2Model;
class CSimpleFrame;
struct lua_State;

class CGCharacterModelBase : public CSimpleModel {
    public:
    static CDataAllocator s_characterModelHeap;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);

    /* 02B0 */ WOWGUID m_unitGuid;
    /* 02B8 */ uint32_t m_sequenceSet;
    /* 02BC */ uint32_t m_sequenceResetTime;
    /* 02C0 */ uint32_t m_sequence;
    /* 02C4 */ uint32_t m_sequenceVariation;
    /* 02C8 */ CreatureStats_C* m_creatureStats;
    /* 02CC */ int32_t m_displayID;
    /* 02D0 */ WOWGUID m_ownerGuid;
    /* 02D8 */ CCharacterComponent* m_component;
    /* 02DC */ float m_portraitZoom;
    /* 02E0 */ float m_camDistanceScale;
    /* 02E4 */ float m_modelAlpha;
    /* 02E8 */ bool m_barberShopAlternateForm;
    /* 02EC */ int32_t m_customRace;
    /* 02F0 */ int32_t m_customSex;

    ~CGCharacterModelBase();
    int32_t GetScriptMetaTable() override;
    bool IsA(int32_t type) override;
    bool IsA(const char* typeName) override;
    const char* GetObjectTypeName() override;
    void HideThis() override;
    void ShowThis(bool loadPending) override;
    void UpdateAlpha() override;
    int32_t IsDrawable() override;
    void UpdateModel() override;
    void OnModelLoaded(CM2Model* model) override;
    virtual void SetUpModel(CGUnit_C* unit);
    virtual CM2Model* GetDuplicateUnitCharacterModel(CGUnit_C* unit);

    CGCharacterModelBase(CSimpleFrame* parent);
    void ConfigureCamera();
    void RefreshUnit(WOWGUID guid);
    void SetUpModel(int32_t displayID, CGPlayer_C* owner);
    void SetCreature(CreatureStats_C* stats, CGPlayer_C* owner);
    void SetDisplayInfo(int32_t displayID, CGPlayer_C* owner);
    void SetCustomRace(int32_t race, uint32_t sex);
    void SetBarberShopAlternateForm();
    void RotateModel(float facing, bool animate);
};

#include "WoW/Source/Ui/CharacterModelBase.inl"

#endif
