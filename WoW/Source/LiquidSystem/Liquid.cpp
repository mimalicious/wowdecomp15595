#include "Liquid.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: #0x00D43B34
CDataAllocator Liquid::s_instanceAllocator(sizeof(Liquid::CInstance), 0x20);

// STATIC: #0x00B23F64
float Liquid::s_unk_B23F64 = 1.0f;

// STATIC: #0x00B23F68
int32_t Liquid::s_specularEnabled = 1;

// STATIC: #0x00D439E8
IDatabase<LiquidMaterialRec>* Liquid::s_liquidMaterialDB;

// STATIC: #0x00D439EC
IDatabase<LiquidTypeRec>* Liquid::s_liquidTypeDB;

// STATIC: #0x00D439F0
char Liquid::s_shaderPath[256];

// STATIC: #0x00D43AF0
int32_t Liquid::s_shadersEnabled;

// STATIC: #0x00D43AF4
int32_t Liquid::s_unk_D43AF4;

// STATIC: #0x00D43AF8
int32_t Liquid::s_unk_D43AF8;

// STATIC: #0x00D43AFC
int32_t Liquid::s_unk_D43AFC;

// OFFSET: #0x008A32C0
Liquid::IEnvironment::~IEnvironment() {
}

// INLINED
Liquid::IGeomFactory::~IGeomFactory() {
}

// INLINED
Liquid::IWaveManager::~IWaveManager() {
}

// INLINED
Liquid::IMaterial::~IMaterial() {
}

// OFFSET: #0x008A1770
int32_t Liquid::Initialize(int32_t a1, float a2, int32_t a3, const char* a4, IDatabase<LiquidTypeRec>* liquidTypeDB, IDatabase<LiquidMaterialRec>* liquidMaterialDB) {
    SStrCopy(Liquid::s_shaderPath, a4, sizeof(Liquid::s_shaderPath));
    Liquid::s_unk_B23F64 = a2;
    Liquid::s_unk_D43AF8++;
    Liquid::s_unk_D43AFC++;
    Liquid::s_unk_D43AF4 = a1;
    Liquid::s_shadersEnabled = a3;
    Liquid::s_specularEnabled = 1;
    Liquid::s_liquidTypeDB = liquidTypeDB;
    Liquid::s_liquidMaterialDB = liquidMaterialDB;
    return 1;
}

// OFFSET: #0x008A2400
void Liquid::Shutdown() {
    int32_t count = Liquid::s_unk_D43AFC--;
    Liquid::s_liquidTypeDB = NULL;
    Liquid::s_liquidMaterialDB = NULL;

    if (count == 1) {
        Liquid::CMaterialBank::Destroy();
        Liquid::s_shadersEnabled = 0;
    }

    Liquid::CSettingsBank::Destroy();
    Liquid::ClearMaterials();
    Liquid::s_instanceAllocator.Clear(".?AVCInstance@Liquid@@", SLOG_OBJECT, 1);
}

// OFFSET: #0x008A1700
void Liquid::CInstance::AddRef() {
    this->m_refCount++;
}

// PROVEN: 259e5a12 IDENTICAL/NONE
// OFFSET: 0x0071BE00/0x1403F0B50
void Liquid::CInstance::Purge() {
    this->m_geomFactory->Purge();
}

// OFFSET: #0x008A1720
void Liquid::SetShadersEnabled(int32_t enabled) {
    Liquid::s_shadersEnabled = enabled;
}

// OFFSET: #0x008A1730
void Liquid::SetSpecularEnabled(int32_t enabled) {
    Liquid::s_specularEnabled = enabled;
}

// OFFSET: #0x008A1740
int32_t Liquid::UseCameraRelativeCoords() {
    return Liquid::s_unk_D43AF4;
}

// OFFSET: #0x008A1750
float Liquid::WorldCoordScale() {
    return Liquid::s_unk_B23F64;
}

// OFFSET: #0x008A1760
const char* Liquid::GetShaderSuffix() {
    return Liquid::s_shaderPath;
}

// OFFSET: #0x008A1A40
Liquid::CInstance::CInstance() {
    this->m_material = NULL;
    this->m_settings = NULL;
    this->m_geomFactory = NULL;
    this->m_waveManager = NULL;
    this->m_environment = NULL;
    this->m_bounds.r = 0.0f;
    this->m_queued = 0;
    this->m_refCount = 1;
}

// OFFSET: #0x008A1AC0
void Liquid::CInstance::Release() {
    if (this->m_refCount-- == 1) {
        this->m_geomFactory->Release();
        this->m_environment->Release();

        if (this->m_waveManager) {
            this->m_waveManager->Release();
        }

        Liquid::s_instanceAllocator.PutData(this, NULL, 0);
    }
}

// OFFSET: #0x008A1B00
Liquid::CInstance* Liquid::CInstance::Create() {
    void* mem = Liquid::s_instanceAllocator.GetData(0, ".?AVCInstance@Liquid@@", SLOG_OBJECT);
    return mem ? new (mem) Liquid::CInstance() : NULL;
}
