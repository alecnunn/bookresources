/*
 *  FILE       : exporter.y
 *
 *  PROGRAMMER : Paul Kelly 
 *               paul_kelly2000@yahoo.com
 *
 *  DESCRIPTION: The yacc specification for parsing the .md3 data file.
 *               
 *
 *  NOTES      : bison was used to test this example.  It can
 *               be found at www.gnu.org in the gnu tools.
 *
 *               Use the following to generate the parser:
 *
 *               bison -d -o weapon_y.c weapon.y
 *
 *               -d generates a file with an enumeration of the tokens that
 *                  can be used by the lexer and it also defines the types
 *                  that can be returned by the lexer.
 *               -o filename that the parser generated source code
 *                  will be placed.
 *
 *               Also, make sure the necessary lib and .h files are included 
 *               so that compiling and linking work.
 *
 */
%{
/*
 *
 *  INCLUDES
 *
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "md3.h"

/*
 *
 *  MACROS
 *
 */

#ifdef _DEBUG  // do only in debug builds
#define YYDEBUG  1 // outputs debug info during parse
#endif


/*
 *
 *  GLOBAL VARIABLES
 *
 */

extern char *yytext;

extern FILE            *outFile;
extern MD3HEADER       hdr;
extern MD3TAG          *tag_data;
extern MD3BONEFRAME    *boneframe_data;
extern MD3MESHFILE     meshhdr_data;
extern char            *textureName_data;
extern TRIANGLEVERT    *triangle_data;
extern VEC2            *texvec_data;
extern MD3VERTEX       *vertex_data;

int boneframeIdx = 0;
int tagIdx = 0;
int texnamesIdx = 0;
int triIdx = 0;
int texvecIdx = 0;
int vertIdx = 0;

char *nextTextureName;

%}

%union
{
   int      integer;     // for INTEGER token
   char     string[80];  // for STRING token
   float    fp;          // for FLOAT token
}

%token TKN_MD3_FILEHDR TKN_MD3_BONEFRAMES TKN_MD3_TAGS TKN_MD3_MESH
%token TKN_MD3_TEXTURENAMES TKN_MD3_TRIANGLEVERTS TKN_MD3_TEXVEC
%token TKN_MD3_VERTEX TKN_END

%token TKN_ID TKN_VERSION TKN_FILENAME TKN_NUM_BONEFRAME TKN_NUM_TAGS
%token TKN_NUM_MESH TKN_MAXTEX_NUM TKN_HEADER_SIZE TKN_TAGS_START
%token TKN_MESH_START TKN_FILE_SIZE 

%token TKN_MINS TKN_MAXS TKN_POSITION TKN_SCALE TKN_CREATOR    

%token TKN_TAG_NAME TKN_POSITION TKN_ROTATION_MAT    

%token TKN_MESH_NAME TKN_NUM_MESHFRAMES TKN_NUM_SKINS TKN_NUM_VERTS
%token TKN_NUM_TRIS TKN_TRI_START_ADDR TKN_HEADER_SIZE TKN_TEXVEC_START_ADDR
%token TKN_VERTEX_START_ADDR TKN_MESH_SIZE 

%token TKN_TEX_NAME   

%token TKN_VERT_IDX         

%token TKN_UV_OR_VEC 

%token TKN_VERT


%token <string> TKN_STRING
%token <integer> TKN_INTEGER
%token <fp> TKN_FLOAT


%%
md3_file:                  md3_filehdr
                           md3_boneframes 
                           md3_tags 
                           md3_meshes
                        ;

md3_meshes:                /* lamda rule - empty rule */
                        |  md3_meshes md3_mesh 
                           md3_texturenames 
                           md3_triverts
                           md3_texvecs
                           md3_vertices
                        ;
md3_filehdr:               TKN_MD3_FILEHDR
                           TKN_ID TKN_STRING
                              { strncpy (hdr.id, $3, 4);}
                           TKN_VERSION TKN_INTEGER
                              { hdr.iVersion = $6; }
                           TKN_FILENAME TKN_STRING
                              { strcpy (hdr.cFileName, $9);}
                           TKN_NUM_BONEFRAME TKN_INTEGER
                              { hdr.iBoneFrameNum = $12; }
                           TKN_NUM_TAGS TKN_INTEGER
                              { hdr.iTagNum = $15; }
                           TKN_NUM_MESH TKN_INTEGER
                              { hdr.iMeshNum = $18; }
                           TKN_MAXTEX_NUM TKN_INTEGER
                              { hdr.iMaxTextureNum = $21; }
                           TKN_HEADER_SIZE TKN_INTEGER
                              { hdr.iHeaderSize = $24; }
                           TKN_TAGS_START TKN_INTEGER
                              { hdr.iTagStart = $27; }
                           TKN_MESH_START TKN_INTEGER
                              { hdr.iMeshStart = $30; }
                           TKN_FILE_SIZE TKN_INTEGER
                              { hdr.iFileSize = $33; }
                           TKN_END
                              { md3BinOutHeader (outFile, &hdr);
                              }
                        ;

