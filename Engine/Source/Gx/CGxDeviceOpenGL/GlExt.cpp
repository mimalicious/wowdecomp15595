#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Gx/CGxDeviceOpenGL/GlExt.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00EC6928/0x140DB8BD8
static const char* s_wglExtensions;

// STATIC: 0x00EC692C/0x140DB8BE0
static const char* s_glExtensions;

// STATIC: 0x00EC6754/0x140DB88E0
int32_t glExtGPUProgramParameters;

// STATIC: 0x00EC6758/0x140DB88E8
PFNGLPROGRAMLOCALPARAMETERS4FVEXTPROC glProgramLocalParameters4fvEXT;

// STATIC: 0x00EC675C/0x140DB88F0
PFNGLPROGRAMENVPARAMETERS4FVEXTPROC glProgramEnvParameters4fvEXT;

// STATIC: 0x00EC6760/0x140DB88E4
int32_t wglExtSwapControl;

// STATIC: 0x00EC6764/0x140DB88F8
int32_t wglArbPbuffer;

// STATIC: 0x00EC6768/0x140DB88FC
int32_t wglArbPixelFormat;

// STATIC: 0x00EC676C/0x140DB8900
int32_t glATIEnvmapBumpmap;

// STATIC: 0x00EC6770/0x140DB8904
int32_t glATIFragmentShader;

int32_t glNVMultisampleFilterHint;

// STATIC: 0x00EC6784/0x140DB8918
int32_t glNVTextureShader3;

// STATIC: 0x00EC6788/0x140DB891C
int32_t glNVTextureShader2;

// STATIC: 0x00EC678C/0x140DB8920
int32_t glNVTextureShader;

// STATIC: 0x00EC6790/0x140DB8924
int32_t glNVRegisterCombiners2;

// STATIC: 0x00EC6794/0x140DB8928
int32_t glNVRegisterCombiners;

// STATIC: 0x00EC67A0/0x140DB8934
int32_t glUnkC60498;

// STATIC: 0x00EC67A4/0x140DB8938
int32_t glSGISGenerateMipmap;

// STATIC: 0x00EC67A8/0x140DB893C
int32_t glExtBlendColor;

// STATIC: 0x00EC67AC/0x140DB8940
int32_t glExtFramebufferMultisample;

// STATIC: 0x00EC67B0/0x140DB8944
int32_t glExtFramebufferObject;

// STATIC: 0x00EC67B4/0x140DB8948
int32_t glExtDrawRangeElements;

// STATIC: 0x00EC67B8/0x140DB894C
int32_t glExtTextureFilterAnisotropic;

// STATIC: 0x00EC67BC/0x140DB8950
int32_t glExtTextureCompressionS3tc;

// STATIC: 0x00EC67C0/0x140DB8954
int32_t glExtTextureCompression;

// STATIC: 0x00EC67C4/0x140DB8958
int32_t glExtTextureLodBias;

// STATIC: 0x00EC67C8/0x140DB895C
int32_t glExtClampToEdge;

// STATIC: 0x00EC67CC/0x140DB8960
int32_t glExtBgra;

// STATIC: 0x00EC67D0/0x140DB8964
int32_t glARBPointSprite;

// STATIC: 0x00EC67D4/0x140DB8968
int32_t glARBPointParameters;

// STATIC: 0x00EC67D8/0x140DB896C
int32_t glARBOcclusionQuery;

// STATIC: 0x00EC67DC/0x140DB8970
int32_t glARBMultisample;

// STATIC: 0x00EC67E0/0x140DB8974
int32_t glARBTextureCubeMap;

// STATIC: 0x00EC67E8
int32_t glARBTextureRectangle;

// STATIC: 0x00EC67EC/0x140DB8980
int32_t glARBTextureEnvCombine;

// STATIC: 0x00EC67F0/0x140DB8984
int32_t glARBVertexBufferObject;

// STATIC: 0x00EC67F4/0x140DB8988
int32_t glARBFragmentProgram;

// STATIC: 0x00EC67F8/0x140DB898C
int32_t glARBVertexProgram;

// STATIC: 0x00EC67E4/0x140DB8978
int32_t glARBTextureNonPowerOfTwo;

