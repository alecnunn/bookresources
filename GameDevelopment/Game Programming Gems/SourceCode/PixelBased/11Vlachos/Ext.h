/* Copyright (C) Alex Vlachos, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Alex Vlachos, 2000"
 */
#ifndef __ext_h
#define __ext_h

#ifdef __cplusplus
   extern "C" {
#endif

#ifndef APIENTRY
#define APIENTRY
#endif

// INCLUDES ==============================================================

// DEFINES ===============================================================
#define GL_CLAMP_TO_EDGE     0x812F
#define GL_CLIP_VOLUME_HINT  0x80F0

#define GL_COMBINE_EXT          0x8570
#define GL_COMBINE_RGB_EXT      0x8571
#define GL_COMBINE_ALPHA_EXT    0x8572
#define GL_SOURCE0_RGB_EXT      0x8580
#define GL_SOURCE1_RGB_EXT      0x8581
#define GL_SOURCE2_RGB_EXT      0x8582
#define GL_SOURCE0_ALPHA_EXT    0x8588
#define GL_SOURCE1_ALPHA_EXT    0x8589
#define GL_SOURCE2_ALPHA_EXT    0x858A
#define GL_OPERAND0_RGB_EXT     0x8590
#define GL_OPERAND1_RGB_EXT     0x8591
#define GL_OPERAND2_RGB_EXT     0x8592
#define GL_OPERAND0_ALPHA_EXT   0x8598
#define GL_OPERAND1_ALPHA_EXT   0x8599
#define GL_OPERAND2_ALPHA_EXT   0x859A
#define GL_RGB_SCALE_EXT        0x8573
#define GL_ADD_SIGNED_EXT       0x8574
#define GL_INTERPOLATE_EXT      0x8575
#define GL_CONSTANT_EXT         0x8576
#define GL_PRIMARY_COLOR_EXT    0x8577
#define GL_PREVIOUS_EXT         0x8578

//Parameters to NV_texgen_reflection
#define GL_NORMAL_MAP_NV        0x8511
#define GL_REFLECTION_MAP_NV    0x8512

//Texture compression
#define GL_RGB_S3TC             0x83A0

//3D Textures
#define GL_TEXTURE_3D           0x806F

/* multitexture */
#define GL_TEXTURE0_ARB                   0x84C0
#define GL_TEXTURE1_ARB                   0x84C1
#define GL_TEXTURE2_ARB                   0x84C2
#define GL_TEXTURE3_ARB                   0x84C3
#define GL_TEXTURE4_ARB                   0x84C4
#define GL_TEXTURE5_ARB                   0x84C5
#define GL_TEXTURE6_ARB                   0x84C6
#define GL_TEXTURE7_ARB                   0x84C7
#define GL_TEXTURE8_ARB                   0x84C8
#define GL_TEXTURE9_ARB                   0x84C9
#define GL_TEXTURE10_ARB                  0x84CA
#define GL_TEXTURE11_ARB                  0x84CB
#define GL_TEXTURE12_ARB                  0x84CC
#define GL_TEXTURE13_ARB                  0x84CD
#define GL_TEXTURE14_ARB                  0x84CE
#define GL_TEXTURE15_ARB                  0x84CF
#define GL_TEXTURE16_ARB                  0x84D0
#define GL_TEXTURE17_ARB                  0x84D1
#define GL_TEXTURE18_ARB                  0x84D2
#define GL_TEXTURE19_ARB                  0x84D3
#define GL_TEXTURE20_ARB                  0x84D4
#define GL_TEXTURE21_ARB                  0x84D5
#define GL_TEXTURE22_ARB                  0x84D6
#define GL_TEXTURE23_ARB                  0x84D7
#define GL_TEXTURE24_ARB                  0x84D8
#define GL_TEXTURE25_ARB                  0x84D9
#define GL_TEXTURE26_ARB                  0x84DA
#define GL_TEXTURE27_ARB                  0x84DB
#define GL_TEXTURE28_ARB                  0x84DC
#define GL_TEXTURE29_ARB                  0x84DD
#define GL_TEXTURE30_ARB                  0x84DE
#define GL_TEXTURE31_ARB                  0x84DF
#define GL_ACTIVE_TEXTURE_ARB             0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB      0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB          0x84E2

//Other
#define GL_EXT_abgr                         1
#define GL_EXT_bgra                         1
#define GL_EXT_compiled_vertex_array        1
#define GL_EXT_packed_pixels                1
#define GL_EXT_paletted_texture             1
#define GL_EXT_vertex_array                 1
#define GL_SGI_compiled_vertex_array        1
#define GL_SGI_cull_vertex                  1
#define GL_SGI_index_array_formats          1
#define GL_SGI_index_func                   1
#define GL_SGI_index_material               1
#define GL_SGI_index_texture                1
#define GL_WIN_swap_hint                    1

/* EXT_vertex_array */
typedef void (APIENTRY * PFNGLARRAYELEMENTEXTPROC) (GLint i);
typedef void (APIENTRY * PFNGLCOLORPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLDRAWARRAYSEXTPROC) (GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLEDGEFLAGPOINTEREXTPROC) (GLsizei stride, GLsizei count, const GLboolean *pointer);
typedef void (APIENTRY * PFNGLGETPOINTERVEXTPROC) (GLenum pname, GLvoid* *params);
typedef void (APIENTRY * PFNGLINDEXPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLNORMALPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLTEXCOORDPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLVERTEXPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);

/* EXT_color_subtable */
typedef void (APIENTRY * PFNGLCOLORSUBTABLEEXTPROC) (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *table);

/* EXT_color_table */
typedef void (APIENTRY * PFNGLCOLORTABLEEXTPROC) (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table);
typedef void (APIENTRY * PFNGLCOPYCOLORTABLEEXTPROC) (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
typedef void (APIENTRY * PFNGLGETCOLORTABLEEXTPROC) (GLenum target, GLenum format, GLenum type, GLvoid *table);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERFVEXTPROC) (GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERIVEXTPROC) (GLenum target, GLenum pname, GLint *params);

/* SGI_compiled_vertex_array */
typedef void (APIENTRY * PFNGLLOCKARRAYSSGIPROC) (GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLUNLOCKARRAYSSGIPROC) (void);

/* SGI_cull_vertex */
typedef void (APIENTRY * PFNGLCULLPARAMETERDVSGIPROC) (GLenum pname, GLdouble* params);
typedef void (APIENTRY * PFNGLCULLPARAMETERFVSGIPROC) (GLenum pname, GLfloat* params);

/* SGI_index_func */
typedef void (APIENTRY * PFNGLINDEXFUNCSGIPROC) (GLenum func, GLclampf ref);

/* SGI_index_material */
typedef void (APIENTRY * PFNGLINDEXMATERIALSGIPROC) (GLenum face, GLenum mode);

/* WIN_swap_hint */
typedef void (APIENTRY * PFNGLADDSWAPHINTRECTWINPROC) (GLint x, GLint y, GLsizei width, GLsizei height);

// New protos for extension data
/* SGI_Multitexture */
typedef void (APIENTRY * PFNGLMTEXCOORD2FEXTPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMTEXCOORD3FEXTPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRY * PFNGLMTEXCOORD4FEXTPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRY * PFNGLMTEXCOORD2FVEXTPROC) (GLenum target, GLfloat *v);
typedef void (APIENTRY * PFNGLMTEXCOORD3FVEXTPROC) (GLenum target, GLfloat *v);
typedef void (APIENTRY * PFNGLMTEXCOORD4FVEXTPROC) (GLenum target, GLfloat *v);
typedef void (APIENTRY * PFNGLMTEXCOORD2IEXTPROC) (GLenum target, GLint s, GLint t);
typedef void (APIENTRY * PFNGLMTEXCOORD3IEXTPROC) (GLenum target, GLint s, GLint t, GLint r);
typedef void (APIENTRY * PFNGLMTEXCOORD4IEXTPROC) (GLenum target, GLint s, GLint t, GLint r, GLint q);
typedef void (APIENTRY * PFNGLMTEXCOORD2IVEXTPROC) (GLenum target, GLint *v);
typedef void (APIENTRY * PFNGLMTEXCOORD3IVEXTPROC) (GLenum target, GLint *v);
typedef void (APIENTRY * PFNGLMTEXCOORD4IVEXTPROC) (GLenum target, GLint *v);
typedef void (APIENTRY * PFNGLSELECTTEXTUREEXTPROC) (GLenum target);

// ATI MultiTextureOp Extension
typedef void (APIENTRY * PFNGLBEGINMULTITEXOBJEXTPROC) (GLenum name);
typedef void (APIENTRY * PFNGLENDMULTITEXOBJEXTPROC) (void);
typedef void (APIENTRY * PFNGLBINDMULTITEXEXTPROC)(GLenum target, GLenum name);    
typedef void (APIENTRY * PFNGLMULTITEXOPFEXTPROC) (GLenum pname, GLenum param);    
typedef void (APIENTRY * PFNGLMULTITEXOPFVEXTPROC)(GLenum pname, GLfloat *params); 

typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);
// ARB multitexture extensions
typedef void (APIENTRY *  PFNGLCLIENTACTIVETEXTUREEXTPROC)(GLenum target); 
typedef void (APIENTRY *  PFNGLACTIVETEXTUREEXTPROC)(GLenum target); 
typedef void (APIENTRY *  PFNGLMULTITEXCOORD1DEXTPROC)(GLenum target, GLdouble x);    
typedef void (APIENTRY *  PFNGLMULTITEXCOORD1DVEXTPROC)(GLenum target, const GLdouble x[1]);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD1FEXTPROC)(GLenum target, GLfloat x);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD1FVEXTPROC)(GLenum target, const GLfloat x[1]);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD1IEXTPROC)(GLenum target, GLint x);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD1IVEXTPROC)(GLenum target, const GLint x[1]);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD1SEXTPROC)(GLenum target, GLshort x);    
typedef void (APIENTRY *  PFNGLMULTITEXCOORD1SVEXTPROC)(GLenum target, const GLshort x[1]);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD2DEXTPROC)(GLenum target, GLdouble x, GLdouble y);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD2DVEXTPROC)(GLenum target, const GLdouble x[2]);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD2FEXTPROC)(GLenum target, GLfloat x, GLfloat y);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD2FVEXTPROC)(GLenum target, const GLfloat x[2]);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD2IEXTPROC)(GLenum target, GLint x, GLint y);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD2IVEXTPROC)(GLenum target, const GLint x[2]);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD2SEXTPROC)(GLenum target, GLshort x, GLshort y);    
typedef void (APIENTRY *  PFNGLMULTITEXCOORD2SVEXTPROC)(GLenum target, const GLshort x[2]);    
typedef void (APIENTRY *  PFNGLMULTITEXCOORD3DEXTPROC)(GLenum target, GLdouble x, GLdouble y, GLdouble z);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD3DVEXTPROC)(GLenum target, const GLdouble x[3]);  
typedef void (APIENTRY *  PFNGLMULTITEXCOORD3FEXTPROC)(GLenum target, GLfloat x, GLfloat y, GLfloat z);     
typedef void (APIENTRY *  PFNGLMULTITEXCOORD3FVEXTPROC)(GLenum target, const GLfloat x[3]);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD3IEXTPROC)(GLenum target, GLint x, GLint y, GLint z);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD3IVEXTPROC)(GLenum target, const GLint x[3]);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD3SEXTPROC)(GLenum target, GLshort x, GLshort y, GLshort z);    
typedef void (APIENTRY *  PFNGLMULTITEXCOORD3SVEXTPROC)(GLenum target, const GLshort x[3]);    
typedef void (APIENTRY *  PFNGLMULTITEXCOORD4DEXTPROC)(GLenum target, GLdouble x, GLdouble y, GLdouble z, GLdouble w);  
typedef void (APIENTRY *  PFNGLMULTITEXCOORD4DVEXTPROC)(GLenum target, const GLdouble x[4]);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD4FEXTPROC)(GLenum target, GLfloat x, GLfloat y, GLfloat z, GLint w);    
typedef void (APIENTRY *  PFNGLMULTITEXCOORD4FVEXTPROC)(GLenum target,  const GLfloat x[4]);  
typedef void (APIENTRY *  PFNGLMULTITEXCOORD4IEXTPROC)(GLenum target, GLint x, GLint y, GLint z, GLint w);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD4IVEXTPROC)(GLenum target, const GLint x[4]);   
typedef void (APIENTRY *  PFNGLMULTITEXCOORD4SEXTPROC)(GLenum target, GLshort x, GLshort y, GLshort z, GLshort w);    
typedef void (APIENTRY *  PFNGLMULTITEXCOORD4SVEXTPROC)(GLenum target, const GLshort x[4]);   

// FUNCTION DEFS =========================================================

#ifdef __cplusplus
      }
#endif

#endif
