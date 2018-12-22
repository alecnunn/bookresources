/*
 *  FILE       : md3.c
 *
 *  PROGRAMMER : Paul Kelly 
 *               paul_kelly2000@yahoo.com
 *
 *  DESCRIPTION: Read, text write, and binary write functions for .md3
 *               files.
 *               
 *
 *  NOTES      : 
 */

/*
 *
 *  INCLUDES
 *
 */

#include <stdio.h>
#include <string.h>

#include "md3.h"


/*
 *-----------------------------------------------------------------------------
 *
 * FUNCTION: md3ReadHeader
 *           md3ReadBoneFrames
 *           md3ReadTags
 *			 md3ReadMeshHeader
 *			 md3ReadTextureNames
 *			 md3ReadTriangleData
 *			 md3ReadTexVecData
 *			 md3ReadVertexData
 *
 *    Read functions for .md3 model file format.
 *
 *
 * RETURN:
 *    
 *
 * OUTPUTS:
 *    none
 *
 * INPUTS:
 *    none
 *
 *-----------------------------------------------------------------------------
 */
MD3HEADER md3ReadHeader (FILE *inFile)
{
    MD3HEADER hdr;
    int       hdr_size;

    hdr_size = fread (&hdr, sizeof (MD3HEADER), 1, inFile);

    return hdr;
}

void md3ReadBoneFrames (MD3BONEFRAME *boneframe_data, FILE *inFile, int numBoneframes)
{
	int cnt;
    cnt = fread (boneframe_data, sizeof (MD3BONEFRAME), numBoneframes, inFile);
}

void md3ReadTags (MD3TAG *tag_data, FILE *inFile, int numTags)
{
	int cnt;
    cnt = fread (tag_data, sizeof (MD3TAG), numTags, inFile);
}

void md3ReadMeshHeader (MD3MESHFILE *meshhdr_data, FILE *inFile)
{
    int cnt;
    cnt = fread (meshhdr_data, sizeof (MD3MESHFILE), 1, inFile);
}

void md3ReadTextureNames (char *textureName_data, FILE *inFile, int numTextures)
{
	int cnt;
    cnt = fread (textureName_data, sizeof (char) * 68, numTextures, inFile);
}

void md3ReadTriangleData (TRIANGLEVERT *triangle_data, FILE *inFile, int numTriangles)
{
	int cnt;
    cnt = fread (triangle_data, sizeof (TRIANGLEVERT), numTriangles, inFile);
}

void md3ReadTexVecData (VEC2 *texvec_data, FILE *inFile, int numVertices)
{
	int cnt;
    cnt = fread (texvec_data, sizeof (VEC2), numVertices, inFile);
}

void md3ReadVertexData (MD3VERTEX *vertex_data, FILE *inFile, int numVertices)
{
	int cnt;
    cnt = fread (vertex_data, sizeof (MD3VERTEX), numVertices, inFile);
}

/*
 *-----------------------------------------------------------------------------
 *
 * FUNCTION: md3PrintHeader
 *           md3PrintBoneFrames
 *           md3PrintTags
 *			 md3PrintMeshHeader
 *			 md3PrintTextureNames
 *			 md3PrintTriangleData
 *			 md3PrintTexVecData
 *			 md3PrintVertexData
 *
 *    Text output functions for .md3 model file format.
 *
 * RETURN:
 *    
 *
 * OUTPUTS:
 *    none
 *
 * INPUTS:
 *    none
 *
 *-----------------------------------------------------------------------------
 */

void md3PrintHeader (FILE *outFile, MD3HEADER *hdr)
{
    char   temp_buf[5];

    fprintf (outFile, "MD3_FILEHDR\n");
    strncpy (temp_buf, hdr->id, 4);
    temp_buf[4] = '\0';
    fprintf (outFile, "    ID            %s\n", temp_buf);
    fprintf (outFile, "    VERSION       %d\n", hdr->iVersion);
    fprintf (outFile, "    FILENAME      %s\n", hdr->cFileName);
    fprintf (outFile, "    NUM_BONEFRAME %d\n", hdr->iBoneFrameNum);
    fprintf (outFile, "    NUM_TAGS      %d\n", hdr->iTagNum);
    fprintf (outFile, "    NUM_MESH      %d\n", hdr->iMeshNum);
    fprintf (outFile, "    MAXTEX_NUM    %d\n", hdr->iMaxTextureNum);
    fprintf (outFile, "    HEADER_SIZE   %d\n", hdr->iHeaderSize);
    fprintf (outFile, "    TAGS_START    %d\n", hdr->iTagStart);
    fprintf (outFile, "    MESH_START    %d\n", hdr->iMeshStart);
    fprintf (outFile, "    FILE_SIZE     %d\n", hdr->iFileSize);
    fprintf (outFile, "END\n\n");
}

