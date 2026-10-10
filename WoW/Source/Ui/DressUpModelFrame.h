#ifndef WOW_SOURCE_UI_DRESSUPMODELFRAME_H
#define WOW_SOURCE_UI_DRESSUPMODELFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "WoW/Source/Ui/CharacterModelBase.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"

class CCharacterComponent;
class CSimpleFrame;
class CGItem_C;
class CM2Model;
struct lua_State;

class CGDressUpModelFrame : public CGCharacterModelBase {
    public:
    static CDataAllocator s_dressUpModelHeap;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);

    /* 02F8 */ CCharacterComponent* m_dressUpComponent;
    /* 02FC */ CGUnitVirtualItem m_mainHand;
    /* 0304 */ CGUnitVirtualItem m_offHand;
    /* 030C */ bool m_isNativeDisplay;

    ~CGDressUpModelFrame();
    int32_t GetScriptMetaTable() override;
    bool IsA(int32_t type) override;
    bool IsA(const char* typeName) override;
    const char* GetObjectTypeName() override;
    void SetModel(CM2Model* model) override;
    void UpdateModel() override;
    void SetUpModel(CGUnit_C* unit) override;
    CM2Model* GetDuplicateUnitCharacterModel(CGUnit_C* unit) override;

    CGDressUpModelFrame(CSimpleFrame* parent);
    void Undress();
    void UndressSlot(int32_t slot);
    void TryOn(int32_t itemId, const int32_t* enchants, int32_t hand);
    void TryOnWithVisual(int32_t itemId, int32_t visual, int32_t hand);
    void TryOnItem(CGItem_C* item, int32_t hand);
};

#include "WoW/Source/Ui/DressUpModelFrame.inl"

#endif
