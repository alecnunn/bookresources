#ifndef __gl_ATI_h_
#define __gl_ATI_h_
//
// Copyright (C) 1998-2000 ATI Technologies Inc.
//

#ifdef __cplusplus
extern "C" {
#endif

#ifndef APIENTRY
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

/*
** Notes:
**
**  Listed support is for current drivers and should really only be used
**  as a guideline.  ISV should still use glGetString() and 
**  wglGetExtensionsString() to determine the exact set of supported
**  GL and WGL extensions.
**
*/

/*
**  ARB EXTENSIONS
*/

/*
** GL_ARB_multitexture
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef GL_ARB_multitexture
#define GL_ARB_multitexture                 1

#define GL_ACTIVE_TEXTURE_ARB               0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1
#define GL_MAX_ACTIVE_TEXTURES_ARB          0x84E2
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
#define GL_TEXTURE4_ARB                     0x84C4
#define GL_TEXTURE5_ARB                     0x84C5
#define GL_TEXTURE6_ARB                     0x84C6
#define GL_TEXTURE7_ARB                     0x84C7
#define GL_TEXTURE8_ARB                     0x84C8
#define GL_TEXTURE9_ARB                     0x84C9
#define GL_TEXTURE10_ARB                    0x84CA
#define GL_TEXTURE11_ARB                    0x84CB
#define GL_TEXTURE12_ARB                    0x84CC
#define GL_TEXTURE13_ARB                    0x84CD
#define GL_TEXTURE14_ARB                    0x84CE
#define GL_TEXTURE15_ARB                    0x84CF
#define GL_TEXTURE16_ARB                    0x84D0
#define GL_TEXTURE17_ARB                    0x84D1
#define GL_TEXTURE18_ARB                    0x84D2
#define GL_TEXTURE19_ARB                    0x84D3
#define GL_TEXTURE20_ARB                    0x84D4
#define GL_TEXTURE21_ARB                    0x84D5
#define GL_TEXTURE22_ARB                    0x84D6
#define GL_TEXTURE23_ARB                    0x84D7
#define GL_TEXTURE24_ARB                    0x84D8
#define GL_TEXTURE25_ARB                    0x84D9
#define GL_TEXTURE26_ARB                    0x84DA
#define GL_TEXTURE27_ARB                    0x84DB
#define GL_TEXTURE28_ARB                    0x84DC
#define GL_TEXTURE29_ARB                    0x84DD
#define GL_TEXTURE30_ARB                    0x84DE
#define GL_TEXTURE31_ARB                    0x84DF

typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DARBPROC) (GLenum texture, GLdouble s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVARBPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FARBPROC) (GLenum texture, GLfloat s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVARBPROC) (GLenum texture, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IARBPROC) (GLenum texture, GLint s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVARBPROC) (GLenum texture, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SARBPROC) (GLenum texture, GLshort s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVARBPROC) (GLenum texture, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DARBPROC) (GLenum texture, GLdouble s, GLdouble t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVARBPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum texture, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVARBPROC) (GLenum texture, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IARBPROC) (GLenum texture, GLint s, GLint t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVARBPROC) (GLenum texture, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SARBPROC) (GLenum texture, GLshort s, GLshort t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVARBPROC) (GLenum texture, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DARBPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVARBPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FARBPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVARBPROC) (GLenum texture, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IARBPROC) (GLenum texture, GLint s, GLint t, GLint r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVARBPROC) (GLenum texture, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SARBPROC) (GLenum texture, GLshort s, GLshort t, GLshort r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVARBPROC) (GLenum texture, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DARBPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVARBPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FARBPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVARBPROC) (GLenum texture, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IARBPROC) (GLenum texture, GLint s, GLint t, GLint r, GLint q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVARBPROC) (GLenum texture, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SARBPROC) (GLenum texture, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVARBPROC) (GLenum texture, const GLshort *v);

#endif /* GL_ARB_multitexture */

/*
** GL_ARB_texture_border_clamp
**
** Support:
**  Rage 128 * based : Supported
**  Radeon   * based : Supported
*/
#ifndef GL_ARB_texture_border_clamp
#define GL_ARB_texture_border_clamp 1

#define GL_CLAMP_TO_BORDER_ARB				0x812D

#endif

/*
** GL_ARB_texture_compression
**
** Support:
**  Rage 128 * based : Not Supported
**  Radeon   * based : Supported
*/
#ifndef GL_ARB_texture_compression
#define GL_ARB_texture_compression 1

#define GL_COMPRESSED_ALPHA_ARB					0x84E9
#define GL_COMPRESSED_LUMINANCE_ARB				0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB		0x84EB
#define GL_COMPRESSED_INTENSITY_ARB				0x84EC
#define GL_COMPRESSED_RGB_ARB					0x84ED
#define GL_COMPRESSED_RGBA_ARB					0x84EE
#define GL_TEXTURE_COMPRESSION_HINT_ARB			0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB	0x86A0
#define GL_TEXTURE_COMPRESSED_ARB				0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB	0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB		0x86A3

typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)(GLenum target, GLint level, 
													   GLenum internalFormat, GLsizei width,
													   GLsizei height, GLsizei depth,
													   GLint border, GLsizei imageSize,
													   const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)(GLenum target, GLint level,
													   GLenum internalFormat, GLsizei width,
													   GLsizei height, GLint border,
													   GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)(GLenum target, GLint level,
													   GLenum internalFormat, GLsizei width,
													   GLint border, GLsizei imageSize,
													   const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC)(GLenum target, GLint level,
														  GLint xoffset, GLint yoffset,
														  GLint zoffset, GLsizei width,
														  GLsizei height, GLsizei depth,
														  GLenum format, GLsizei imageSize,
														  const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)(GLenum target, GLint level,
														  GLint xoffset, GLint yoffset,
														  GLsizei width, GLsizei height,
														  GLenum format, GLsizei imageSize,
														  const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC)(GLenum target, GLint level,
														  GLint xoffset, GLsizei width,
														  GLenum format, GLsizei imageSize,
														  const GLvoid *data);
