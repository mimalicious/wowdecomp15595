#include <cstdint>
#include <cstring>
#include "Engine/Source/Gx/CGxDeviceD3d11/CGxDeviceD3d11.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxShaderPermute.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: %0x007D0190
void CGxDeviceD3d11::IShaderCreate(CGxShaderPermute* permute) {
    permute->m_valid = 0;

    if (!this->m_context) {
        return;
    }

    permute->m_loaded = 1;

    uint8_t* data = permute->Load();
    uint32_t size = *reinterpret_cast<uint32_t*>(data + 28);

    if (!size) {
        SMemFree(data, __FILE__, __LINE__, 0);
        return;
    }

    const void* code = data + 32;
    HRESULT result;
    void* shader;

    switch (permute->m_target) {
        case GxSh_Vertex:
            result = this->m_d3dDevice->CreateVertexShader(code, size, NULL, reinterpret_cast<ID3D11VertexShader**>(&shader));
            break;

        case GxSh_Hull:
            result = this->m_d3dDevice->CreateHullShader(code, size, NULL, reinterpret_cast<ID3D11HullShader**>(&shader));
            break;

        case GxSh_Domain:
            result = this->m_d3dDevice->CreateDomainShader(code, size, NULL, reinterpret_cast<ID3D11DomainShader**>(&shader));
            break;

        case GxSh_Geometry:
            result = this->m_d3dDevice->CreateGeometryShader(code, size, NULL, reinterpret_cast<ID3D11GeometryShader**>(&shader));
            break;

        case GxSh_Pixel:
            result = this->m_d3dDevice->CreatePixelShader(code, size, NULL, reinterpret_cast<ID3D11PixelShader**>(&shader));
            break;

        default:
            SMemFree(data, __FILE__, __LINE__, 0);
            return;
    }

    if (SUCCEEDED(result)) {
        permute->m_valid = 1;
        permute->m_apiSpecific = shader;
    }

    SMemFree(data, __FILE__, __LINE__, 0);
}

// PROVEN: 95e90cd5 IDENTICAL/NONE
// OFFSET: 0x00871330
void CGxDeviceD3d11::IShaderDestroy(CGxShaderPermute* permute) {
    if (permute->m_apiSpecific) {
        static_cast<IUnknown*>(permute->m_apiSpecific)->Release();
        permute->m_apiSpecific = NULL;
    }

    permute->m_loaded = 0;
}

// OFFSET: %0x007D02D0
void CGxDeviceD3d11::IBindVertexShader(CGxShaderPermute* permute) {
    if (!permute->m_loaded) {
        this->IShaderCreate(permute);

        if (!permute->m_loaded) {
            return;
        }
    }

    this->m_d3dContext->VSSetShader(static_cast<ID3D11VertexShader*>(permute->m_apiSpecific), NULL, 0);
}

// OFFSET: %0x007D0320
void CGxDeviceD3d11::IBindHullShader(CGxShaderPermute* permute) {
    if (!permute) {
        this->m_d3dContext->HSSetShader(NULL, NULL, 0);
        return;
    }

    if (!permute->m_loaded) {
        this->IShaderCreate(permute);

        if (!permute->m_loaded) {
            return;
        }
    }

    this->m_d3dContext->HSSetShader(static_cast<ID3D11HullShader*>(permute->m_apiSpecific), NULL, 0);
}

// OFFSET: %0x007D0390
void CGxDeviceD3d11::IBindDomainShader(CGxShaderPermute* permute) {
    if (!permute) {
        this->m_d3dContext->DSSetShader(NULL, NULL, 0);
        return;
    }

    if (!permute->m_loaded) {
        this->IShaderCreate(permute);

        if (!permute->m_loaded) {
            return;
        }
    }

    this->m_d3dContext->DSSetShader(static_cast<ID3D11DomainShader*>(permute->m_apiSpecific), NULL, 0);
}

// OFFSET: %0x007D0400
void CGxDeviceD3d11::IBindGeometryShader(CGxShaderPermute* permute) {
    if (!permute) {
        this->m_d3dContext->GSSetShader(NULL, NULL, 0);
        return;
    }

    if (!permute->m_loaded) {
        this->IShaderCreate(permute);

        if (!permute->m_loaded) {
            return;
        }
    }

    this->m_d3dContext->GSSetShader(static_cast<ID3D11GeometryShader*>(permute->m_apiSpecific), NULL, 0);
}

// OFFSET: %0x007D0470
void CGxDeviceD3d11::IBindPixelShader(CGxShaderPermute* permute) {
    if (!permute->m_loaded) {
        this->IShaderCreate(permute);

        if (!permute->m_loaded) {
            return;
        }
    }

    this->m_d3dContext->PSSetShader(static_cast<ID3D11PixelShader*>(permute->m_apiSpecific), NULL, 0);
}

// OFFSET: %0x007D04C0
void CGxDeviceD3d11::IShaderConstantsFlush() {
    int32_t renderStates[GxShTargets_Last] = { GxRs_VertexShader, GxRs_HullShader, GxRs_DomainShader, GxRs_GeometryShader, GxRs_PixelShader, -1 };

    for (uint32_t target = 0; target < GxShTargets_Last; target++) {
        ShadowConstants& shadow = CGxDevice::s_shadowConstants[target];

        if (shadow.dirtyMin > shadow.dirtyMax || renderStates[target] < 0) {
            continue;
        }

        CGxShaderPermute* permute = static_cast<CGxShaderPermute*>(this->m_appRenderStates[renderStates[target]].m_value.m_data.p);

        if (!permute) {
            continue;
        }

        uint32_t count = (permute->unk_0014 + 15) >> 4;

        if (shadow.dirtyMin >= count) {
            continue;
        }

        ID3D11Buffer* buffer = this->m_constantBuffers[target];
        D3D11_MAPPED_SUBRESOURCE mapped;

        if (SUCCEEDED(this->m_d3dContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
            memcpy(mapped.pData, shadow.constants, permute->unk_0014);
            this->m_d3dContext->Unmap(buffer, 0);
        }

        shadow.dirtyMin = count;
    }
}

// OFFSET: %0x007AED20
void CGxDeviceD3d11::IReleaseD3dShaders(int32_t freeShaders) {
    if (!freeShaders) {
        return;
    }

    for (uint32_t target = 0; target < GxShTargets_Last; target++) {
        for (CGxShader* shader = CGxDevice::s_shaderList[target].Head(); shader; shader = CGxDevice::s_shaderList[target].Next(shader)) {
            for (uint32_t i = 0; i < shader->m_permutes.Count(); i++) {
                this->IShaderDestroy(&shader->m_permutes[i]);
            }
        }
    }
}
