#ifndef BISON_EXPORTER_Y_H
# define BISON_EXPORTER_Y_H

#ifndef YYSTYPE
typedef union
{
   int      integer;     // for INTEGER token
   char     string[80];  // for STRING token
   float    fp;          // for FLOAT token
} yystype;
# define YYSTYPE yystype
#endif
# define	TKN_MD3_FILEHDR	257
# define	TKN_MD3_BONEFRAMES	258
# define	TKN_MD3_TAGS	259
# define	TKN_MD3_MESH	260
# define	TKN_MD3_TEXTURENAMES	261
# define	TKN_MD3_TRIANGLEVERTS	262
# define	TKN_MD3_TEXVEC	263
# define	TKN_MD3_VERTEX	264
# define	TKN_END	265
# define	TKN_ID	266
# define	TKN_VERSION	267
# define	TKN_FILENAME	268
# define	TKN_NUM_BONEFRAME	269
# define	TKN_NUM_TAGS	270
# define	TKN_NUM_MESH	271
# define	TKN_MAXTEX_NUM	272
# define	TKN_HEADER_SIZE	273
# define	TKN_TAGS_START	274
# define	TKN_MESH_START	275
# define	TKN_FILE_SIZE	276
# define	TKN_MINS	277
# define	TKN_MAXS	278
# define	TKN_POSITION	279
# define	TKN_SCALE	280
# define	TKN_CREATOR	281
# define	TKN_TAG_NAME	282
# define	TKN_ROTATION_MAT	283
# define	TKN_MESH_NAME	284
# define	TKN_NUM_MESHFRAMES	285
# define	TKN_NUM_SKINS	286
# define	TKN_NUM_VERTS	287
# define	TKN_NUM_TRIS	288
# define	TKN_TRI_START_ADDR	289
# define	TKN_TEXVEC_START_ADDR	290
# define	TKN_VERTEX_START_ADDR	291
# define	TKN_MESH_SIZE	292
# define	TKN_TEX_NAME	293
# define	TKN_VERT_IDX	294
# define	TKN_UV_OR_VEC	295
# define	TKN_VERT	296
# define	TKN_STRING	297
# define	TKN_INTEGER	298
# define	TKN_FLOAT	299


extern YYSTYPE yylval;

#endif /* not BISON_EXPORTER_Y_H */