typedef void (APIENTRY * PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)(GLenum target, GLint lod, 
														GLvoid *img);
														

#endif /* GL_ARB_texture_compression */

/*
** GL_ARB_texture_cube_map
**
**  Support:
**   Rage 128 * based : Not Supported
**   Radeon   * based : Supported
*/
#ifndef GL_ARB_texture_cube_map
#define GL_ARB_texture_cube_map 1
 
#define GL_NORMAL_MAP_ARB					0x8511
#define GL_REFLECTION_MAP_ARB				0x8512
#define GL_TEXTURE_CUBE_MAP_ARB				0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_ARB		0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB	0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB	0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB	0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB  0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB  0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB  0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARB		0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB	0x851C
 
#endif /* GL_ARB_texture_cube_map */

/*
** GL_ARB_texture_env_add
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon	  * based : Supported
*/
#ifndef GL_ARB_texture_env_add
#define GL_ARB_texture_env_add				1

#endif /* GL_ARB_texture_env_add */

/*
** GL_ARB_transpose_matrix
**
**  Support:
**	 Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_ARB_transpose_matrix
#define GL_ARB_transpose_matrix 1

#define GL_TRANSPOSE_MODELVIEW_MATRIX_ARB	0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX_ARB  0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX_ARB		0x84E5

typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXFARBPROC)(const GLfloat m[16]);
typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXDARBPROC)(const GLdouble m[16]);
typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXFARBPROC)(const GLfloat m[16]);
typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXDARBPROC)(const GLdouble m[16]);

#endif /* GL_ARB_transpose_matrix */

/*
** GL_ARB_vertex_blend
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_ARB_vertex_blend 
#define GL_ARB_vertex_blend					1

#define GL_MAX_VERTEX_UNITS_ARB				0x86A4
#define GL_ACTIVE_VERTEX_UNITS_ARB			0x86A5
#define GL_WEIGHT_SUM_UNITY_ARB				0x86A6		
#define GL_VERTEX_BLEND_ARB					0x86A7
#define GL_CURRENT_WEIGHT_ARB				0x86A8
#define GL_WEIGHT_ARRAY_TYPE_ARB			0x86A9
#define GL_WEIGHT_ARRAY_STRIDE_ARB			0x86AA
#define GL_WEIGHT_ARRAY_SIZE_ARB			0x86AB
#define GL_WEIGHT_ARRAY_POINTER_ARB			0x86AC
#define GL_WEIGHT_ARRAY_ARB					0x86AD
#define GL_MODELVIEW0_ARB					0x1700
#define GL_MODELVIEW1_ARB					0x850a
#define GL_MODELVIEW2_ARB					0x8722
#define GL_MODELVIEW3_ARB					0x8723
#define GL_MODELVIEW4_ARB					0x8724
#define GL_MODELVIEW5_ARB					0x8725
#define GL_MODELVIEW6_ARB					0x8726
#define GL_MODELVIEW7_ARB					0x8727
#define GL_MODELVIEW8_ARB					0x8728
#define GL_MODELVIEW9_ARB					0x8729
#define GL_MODELVIEW10_ARB					0x872A
#define GL_MODELVIEW11_ARB					0x872B
#define GL_MODELVIEW12_ARB					0x872C
#define GL_MODELVIEW13_ARB					0x872D
#define GL_MODELVIEW14_ARB					0x872E
#define GL_MODELVIEW15_ARB					0x872F
#define GL_MODELVIEW16_ARB					0x8730
#define GL_MODELVIEW17_ARB					0x8731
#define GL_MODELVIEW18_ARB					0x8732
#define GL_MODELVIEW19_ARB					0x8733
#define GL_MODELVIEW20_ARB					0x8734
#define GL_MODELVIEW21_ARB					0x8735
#define GL_MODELVIEW22_ARB					0x8736
#define GL_MODELVIEW23_ARB					0x8737
#define GL_MODELVIEW24_ARB					0x8738
#define GL_MODELVIEW25_ARB					0x8739
#define GL_MODELVIEW26_ARB					0x873A
#define GL_MODELVIEW27_ARB					0x873B
#define GL_MODELVIEW28_ARB					0x873C
#define GL_MODELVIEW29_ARB					0x873D
#define GL_MODELVIEW30_ARB					0x873E
#define GL_MODELVIEW31_ARB					0x873F

typedef void (APIENTRY *PFNGLWEIGHTBVARBPROC)(GLint size, GLbyte *weights);
typedef void (APIENTRY *PFNGLWEIGHTSVARBPROC)(GLint size, GLshort *weights);
typedef void (APIENTRY *PFNGLWEIGHTIVARBPROC)(GLint size, GLint *weights);
typedef void (APIENTRY *PFNGLWEIGHTFVARBPROC)(GLint size, GLfloat *weights);
typedef void (APIENTRY *PFNGLWEIGHTDVARBPROC)(GLint size, GLdouble *weights);
typedef void (APIENTRY *PFNGLWEIGHTUBVARBPROC)(GLint size, GLubyte *weights);
typedef void (APIENTRY *PFNGLWEIGHTUSVARBPROC)(GLint size, GLushort *weights);
typedef void (APIENTRY *PFNGLWEIGHTUIVARBPROC)(GLint size, GLuint *weights);
typedef void (APIENTRY *PFNGLWEIGHTPOINTERARBPROC)(GLint size, GLenum type, GLsizei stride, GLvoid *pointer);
typedef void (APIENTRY *PFNGLVERTEXBLENDARBPROC)(GLint count);

#endif /* GL_ARB_vertex_blend */


