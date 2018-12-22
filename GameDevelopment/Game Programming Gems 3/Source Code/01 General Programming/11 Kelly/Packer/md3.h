/*
 *  FILE       : md3.h
 *
 *  PROGRAMMER : Paul Kelly 
 *               paul_kelly2000@yahoo.com
 *
 *  DESCRIPTION: Data structures for the .md3 file format.
 *               Read, text write, and binary write functions for .md3
 *               files.
 *
 *  NOTES      : 
 */

/*
 *
 * INCLUDES
 *
 */

/*
 *
 * TYPES
 *
 */

typedef int TRIANGLEVERT[3];
typedef char TEXNAMES[68];
typedef float VEC2[2];
typedef float VEC3[3];
typedef VEC3 *VMESHFRAME;

typedef struct
{
	float u;
	float v;
} TEXCOORDS;  

//------------------------------------------------------------
// the length of MD3HEADER, MD3BONEFRAME, MD3TAG, MD3MESHFILE 
// and MD3VERTEX is fixed in the md3 file
//------------------------------------------------------------
typedef struct
{
	char id[4];			// id of file, always "IDP3"
	int iVersion;		// version number
	char cFileName[68];	
	int iBoneFrameNum;	// Number of animation key frames in the whole model=every mesh
	int iTagNum;		// Number of tags
	int iMeshNum;		// Number of meshes
	int iMaxTextureNum;	// maximum number of unique textures used in a md3 model
	int iHeaderSize;	// size of header
	int iTagStart;		// starting position of tag frame structures
	int iMeshStart;		// starting position of mesh structures
	int iFileSize;
} MD3HEADER;

typedef struct
{
    float fMins[3];
	float fMaxs[3];		// extrema of the bounding box 
    float pos[3];		// position of the bounding box 
    float scale;		// scaling the bounding box
    char  creator[16];
} MD3BONEFRAME;

typedef struct
{
	char  cTagName[12];
	char  unknown [52];
    float pos[3];		// position of tag relative to the model that contains the tag
    float fMatrix[3][3];// the direction the tag is facing
} MD3TAG;

typedef struct
{
	char cId[4];
	char cName[68];		
	int iMeshFrameNum;  // number of frames in mesh
	int iTextureNum;	// number of textures=skins in this mesh
	int iVertexNum;		// number of vertices in this mesh
	int iTriangleNum;	// number of triangles
	int iTriangleStart;	// starting position of triangle data, relative to the start of MD3Mesh
	int iHeaderSize;	// Headersize = starting position of texture data
	int iTecVecStart;	// starting position of the texture vector data
	int iVertexStart;	// starting position of the vertex data
	int iMeshSize;
} MD3MESHFILE;

typedef struct
{
    short vec[3];
    unsigned char unknown;
} MD3VERTEX;

/*
 *
 * PROTOTYPES
 *
 */

MD3HEADER md3ReadHeader (FILE *inFile);
void md3ReadBoneFrames (MD3BONEFRAME *boneframe_data, FILE *inFile, int numBoneframes);
void md3ReadTags (MD3TAG *tag_data, FILE *inFile, int numTags);
void md3ReadMeshHeader (MD3MESHFILE *meshhdr_data, FILE *inFile);
void md3ReadTextureNames (char *textureName_data, FILE *inFile, int numTextures);
void md3ReadTriangleData (TRIANGLEVERT *triangle_data, FILE *inFile, int numTriangles);
void md3ReadTexVecData (VEC2 *texvec_data, FILE *inFile, int numVertices);
void md3ReadVertexData (MD3VERTEX *vertex_data, FILE *inFile, int numVertices);

void md3PrintHeader (FILE *outFile, MD3HEADER *hdr);
void md3PrintBoneFrames (FILE *outFile, MD3BONEFRAME *boneframe_data, int numBoneframes);
void md3PrintTags (FILE *outFile, MD3TAG *tag_data, int numTags);
void md3PrintMeshHeader (FILE *outFile, MD3MESHFILE *meshhdr_data);
void md3PrintTextureNames (FILE *outFile, char *textureName_data, int numTextures);
void md3PrintTriangleData (FILE *outFile, TRIANGLEVERT *triangle_data, int numTriangles);
void md3PrintTexVecData (FILE *outFile, VEC2 *texvec_data, int numVertices);
void md3PrintVertexData (FILE *outFile, MD3VERTEX *vertex_data, int numVertices);

void md3BinOutHeader (FILE *outFile, MD3HEADER *hdr);
void md3BinOutBoneFrames (FILE *outFile, MD3BONEFRAME *boneframe_data, int numBoneframes, MD3HEADER *hdr);
void md3BinOutTags (FILE *outFile, MD3TAG *tag_data, int numTags, MD3HEADER *hdr);
void md3BinOutMeshHeader (FILE *outFile, MD3MESHFILE *meshhdr_data);
void md3BinOutTextureNames (FILE *outFile, char *textureName_data, int numTextures, MD3MESHFILE *meshhdr_data);
void md3BinOutTriangleData (FILE *outFile, TRIANGLEVERT *triangle_data, int numTriangles,  MD3MESHFILE *meshhdr_data);
void md3BinOutTexVecData (FILE *outFile, VEC2 *texvec_data, int numVertices,  MD3MESHFILE *meshhdr_data);
void md3BinOutVertexData (FILE *outFile, MD3VERTEX *vertex_data, int numVertices,  MD3MESHFILE *meshhdr_data);
