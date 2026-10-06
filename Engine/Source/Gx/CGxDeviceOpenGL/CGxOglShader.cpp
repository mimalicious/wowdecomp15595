#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "Engine/Source/Gx/CGxDeviceOpenGL/CGxDeviceOpenGl.h"
#include "Engine/Source/Gx/CGxDeviceOpenGL/Types.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxShaderPermute.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: d24cdf26 IDENTICAL/IDENTICAL
// OFFSET: 0x00869890/0x14059D580
void NvCombinerStage::Apply(GLenum stage, GLenum portion) {
    for (uint32_t i = 0; i <= 3; i++) {
        glCombinerInputNV(stage, portion, GL_VARIABLE_A_NV + i, this->m_inputs[i].m_input, this->m_inputs[i].m_mapping, this->m_inputs[i].m_componentUsage);
    }

    glCombinerOutputNV(stage, portion, this->m_abOutput, this->m_cdOutput, this->m_sumOutput, this->m_scale, this->m_bias, this->m_abDotProduct, this->m_cdDotProduct, this->m_muxSum);
}

// OFFSET: %0x007C61D0
void NvPixelShader::SetupRegisterCombiners() {
    glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, this->m_numGeneralCombiners);
    glCombinerParameteriNV(GL_COLOR_SUM_CLAMP_NV, this->m_colorSumClamp);
    glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, this->m_constants[0]);
    glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, this->m_constants[1]);

    for (GLenum stage = GL_COMBINER0_NV; static_cast<GLint>(stage - GL_COMBINER0_NV) < this->m_numGeneralCombiners; stage++) {
        NvGeneralCombiner& combiner = this->m_generalCombiners[stage - GL_COMBINER0_NV];
        GLboolean perStageConstants = this->m_perStageConstants;
        combiner.m_rgb.Apply(stage, GL_RGB);
        combiner.m_alpha.Apply(stage, GL_ALPHA);

        if (perStageConstants) {
            glCombinerStageParameterfvNV(stage, GL_CONSTANT_COLOR0_NV, combiner.m_constants[0]);
            glCombinerStageParameterfvNV(stage, GL_CONSTANT_COLOR1_NV, combiner.m_constants[1]);
        }
    }

    for (uint32_t i = 0; i <= 6; i++) {
        glFinalCombinerInputNV(GL_VARIABLE_A_NV + i, this->m_finalInputs[i].m_input, this->m_finalInputs[i].m_mapping, this->m_finalInputs[i].m_componentUsage);
    }
}

// PROVEN: ad033c45 IDENTICAL/DIFFERENT
// OFFSET: 0x00869A00/0x14059D780
void NvTextureShaderStage::Apply(uint32_t tmu) {
    glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, this->m_shaderOperation);

    GLint cullModes[4];
    cullModes[0] = this->m_cullModes[0] ? GL_LESS : GL_GEQUAL;
    cullModes[1] = this->m_cullModes[1] ? GL_LESS : GL_GEQUAL;
    cullModes[2] = this->m_cullModes[2] ? GL_LESS : GL_GEQUAL;
    cullModes[3] = this->m_cullModes[3] ? GL_LESS : GL_GEQUAL;
    glTexEnviv(GL_TEXTURE_SHADER_NV, GL_CULL_MODES_NV, cullModes);

    glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV, this->m_dotProductMapping);

    if (this->m_previousInput - GL_TEXTURE0_ARB < tmu) {
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV, this->m_previousInput);
    }
}

// PROVEN: 012ecb6e IDENTICAL/IDENTICAL
// OFFSET: 0x00869AC0/0x14059D830
void NvTextureShader::Apply() {
    for (uint32_t tmu = 0; tmu < 4; tmu++) {
        static_cast<CGxDeviceOpenGl*>(g_theGxDevicePtr)->DsSet(CGxDeviceOpenGl::Ds_ActiveTexture, tmu, 0);
        this->m_stages[tmu].Apply(tmu);
    }
}