/*
**  ATI EXTENSIONS
*/

/*
** GL_ATIX_texture_env_subtract
**
** Support:
**  Rage 128 * based  : Not Supported
**  Radeon   * based  : Supported
*/
#ifndef GL_ATIX_texture_env_subtract
#define GL_ATIX_texture_env_subtract 1

#define GL_SUBTRACT_ATIX				    0x6072

#endif

/*
** GL_ATI_texture_mirror_once
**
** Support:
**  Rage 128 * based : Supported
**  Radeon   * based : Supported
*/
#ifndef GL_ATI_texture_mirror_once
#define GL_ATI_texture_mirror_once 1

#define GL_MIRROR_CLAMP_ATI					0x6080
#define GL_MIRROR_CLAMP_TO_EDGE_ATI			0x6081

#endif

/*
** GL_ATI_vertex_streams
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_ATI_vertex_streams 
#define GL_ATI_vertex_streams					1

#define GL_MAX_VERTEX_STREAMS_ATI				0x6050
#define GL_VERTEX_SOURCE_ATI					0x6051
#define GL_VERTEX_STREAM0_ATI					0x6030
#define GL_VERTEX_STREAM1_ATI					0x6031
#define GL_VERTEX_STREAM2_ATI					0x6032
#define GL_VERTEX_STREAM3_ATI					0x6033
#define GL_VERTEX_STREAM4_ATI					0x6034
#define GL_VERTEX_STREAM5_ATI					0x6035
#define GL_VERTEX_STREAM6_ATI					0x6036
#define GL_VERTEX_STREAM7_ATI					0x6037
#define GL_VERTEX_STREAM8_ATI					0x6038
#define GL_VERTEX_STREAM9_ATI					0x6039
#define GL_VERTEX_STREAM10_ATI					0x603A
#define GL_VERTEX_STREAM11_ATI					0x603B
#define GL_VERTEX_STREAM12_ATI					0x603C
#define GL_VERTEX_STREAM13_ATI					0x603D
#define GL_VERTEX_STREAM14_ATI					0x603E
#define GL_VERTEX_STREAM15_ATI					0x603F
#define GL_VERTEX_STREAM16_ATI					0x6040
#define GL_VERTEX_STREAM17_ATI					0x6041
#define GL_VERTEX_STREAM18_ATI					0x6042
#define GL_VERTEX_STREAM19_ATI					0x6043
#define GL_VERTEX_STREAM20_ATI					0x6044
#define GL_VERTEX_STREAM21_ATI					0x6045
#define GL_VERTEX_STREAM22_ATI					0x6046
#define GL_VERTEX_STREAM23_ATI					0x6047
#define GL_VERTEX_STREAM24_ATI					0x6048
#define GL_VERTEX_STREAM25_ATI					0x6049
#define GL_VERTEX_STREAM26_ATI					0x604A
#define GL_VERTEX_STREAM27_ATI					0x604B
#define GL_VERTEX_STREAM28_ATI					0x604C
#define GL_VERTEX_STREAM29_ATI					0x604D
#define GL_VERTEX_STREAM30_ATI					0x604E
#define GL_VERTEX_STREAM31_ATI					0x604F

typedef void (APIENTRY * PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC) (GLenum stream);
typedef void (APIENTRY * PFNGLVERTEXBLENDENVIATIPROC) (GLenum pname, GLint param);
typedef void (APIENTRY * PFNGLVERTEXBLENDENVFATIPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2SATIPROC) (GLenum stream, GLshort x, GLshort y);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2SVATIPROC) (GLenum stream, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2IATIPROC) (GLenum stream, GLint x, GLint y);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2IVATIPROC) (GLenum stream, const GLint *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2FATIPROC) (GLenum stream, GLfloat x, GLfloat y);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2FVATIPROC) (GLenum stream, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2DATIPROC) (GLenum stream, GLdouble x, GLdouble y);
typedef void (APIENTRY * PFNGLVERTEXSTREAM2DVATIPROC) (GLenum stream, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3SVATIPROC) (GLenum stream, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3IATIPROC) (GLenum stream, GLint x, GLint y, GLint z);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3IVATIPROC) (GLenum stream, const GLint *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3FVATIPROC) (GLenum stream, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRY * PFNGLVERTEXSTREAM3DVATIPROC) (GLenum stream, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4SVATIPROC) (GLenum stream, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4IATIPROC) (GLenum stream, GLint x, GLint y, GLint z, GLint w);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4IVATIPROC) (GLenum stream, const GLint *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4FVATIPROC) (GLenum stream, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRY * PFNGLVERTEXSTREAM4DVATIPROC) (GLenum stream, const GLdouble *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3BATIPROC) (GLenum stream, GLbyte x, GLbyte y, GLbyte z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3BVATIPROC) (GLenum stream, const GLbyte *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3SVATIPROC) (GLenum stream, const GLshort *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3IATIPROC) (GLenum stream, GLint x, GLint y, GLint z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3IVATIPROC) (GLenum stream, const GLint *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3FVATIPROC) (GLenum stream, const GLfloat *v);
typedef void (APIENTRY * PFNGLNORMALSTREAM3DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRY * PFNGLNORMALSTREAM3DVATIPROC) (GLenum stream, const GLdouble *v);
#endif /* GL_ATI_vertex_streams */