md3_boneframes:            TKN_MD3_BONEFRAMES
                              { int size = sizeof (MD3BONEFRAME) *
                                    hdr.iBoneFrameNum;
                                boneframe_data = (MD3BONEFRAME *)
                                    malloc (sizeof (MD3BONEFRAME) *
                                    hdr.iBoneFrameNum); 
                              }
                           md3_boneframe
                           TKN_END
                              { md3BinOutBoneFrames (outFile, boneframe_data, hdr.iBoneFrameNum, &hdr);
                              }
                        ;

md3_boneframe:             /* lamda rule - empty rule */
                        |  md3_boneframe TKN_MINS TKN_FLOAT TKN_FLOAT TKN_FLOAT
                              { boneframe_data[ boneframeIdx ].fMins[0] = $3;
                                boneframe_data[ boneframeIdx ].fMins[1] = $4;
                                boneframe_data[ boneframeIdx ].fMins[2] = $5;
                              }
                           TKN_MAXS TKN_FLOAT TKN_FLOAT TKN_FLOAT
                              { boneframe_data[ boneframeIdx ].fMaxs[0] = $8;
                                boneframe_data[ boneframeIdx ].fMaxs[1] = $9;
                                boneframe_data[ boneframeIdx ].fMaxs[2] = $10;
                              }
                           TKN_POSITION TKN_FLOAT TKN_FLOAT TKN_FLOAT
                              { boneframe_data[ boneframeIdx ].pos[0] = $13;
                                boneframe_data[ boneframeIdx ].pos[1] = $14;
                                boneframe_data[ boneframeIdx ].pos[2] = $15;
                              }
                           TKN_SCALE TKN_FLOAT
                              { boneframe_data[ boneframeIdx ].scale = $18;
                              }
                           TKN_CREATOR TKN_STRING
                              { strncpy (boneframe_data[ boneframeIdx ].creator, $21, 15);
                                boneframe_data[ boneframeIdx ].creator[15] = '\0';
                                boneframeIdx++;
                              }
                        ;
                                                   
md3_tags:                  TKN_MD3_TAGS
                              { int size = sizeof (MD3TAG) * hdr.iTagNum * hdr.iBoneFrameNum;
                                tag_data = (MD3TAG *) malloc (sizeof (MD3TAG) * hdr.iTagNum * hdr.iBoneFrameNum); }
                           md3_tag
                           TKN_END
                              { md3BinOutTags (outFile, tag_data, hdr.iTagNum * hdr.iBoneFrameNum, &hdr);
                              }
                        ;

md3_tag:                   /* lamda rule - empty rule */
                        |  md3_tag TKN_TAG_NAME TKN_STRING
                              { strncpy (tag_data[ tagIdx ].cTagName, $3, 12);
                                tag_data[ tagIdx ].cTagName[11] = '\0';
                              }
                           TKN_POSITION TKN_FLOAT TKN_FLOAT TKN_FLOAT
                              { tag_data[ tagIdx ].pos[0] = $6;
                                tag_data[ tagIdx ].pos[1] = $7;
                                tag_data[ tagIdx ].pos[2] = $8;
                              }   
                           TKN_ROTATION_MAT TKN_FLOAT TKN_FLOAT TKN_FLOAT TKN_FLOAT TKN_FLOAT TKN_FLOAT TKN_FLOAT TKN_FLOAT TKN_FLOAT
                              { tag_data[ tagIdx ].fMatrix[0][0] = $11;
                                tag_data[ tagIdx ].fMatrix[0][1] = $12;
                                tag_data[ tagIdx ].fMatrix[0][2] = $13;
                                tag_data[ tagIdx ].fMatrix[1][0] = $14;
                                tag_data[ tagIdx ].fMatrix[1][1] = $15;
                                tag_data[ tagIdx ].fMatrix[1][2] = $16;
                                tag_data[ tagIdx ].fMatrix[2][0] = $17;
                                tag_data[ tagIdx ].fMatrix[2][1] = $18;
                                tag_data[ tagIdx ].fMatrix[2][2] = $19;
                                tagIdx++;
                              }
                        ;

md3_mesh:                  TKN_MD3_MESH
                           TKN_ID TKN_STRING
                              { strncpy (meshhdr_data.cId, $3, 4);
                              }
                           TKN_MESH_NAME TKN_STRING
                              { strncpy (meshhdr_data.cName, $6, 68);
                                meshhdr_data.cName[67] = '\0';
                              }
                           TKN_NUM_MESHFRAMES TKN_INTEGER
                              { meshhdr_data.iMeshFrameNum = $9;
                              }
                           TKN_NUM_SKINS TKN_INTEGER
                              { meshhdr_data.iTextureNum = $12;
                              }
                           TKN_NUM_VERTS TKN_INTEGER
                              { meshhdr_data.iVertexNum = $15;
                              }
                           TKN_NUM_TRIS TKN_INTEGER
                              { meshhdr_data.iTriangleNum = $18;
                              }
                           TKN_TRI_START_ADDR TKN_INTEGER
                              { meshhdr_data.iTriangleStart = $21;
                              }
                           TKN_HEADER_SIZE TKN_INTEGER
                              { meshhdr_data.iHeaderSize = $24;
                              }
                           TKN_TEXVEC_START_ADDR TKN_INTEGER
                              { meshhdr_data.iTecVecStart = $27;
                              }
                           TKN_VERTEX_START_ADDR TKN_INTEGER
                              { meshhdr_data.iVertexStart = $30;
                              }
                           TKN_MESH_SIZE TKN_INTEGER
                              { meshhdr_data.iMeshSize = $33;
                              }
                           TKN_END
                              { md3BinOutMeshHeader (outFile, &meshhdr_data);
                              }
                        ;