// OFFSET: %0x007C63B0
void CGxDeviceOpenGl::IVertexShaderPatch(char* code, uint32_t size) {
    uint32_t numConstants = this->m_caps.m_shaderConstants[GxSh_Vertex];
    uint32_t paramSize;

    if (numConstants < 223) {
        paramSize = numConstants < 192 ? 94 : 192;
    } else {
        paramSize = 223;
    }

    char* end = code + size;

    switch (this->m_caps.m_shaderTargets[GxSh_Vertex]) {
        case GxShProfile_Arbvp1: {
            for (char* c = code; c < end; c++) {
                if (!strncmp(c, ".local", 6)) {
                    int32_t bracket = c[6] == '[';
                    memcpy(c, ".env  ", 6);

                    int32_t first;
                    int32_t last;
                    int32_t consumed;

                    if (bracket && sscanf(c + 4, " [%d..%d]%n", &first, &last, &consumed) == 2 && last == 93) {
                        char buffer[64];
                        SStrPrintf(buffer, sizeof(buffer), "[%d..%d]          ", first, paramSize - 1);
                        memcpy(c + 4, buffer, consumed);
                    }
                }

                int32_t index;
                int32_t consumed;

                if (sscanf(c, "PARAM c[%d] = { %n", &index, &consumed) == 1 && index == 94) {
                    char buffer[64];
                    SStrPrintf(buffer, sizeof(buffer), "PARAM c[%d]={              ", paramSize);
                    memcpy(c, buffer, consumed);
                }
            }

            break;
        }

        case GxShProfile_Arbvp1Cg12: {
            for (char* c = code; c < end; c++) {
                char* start = c;

                if (c + 6 >= end || memcmp(c, "PARAM c", 7)) {
                    continue;
                }

                c += 7;

                uint32_t reg = 0;
                uint32_t last = 0;

                if (c < end) {
                    while (isdigit(*c)) {
                        reg = *c++ + 10 * reg - '0';

                        if (c >= end) {
                            break;
                        }
                    }

                    if (c < end && *c == '[') {
                        c++;

                        uint32_t count = 0;

                        while (c < end && isdigit(*c)) {
                            count = *c++ + 10 * count - '0';
                        }

                        last = count + reg - 1;

                        if (c >= end || *c != ']') {
                            continue;
                        }

                        c++;
                    }
                }

                char localBlock[] = " = { program.local";
                char envBlock[] = " program.env  ";
                char local[] = " = program.local";
                char env[] = "rogram.env  ";

                if (c + 17 >= end) {
                    continue;
                }

                if (!memcmp(c, localBlock, 18)) {
                    memcpy(c, " = {", 4);
                    memcpy(c + 4, envBlock, 14);
                } else if (!memcmp(c, local, 16)) {
                    memcpy(c, " = p", 4);
                    memcpy(c + 4, env, 12);
                } else {
                    continue;
                }

                if (reg == 31 && last == 93) {
                    int32_t written = sprintf(start, "PARAM c%u[%u]={program.env[%u..%u]}", 31, paramSize - 31, 31, paramSize - 1);
                    char* tail = start + written;
                    *tail = ';';
                    tail++;

                    if (*tail != '\n') {
                        do {
                            *tail++ = ' ';
                        } while (*tail != '\n');
                    }
                }
            }

            break;
        }

        default:
            break;
    }
}

// OFFSET: %0x007C6780
void CGxDeviceOpenGl::IPixelShaderPatch(char* code, uint32_t size) {
    if (this->m_caps.m_shaderTargets[GxSh_Pixel] != GxShProfile_Arbfp1) {
        return;
    }

    for (char* c = code; c < code + size; c++) {
        if (!strncmp(c, ".local", 6)) {
            memcpy(c, ".env  ", 6);
        }
    }
}