/*
**  EXT EXTENSIONS
*/

/*
** GL_EXT_abgr 
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_abgr 
#define GL_EXT_abgr                         1

#define GL_ABGR_EXT                         0x8000

#endif /* GL_EXT_abgr */ 


/*
** GL_EXT_bgra
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_bgra
#define GL_EXT_bgra                         1

#define GL_BGR_EXT                          0x80E0
#define GL_BGRA_EXT                         0x80E1

#endif /* GL_EXT_bgra */

/*
** GL_EXT_clip_volume_hint
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_clip_volume_hint
#define GL_EXT_clip_volume_hint             1

#define GL_CLIP_VOLUME_CLIPPING_HINT_EXT    0x80F0

#endif /*GL_EXT_clip_volume_hint */

/*
** GL_EXT_compiled_vertex_array
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_compiled_vertex_array
#define GL_EXT_compiled_vertex_array        1

#define GL_ARRAY_ELEMENT_LOCK_FIRST_EXT     0x81A8
#define GL_ARRAY_ELEMENT_LOCK_COUNT_EXT     0x81A9

typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);

#endif /* GL_EXT_compiled_vertex_array */

/*
** GL_EXT_draw_range_elements
**
**  Support:
**	 Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_draw_range_elements
#define GL_EXT_draw_range_elements			1

#define GL_MAX_ELEMENTS_VERTICES_EXT		0x80E8
#define GL_MAX_ELEMENTS_INDICES_EXT			0x80E9

typedef void (APIENTRY * PFNGLDRAWRANGEELEMENTSEXTPROC) (GLenum  mode, GLuint start, 
														 GLuint end, GLsizei count, 
														 GLenum type, const GLvoid *indices);

#endif /* GL_EXT_draw_range_elemetns */

/*
** GL_EXT_fog_coord
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_fog_coord
#define GL_EXT_fog_coord                    1

#define GL_FOG_COORDINATE_SOURCE_EXT        0x8450
#define GL_FOG_COORDINATE_EXT               0x8451
#define GL_FRAGMENT_DEPTH_EXT               0x8452
#define GL_CURRENT_FOG_COORDINATE_EXT       0x8453
#define GL_FOG_COORDINATE_ARRAY_TYPE_EXT    0x8454
#define GL_FOG_COORDINATE_ARRAY_STRIDE_EXT  0x8455
#define GL_FOG_COORDINATE_ARRAY_POINTER_EXT 0x8456
#define GL_FOG_COORDINATE_ARRAY_EXT         0x8457

typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat f);
typedef void (APIENTRY * PFNGLFOGCOORDDEXTPROC) (GLdouble f);
typedef void (APIENTRY * PFNGLFOGCOORDFVEXTPROC) (const GLfloat *v);
typedef void (APIENTRY * PFNGLFOGCOORDDVEXTPROC) (const GLdouble *v);
typedef void (APIENTRY * PFNGLFOGCOORDPOINTEREXTPROC) (GLenum type, GLsizei stride, void *pointer);

#endif /* GL_EXT_fog_coord */

