/*
 *  FILE       : main.c
 *
 *  PROGRAMMERS: Paul Kelly
 *               paul_kelly2000@yahoo.com
 *
 *  DESCRIPTION: Tool for converting a text description of a .md3 model
 *               file into a binary file.
 *
 *			     USAGE:
 *					packer outputfile inputfile
 *
 *				 where
 *					outputfile - binary .md3 model file
 *					inputfile - text .md3 model file
 *               
 *  NOTES      : 
 */

/*
 *
 * INCLUDES
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "md3.h"

/*
 *
 * GLOBAL VARIABLES
 *
 */

extern FILE *yyin;
#ifdef _DEBUG
extern int yydebug;  // turns on parser debug info. #define YYDEBUG must 
#endif               // also be defined in the .y file.

FILE            *outFile;

MD3HEADER       hdr;
MD3TAG          *tag_data;
MD3BONEFRAME    *boneframe_data;
MD3MESHFILE     meshhdr_data;
char            *textureName_data;
TRIANGLEVERT    *triangle_data;
VEC2            *texvec_data;
MD3VERTEX       *vertex_data;

/*
 *
 *  PROTOTYPES
 *
 */

extern int yyparse (void);

/*
 *
 * FUNCTIONS
 *
 */

void WriteTextureNames (FILE *outFile)
{
	// write texture names
	fwrite (textureName_data, sizeof(char), 68 * meshhdr_data.iTextureNum, outFile);
}

void GenerateBinaryFile (FILE *outFile)
{
	// write file header
	fwrite (&hdr, sizeof (MD3HEADER), 1, outFile);

	// write boneframe
	fwrite (boneframe_data, sizeof (MD3BONEFRAME), hdr.iBoneFrameNum, outFile);

	// write tags
	fwrite (tag_data, sizeof (MD3TAG),hdr.iTagNum * hdr.iBoneFrameNum, outFile);
	
	// write mesh header
	fwrite (&meshhdr_data, sizeof (MD3MESHFILE), 1, outFile);

	// write texture names
	fwrite (textureName_data, sizeof (char) * 68, meshhdr_data.iTextureNum, outFile);

	// write triangle data
	fwrite (triangle_data, sizeof (TRIANGLEVERT), meshhdr_data.iTriangleNum, outFile);

	// write texvec data
	fwrite (texvec_data, sizeof (VEC2), meshhdr_data.iVertexNum, outFile);

	// write vertex data
	fwrite (vertex_data, sizeof (MD3VERTEX), meshhdr_data.iVertexNum, outFile);
}

void PrintUsage()
{
	printf ("packer output_file input_filename\n");
}

int main (int argc, char **argv)
{
    FILE *inFile; 

	if (argc != 3)
	{
		PrintUsage();
		return 1;
	}

    inFile = fopen (argv[2], "r");
	outFile = fopen (argv[1], "wb");

    if ((inFile == NULL) || (outFile == NULL))
        return 1;

    #ifdef _DEBUG
    // turn on parser debug info if yydebug = 1.
	// "#define YYDEBUG" must be defined in the .y 
    // file also.
    yydebug = 1;
    #endif

    yyin = inFile;

    if (yyparse() == 1)
        return 1;

    fprintf (stderr, "Successful file creation...\n");

	free (tag_data);
	free (boneframe_data);

	fclose (inFile);
	fclose (outFile);

    return 0;
}