// STATIC: 0x00EC67FC/0x140DB8990
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

// STATIC: 0x00EC6800/0x140DB8998
PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB;

// STATIC: 0x00EC6804/0x140DB89A0
PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB;

// STATIC: 0x00EC6808/0x140DB89A8
PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;

// STATIC: 0x00EC680C/0x140DB89B0
PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB;

// STATIC: 0x00EC6810/0x140DB89B8
PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB;

// STATIC: 0x00EC6814/0x140DB89C0
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

// STATIC: 0x00EC681C/0x140DB89D0
PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;

// STATIC: 0x00EC6820/0x140DB89D8
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;

// STATIC: 0x00EC6824/0x140DB89E0
PFNGLBLENDCOLOREXTPROC glBlendColorEXT;

// STATIC: 0x00EC6828/0x140DB89E8
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;

// STATIC: 0x00EC682C/0x140DB89F0
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT;

// STATIC: 0x00EC6830/0x140DB89F8
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;

// STATIC: 0x00EC6834/0x140DB8A00
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT;

// STATIC: 0x00EC6838/0x140DB8A08
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;

// STATIC: 0x00EC683C/0x140DB8A10
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;

// STATIC: 0x00EC6840/0x140DB8A18
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;

// STATIC: 0x00EC6844/0x140DB8A20
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;

// STATIC: 0x00EC6848/0x140DB8A28
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;

// STATIC: 0x00EC684C/0x140DB8A30
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;

// STATIC: 0x00EC6850/0x140DB8A38
PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;

// STATIC: 0x00EC6854/0x140DB8A40
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;

// STATIC: 0x00EC6858/0x140DB8A48
PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB;

// STATIC: 0x00EC685C/0x140DB8A50
PFNGLPOINTPARAMETERFARBPROC glPointParameterfARB;

// STATIC: 0x00EC6860/0x140DB8A58
PFNGLGETQUERYOBJECTUIVARBPROC glGetQueryObjectuivARB;

// STATIC: 0x00EC6864/0x140DB8A60
PFNGLGETQUERYOBJECTIVARBPROC glGetQueryObjectivARB;

// STATIC: 0x00EC6868/0x140DB8A68
PFNGLGETQUERYIVARBPROC glGetQueryivARB;

// STATIC: 0x00EC686C/0x140DB8A70
PFNGLENDQUERYARBPROC glEndQueryARB;

// STATIC: 0x00EC6870/0x140DB8A78
PFNGLBEGINQUERYARBPROC glBeginQueryARB;

// STATIC: 0x00EC6874/0x140DB8A80
PFNGLDELETEQUERIESARBPROC glDeleteQueriesARB;

// STATIC: 0x00EC6878/0x140DB8A88
PFNGLGENQUERIESARBPROC glGenQueriesARB;

// STATIC: 0x00EC687C/0x140DB8A90
PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;

// STATIC: 0x00EC6880/0x140DB8A98
PFNGLMAPBUFFERARBPROC glMapBufferARB;

// STATIC: 0x00EC6884/0x140DB8AA0
PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;

// STATIC: 0x00EC6888/0x140DB8AA8
PFNGLBUFFERDATAARBPROC glBufferDataARB;

// STATIC: 0x00EC688C/0x140DB8AB0
PFNGLGENBUFFERSARBPROC glGenBuffersARB;

// STATIC: 0x00EC6890/0x140DB8AB8
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;

// STATIC: 0x00EC6894/0x140DB8AC0
PFNGLBINDBUFFERARBPROC glBindBufferARB;

PFNGLGENPROGRAMSNVPROC glGenProgramsNV;

// STATIC: 0x00EC68A8/0x140DB8AE8
PFNGLDELETEPROGRAMSNVPROC glDeleteProgramsNV;

PFNGLPROGRAMPARAMETERS4FVNVPROC glProgramParameters4fvNV;

// STATIC: 0x00EC68B0/0x140DB8AF8
PFNGLBINDPROGRAMNVPROC glBindProgramNV;

PFNGLLOADPROGRAMNVPROC glLoadProgramNV;

// STATIC: 0x00EC68B8/0x140DB8B08
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB;