/*
** GL_EXT_packed_pixels
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_packed_pixels
#define GL_EXT_packed_pixels                1

#define GL_UNSIGNED_BYTE_3_3_2_EXT          0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4_EXT       0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1_EXT       0x8034
#define GL_UNSIGNED_INT_8_8_8_8_EXT         0x8035
#define GL_UNSIGNED_INT_10_10_10_2_EXT      0x8036

#endif /* GL_EXT_packed_pixels */

/*
** GL_EXT_rescale_normal
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef GL_EXT_rescale_normal
#define GL_EXT_rescale_normal				1

#define GL_RESCALE_NORMAL_EXT				0x803A

#endif /* GL_EXT_rescale_normal */

/*
** GL_EXT_separate_specular_color
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_separate_specular_color
#define GL_EXT_separate_specular_color      1

#define GL_LIGHT_MODEL_COLOR_CONTROL_EXT    0x81F8
#define GL_SINGLE_COLOR_EXT                 0x81F9
#define GL_SEPARATE_SPECULAR_COLOR_EXT      0x81FA

#endif /* GL_EXT_separate_specular_color */

/*
** GL_EXT_secondary_color
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_secondary_color
#define GL_EXT_secondary_color				1

#define GL_COLOR_SUM_EXT					0x8458
#define GL_CURRENT_SECONDARY_COLOR_EXT		0x8459
#define GL_SECONDARY_COLOR_ARRAY_SIZE_EXT	0x845A
#define GL_SECONDARY_COLOR_ARRAY_TYPE_EXT	0x845B
#define GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT	0x845C
#define GL_SECONDARY_COLOR_ARRAY_POINTER_EXT 0x845D
#define GL_SECONDARY_COLOR_ARRAY_EXT		0x845E

typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FEXT) (GLfloat r, GLfloat g, GLfloat b);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DEXT) (GLdouble r, GLdouble g, GLdouble b);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BEXT) (GLbyte r, GLbyte g, GLbyte b);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SEXT) (GLshort r, GLshort g, GLshort b);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IEXT) (GLint r, GLint g, GLint b);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBEXT)(GLubyte r, GLubyte g, GLubyte b);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USEXT)(GLushort r, GLushort g, GLushort b);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIEXT)(GLuint r, GLuint g, GLuint b);

typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FVEXT) (const GLfloat *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DVEXT) (const GLdouble *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BVEXT) (const GLbyte *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SVEXT) (const GLshort *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IVEXT) (const GLint *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBVEXT)(const GLubyte *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USVEXT)(const GLushort *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIVEXT)(const GLuint *v);

typedef void (APIENTRY * PFNGLSECONDARYCOLORPOINTEREXT)(GLint size, GLenum type, GLsizei stride, void *pointer);

#endif /* GL_EXT_secondary_color */

/*
** GL_EXT_texgen_reflection
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_texgen_reflection
#define GL_EXT_texgen_reflection            1

#define GL_NORMAL_MAP_EXT                   0x8511
#define GL_REFLECTION_MAP_EXT               0x8512

#endif /* GL_EXT_texgen_reflection */


/*
** GL_EXT_texture_cube_map
**
**  Support:
**   Rage 128 * based : Not Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_texture_cube_map
#define GL_EXT_texture_cube_map            1

#define GL_TEXTURE_CUBE_MAP_EXT				0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_EXT		0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT	0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT	0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT	0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT  0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT  0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT  0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_EXT		0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT	0x851C

#endif /* GL_EXT_texture_cube_map */

/*
** GL_EXT_texture3D
**
**	Support:
**		Rage 128 * based : Supported (software)
**	    Radeon   * based : Supported
*/
#ifndef GL_EXT_texture3D
#define GL_EXT_texture3D					1

#define GL_TEXTURE_BINDING_3D_EXT		    0x806A
#define GL_PACK_SKIP_IMAGES_EXT             0x806B
#define GL_PACK_IMAGE_HEIGHT_EXT            0x806C
#define GL_UNPACK_SKIP_IMAGES_EXT           0x806D
#define GL_UNPACK_IMAGE_HEIGHT_EXT          0x806E
#define GL_TEXTURE_3D_EXT                   0x806F
#define GL_PROXY_TEXTURE_3D_EXT             0x8070
#define GL_TEXTURE_DEPTH_EXT                0x8071
#define GL_TEXTURE_WRAP_R_EXT               0x8072
#define GL_MAX_3D_TEXTURE_SIZE_EXT          0x8073


typedef void (APIENTRY * PFNGLTEXIMAGE3DEXT)(GLenum target, GLint level, GLenum internalformat,
											 GLsizei width, GLsizei height, GLsizei depth,
											 GLint border, GLenum format, GLenum type,
											 const GLvoid* pixels);

#endif /* GL_EXT_texture3D */

/*
** GL_EXT_texture_compression_s3tc
**
** Support:
**	Rage 128 * based : Not Supported
**  Radeon   * based : Supported
*/
#ifndef GL_EXT_texture_compression_s3tc
#define GL_EXT_texture_compression_s3tc 1

#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT      0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT     0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT     0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT     0x83F3

