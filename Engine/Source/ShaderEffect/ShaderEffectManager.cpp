#include <cstddef>
#include <cstdint>
#include <cstdio>
#include "Engine/Source/ShaderEffect/ShaderEffectManager.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00C3CD58
TSHashTable<CShaderEffect, HASHKEY_STRI> CShaderEffectManager::m_ShaderList;

// OFFSET: 0x004347C0
void CShaderEffectManager::DestroyAllEffects() {
    CShaderEffect* effect = CShaderEffectManager::m_ShaderList.Head();

    while (effect) {
        CShaderEffect* next = CShaderEffectManager::m_ShaderList.Next(effect);
        CShaderEffectManager::m_ShaderList.Delete(effect);
        effect = next;
    }
}

// OFFSET: 0x00434630/0x14003D7E0
CShaderEffect* CShaderEffectManager::GetEffect(const char* name) {
    return CShaderEffectManager::m_ShaderList.Ptr(name);
}

// OFFSET: 0x00434EB0/0x14003E410
CShaderEffect* CShaderEffectManager::CreateEffect(const char* name) {
    CShaderEffectManager::m_ShaderList.Ptr(name);

    return CShaderEffectManager::m_ShaderList.New(name, 0, 0);
}

// OFFSET: 0x00434F70/0x14003E590
void CShaderEffectManager::ParseEffectCallback(const CShaderEffectParser::ParsedEffect& effect, void* param) {
    CShaderEffect* shaderEffect = CShaderEffectManager::m_ShaderList.New(effect.m_name, 0, 0);

    shaderEffect->InitEffect(effect.m_shaderPasses[0].m_vertexShader, NULL, NULL, effect.m_shaderPasses[0].m_pixelShader);

    for (uint32_t i = 0; i < effect.m_fixedFuncPassCount; i++) {
        const CShaderEffectParser::ParsedEffect::FixedFuncPass& pass = effect.m_fixedFuncPasses[i];
        shaderEffect->InitFixedFuncPass(pass.m_colorOps, pass.m_alphaOps, pass.m_pass);
    }
}

// OFFSET: 0x00435070/0x14003E770
void CShaderEffectManager::AddEffectFile(const char* fileName) {
    char path[256];
    sprintf(path, "Shaders\\Effects\\%s", fileName);

    SFileImpl* file;
    SFileOpenFile(path, &file);

    if (!file) {
        return;
    }

    uint32_t size = SFileGetFileSize(file, NULL);
    char* data = static_cast<char*>(SMemAlloc(size + 1, __FILE__, __LINE__, 0));
    SFile::Read(file, data, size, NULL);
    SFileCloseFile(file);

    CShaderEffectParser::ParseEffectFile(data, size, CShaderEffectManager::ParseEffectCallback, NULL);

    delete[] data;
}