// STATIC: 0x00EC68BC/0x140DB8B10
PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB;

// STATIC: 0x00EC68C0/0x140DB8B18
PFNGLVERTEXATTRIBPOINTERARBPROC glVertexAttribPointerARB;

// STATIC: 0x00EC68C4/0x140DB8B20
PFNGLGETPROGRAMIVARBPROC glGetProgramivARB;

// STATIC: 0x00EC68C8/0x140DB8B28
PFNGLISPROGRAMARBPROC glIsProgramARB;

// STATIC: 0x00EC68CC/0x140DB8B30
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB;

// STATIC: 0x00EC68D0/0x140DB8B38
PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB;

// STATIC: 0x00EC68D4/0x140DB8B40
PFNGLGENPROGRAMSARBPROC glGenProgramsARB;

// STATIC: 0x00EC68D8/0x140DB8B48
PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB;

// STATIC: 0x00EC68DC/0x140DB8B50
PFNGLBINDPROGRAMARBPROC glBindProgramARB;

// STATIC: 0x00EC68E0/0x140DB8B58
PFNGLPROGRAMSTRINGARBPROC glProgramStringARB;

// STATIC: 0x00EC68E4/0x140DB8B60
PFNGLCOMBINERPARAMETERINVPROC glCombinerParameteriNV;

// STATIC: 0x00EC68E8/0x140DB8B68
PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV;

// STATIC: 0x00EC68EC/0x140DB8B70
PFNGLFINALCOMBINERINPUTNVPROC glFinalCombinerInputNV;

// STATIC: 0x00EC68F0/0x140DB8B78
PFNGLCOMBINERSTAGEPARAMETERFVNVPROC glCombinerStageParameterfvNV;

// STATIC: 0x00EC68F4/0x140DB8B80
PFNGLCOMBINEROUTPUTNVPROC glCombinerOutputNV;

// STATIC: 0x00EC68F8/0x140DB8B88
PFNGLCOMBINERINPUTNVPROC glCombinerInputNV;

// STATIC: 0x00EC68FC/0x140DB8B90
PFNGLDRAWRANGEELEMENTSEXTPROC glDrawRangeElementsEXT;

// STATIC: 0x00EC6900/0x140DB8B98
PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC glCompressedTexSubImage2DARB;

// STATIC: 0x00EC6904/0x140DB8BA0
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB;

// STATIC: 0x00EC6908/0x140DB8BA8
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;

// STATIC: 0x00EC690C/0x140DB8BB0
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;

// STATIC: 0x00EC6910/0x140DB8BB8
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;

// STATIC: 0x00EC6914/0x140DB8BC0
PFNGLLOCKARRAYSEXTPROC glLockArraysEXT;

// STATIC: 0x00EC6918/0x140DB8BC8
GLint glMaxTextureImageUnitsARB;

// STATIC: 0x00EC691C/0x140DB8BCC
GLint glMaxTextureUnits;

// STATIC: 0x00EC6920/0x140DB8BD0
int32_t glExtCVA;

// STATIC: 0x00EC6924/0x140DB8BD4
uint32_t glVersion;

// PROVEN: d6c7ee57 IDENTICAL/IDENTICAL
// OFFSET: 0x0085B640/0x14058D1F0
void UnbindGlExtensions() {
    s_glExtensions = NULL;
    s_wglExtensions = NULL;
}