void md3PrintBoneFrames (FILE *outFile, MD3BONEFRAME *boneframe_data, int numBoneframes)
{
    int idx;

    fprintf (outFile, "MD3_BONEFRAMES\n");
    for (idx = 0; idx < numBoneframes; idx++)
    {
        fprintf (outFile, "    MINS       %8.3f %8.3f %8.3f\n",
            boneframe_data[idx].fMins[0],
            boneframe_data[idx].fMins[1],
            boneframe_data[idx].fMins[2]);
        fprintf (outFile, "    MAXS       %8.3f %8.3f %8.3f\n",
            boneframe_data[idx].fMaxs[0],
            boneframe_data[idx].fMaxs[1],
            boneframe_data[idx].fMaxs[2]);
        fprintf (outFile, "    POSITION   %8.3f %8.3f %8.3f\n",
            boneframe_data[idx].pos[0],
            boneframe_data[idx].pos[1],
            boneframe_data[idx].pos[2]);
        fprintf (outFile, "    SCALE      %8.3f\n", boneframe_data[idx].scale);
        fprintf (outFile, "    CREATOR    \"%s\"\n",
            boneframe_data[idx].creator);
    }
    fprintf (outFile, "END\n");
}

void md3PrintTags (FILE *outFile, MD3TAG *tag_data, int numTags)
{
    int idx;


    fprintf (outFile, "MD3_TAGS\n");
    for (idx = 0; idx < numTags; idx++)
    {
        fprintf (outFile, "    TAG_NAME     %s\n", tag_data[idx].cTagName);
        fprintf (outFile, "    POSITION     %8.3f %8.3f %8.3f\n",
            tag_data[idx].pos[0],
            tag_data[idx].pos[1],
            tag_data[idx].pos[2]);
        fprintf (outFile, "    ROTATION_MAT %8.3f %8.3f %8.3f\n" \
                          "                 %8.3f %8.3f %8.3f\n" \
                          "                 %8.3f %8.3f %8.3f\n",
            tag_data[idx].fMatrix[0][0],
            tag_data[idx].fMatrix[0][1],
            tag_data[idx].fMatrix[0][2],
            tag_data[idx].fMatrix[1][0],
            tag_data[idx].fMatrix[1][1],
            tag_data[idx].fMatrix[1][2],
            tag_data[idx].fMatrix[2][0],
            tag_data[idx].fMatrix[2][1],
            tag_data[idx].fMatrix[2][2]);
    }
    fprintf (outFile, "END\n\n");
}

void md3PrintMeshHeader (FILE *outFile, MD3MESHFILE *meshhdr_data)
{
    char   temp_buf[5];
    fprintf (outFile, "MD3_MESH\n");
    strncpy (temp_buf, meshhdr_data->cId, 4);
    temp_buf[4] = '\0';
    fprintf (outFile, "    ID                 %4s\n", temp_buf);
    fprintf (outFile, "    MESH_NAME          %s\n", meshhdr_data->cName);
    fprintf (outFile, "    NUM_MESHFRAMES     %d\n", meshhdr_data->iMeshFrameNum);
    fprintf (outFile, "    NUM_SKINS          %d\n", meshhdr_data->iTextureNum);
    fprintf (outFile, "    NUM_VERTS          %d\n", meshhdr_data->iVertexNum);
    fprintf (outFile, "    NUM_TRIS           %d\n", meshhdr_data->iTriangleNum);
    fprintf (outFile, "    TRI_START_ADDR     %d\n", meshhdr_data->iTriangleStart);
    fprintf (outFile, "    HEADER_SIZE        %d\n", meshhdr_data->iHeaderSize);
    fprintf (outFile, "    TEXVEC_START_ADDR  %d\n", meshhdr_data->iTecVecStart);
    fprintf (outFile, "    VERTEX_START_ADDR  %d\n", meshhdr_data->iVertexStart);
    fprintf (outFile, "    MESH_SIZE          %d\n", meshhdr_data->iMeshSize);
    fprintf (outFile, "END\n\n");
}