// OFFSET: %0x007C67E0
void CGxDeviceOpenGl::IShaderDestroyVertex(CGxShaderPermute* permute) {
    if (permute->m_apiSpecific) {
        switch (this->m_caps.m_shaderTargets[GxSh_Vertex]) {
            case GxShProfile_Arbvp1:
            case GxShProfile_Arbvp1Cg12:
                glDeleteProgramsARB(1, reinterpret_cast<GLuint*>(&permute->m_apiSpecific));
                break;

            case GxShProfile_Vp20:
            case GxShProfile_Vp30:
                glDeleteProgramsNV(1, reinterpret_cast<GLuint*>(&permute->m_apiSpecific));
                break;

            default:
                break;
        }

        permute->m_apiSpecific = NULL;
    }

    permute->m_loaded = 0;
}

// OFFSET: %0x007C6850
void CGxDeviceOpenGl::IShaderDestroyPixel(CGxShaderPermute* permute) {
    if (permute->m_apiSpecific) {
        int32_t target = this->m_caps.m_shaderTargets[GxSh_Pixel];

        if (target == GxShProfile_Arbfp1) {
            glDeleteProgramsARB(1, reinterpret_cast<GLuint*>(&permute->m_apiSpecific));
        } else if (static_cast<uint32_t>(target - GxShProfile_Fp10) <= GxShProfile_Fp25 - GxShProfile_Fp10) {
            SMemFree(permute->m_apiSpecific, __FILE__, __LINE__, 0);
        }

        permute->m_apiSpecific = NULL;
    }

    permute->m_loaded = 0;
}

// OFFSET: %0x007C68B0
void CGxDeviceOpenGl::IShaderDestroy(CGxShaderPermute* permute) {
    switch (permute->m_target) {
        case GxSh_Vertex:
            this->IShaderDestroyVertex(permute);
            break;

        case GxSh_Pixel:
            this->IShaderDestroyPixel(permute);
            break;

        default:
            break;
    }
}

// OFFSET: %0x007C68E0
void CGxDeviceOpenGl::ShaderConstantsSet(EGxShTarget target, uint32_t index, const float* constants, uint32_t count) {
    if (target == GxSh_Vertex) {
        ShadowConstants& shadow = CGxDevice::s_shadowConstants[GxSh_Vertex];
        float* dest = &shadow.constants[index * 4];

        for (uint32_t i = index; i < index + count; i++) {
            int32_t changed = 0;

            if (constants[0] != dest[0]) {
                dest[0] = constants[0];
                changed = 1;
            }

            if (constants[1] != dest[1]) {
                dest[1] = constants[1];
                changed = 1;
            }

            if (constants[2] != dest[2]) {
                dest[2] = constants[2];
                changed = 1;
            }

            if (constants[3] != dest[3]) {
                dest[3] = constants[3];
                changed = 1;
            }

            if (changed) {
                if (shadow.dirtyMin > i) {
                    shadow.dirtyMin = i;
                }

                if (shadow.dirtyMax < i) {
                    shadow.dirtyMax = i;
                }
            }

            dest += 4;
            constants += 4;
        }
    } else {
        ShadowConstants& shadow = CGxDevice::s_shadowConstants[GxSh_Pixel];

        if (shadow.dirtyMin > index) {
            shadow.dirtyMin = index;
        }

        if (shadow.dirtyMax < index + count - 1) {
            shadow.dirtyMax = index + count - 1;
        }

        float* dest = &shadow.constants[index * 4];

        for (uint32_t i = 0; i < count; i++) {
            dest[0] = constants[0];
            dest[1] = constants[1];
            dest[2] = constants[2];
            dest[3] = constants[3];
            dest += 4;
            constants += 4;
        }

    }
}

