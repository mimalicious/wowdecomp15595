#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Buffer.h"

// STATIC: %0x00AFB3D8
static const CGxVertexAttrib s_vertexAttribsP[] = {
    { GxVA_Position, 4, 0, 0 }
};

// STATIC: %0x00AFB3E8
static const CGxVertexAttrib s_vertexAttribsPN[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Normal, 4, 12, 0 }
};

// STATIC: %0x00AFB408
static const CGxVertexAttrib s_vertexAttribsPNC[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Normal, 4, 12, 0 },
    { GxVA_Color0, 0, 24, 0 }
};

// STATIC: %0x00AFB438
static const CGxVertexAttrib s_vertexAttribsPNT[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Normal, 4, 12, 0 },
    { GxVA_TexCoord0, 3, 24, 0 }
};

// STATIC: %0x00AFB468
static const CGxVertexAttrib s_vertexAttribsPNCT[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Normal, 4, 12, 0 },
    { GxVA_Color0, 0, 24, 0 },
    { GxVA_TexCoord0, 3, 28, 0 }
};

// STATIC: %0x00AFB538
static const CGxVertexAttrib s_vertexAttribsPNCT_TS[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Normal, 4, 12, 0 },
    { GxVA_Color0, 0, 24, 0 },
    { GxVA_TexCoord0, 3, 28, 0 },
    { GxVA_TexCoord1, 7, 36, 0 }
};

// STATIC: %0x00AFB4A8
static const CGxVertexAttrib s_vertexAttribsPNT2[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Normal, 4, 12, 0 },
    { GxVA_TexCoord0, 3, 24, 0 },
    { GxVA_TexCoord1, 3, 32, 0 }
};

// STATIC: %0x00AFB4E8
static const CGxVertexAttrib s_vertexAttribsPNCT2[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Normal, 4, 12, 0 },
    { GxVA_Color0, 0, 24, 0 },
    { GxVA_TexCoord0, 3, 28, 0 },
    { GxVA_TexCoord1, 3, 36, 0 }
};

// STATIC: %0x00AFB588
static const CGxVertexAttrib s_vertexAttribsPC[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Color0, 0, 12, 0 }
};

// STATIC: %0x00AFB5A8
static const CGxVertexAttrib s_vertexAttribsPCT[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Color0, 0, 12, 0 },
    { GxVA_TexCoord0, 3, 16, 0 }
};

// STATIC: %0x00AFB5D8
static const CGxVertexAttrib s_vertexAttribsPCT2[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Color0, 0, 12, 0 },
    { GxVA_TexCoord0, 3, 16, 0 },
    { GxVA_TexCoord1, 3, 24, 0 }
};

// STATIC: %0x00AFB618
static const CGxVertexAttrib s_vertexAttribsPCT3[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Color0, 0, 12, 0 },
    { GxVA_TexCoord0, 3, 16, 0 },
    { GxVA_TexCoord1, 3, 24, 0 },
    { GxVA_TexCoord2, 3, 32, 0 }
};

// STATIC: %0x00AFB668
static const CGxVertexAttrib s_vertexAttribsPCT4[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Color0, 0, 12, 0 },
    { GxVA_TexCoord0, 3, 16, 0 },
    { GxVA_TexCoord1, 3, 24, 0 },
    { GxVA_TexCoord2, 3, 32, 0 },
    { GxVA_TexCoord3, 3, 40, 0 }
};

// STATIC: %0x00AFB6C8
static const CGxVertexAttrib s_vertexAttribsPT[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_TexCoord0, 3, 12, 0 }
};

// STATIC: %0x00AFB6E8
static const CGxVertexAttrib s_vertexAttribsPT2[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_TexCoord0, 3, 12, 0 },
    { GxVA_TexCoord1, 3, 20, 0 }
};

// STATIC: %0x00AFB718
static const CGxVertexAttrib s_vertexAttribsPBNT2[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_BlendWeight, 2, 12, 0 },
    { GxVA_BlendIndices, 1, 16, 0 },
    { GxVA_Normal, 4, 20, 0 },
    { GxVA_TexCoord0, 3, 32, 0 },
    { GxVA_TexCoord1, 3, 40, 0 }
};

