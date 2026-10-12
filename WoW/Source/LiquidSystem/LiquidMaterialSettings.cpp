#include "WoW/Source/LiquidSystem/Liquid.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00E4EBCC
Liquid::TEXTURE_LOAD_CALLBACK Liquid::s_textureLoadCallback;

// OFFSET: 0x0071C200/0x1403F10A0
Liquid::CMaterialSettings::~CMaterialSettings() {
}

// OFFSET: 0x0071C270/0x1403F1150
Liquid::CMaterialSettings::CMaterialSettings() {
    *reinterpret_cast<uint32_t*>(&this->m_colors[0]) = 0;
    *reinterpret_cast<uint32_t*>(&this->m_colors[1]) = 0;
}

// OFFSET: 0x0071C820/0x1403F1900
void Liquid::CMaterialSettings::Initialize(uint32_t liquidType) {
    this->m_liquidType = liquidType;
}

// OFFSET: 0x0071C830/0x1403F1910
int32_t Liquid::CMaterialSettings::GetInt(uint32_t index) {
    return this->m_ints[index];
}

// OFFSET: 0x0071C850/0x1403F1920
float Liquid::CMaterialSettings::GetFloat(uint32_t index) {
    return this->m_floats[index];
}

// OFFSET: 0x0071C870/0x1403F1930
void Liquid::CMaterialSettings::Destroy() {
    for (uint32_t i = 0; i < 6; i++) {
        for (uint32_t j = 0; j < this->m_textures[i].Count(); j++) {
            TextureRelease(this->m_textures[i].m_data[j]);
        }

        this->m_textures[i].m_count = 0;

        for (uint32_t j = 0; j < this->m_pendingTextures[i].Count(); j++) {
            TextureRelease(this->m_pendingTextures[i].m_data[j]);
        }

        this->m_pendingTextures[i].m_count = 0;
    }
}

// OFFSET: 0x0071C8D0/0x1403F19D0
int32_t Liquid::CMaterialSettings::IsAllLoaded(uint32_t index) {
    if (this->m_texturesReady[index]) {
        return 1;
    }

    TSGrowableArray<CTexture*>& textures = this->m_textures[index];
    uint32_t count = textures.Count();
    uint8_t ready = 1;

    if (this->m_pendingTextures[index].Count()) {
        for (uint32_t i = 0; i < count && ready; i++) {
            if (TextureIsBlob(textures.m_data[i])) {
                ready = 0;
            }
        }

        if (ready) {
            for (uint32_t i = 0; i < this->m_pendingTextures[index].Count(); i++) {
                TextureRelease(this->m_pendingTextures[index].m_data[i]);
            }

            this->m_pendingTextures[index].m_count = 0;
        }
    } else {
        for (uint32_t i = 0; i < count && ready; i++) {
            if (!TextureGetGxTex(textures.m_data[i])) {
                ready = 0;
            }
        }
    }

    this->m_texturesReady[index] = ready;
    return ready;
}

// OFFSET: 0x0071C9E0/0x1403F1B20
void Liquid::CMaterialSettings::InitTexture(uint32_t index) {
    this->m_texturesReady[index] = 0;
    const char* name = this->m_textureNames[index];

    if (!*name) {
        return;
    }

    int32_t procedural = SStrStr(name, "procedural") != NULL;

    CGxTexFlags flags;
    flags.m_filter = procedural ? GxTex_Linear : GxTex_LinearMipLinear;

    if (!procedural) {
        flags.m_wrapU = 1;
        flags.m_wrapV = 1;
    }

    if (!procedural && Liquid::s_textureLoadCallback) {
        Liquid::s_textureLoadCallback(name);
    }

    CStatus loadStatus;
    CStatus status;
    CTexture* texture = NULL;

    if (SStrStr(name, "%d")) {
        char fileName[260];
        int32_t blobs = 0;
        bool done = false;

        for (uint32_t i = 1; i <= 30 && !done; i++) {
            SStrPrintf(fileName, sizeof(fileName), name, i);
            texture = TextureCreate(fileName, flags, &status, 0, NULL, 2);

            if (status.m_highestSeverity < STATUS_ERROR) {
                this->m_textures[index].New(texture);

                if (TextureIsBlob(texture)) {
                    blobs = 1;
                }
            } else {
                TextureRelease(texture);
                texture = NULL;
                done = true;
            }
        }

        if (blobs) {
            for (uint32_t i = 1; i <= 30; i++) {
                SStrPrintf(fileName, sizeof(fileName), name, i);
                texture = TextureCreateBlob(fileName, flags, &status, 1);

                if (texture) {
                    this->m_pendingTextures[index].New(texture);
                }
            }

            if (this->m_pendingTextures[index].Count() != this->m_textures[index].Count()) {
                for (uint32_t i = 0; i < this->m_pendingTextures[index].Count(); i++) {
                    TextureRelease(this->m_pendingTextures[index].m_data[i]);
                }

                this->m_pendingTextures[index].m_count = 0;
            }
        }

        return;
    }

    if (procedural) {
        texture = TextureCacheGetTexture(name);

        if (!texture) {
            CImVector color;
            *reinterpret_cast<uint32_t*>(&color) = 0xFF00FF00;
            texture = TextureCreateSolid(color);
        }
    } else {
        texture = TextureCreate(name, flags, &status, 0, NULL, 2);

        if (status.m_highestSeverity >= STATUS_ERROR) {
            TextureRelease(texture);
            texture = NULL;
            return;
        }
    }

    if (texture) {
        this->m_textures[index].New(texture);
    }
}

// OFFSET: 0x0071CD00/0x1403F1FA0
CGxTex* Liquid::CMaterialSettings::GetTexture(uint32_t index, uint32_t period) {
    TSGrowableArray<CTexture*>* textures = &this->m_textures[index];
    uint32_t count = textures->Count();

    if (!count) {
        this->InitTexture(index);
        count = textures->Count();

        if (!count) {
            return NULL;
        }
    }

    if (count == 1) {
        return TextureGetGxTex(textures->m_data[0]);
    }

    if (!this->IsAllLoaded(index)) {
        textures = &this->m_pendingTextures[index];

        if (!textures->Count()) {
            return NULL;
        }
    }

    if (!period) {
        period = 1;
    }

    double t = static_cast<double>(FrameTime::GetCurTimeMs() % period) / static_cast<double>(period);
    return TextureGetGxTex(textures->m_data[static_cast<int64_t>(t * static_cast<double>(count))]);
}

// OFFSET: 0x0071CE00/0x1403F2080
void Liquid::CMaterialSettings::LinkToAsyncHead(uint32_t index) {
    TSGrowableArray<CTexture*>& textures = this->m_textures[index];

    if (!textures.Count()) {
        this->InitTexture(index);

        if (!textures.Count()) {
            return;
        }
    }

    for (uint32_t i = 0; i < textures.Count(); i++) {
        TextureIncreasePriority(textures.m_data[i]);
    }
}