// OFFSET: %0x007C6AA0
void CGxDeviceOpenGl::IShaderCreatePixel(CGxShaderPermute* permute) {
    permute->m_valid = 0;
    permute->m_loaded = 1;

    uint8_t* data = permute->Load();
    uint32_t codeSize = *reinterpret_cast<uint32_t*>(data + 28);

    if (!codeSize) {
        SMemFree(data, __FILE__, __LINE__, 0);
        return;
    }

    char* code = reinterpret_cast<char*>(data + 32);
    this->IPixelShaderPatch(code, codeSize);

    int32_t target = this->m_caps.m_shaderTargets[GxSh_Pixel];

    if (target == GxShProfile_Arbfp1) {
        GLint current;
        glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_BINDING_ARB, &current);

        if (!permute->m_apiSpecific) {
            glGenProgramsARB(1, reinterpret_cast<GLuint*>(&permute->m_apiSpecific));
        }

        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, static_cast<GLuint>(reinterpret_cast<uintptr_t>(permute->m_apiSpecific)));
        glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, *reinterpret_cast<uint32_t*>(data + 28) - 1, code);

        GLint errorPosition;
        glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errorPosition);

        GLint underNativeLimits;
        glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB, &underNativeLimits);

        permute->m_valid = errorPosition == -1 && underNativeLimits;
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, current);
        SMemFree(data, __FILE__, __LINE__, 0);
    } else if (static_cast<uint32_t>(target - GxShProfile_Fp10) <= GxShProfile_Fp25 - GxShProfile_Fp10) {
        permute->m_valid = 1;
        permute->m_apiSpecific = data;
    }
}

// OFFSET: %0x007C6BD0
void CGxDeviceOpenGl::IShaderCreateVertex(CGxShaderPermute* permute) {
    permute->m_valid = 0;
    permute->m_loaded = 1;

    uint8_t* data = permute->Load();
    uint32_t codeSize = *reinterpret_cast<uint32_t*>(data + 28);

    if (!codeSize) {
        SMemFree(data, __FILE__, __LINE__, 0);
        return;
    }

    char* code = reinterpret_cast<char*>(data + 32);
    this->IVertexShaderPatch(code, codeSize);

    switch (this->m_caps.m_shaderTargets[GxSh_Vertex]) {
        case GxShProfile_Arbvp1:
        case GxShProfile_Arbvp1Cg12: {
            GLint current;
            glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_BINDING_ARB, &current);

            if (!permute->m_apiSpecific) {
                glGenProgramsARB(1, reinterpret_cast<GLuint*>(&permute->m_apiSpecific));
            }

            glBindProgramARB(GL_VERTEX_PROGRAM_ARB, static_cast<GLuint>(reinterpret_cast<uintptr_t>(permute->m_apiSpecific)));
            glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, *reinterpret_cast<uint32_t*>(data + 28) - 1, code);

            GLint errorPosition;
            glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errorPosition);

            GLint underNativeLimits;
            glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB, &underNativeLimits);

            permute->m_valid = errorPosition == -1 && underNativeLimits;
            glBindProgramARB(GL_VERTEX_PROGRAM_ARB, current);
            break;
        }

        case GxShProfile_Vp20:
        case GxShProfile_Vp30: {
            glGenProgramsNV(1, reinterpret_cast<GLuint*>(&permute->m_apiSpecific));
            glLoadProgramNV(GL_VERTEX_PROGRAM_ARB, static_cast<GLuint>(reinterpret_cast<uintptr_t>(permute->m_apiSpecific)), *reinterpret_cast<uint32_t*>(data + 28) - 1, reinterpret_cast<const GLubyte*>(code));

            GLint errorPosition;
            glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errorPosition);
            permute->m_valid = errorPosition == -1;
            break;
        }

        default:
            break;
    }

    SMemFree(data, __FILE__, __LINE__, 0);
}

// OFFSET: %0x007C6D50
void CGxDeviceOpenGl::IShaderCreate(CGxShaderPermute* permute) {
    switch (permute->m_target) {
        case GxSh_Vertex:
            this->IShaderCreateVertex(permute);
            break;

        case GxSh_Pixel:
            this->IShaderCreatePixel(permute);
            break;

        default:
            break;
    }
}