#endif

/*
** GL_EXT_texture_edge_clamp
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_texture_edge_clamp
#define GL_EXT_texture_edge_clamp           1

#define GL_CLAMP_TO_EDGE_EXT                0x812F

#endif /* GL_EXT_texture_edge_clamp */

/*
** GL_EXT_texture_env_add
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_texture_env_add
#define GL_EXT_texture_env_add				1

#endif /* GL_EXT_texture_env_add */

/*
** GL_EXT_texture_env_combine
**
**  Support:
**   Rage 128 * based : Supported
**	 Radeon   * based : Supported
*/
#ifndef GL_EXT_texture_env_combine
#define GL_EXT_texture_env_combine			1

#define GL_COMBINE_EXT						0x8570
#define GL_COMBINE_RGB_EXT					0x8571
#define GL_COMBINE_ALPHA_EXT				0x8572
#define GL_SOURCE0_RGB_EXT					0x8580
#define GL_SOURCE1_RGB_EXT					0x8581
#define GL_SOURCE2_RGB_EXT					0x8582
#define GL_SOURCE0_ALPHA_EXT				0x8588
#define GL_SOURCE1_ALPHA_EXT				0x8589
#define GL_SOURCE2_ALPHA_EXT				0x858A
#define GL_OPERAND0_RGB_EXT					0x8590
#define GL_OPERAND1_RGB_EXT					0x8591
#define GL_OPERAND2_RGB_EXT					0x8592
#define GL_OPERAND0_ALPHA_EXT				0x8598
#define GL_OPERAND1_ALPHA_EXT				0x8599
#define GL_OPERAND2_ALPHA_EXT				0x859A
#define GL_RGB_SCALE_EXT					0x8573
#define GL_ADD_SIGNED_EXT					0x8574
#define GL_INTERPOLATE_EXT					0x8575
#define GL_CONSTANT_EXT						0x8576
#define GL_PRIMARY_COLOR_EXT				0x8577
#define GL_PREVIOUS_EXT						0x8578

#endif /* GL_EXT_texture_env_combine */

/*
** GL_EXT_texture_env_dot3
**
** Support:
**	Rage 128 * based  : Not Supported
**  Radeon   * based  : Supported
*/
#ifndef GL_EXT_texture_env_dot3
#define GL_EXT_texture_env_dot3 1

#define GL_DOT3_RGB_EXT						0x8740
#define GL_DOT3_RGBA_EXT					0x8741

#endif

/*
** GL_EXT_texture_filter_anisotropic
**
** Support:
**
**		Rage128	   * based : Not supported
**		Radeon     * based : Supported
*/
#ifndef GL_EXT_texture_filter_anisotropic
#define GL_EXT_texture_filter_anisotropic

#define GL_TEXTURE_MAX_ANISOTROPY_EXT		0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT	0x84FF

#endif /* GL_EXT_texture_filter_anisotropic */

/*
** GL_EXT_texture_object
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef GL_EXT_texture_object
#define GL_EXT_texture_object				1

#define GL_TEXTURE_PRIORITY_EXT				0x8066
#define GL_TEXTURE_RESIDENT_EXT				0x8067
#define GL_TEXTURE_1D_BINDING_EXT			0x8068
#define GL_TEXTURE_2D_BINDING_EXT			0x8069
#define GL_TEXTURE_3D_BINDING_EXT			0x806A

typedef void (APIENTRY * PFNGLGENTEXTURESEXTPROC) ( GLsizei n, GLuint *textures);
typedef void (APIENTRY * PFNGLDELETETEXTURESEXTPROC) ( GLsizei n, const GLuint *textures);
typedef void (APIENTRY * PFNGLBINDTEXTUREEXTPROC) ( GLenum target, GLuint texture);
typedef void (APIENTRY * PFNGLPRIORITIZETEXTURESEXTPROC) ( GLsizei n, const GLuint *textures, const GLclampf *priorities);
typedef GLboolean (APIENTRY * PFNGLARETEXTURESRESIDENTEXTPROC) ( GLsizei n, const GLuint *textures, GLboolean *residences);
typedef GLboolean (APIENTRY * PFNGLISTEXTUREEXTPROC) ( GLuint texture);

#endif /* GL_EXT_texture_object */