md3_texturenames:          TKN_MD3_TEXTURENAMES
                              { int size = sizeof(char) * 68 * meshhdr_data.iTextureNum;

                                textureName_data = (char *) malloc (size);

                                if (textureName_data == NULL)
                                    YYERROR;

                                nextTextureName = textureName_data;
                              }
                           md3_texturename
                           TKN_END
                              { md3BinOutTextureNames (outFile, textureName_data, meshhdr_data.iTextureNum, &meshhdr_data);
                                free (textureName_data);
                              }
                        ;

md3_texturename:           /* lamda rule - empty rule */
                        |  md3_texturename TKN_TEX_NAME TKN_STRING
                              {  strncpy (nextTextureName, $3, 68);
                                 nextTextureName[67] = '\0';
                                 texnamesIdx++;
                                 nextTextureName += 68 * texnamesIdx;
                              }

                        ;

md3_triverts:              TKN_MD3_TRIANGLEVERTS
                              {
                                 int size = sizeof (TRIANGLEVERT);
                                 triangle_data = (TRIANGLEVERT *) malloc (sizeof (TRIANGLEVERT) * meshhdr_data.iTriangleNum);
                                 if (triangle_data == NULL)
                                     YYERROR;
                                 triIdx = 0;
                              }
                           md3_trivert
                           TKN_END
                              { md3BinOutTriangleData (outFile, triangle_data, meshhdr_data.iTriangleNum,  &meshhdr_data);
                                free (triangle_data);
                              }
                        ;

md3_trivert:               /* lamda rule - empty rule */
                        |  md3_trivert TKN_VERT_IDX TKN_INTEGER TKN_INTEGER TKN_INTEGER      
                           { triangle_data[ triIdx ][0] = $3;
                             triangle_data[ triIdx ][1] = $4; 
                             triangle_data[ triIdx ][2] = $5;
                             triIdx++;
                           }                            							
                        ;

md3_texvecs:               TKN_MD3_TEXVEC
                              {  texvec_data = (VEC2 *) malloc (sizeof (VEC2) * meshhdr_data.iVertexNum);
                                 if (texvec_data == NULL)
                                    YYERROR;
                                 texvecIdx = 0;
                              }
                           md3_texvec
                           TKN_END
                              { md3BinOutTexVecData (outFile, texvec_data, meshhdr_data.iVertexNum,  &meshhdr_data);
                                free (texvec_data);
                              }
                        ;

md3_texvec:                /* lamda rule - empty rule */
                        |  md3_texvec TKN_UV_OR_VEC TKN_FLOAT TKN_FLOAT
                              { texvec_data[ texvecIdx ][0] = $3;
                                texvec_data[ texvecIdx ][1] = $4;
                                texvecIdx++;
                              }
                        ;

md3_vertices:              TKN_MD3_VERTEX
                              { vertex_data = (MD3VERTEX *) malloc (sizeof (MD3VERTEX) * meshhdr_data.iVertexNum);
                                if (vertex_data == NULL)
                                   YYERROR;
                                vertIdx = 0;
                              }
                           md3_vertex
                           TKN_END
                              { md3BinOutVertexData (outFile, vertex_data, meshhdr_data.iVertexNum,  &meshhdr_data);
                                free (vertex_data);
                              }
                        ;

md3_vertex:              /* lamda rule - empty rule */
                        |  md3_vertex TKN_VERT TKN_INTEGER TKN_INTEGER TKN_INTEGER
                              { vertex_data[ vertIdx ].vec[0] = $3;
                                vertex_data[ vertIdx ].vec[1] = $4;
                                vertex_data[ vertIdx ].vec[2] = $5;
                                vertIdx++;
                              }
                        ;

%%
/* C code section for yacc specification */

/*
 *-----------------------------------------------------------------------------
 *
 * FUNCTION: yyerror
 *    Call if a parse error is encountered.
 *    
 *
 *
 * RETURN:
 *    0 to signal error and stop parse.
 *
 * OUTPUTS:
 *    none
 *
 * INPUTS:
 *    none
 *
 *-----------------------------------------------------------------------------
 */
int yyerror (const char *msg)
{
   printf ("%s at '%s'\n", msg, yytext);

   return 0;
}