// OFFSET: 0x0085B650/0x14058D210
static bool ScanString(const char* extensions, const char* name) {
    size_t length = strlen(name);

    if (!*extensions) {
        return false;
    }

    while (_strnicmp(extensions, name, length)) {
        while (*extensions && *extensions != ' ') {
            extensions++;
        }

        while (*extensions == ' ') {
            extensions++;
        }

        if (!*extensions) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x0085B6B0
bool FindGlExt(const char* name) {
    const char* value = getenv(name);

    if (value && !SStrToInt(value)) {
        return false;
    }

    if (!s_glExtensions) {
        s_glExtensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    }

    return ScanString(s_glExtensions, name);
}

// PROVEN: 35fad743 DIFFERENT/IDENTICAL
// OFFSET: 0x0085B700/0x14058D2B0
bool HasWGLExtension(const char* name) {
    if (!wglGetExtensionsStringARB) {
        return false;
    }

    if (!s_wglExtensions) {
        s_wglExtensions = wglGetExtensionsStringARB(wglGetCurrentDC());
    }

    return ScanString(s_wglExtensions, name);
}

// OFFSET: 0x0085B740/0x14058D300
void BindGlExtensions() {
    int32_t major = -1;
    int32_t minor = -1;
    sscanf(reinterpret_cast<const char*>(glGetString(GL_VERSION)), "%d.%d", &major, &minor);
    glVersion = minor | (major << 16);

    glExtCVA = FindGlExt("GL_EXT_compiled_vertex_array");

    if (glExtCVA) {
        glLockArraysEXT = reinterpret_cast<PFNGLLOCKARRAYSEXTPROC>(wglGetProcAddress("glLockArraysEXT"));
        glUnlockArraysEXT = reinterpret_cast<PFNGLUNLOCKARRAYSEXTPROC>(wglGetProcAddress("glUnlockArraysEXT"));
    }

    if (FindGlExt("GL_ARB_multitexture")) {
        glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &glMaxTextureUnits);
        glActiveTextureARB = reinterpret_cast<PFNGLACTIVETEXTUREARBPROC>(wglGetProcAddress("glActiveTextureARB"));
        glClientActiveTextureARB = reinterpret_cast<PFNGLCLIENTACTIVETEXTUREARBPROC>(wglGetProcAddress("glClientActiveTextureARB"));
    } else {
        glMaxTextureUnits = 1;
    }

    glExtBgra = FindGlExt("GL_EXT_bgra");

    if (FindGlExt("GL_SGIS_texture_edge_clamp") || FindGlExt("GL_EXT_texture_edge_clamp")) {
        glExtClampToEdge = 1;
    } else {
        glExtClampToEdge = 0;
    }

    glExtTextureLodBias = FindGlExt("GL_EXT_texture_lod_bias");

    const char* compressedTexImage2D = NULL;

    if (glExtTextureLodBias) {
        GLfloat maxLodBias = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS_EXT, &maxLodBias);

        if (maxLodBias == 0.0f) {
            glExtTextureLodBias = 0;
        }
    }

    const char* compressedTexSubImage2D = NULL;
    glExtTextureCompression = 0;

    if (FindGlExt("GL_ARB_texture_compression")) {
        compressedTexImage2D = "glCompressedTexImage2DARB";
        compressedTexSubImage2D = "glCompressedTexSubImage2DARB";
        glExtTextureCompression = 1;
    } else if (FindGlExt("GL_EXT_texture_compression")) {
        compressedTexImage2D = "glCompressedTexImage2DEXT";
        compressedTexSubImage2D = "glCompressedTexSubImage2DEXT";
        glExtTextureCompression = 1;
    }

    glExtTextureCompressionS3tc = FindGlExt("GL_EXT_texture_compression_s3tc");

    if (glExtTextureCompression && glExtTextureCompressionS3tc) {
        glCompressedTexImage2DARB = reinterpret_cast<PFNGLCOMPRESSEDTEXIMAGE2DARBPROC>(wglGetProcAddress(compressedTexImage2D));
        glCompressedTexSubImage2DARB = reinterpret_cast<PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC>(wglGetProcAddress(compressedTexSubImage2D));
    }

    if (!glCompressedTexImage2DARB || !glCompressedTexSubImage2DARB) {
        glExtTextureCompression = 0;
        glExtTextureCompressionS3tc = 0;
    }

    glSGISGenerateMipmap = FindGlExt("GL_SGIS_generate_mipmap");
    glExtTextureFilterAnisotropic = FindGlExt("GL_EXT_texture_filter_anisotropic");
    glARBVertexBufferObject = FindGlExt("GL_ARB_vertex_buffer_object");

    if (glARBVertexBufferObject) {
        glBindBufferARB = reinterpret_cast<PFNGLBINDBUFFERARBPROC>(wglGetProcAddress("glBindBufferARB"));
        glDeleteBuffersARB = reinterpret_cast<PFNGLDELETEBUFFERSARBPROC>(wglGetProcAddress("glDeleteBuffersARB"));
        glGenBuffersARB = reinterpret_cast<PFNGLGENBUFFERSARBPROC>(wglGetProcAddress("glGenBuffersARB"));
        glBufferDataARB = reinterpret_cast<PFNGLBUFFERDATAARBPROC>(wglGetProcAddress("glBufferDataARB"));
        glBufferSubDataARB = reinterpret_cast<PFNGLBUFFERSUBDATAARBPROC>(wglGetProcAddress("glBufferSubDataARB"));
        glMapBufferARB = reinterpret_cast<PFNGLMAPBUFFERARBPROC>(wglGetProcAddress("glMapBufferARB"));
        glUnmapBufferARB = reinterpret_cast<PFNGLUNMAPBUFFERARBPROC>(wglGetProcAddress("glUnmapBufferARB"));
    }

    glExtDrawRangeElements = FindGlExt("GL_EXT_draw_range_elements");

    if (glExtDrawRangeElements) {
        glDrawRangeElementsEXT = reinterpret_cast<PFNGLDRAWRANGEELEMENTSEXTPROC>(wglGetProcAddress("glDrawRangeElementsEXT"));

        GLint maxVertices;
        GLint maxIndices;
        glGetIntegerv(GL_MAX_ELEMENTS_VERTICES_EXT, &maxVertices);
        glGetIntegerv(GL_MAX_ELEMENTS_INDICES_EXT, &maxIndices);
    }

    glUnkC60498 = 0;
    glARBTextureEnvCombine = FindGlExt("GL_ARB_texture_env_combine");
    glNVRegisterCombiners = FindGlExt("GL_NV_register_combiners");

    if (glNVRegisterCombiners) {
        glCombinerInputNV = reinterpret_cast<PFNGLCOMBINERINPUTNVPROC>(wglGetProcAddress("glCombinerInputNV"));
        glCombinerOutputNV = reinterpret_cast<PFNGLCOMBINEROUTPUTNVPROC>(wglGetProcAddress("glCombinerOutputNV"));
        glFinalCombinerInputNV = reinterpret_cast<PFNGLFINALCOMBINERINPUTNVPROC>(wglGetProcAddress("glFinalCombinerInputNV"));
        glCombinerParameterfvNV = reinterpret_cast<PFNGLCOMBINERPARAMETERFVNVPROC>(wglGetProcAddress("glCombinerParameterfvNV"));
        glCombinerParameteriNV = reinterpret_cast<PFNGLCOMBINERPARAMETERINVPROC>(wglGetProcAddress("glCombinerParameteriNV"));
    }

    glNVRegisterCombiners2 = FindGlExt("GL_NV_register_combiners2");

    if (glNVRegisterCombiners2) {
        glCombinerStageParameterfvNV = reinterpret_cast<PFNGLCOMBINERSTAGEPARAMETERFVNVPROC>(wglGetProcAddress("glCombinerStageParameterfvNV"));
    }

    glNVTextureShader = FindGlExt("GL_NV_texture_shader");
    glNVTextureShader2 = FindGlExt("GL_NV_texture_shader2");
    glNVTextureShader3 = FindGlExt("GL_NV_texture_shader3");
    glATIFragmentShader = FindGlExt("GL_ATI_fragment_shader");
    glATIEnvmapBumpmap = FindGlExt("GL_ATI_envmap_bumpmap");
    glARBFragmentProgram = FindGlExt("GL_ARB_fragment_program");

    if (glARBFragmentProgram) {
        glProgramStringARB = reinterpret_cast<PFNGLPROGRAMSTRINGARBPROC>(wglGetProcAddress("glProgramStringARB"));
        glBindProgramARB = reinterpret_cast<PFNGLBINDPROGRAMARBPROC>(wglGetProcAddress("glBindProgramARB"));
        glProgramLocalParameter4fvARB = reinterpret_cast<PFNGLPROGRAMLOCALPARAMETER4FVARBPROC>(wglGetProcAddress("glProgramLocalParameter4fvARB"));
        glDeleteProgramsARB = reinterpret_cast<PFNGLDELETEPROGRAMSARBPROC>(wglGetProcAddress("glDeleteProgramsARB"));
        glGenProgramsARB = reinterpret_cast<PFNGLGENPROGRAMSARBPROC>(wglGetProcAddress("glGenProgramsARB"));
        glIsProgramARB = reinterpret_cast<PFNGLISPROGRAMARBPROC>(wglGetProcAddress("glIsProgramARB"));
        glGetProgramivARB = reinterpret_cast<PFNGLGETPROGRAMIVARBPROC>(wglGetProcAddress("glGetProgramivARB"));
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &glMaxTextureImageUnitsARB);
    }

    glARBVertexProgram = FindGlExt("GL_ARB_vertex_program");

    if (glARBVertexProgram) {
        glVertexAttribPointerARB = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERARBPROC>(wglGetProcAddress("glVertexAttribPointerARB"));
        glEnableVertexAttribArrayARB = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYARBPROC>(wglGetProcAddress("glEnableVertexAttribArrayARB"));
        glDisableVertexAttribArrayARB = reinterpret_cast<PFNGLDISABLEVERTEXATTRIBARRAYARBPROC>(wglGetProcAddress("glDisableVertexAttribArrayARB"));
    }

    if (glARBVertexProgram || glARBFragmentProgram) {
        glProgramStringARB = reinterpret_cast<PFNGLPROGRAMSTRINGARBPROC>(wglGetProcAddress("glProgramStringARB"));
        glBindProgramARB = reinterpret_cast<PFNGLBINDPROGRAMARBPROC>(wglGetProcAddress("glBindProgramARB"));
        glProgramEnvParameter4fvARB = reinterpret_cast<PFNGLPROGRAMENVPARAMETER4FVARBPROC>(wglGetProcAddress("glProgramEnvParameter4fvARB"));
        glProgramLocalParameter4fvARB = reinterpret_cast<PFNGLPROGRAMLOCALPARAMETER4FVARBPROC>(wglGetProcAddress("glProgramLocalParameter4fvARB"));
        glDeleteProgramsARB = reinterpret_cast<PFNGLDELETEPROGRAMSARBPROC>(wglGetProcAddress("glDeleteProgramsARB"));
        glGenProgramsARB = reinterpret_cast<PFNGLGENPROGRAMSARBPROC>(wglGetProcAddress("glGenProgramsARB"));
        glIsProgramARB = reinterpret_cast<PFNGLISPROGRAMARBPROC>(wglGetProcAddress("glIsProgramARB"));
        glGetProgramivARB = reinterpret_cast<PFNGLGETPROGRAMIVARBPROC>(wglGetProcAddress("glGetProgramivARB"));
        glExtGPUProgramParameters = FindGlExt("GL_EXT_gpu_program_parameters");

        if (glExtGPUProgramParameters) {
            glProgramEnvParameters4fvEXT = reinterpret_cast<PFNGLPROGRAMENVPARAMETERS4FVEXTPROC>(wglGetProcAddress("glProgramEnvParameters4fvEXT"));
            glProgramLocalParameters4fvEXT = reinterpret_cast<PFNGLPROGRAMLOCALPARAMETERS4FVEXTPROC>(wglGetProcAddress("glProgramLocalParameters4fvEXT"));
        }
    }

    if (glVersion >= 0x10003 || FindGlExt("GL_ARB_texture_cube_map")) {
        glARBTextureCubeMap = 1;
    } else {
        glARBTextureCubeMap = 0;
    }

    glARBMultisample = FindGlExt("GL_ARB_multisample");

    if (FindGlExt("GL_ARB_texture_rectangle") || FindGlExt("GL_EXT_texture_rectangle")) {
        glARBTextureRectangle = 1;
    } else {
        bool found = FindGlExt("GL_NV_texture_rectangle");
        glARBTextureRectangle = 0;

        if (found) {
            glARBTextureRectangle = 1;
        }
    }

    if (glVersion >= 0x10005 || FindGlExt("GL_ARB_occlusion_query")) {
        glARBOcclusionQuery = 1;
        glGenQueriesARB = reinterpret_cast<PFNGLGENQUERIESARBPROC>(wglGetProcAddress("glGenQueriesARB"));
        glDeleteQueriesARB = reinterpret_cast<PFNGLDELETEQUERIESARBPROC>(wglGetProcAddress("glDeleteQueriesARB"));
        glBeginQueryARB = reinterpret_cast<PFNGLBEGINQUERYARBPROC>(wglGetProcAddress("glBeginQueryARB"));
        glEndQueryARB = reinterpret_cast<PFNGLENDQUERYARBPROC>(wglGetProcAddress("glEndQueryARB"));
        glGetQueryivARB = reinterpret_cast<PFNGLGETQUERYIVARBPROC>(wglGetProcAddress("glGetQueryivARB"));
        glGetQueryObjectivARB = reinterpret_cast<PFNGLGETQUERYOBJECTIVARBPROC>(wglGetProcAddress("glGetQueryObjectivARB"));
        glGetQueryObjectuivARB = reinterpret_cast<PFNGLGETQUERYOBJECTUIVARBPROC>(wglGetProcAddress("glGetQueryObjectuivARB"));
    } else {
        glARBOcclusionQuery = 0;
    }

    if (glVersion >= 0x10004 || FindGlExt("GL_ARB_point_parameters")) {
        glARBPointParameters = 1;
        glPointParameterfARB = reinterpret_cast<PFNGLPOINTPARAMETERFARBPROC>(wglGetProcAddress("glPointParameterfARB"));
        glPointParameterfvARB = reinterpret_cast<PFNGLPOINTPARAMETERFVARBPROC>(wglGetProcAddress("glPointParameterfvARB"));
    } else {
        glARBPointParameters = 0;
    }

    if (glVersion >= 0x20000 || FindGlExt("GL_ARB_point_sprite")) {
        glARBPointSprite = 1;
    } else {
        glARBPointSprite = 0;
    }

    glExtFramebufferObject = FindGlExt("GL_EXT_framebuffer_object");
    glExtFramebufferMultisample = FindGlExt("GL_EXT_framebuffer_multisample");

    if (glExtFramebufferObject) {
        glBindRenderbufferEXT = reinterpret_cast<PFNGLBINDRENDERBUFFEREXTPROC>(wglGetProcAddress("glBindRenderbufferEXT"));
        glDeleteRenderbuffersEXT = reinterpret_cast<PFNGLDELETERENDERBUFFERSEXTPROC>(wglGetProcAddress("glDeleteRenderbuffersEXT"));
        glGenRenderbuffersEXT = reinterpret_cast<PFNGLGENRENDERBUFFERSEXTPROC>(wglGetProcAddress("glGenRenderbuffersEXT"));
        glRenderbufferStorageEXT = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEEXTPROC>(wglGetProcAddress("glRenderbufferStorageEXT"));
        glBindFramebufferEXT = reinterpret_cast<PFNGLBINDFRAMEBUFFEREXTPROC>(wglGetProcAddress("glBindFramebufferEXT"));
        glDeleteFramebuffersEXT = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSEXTPROC>(wglGetProcAddress("glDeleteFramebuffersEXT"));
        glGenFramebuffersEXT = reinterpret_cast<PFNGLGENFRAMEBUFFERSEXTPROC>(wglGetProcAddress("glGenFramebuffersEXT"));
        glCheckFramebufferStatusEXT = reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC>(wglGetProcAddress("glCheckFramebufferStatusEXT"));
        glFramebufferTexture1DEXT = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE1DEXTPROC>(wglGetProcAddress("glFramebufferTexture1DEXT"));
        glFramebufferTexture2DEXT = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DEXTPROC>(wglGetProcAddress("glFramebufferTexture2DEXT"));
        glFramebufferTexture3DEXT = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE3DEXTPROC>(wglGetProcAddress("glFramebufferTexture3DEXT"));
        glFramebufferRenderbufferEXT = reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC>(wglGetProcAddress("glFramebufferRenderbufferEXT"));
    }

    glExtBlendColor = FindGlExt("GL_EXT_blend_color");

    if (glExtBlendColor) {
        glBlendColorEXT = reinterpret_cast<PFNGLBLENDCOLOREXTPROC>(wglGetProcAddress("glBlendColorEXT"));
    }

    glARBTextureNonPowerOfTwo = FindGlExt("GL_ARB_texture_non_power_of_two");
}
