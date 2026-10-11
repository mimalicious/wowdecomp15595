#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Gx/CGxDeviceOpenGL/GlExt.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00EC6928
static const char* s_wglExtensions;

// STATIC: 0x00EC692C
static const char* s_glExtensions;

// STATIC: 0x00EC6754
int32_t glExtGPUProgramParameters;

// STATIC: 0x00EC6758
PFNGLPROGRAMLOCALPARAMETERS4FVEXTPROC glProgramLocalParameters4fvEXT;

// STATIC: 0x00EC675C
PFNGLPROGRAMENVPARAMETERS4FVEXTPROC glProgramEnvParameters4fvEXT;

// STATIC: 0x00EC6760
int32_t wglExtSwapControl;

// STATIC: 0x00EC6764
int32_t wglArbPbuffer;

// STATIC: 0x00EC6768
int32_t wglArbPixelFormat;

// STATIC: 0x00EC676C
int32_t glATIEnvmapBumpmap;

// STATIC: 0x00EC6770
int32_t glATIFragmentShader;

int32_t glNVMultisampleFilterHint;

// STATIC: 0x00EC6784
int32_t glNVTextureShader3;

// STATIC: 0x00EC6788
int32_t glNVTextureShader2;

// STATIC: 0x00EC678C
int32_t glNVTextureShader;

// STATIC: 0x00EC6790
int32_t glNVRegisterCombiners2;

// STATIC: 0x00EC6794
int32_t glNVRegisterCombiners;

// STATIC: 0x00EC67A0
int32_t glUnkC60498;

// STATIC: 0x00EC67A4
int32_t glSGISGenerateMipmap;

// STATIC: 0x00EC67A8
int32_t glExtBlendColor;

// STATIC: 0x00EC67AC
int32_t glExtFramebufferMultisample;

// STATIC: 0x00EC67B0
int32_t glExtFramebufferObject;

// STATIC: 0x00EC67B4
int32_t glExtDrawRangeElements;

// STATIC: 0x00EC67B8
int32_t glExtTextureFilterAnisotropic;

// STATIC: 0x00EC67BC
int32_t glExtTextureCompressionS3tc;

// STATIC: 0x00EC67C0
int32_t glExtTextureCompression;

// STATIC: 0x00EC67C4
int32_t glExtTextureLodBias;

// STATIC: 0x00EC67C8
int32_t glExtClampToEdge;

// STATIC: 0x00EC67CC
int32_t glExtBgra;

// STATIC: 0x00EC67D0
int32_t glARBPointSprite;

// STATIC: 0x00EC67D4
int32_t glARBPointParameters;

// STATIC: 0x00EC67D8
int32_t glARBOcclusionQuery;

// STATIC: 0x00EC67DC
int32_t glARBMultisample;

// STATIC: 0x00EC67E0
int32_t glARBTextureCubeMap;

// STATIC: 0x00EC67E8
int32_t glARBTextureRectangle;

// STATIC: 0x00EC67EC
int32_t glARBTextureEnvCombine;

// STATIC: 0x00EC67F0
int32_t glARBVertexBufferObject;

// STATIC: 0x00EC67F4
int32_t glARBFragmentProgram;

// STATIC: 0x00EC67F8
int32_t glARBVertexProgram;

// STATIC: 0x00EC67E4
int32_t glARBTextureNonPowerOfTwo;

// STATIC: 0x00EC67FC
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

// STATIC: 0x00EC6800
PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB;

// STATIC: 0x00EC6804
PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB;

// STATIC: 0x00EC6808
PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;

// STATIC: 0x00EC680C
PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB;

// STATIC: 0x00EC6810
PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB;

// STATIC: 0x00EC6814
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

// STATIC: 0x00EC681C
PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;

// STATIC: 0x00EC6820
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;

// STATIC: 0x00EC6824
PFNGLBLENDCOLOREXTPROC glBlendColorEXT;

// STATIC: 0x00EC6828
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;

// STATIC: 0x00EC682C
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT;

// STATIC: 0x00EC6830
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;

// STATIC: 0x00EC6834
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT;

// STATIC: 0x00EC6838
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;

// STATIC: 0x00EC683C
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;

// STATIC: 0x00EC6840
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;

// STATIC: 0x00EC6844
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;

// STATIC: 0x00EC6848
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;

// STATIC: 0x00EC684C
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;

// STATIC: 0x00EC6850
PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;

// STATIC: 0x00EC6854
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;

// STATIC: 0x00EC6858
PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB;

