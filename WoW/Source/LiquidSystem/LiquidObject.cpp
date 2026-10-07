#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00E4EBE4
static Liquid::Object s_defaultObject;

// STATIC: 0x00E4EC1C
static Liquid::Object s_slimeObject;

// STATIC: 0x00E4EC54
static Liquid::Object s_oceanObject;

// STATIC: 0x00E4EC8C
static Liquid::Object s_magmaObject;

// STATIC: 0x00E4ECC4
static TSHashTable<Liquid::Object, HASHKEY_NONE> s_objectHash;

// PROVEN: 069029d2 IDENTICAL/NONE
// OFFSET: 0x0071CE80
void Liquid::Object::SetReflectionPlane(float height) {
    this->m_reflectionPlane.n.x = 0.0f;
    this->m_reflectionPlane.n.y = 0.0f;
    this->m_reflectionPlane.n.z = 1.0f;
    this->m_reflectionPlane.d = height;
}

// INLINED
void Liquid::InitLiquidObjects() {
    s_defaultObject.m_noReflection = 1;
    s_slimeObject.m_noReflection = 1;
    s_slimeObject.m_reflectionType = 2;
    s_oceanObject.m_noReflection = 1;
    s_oceanObject.m_reflectionType = 1;
}

// OFFSET: 0x0071CF00
void Liquid::UnregisterLiquidObject(Object* object) {
    object->m_refCount--;

    if (object->m_objectId && !object->m_refCount) {
        s_objectHash.Delete(object);
    }
}

// OFFSET: 0x0071D760
Liquid::Object* Liquid::RegisterLiquidObject(uint32_t a1, uint32_t liquidType, uint32_t materialId) {
    Object* object = &s_defaultObject;

    if (a1 >= 42) {
        if (a1 == 42 || liquidType == 14) {
            object = &s_oceanObject;
            object->m_refCount++;
            return object;
        }

        HASHKEY_NONE key;
        object = s_objectHash.Ptr(a1, key);

        if (object) {
            object->m_refCount++;
            return object;
        }

        LiquidObjectRec* rec = g_liquidObjectDB.GetRecord(a1);

        if (rec) {
            object = s_objectHash.New(a1, key, 0, 0);
            object->m_hashval = a1;
            object->m_objectId = a1;
            object->m_noReflection = rec->unk_0008 <= 0.0f;
            object->m_reflectionType = rec->unk_0014;
            object->m_refCount++;
            return object;
        }

        object = &s_defaultObject;
    }

    if (materialId == 1) {
        if (liquidType == 2 || liquidType == 14) {
            object = &s_oceanObject;
        } else if (liquidType == 17) {
            object = &s_slimeObject;
            object->m_refCount++;
            return object;
        }
    } else if (materialId == 2) {
        object = &s_magmaObject;
        object->m_refCount++;
        return object;
    } else if (materialId == 5 && (liquidType == 350 || liquidType == 412)) {
        object = &s_slimeObject;
        object->m_refCount++;
        return object;
    }

    object->m_refCount++;
    return object;
}