void md3PrintTextureNames (FILE *outFile, char *textureName_data, int numTextures)
{
    int idx;
    fprintf (outFile, "MD3_TEXTURENAMES\n");
    for (idx = 0; idx < numTextures; idx++)
    {
        fprintf (outFile, "    TEX_NAME   %s\n", textureName_data);
        textureName_data += 68;
    }
    fprintf (outFile, "END\n\n");
}

void md3PrintTriangleData (FILE *outFile, TRIANGLEVERT *triangle_data, int numTriangles)
{
    int idx;


    fprintf (outFile, "MD3_TRIANGLEVERTS\n");
    for (idx = 0; idx < numTriangles; idx++)
    {
        fprintf (outFile, "    VERT_IDX   %8d %8d %8d\n",
            triangle_data[ idx ][0],
            triangle_data[ idx ][1],
            triangle_data[ idx ][2]);
    }
    fprintf (outFile, "END\n\n");
}

void md3PrintTexVecData (FILE *outFile, VEC2 *texvec_data, int numVertices)
{
    int idx;


    fprintf (outFile, "MD3_TEXVEC\n");
    for (idx = 0; idx < numVertices; idx++)
    {
        fprintf (outFile, "    UV_OR_VEC   %8.3f %8.3f\n",
            texvec_data[ idx ][0],
            texvec_data[ idx ][1]);
    }
    fprintf (outFile, "END\n\n");
}

void md3PrintVertexData (FILE *outFile, MD3VERTEX *vertex_data, int numVertices)
{
    int idx;


    fprintf (outFile, "MD3_VERTEX\n");
    for (idx = 0; idx < numVertices; idx++)
    {
        fprintf (outFile, "    VERT    %8d %8d %8d\n",
            vertex_data[ idx ].vec[0],
            vertex_data[ idx ].vec[1],
            vertex_data[ idx ].vec[2]);
    }
    fprintf (outFile, "END\n\n");
}

/*
 *-----------------------------------------------------------------------------
 *
 * FUNCTION: md3BinOutHeader
 *           md3BinOutBoneFrames
 *           md3BinOutTags
 *			 md3BinOutMeshHeader
 *			 md3BinOutTextureNames
 *			 md3BinOutTriangleData
 *			 md3BinOutTexVecData
 *			 md3BinOutVertexData
 *
 *    Binary output functions for .md3 model file format.
 *
 * RETURN:
 *    
 *
 * OUTPUTS:
 *    none
 *
 * INPUTS:
 *    none
 *
 *-----------------------------------------------------------------------------
 */

void md3BinOutHeader (FILE *outFile, MD3HEADER *hdr)
{
	// write file header
	fwrite (hdr, sizeof (MD3HEADER), 1, outFile);
}

void md3BinOutBoneFrames (FILE *outFile, MD3BONEFRAME *boneframe_data, int numBoneframes, MD3HEADER *hdr)
{
	// write boneframe
	fwrite (boneframe_data, sizeof (MD3BONEFRAME), hdr->iBoneFrameNum, outFile);
}

void md3BinOutTags (FILE *outFile, MD3TAG *tag_data, int numTags, MD3HEADER *hdr)
{
	// write tags
	fwrite (tag_data, sizeof (MD3TAG), hdr->iTagNum * hdr->iBoneFrameNum, outFile);
}

void md3BinOutMeshHeader (FILE *outFile, MD3MESHFILE *meshhdr_data)
{
	// write mesh header
	fwrite (meshhdr_data, sizeof (MD3MESHFILE), 1, outFile);
}

void md3BinOutTextureNames (FILE *outFile, char *textureName_data, int numTextures, MD3MESHFILE *meshhdr_data)
{
	// write texture names
	fwrite (textureName_data, sizeof (char) * 68, meshhdr_data->iTextureNum, outFile);
}

void md3BinOutTriangleData (FILE *outFile, TRIANGLEVERT *triangle_data, int numTriangles,  MD3MESHFILE *meshhdr_data)
{
	// write triangle data
	fwrite (triangle_data, sizeof (TRIANGLEVERT), meshhdr_data->iTriangleNum, outFile);
}

void md3BinOutTexVecData (FILE *outFile, VEC2 *texvec_data, int numVertices,  MD3MESHFILE *meshhdr_data)
{
	// write texvec data
	fwrite (texvec_data, sizeof (VEC2), meshhdr_data->iVertexNum, outFile);
}

void md3BinOutVertexData (FILE *outFile, MD3VERTEX *vertex_data, int numVertices,  MD3MESHFILE *meshhdr_data)
{
	// write vertex data
	fwrite (vertex_data, sizeof (MD3VERTEX), meshhdr_data->iVertexNum, outFile);
}