/*
** GL_EXT_vertex_array
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef GL_EXT_vertex_array
#define GL_EXT_vertex_array                 1

#define GL_VERTEX_ARRAY_EXT                 0x8074
#define GL_NORMAL_ARRAY_EXT                 0x8075
#define GL_COLOR_ARRAY_EXT                  0x8076
#define GL_INDEX_ARRAY_EXT                  0x8077
#define GL_TEXTURE_COORD_ARRAY_EXT          0x8078
#define GL_EDGE_FLAG_ARRAY_EXT              0x8079
#define GL_VERTEX_ARRAY_SIZE_EXT            0x807A
#define GL_VERTEX_ARRAY_TYPE_EXT            0x807B
#define GL_VERTEX_ARRAY_STRIDE_EXT          0x807C
#define GL_VERTEX_ARRAY_COUNT_EXT           0x807D
#define GL_NORMAL_ARRAY_TYPE_EXT            0x807E
#define GL_NORMAL_ARRAY_STRIDE_EXT          0x807F
#define GL_NORMAL_ARRAY_COUNT_EXT           0x8080
#define GL_COLOR_ARRAY_SIZE_EXT             0x8081
#define GL_COLOR_ARRAY_TYPE_EXT             0x8082
#define GL_COLOR_ARRAY_STRIDE_EXT           0x8083
#define GL_COLOR_ARRAY_COUNT_EXT            0x8084
#define GL_INDEX_ARRAY_TYPE_EXT             0x8085
#define GL_INDEX_ARRAY_STRIDE_EXT           0x8086
#define GL_INDEX_ARRAY_COUNT_EXT            0x8087
#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT     0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT     0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT   0x808A
#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT    0x808B
#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT       0x808C
#define GL_EDGE_FLAG_ARRAY_COUNT_EXT        0x808D
#define GL_VERTEX_ARRAY_POINTER_EXT         0x808E
#define GL_NORMAL_ARRAY_POINTER_EXT         0x808F
#define GL_COLOR_ARRAY_POINTER_EXT          0x8090
#define GL_INDEX_ARRAY_POINTER_EXT          0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT  0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER_EXT      0x8093

typedef void (APIENTRY * PFNGLARRAYELEMENTEXTPROC) (GLint i);
typedef void (APIENTRY * PFNGLCOLORPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLDRAWARRAYSEXTPROC) (GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLEDGEFLAGPOINTEREXTPROC) (GLsizei stride, GLsizei count, const GLboolean *pointer);
typedef void (APIENTRY * PFNGLGETPOINTERVEXTPROC) (GLenum pname, GLvoid* *params);
typedef void (APIENTRY * PFNGLINDEXPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLNORMALPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLTEXCOORDPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLVERTEXPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);

#endif /* GL_EXT_vertex_array */

/*
**  IHV EXTENSIONS
*/

/*
** GL_NV_texgen_reflection
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef GL_NV_texgen_reflection
#define GL_NV_texgen_reflection            1

#define GL_NORMAL_MAP_NV                   0x8511
#define GL_REFLECTION_MAP_NV               0x8512

#endif /* GL_NV_texgen_reflection */

/*
** GL_S3_s3tc
**
**  Support:
**   Rage 128 * based : Unsupported
**   Radeon   * based : Supported
*/
#ifndef GL_S3_s3tc
#define GL_S3_s3tc                          1

#define GL_RGB_S3TC                         0x83A0
#define GL_RGB4_S3TC                        0x83A1
#define GL_RGBA_S3TC                        0x83A2
#define GL_RGBA4_S3TC                       0x83A3

#endif /* GL_S3_s3tc */

/*
** GL_SGI_texture_edge_clamp
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef GL_SGI_texture_edge_clamp
#define GL_SGI_texture_edge_clamp           1

#define GL_CLAMP_TO_EDGE_SGI                0x812F

#endif /* GL_SGI_texture_edge_clamp */