// OFFSET: %0x007C6D80
void CGxDeviceOpenGl::IShaderConstantsSend(EGxShTarget target, uint32_t index, const float* constants, uint32_t count) {
    if (!this->m_caps.m_shaderTargets[target]) {
        return;
    }

    this->IRsSync(0);

    if (target == GxSh_Vertex) {
        switch (this->m_caps.m_shaderTargets[GxSh_Vertex]) {
            case GxShProfile_Arbvp1:
            case GxShProfile_Arbvp1Cg12:
                if (glExtGPUProgramParameters) {
                    glProgramEnvParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, index, count, constants);
                } else {
                    for (uint32_t i = 0; i < count; i++) {
                        glProgramEnvParameter4fvARB(GL_VERTEX_PROGRAM_ARB, i + index, constants);
                        constants += 4;
                    }
                }

                break;

            case GxShProfile_Vp20:
            case GxShProfile_Vp30:
                glProgramParameters4fvNV(GL_VERTEX_PROGRAM_ARB, index, count, constants);
                break;

            default:
                break;
        }

        return;
    }

    if (target != GxSh_Pixel) {
        return;
    }

    switch (this->m_caps.m_shaderTargets[GxSh_Pixel]) {
        case GxShProfile_Arbfp1:
            if (glExtGPUProgramParameters) {
                glProgramEnvParameters4fvEXT(GL_FRAGMENT_PROGRAM_ARB, index, count, constants);
            } else {
                for (uint32_t i = 0; i < count; i++) {
                    glProgramEnvParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, i + index, constants);
                    constants += 4;
                }
            }

            break;

        case GxShProfile_Fp10:
            for (uint32_t i = 0; i < count; i++) {
                glCombinerParameterfvNV(index + i + GL_CONSTANT_COLOR0_NV, constants);
                constants += 4;
            }

            break;

        case GxShProfile_Fp20:
        case GxShProfile_Fp25: {
            CGxShaderPermute* permute = static_cast<CGxShaderPermute*>(this->m_appRenderStates[GxRs_PixelShader].m_value.m_data.p);

            if (!permute) {
                break;
            }

            NvPixelShader* program = reinterpret_cast<NvPixelShader*>(static_cast<uint8_t*>(permute->m_apiSpecific) + 32);

            if (!program) {
                break;
            }

            for (uint32_t i = 0; i < count; i++) {
                for (uint32_t j = 0; j < program->m_constantMapCount; j++) {
                    if (program->m_constantMap[j] >> 5 == index + i) {
                        uint32_t slot = program->m_constantMap[j] & 0x1F;

                        if (slot < 2) {
                            glCombinerParameterfvNV(slot + GL_CONSTANT_COLOR0_NV, constants);
                        } else {
                            glCombinerStageParameterfvNV((slot >> 1) - 1 + GL_COMBINER0_NV, (slot & 1) + GL_CONSTANT_COLOR0_NV, constants);
                        }
                    }
                }

                constants += 4;
            }

            break;
        }

        default:
            break;
    }
}

// OFFSET: %0x007C6FE0
void CGxDeviceOpenGl::IShaderConstantsFlush() {
    ShadowConstants& vertex = CGxDevice::s_shadowConstants[GxSh_Vertex];

    if (vertex.dirtyMin <= vertex.dirtyMax) {
        this->IShaderConstantsSend(GxSh_Vertex, vertex.dirtyMin, &vertex.constants[vertex.dirtyMin * 4], vertex.dirtyMax - vertex.dirtyMin + 1);
    }

    vertex.dirtyMin = 255;
    vertex.dirtyMax = 0;

    ShadowConstants& pixel = CGxDevice::s_shadowConstants[GxSh_Pixel];

    if (pixel.dirtyMin <= pixel.dirtyMax) {
        this->IShaderConstantsSend(GxSh_Pixel, pixel.dirtyMin, &pixel.constants[pixel.dirtyMin * 4], pixel.dirtyMax - pixel.dirtyMin + 1);
    }

    pixel.dirtyMax = 0;
    pixel.dirtyMin = 255;
}

