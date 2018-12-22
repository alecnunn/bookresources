/*
 *  FILE       : main.c
 *
 *  PROGRAMMER : Paul Kelly
 *				 paul_kelly2000@yahoo.com
 *
 *  DESCRIPTION: Tool for converting a binary .md3 model file
 *               to its text description.
 *               
 *			     USAGE:
 *					unpacker outputfile inputfile
 *
 *				 where
 *					outputfile - text .md3 model file
 *					inputfile - binary .md3 model file
 *
 *  NOTES      : 
 */

/*
 *
 *  INCLUDES
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "md3.h"


void PrintUsage()
{
	printf ("unpacker output_file input_filename\n");
}

/*
 *-----------------------------------------------------------------------------
 *
 * FUNCTION: main
 *
 *    
 *
 *
 * RETURN:
 *    
 *    Returns the status of the program:
 *      0 = success
 *
 * OUTPUTS:
 *    none
 *
 * INPUTS:
 *    none
 *
 *-----------------------------------------------------------------------------
 */
int main (int argc, char **argv)
{
    FILE            *inFile, *outFile;
    MD3HEADER       hdr;
    MD3TAG          *tag_data;
    MD3BONEFRAME    *boneframe_data;
    MD3MESHFILE     meshhdr_data;
    char            *textureName_data;
    TRIANGLEVERT    *triangle_data;
    VEC2            *texvec_data;
    MD3VERTEX       *vertex_data;
    int             cnt;

	if (argc != 3)
	{
		PrintUsage();
		return 1;
	}

    inFile = fopen (argv[2], "rb");
	outFile = fopen (argv[1], "w");

    if (inFile == NULL)
        return 1;   // error opening file

    hdr = md3ReadHeader (inFile);

    boneframe_data = (MD3BONEFRAME *) malloc (sizeof (MD3BONEFRAME) * hdr.iBoneFrameNum);
    md3ReadBoneFrames (boneframe_data, inFile, hdr.iBoneFrameNum);

    tag_data = (MD3TAG *) malloc (sizeof (MD3TAG) * hdr.iTagNum * hdr.iBoneFrameNum);
    md3ReadTags (tag_data, inFile, hdr.iTagNum * hdr.iBoneFrameNum);

    md3PrintHeader (outFile, &hdr);
    md3PrintBoneFrames (outFile, boneframe_data, hdr.iBoneFrameNum);
    md3PrintTags (outFile, tag_data, hdr.iTagNum * hdr.iBoneFrameNum);
    
    for (cnt = 0; cnt < hdr.iMeshNum; cnt++)
    {

        md3ReadMeshHeader (&meshhdr_data, inFile);

        textureName_data = (char *) calloc (meshhdr_data.iTextureNum, sizeof (char) * 68);
        md3ReadTextureNames (textureName_data, inFile, meshhdr_data.iTextureNum);

        triangle_data = (TRIANGLEVERT*) calloc (meshhdr_data.iTriangleNum, sizeof (TRIANGLEVERT));
        md3ReadTriangleData (triangle_data, inFile, meshhdr_data.iTriangleNum);

        texvec_data = (VEC2 *) calloc (meshhdr_data.iVertexNum, sizeof (VEC2));
        md3ReadTexVecData (texvec_data, inFile, meshhdr_data.iVertexNum);

        vertex_data = (MD3VERTEX *) calloc (meshhdr_data.iVertexNum, sizeof (MD3VERTEX));
        md3ReadVertexData (vertex_data, inFile, meshhdr_data.iVertexNum);

        md3PrintMeshHeader (outFile, &meshhdr_data);
        md3PrintTextureNames (outFile, textureName_data, meshhdr_data.iTextureNum);
        md3PrintTriangleData (outFile, triangle_data, meshhdr_data.iTriangleNum);
        md3PrintTexVecData (outFile, texvec_data, meshhdr_data.iVertexNum);
        md3PrintVertexData (outFile, vertex_data, meshhdr_data.iVertexNum);

        free (textureName_data);
        free (triangle_data);
        free (texvec_data);
        free (vertex_data);
    
    }

    free (boneframe_data);
    free (tag_data);

    fclose (inFile);
    fclose (outFile);

	fprintf (stderr, "Successful file creation.\n");

    return 0; // success!
}