/*
** GL_SGIS_multitexture
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef GL_SGIS_multitexture
#define GL_SGIS_multitexture                1

#define TEXTURE0_SGIS                       0x835E
#define TEXTURE1_SGIS                       0x835F
#define GL_SELECTED_TEXTURE_SGIS            0x83C0
#define GL_SELECTED_TEXTURE_COORD_SET_SGIS  0x83C1
#define GL_SELECTED_TEXTURE_TRANSFORM_SGIS  0x83C2
#define GL_MAX_TEXTURES_SGIS                0x83C3
#define GL_MAX_TEXTURE_COORD_SETS_SGIS      0x83C4
#define GL_TEXTURE_ENV_COORD_SET_SGIS       0x83C5
#define GL_TEXTURE0_SGIS                    0x83C6
#define GL_TEXTURE1_SGIS                    0x83C7
#define GL_TEXTURE2_SGIS                    0x83C8
#define GL_TEXTURE3_SGIS                    0x83C9
#define GL_TEXTURE4_SGIS                    0x83CA
#define GL_TEXTURE5_SGIS                    0x83CB
#define GL_TEXTURE6_SGIS                    0x83CC
#define GL_TEXTURE7_SGIS                    0x83CD
#define GL_TEXTURE8_SGIS                    0x83CE
#define GL_TEXTURE9_SGIS                    0x83CF
#define GL_TEXTURE10_SGIS                   0x83D0
#define GL_TEXTURE11_SGIS                   0x83D1
#define GL_TEXTURE12_SGIS                   0x83D2
#define GL_TEXTURE13_SGIS                   0x83D3
#define GL_TEXTURE14_SGIS                   0x83D4
#define GL_TEXTURE15_SGIS                   0x83D5
#define GL_TEXTURE16_SGIS                   0x83D6
#define GL_TEXTURE17_SGIS                   0x83D7
#define GL_TEXTURE18_SGIS                   0x83D8
#define GL_TEXTURE19_SGIS                   0x83D9
#define GL_TEXTURE20_SGIS                   0x83DA
#define GL_TEXTURE21_SGIS                   0x83DB
#define GL_TEXTURE22_SGIS                   0x83DC
#define GL_TEXTURE23_SGIS                   0x83DD
#define GL_TEXTURE24_SGIS                   0x83DE
#define GL_TEXTURE25_SGIS                   0x83DF
#define GL_TEXTURE26_SGIS                   0x83E0
#define GL_TEXTURE27_SGIS                   0x83E1
#define GL_TEXTURE28_SGIS                   0x83E2
#define GL_TEXTURE29_SGIS                   0x83E3
#define GL_TEXTURE30_SGIS                   0x83E4
#define GL_TEXTURE31_SGIS                   0x83E5

typedef void (APIENTRY * PFNGLSELECTTEXTURESGISPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLSELECTTEXTURETRANSFORMSGISPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLSELECTTEXTURECOORDSETSGISPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DSGISPROC) (GLenum texture, GLdouble s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVSGISPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FSGISPROC) (GLenum texture, GLfloat s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVSGISPROC) (GLenum texture, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1ISGISPROC) (GLenum texture, GLint s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVSGISPROC) (GLenum texture, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SSGISPROC) (GLenum texture, GLshort s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVSGISPROC) (GLenum texture, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DSGISPROC) (GLenum texture, GLdouble s, GLdouble t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVSGISPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FSGISPROC) (GLenum texture, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVSGISPROC) (GLenum texture, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2ISGISPROC) (GLenum texture, GLint s, GLint t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVSGISPROC) (GLenum texture, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SSGISPROC) (GLenum texture, GLshort s, GLshort t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVSGISPROC) (GLenum texture, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DSGISPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVSGISPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FSGISPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVSGISPROC) (GLenum texture, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3ISGISPROC) (GLenum texture, GLint s, GLint t, GLint r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVSGISPROC) (GLenum texture, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SSGISPROC) (GLenum texture, GLshort s, GLshort t, GLshort r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVSGISPROC) (GLenum texture, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DSGISPROC) (GLenum texture, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVSGISPROC) (GLenum texture, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FSGISPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVSGISPROC) (GLenum texture, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4ISGISPROC) (GLenum texture, GLint s, GLint t, GLint r, GLint q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVSGISPROC) (GLenum texture, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SSGISPROC) (GLenum texture, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVSGISPROC) (GLenum texture, const GLshort *v);

#endif /* GL_SGIS_multitexture */

/*
** GL_SGIS_texture_border_clamp
**
** Support:
**  Rage 128 * based : Supported
**  Radeon   * based : Supported
*/
#ifndef GL_SGIS_texture_border_clamp
#define GL_SGIS_texture_border_clamp 1

#define GL_CLAMP_TO_BORDER_SGIS				0x812D

#endif


/*
** GL_SGIS_texture_lod
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef GL_SGIS_texture_lod
#define GL_SGIS_texture_lod                1

#define GL_TEXTURE_MIN_LOD_SGIS				0x813A
#define GL_TEXTURE_MAX_LOD_SGIS				0x813B
#define GL_TEXTURE_BASE_LEVEL_SGIS			0x813C
#define GL_TEXTURE_MAX_LEVEL_SGIS			0x813D

#endif /* GL_SGIS_texture_lod */


/*
**  ISV EXTENSIONS
*/

/*
** GL_KTX_buffer_region
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef GL_KTX_buffer_region
#define GL_KTX_buffer_region				1

#define GL_KTX_FRONT_REGION					0x0
#define GL_KTX_BACK_REGION					0x1
#define GL_KTX_Z_REGION						0x2
#define GL_KTX_STENCIL_REGION				0x3

typedef GLuint (APIENTRY * PFNGLBUFFERREGIONENABLEDEXTPROC) ( void );
typedef GLuint (APIENTRY * PFNGLNEWBUFFERREGIONEXTPROC) ( GLenum region );
typedef void (APIENTRY * PFNGLDELETEBUFFERREGIONEXTPROC) ( GLenum region );
typedef void (APIENTRY * PFNGLREADBUFFERREGIONEXTPROC) ( GLuint region, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRY * PFNGLDRAWBUFFERREGIONEXTPROC) ( GLuint region, GLint x, GLint y, GLsizei width, GLsizei height, GLint xDest, GLint yDest);

#endif /* GL_KTX_buffer_region */

/*
** GL_WIN_swap_hint
**
**  Support:
**   Rage 128 * based : Supported
**   Radeon   * based : Supported
*/
#ifndef GL_WIN_swap_hint
#define GL_WIN_swap_hint                    1

typedef void (APIENTRY * PFNGLADDSWAPHINTRECTWINPROC) (GLint x, GLint y, GLsizei width, GLsizei height);

#endif /* GL_WIN_swap_hint */

#ifdef __cplusplus
}
#endif

#endif /* __gl_ATI_h_ */