// OFFSET: %0x007C7060
void CGxDeviceOpenGl::IBindPixelShader(CGxShaderPermute* permute) {
    this->IResourceTouch(permute);

    if (!permute) {
        switch (this->m_caps.m_shaderTargets[GxSh_Pixel]) {
            case GxShProfile_Arbfp1:
                this->DsSet(Ds_FragmentProgram, 0, 0);
                break;

            case GxShProfile_Fp10:
                this->DsSet(Ds_RegisterCombiners, 0, 0);
                break;

            case GxShProfile_Fp20:
            case GxShProfile_Fp25:
                this->DsSet(Ds_RegisterCombiners, 0, 0);
                this->DsSet(Ds_TextureShader, 0, 0);
                break;

            default:
                break;
        }
    } else {
        if (!permute->m_loaded) {
            this->IShaderCreatePixel(permute);
        }

        switch (this->m_caps.m_shaderTargets[GxSh_Pixel]) {
            case GxShProfile_Arbfp1:
                glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, static_cast<GLuint>(reinterpret_cast<uintptr_t>(permute->m_apiSpecific)));
                this->DsSet(Ds_FragmentProgram, 1, 0);
                break;

            case GxShProfile_Fp10: {
                NvPixelShader* program = reinterpret_cast<NvPixelShader*>(static_cast<uint8_t*>(permute->m_apiSpecific) + 32);

                if (program) {
                    program->SetupRegisterCombiners();
                    this->DsSet(Ds_PerStageConstants, program->m_perStageConstants, 0);
                    this->DsSet(Ds_RegisterCombiners, 1, 0);
                }

                break;
            }

            case GxShProfile_Fp20:
            case GxShProfile_Fp25: {
                NvPixelShader* program = reinterpret_cast<NvPixelShader*>(static_cast<uint8_t*>(permute->m_apiSpecific) + 32);

                if (program) {
                    program->SetupRegisterCombiners();
                    program->m_textureShader.Apply();
                    this->DsSet(Ds_PerStageConstants, program->m_perStageConstants, 0);
                    this->DsSet(Ds_RegisterCombiners, 1, 0);
                    this->DsSet(Ds_TextureShader, 1, 0);
                }

                break;
            }

            default:
                break;
        }
    }
}

// OFFSET: %0x007C71E0
void CGxDeviceOpenGl::IBindVertexShader(CGxShaderPermute* permute) {
    this->IResourceTouch(permute);

    if (!permute) {
        switch (this->m_caps.m_shaderTargets[GxSh_Vertex]) {
            case GxShProfile_Arbvp1:
            case GxShProfile_Arbvp1Cg12:
                this->DsSet(Ds_VertexProgram, 0, 0);
                break;

            case GxShProfile_Vp20:
            case GxShProfile_Vp30:
                this->DsSet(Ds_VertexProgramNV, 0, 0);
                break;

            default:
                break;
        }
    } else {
        if (!permute->m_loaded) {
            this->IShaderCreateVertex(permute);
        }

        switch (this->m_caps.m_shaderTargets[GxSh_Vertex]) {
            case GxShProfile_Arbvp1:
            case GxShProfile_Arbvp1Cg12:
                glBindProgramARB(GL_VERTEX_PROGRAM_ARB, static_cast<GLuint>(reinterpret_cast<uintptr_t>(permute->m_apiSpecific)));
                this->DsSet(Ds_VertexProgram, 1, 0);
                break;

            case GxShProfile_Vp20:
            case GxShProfile_Vp30:
                glBindProgramNV(GL_VERTEX_PROGRAM_ARB, static_cast<GLuint>(reinterpret_cast<uintptr_t>(permute->m_apiSpecific)));
                this->DsSet(Ds_VertexProgramNV, 1, 0);
                break;

            default:
                break;
        }
    }
}