// STATIC: %0x00AFB778
static const CGxVertexAttrib s_vertexAttribsPNC2[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Normal, 4, 12, 0 },
    { GxVA_Color0, 0, 24, 0 },
    { GxVA_Color1, 0, 28, 0 }
};

// STATIC: %0x00AFB7B8
static const CGxVertexAttrib s_vertexAttribsPNC2T[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Normal, 4, 12, 0 },
    { GxVA_Color0, 0, 24, 0 },
    { GxVA_Color1, 0, 28, 0 },
    { GxVA_TexCoord0, 3, 32, 0 }
};

// STATIC: %0x00AFB808
static const CGxVertexAttrib s_vertexAttribsPNC2T_TS[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Normal, 4, 12, 0 },
    { GxVA_Color0, 0, 24, 0 },
    { GxVA_Color1, 0, 28, 0 },
    { GxVA_TexCoord0, 3, 32, 0 },
    { GxVA_TexCoord1, 7, 40, 0 }
};

// STATIC: %0x00AFB868
static const CGxVertexAttrib s_vertexAttribsPNC2T2[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_Normal, 4, 12, 0 },
    { GxVA_Color0, 0, 24, 0 },
    { GxVA_Color1, 0, 28, 0 },
    { GxVA_TexCoord0, 3, 32, 0 },
    { GxVA_TexCoord1, 3, 40, 0 }
};

// STATIC: %0x00AFB8C8
static const CGxVertexAttrib s_vertexAttribsPT2_iDD[] = {
    { GxVA_Position, 4, 0, 0 },
    { GxVA_TexCoord0, 3, 12, 0 },
    { GxVA_TexCoord1, 3, 20, 0 },
    { GxVA_Normal, 2, 0, 1 },
    { GxVA_Color0, 0, 4, 1 },
    { GxVA_Color1, 0, 8, 1 },
    { GxVA_TexCoord2, 4, 12, 1 },
    { GxVA_TexCoord3, 2, 24, 1 },
    { GxVA_TexCoord4, 2, 28, 1 }
};

// STATIC: %0x00C0DC40
VertexBufDesc Buffer::s_vertexBufDesc[GxVertexBufferFormats_Last] = {
    { "P", s_vertexAttribsP, 1, 12 },
    { "PN", s_vertexAttribsPN, 2, 24 },
    { "PNC", s_vertexAttribsPNC, 3, 28 },
    { "PNT", s_vertexAttribsPNT, 3, 32 },
    { "PNCT", s_vertexAttribsPNCT, 4, 36 },
    { "PNCT_TS", s_vertexAttribsPNCT_TS, 5, 52 },
    { "PNT2", s_vertexAttribsPNT2, 4, 40 },
    { "PNCT2", s_vertexAttribsPNCT2, 5, 44 },
    { "PC", s_vertexAttribsPC, 2, 16 },
    { "PCT", s_vertexAttribsPCT, 3, 24 },
    { "PCT2", s_vertexAttribsPCT2, 4, 32 },
    { "PCT3", s_vertexAttribsPCT3, 5, 40 },
    { "PCT4", s_vertexAttribsPCT4, 6, 48 },
    { "PT", s_vertexAttribsPT, 2, 20 },
    { "PT2", s_vertexAttribsPT2, 3, 28 },
    { "PBNT2", s_vertexAttribsPBNT2, 6, 48 },
    { "PNC2", s_vertexAttribsPNC2, 4, 32 },
    { "PNC2T", s_vertexAttribsPNC2T, 5, 40 },
    { "PNC2T_TS", s_vertexAttribsPNC2T_TS, 6, 56 },
    { "PNC2T2", s_vertexAttribsPNC2T2, 6, 48 },
    { "", NULL, 0, 32 },
    { "PT2_iDD", s_vertexAttribsPT2_iDD, 9, 60 }
};