// STATIC: 0x00EC685C
PFNGLPOINTPARAMETERFARBPROC glPointParameterfARB;

// STATIC: 0x00EC6860
PFNGLGETQUERYOBJECTUIVARBPROC glGetQueryObjectuivARB;

// STATIC: 0x00EC6864
PFNGLGETQUERYOBJECTIVARBPROC glGetQueryObjectivARB;

// STATIC: 0x00EC6868
PFNGLGETQUERYIVARBPROC glGetQueryivARB;

// STATIC: 0x00EC686C
PFNGLENDQUERYARBPROC glEndQueryARB;

// STATIC: 0x00EC6870
PFNGLBEGINQUERYARBPROC glBeginQueryARB;

// STATIC: 0x00EC6874
PFNGLDELETEQUERIESARBPROC glDeleteQueriesARB;

// STATIC: 0x00EC6878
PFNGLGENQUERIESARBPROC glGenQueriesARB;

// STATIC: 0x00EC687C
PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;

// STATIC: 0x00EC6880
PFNGLMAPBUFFERARBPROC glMapBufferARB;

// STATIC: 0x00EC6884
PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;

// STATIC: 0x00EC6888
PFNGLBUFFERDATAARBPROC glBufferDataARB;

// STATIC: 0x00EC688C
PFNGLGENBUFFERSARBPROC glGenBuffersARB;

// STATIC: 0x00EC6890
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;

// STATIC: 0x00EC6894
PFNGLBINDBUFFERARBPROC glBindBufferARB;

PFNGLGENPROGRAMSNVPROC glGenProgramsNV;

// STATIC: 0x00EC68A8
PFNGLDELETEPROGRAMSNVPROC glDeleteProgramsNV;

PFNGLPROGRAMPARAMETERS4FVNVPROC glProgramParameters4fvNV;

// STATIC: 0x00EC68B0/0x140DB8AF8
PFNGLBINDPROGRAMNVPROC glBindProgramNV;

PFNGLLOADPROGRAMNVPROC glLoadProgramNV;

// STATIC: 0x00EC68B8
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB;

// STATIC: 0x00EC68BC
PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB;

// STATIC: 0x00EC68C0
PFNGLVERTEXATTRIBPOINTERARBPROC glVertexAttribPointerARB;

// STATIC: 0x00EC68C4
PFNGLGETPROGRAMIVARBPROC glGetProgramivARB;

// STATIC: 0x00EC68C8
PFNGLISPROGRAMARBPROC glIsProgramARB;

// STATIC: 0x00EC68CC
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB;

// STATIC: 0x00EC68D0
PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB;

// STATIC: 0x00EC68D4
PFNGLGENPROGRAMSARBPROC glGenProgramsARB;

// STATIC: 0x00EC68D8
PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB;

// STATIC: 0x00EC68DC
PFNGLBINDPROGRAMARBPROC glBindProgramARB;

// STATIC: 0x00EC68E0
PFNGLPROGRAMSTRINGARBPROC glProgramStringARB;

// STATIC: 0x00EC68E4
PFNGLCOMBINERPARAMETERINVPROC glCombinerParameteriNV;

// STATIC: 0x00EC68E8
PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV;

// STATIC: 0x00EC68EC
PFNGLFINALCOMBINERINPUTNVPROC glFinalCombinerInputNV;

// STATIC: 0x00EC68F0
PFNGLCOMBINERSTAGEPARAMETERFVNVPROC glCombinerStageParameterfvNV;

// STATIC: 0x00EC68F4
PFNGLCOMBINEROUTPUTNVPROC glCombinerOutputNV;

// STATIC: 0x00EC68F8
PFNGLCOMBINERINPUTNVPROC glCombinerInputNV;

// STATIC: 0x00EC68FC
PFNGLDRAWRANGEELEMENTSEXTPROC glDrawRangeElementsEXT;

// STATIC: 0x00EC6900
PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC glCompressedTexSubImage2DARB;

// STATIC: 0x00EC6904
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB;

// STATIC: 0x00EC6908
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;

// STATIC: 0x00EC690C
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;

// STATIC: 0x00EC6910
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;

// STATIC: 0x00EC6914
PFNGLLOCKARRAYSEXTPROC glLockArraysEXT;

// STATIC: 0x00EC6918
GLint glMaxTextureImageUnitsARB;

// STATIC: 0x00EC691C
GLint glMaxTextureUnits;

// STATIC: 0x00EC6920
int32_t glExtCVA;

// STATIC: 0x00EC6924
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
