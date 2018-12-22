#include <nvIO/nvObjReader.h>
#include <nvIO/NVMeshMender.h>
#include <math.h>
#include <stdlib.h>
/*
void scale_down( int nverts, float *&vertexdata );

static void normalize( float v[3] )
{
    float len = (float) sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
    if ( len == 0.0 )
        {
        fprintf( stderr, "." );
        v[0] = 0.0;
        v[1] = 0.0;
        v[2] = 1.0;
        return;
        }
    len = 1.0f / len;
    v[0] = v[0]*len;
    v[1] = v[1]*len;
    v[2] = v[2]*len;
}

static void cross( float C[3], float A[3], float B[3] )
{
    C[0] = (A[1]*B[2] - A[2]*B[1]);
    C[1] = (A[2]*B[0] - A[0]*B[2]);
    C[2] = (A[0]*B[1] - A[1]*B[0]);
}

bool LoadObjModel( char *name, char *dirname,
                   unsigned int &nverts, unsigned int &nindices, unsigned int *&indices,
                   float *&vertexdata, float *&normaldata, float *&tangendata, float *&binormdata,
				   float *&texcoords )
{
    char fileName[512];
	if (dirname) {
	    sprintf( fileName, "%s\\%s", dirname, name );
    } else {
	    sprintf( fileName, "%s", name );
 	}

   	nvObjReader* reader = new nvObjReader();
	reader->ReadFile(fileName,0);

	std::vector<int> triIndices;
	if (!reader->GetTriangleIndices(triIndices))
		return false;

	std::vector<float> vpos;
	if (!reader->GetAttribute(nvMeshAttribute::VERTEX_POSITION,vpos))
		return false;

	std::vector<float> vnor;
	bool normals = false;
	if (reader->GetAttribute(nvMeshAttribute::VERTEX_NORMAL,vnor))
		normals = true;

	std::vector<float> vtex;
	bool textures = false;
	if (reader->GetAttribute(nvMeshAttribute::TEX_COORD0,vtex))
		textures = true;

    nverts   = vpos.size()/3;
    nindices = triIndices.size();

    std::vector<NVMeshMender::VertexAttribute> inputAtts;
	std::vector<NVMeshMender::VertexAttribute> outputAtts;

	NVMeshMender::VertexAttribute posAtt;
	posAtt.Name_ = "position";
	posAtt.floatVector_ = vpos;

	NVMeshMender::VertexAttribute triIndAtt;
	triIndAtt.Name_ = "indices";
	triIndAtt.intVector_ = triIndices;

	NVMeshMender::VertexAttribute norAtt;
	norAtt.Name_ = "normal";
	norAtt.floatVector_ = vnor;

	NVMeshMender::VertexAttribute texCoordAtt;
	texCoordAtt.Name_ = "tex0";
	texCoordAtt.floatVector_ = vtex;

	std::vector<float> vtan;
    NVMeshMender::VertexAttribute tgtSpaceAtt;
	tgtSpaceAtt.Name_ = "tangent";
    tgtSpaceAtt.floatVector_ = vtan;

    std::vector<float> vbin;
    NVMeshMender::VertexAttribute binSpaceAtt;
	binSpaceAtt.Name_ = "binormal";
    binSpaceAtt.floatVector_ = vbin;
	
	inputAtts.push_back(posAtt);
	inputAtts.push_back(triIndAtt);
	if (normals)
		inputAtts.push_back(norAtt);
	if (textures)
		inputAtts.push_back(texCoordAtt);
	
	outputAtts.push_back(posAtt);
	outputAtts.push_back(triIndAtt);
	outputAtts.push_back(norAtt);
	outputAtts.push_back(texCoordAtt);
	outputAtts.push_back(tgtSpaceAtt);
	outputAtts.push_back(binSpaceAtt);
	
	NVMeshMender mger;
	mger.Munge(inputAtts,outputAtts);

    
    nverts = outputAtts[ 0 ].floatVector_.size() / 3;


    indices    = new unsigned int[nindices];
    vertexdata = new float[3*nverts];
    normaldata = new float[3*nverts];
    tangendata = new float[3*nverts];
    binormdata = new float[3*nverts];
    texcoords  = new float[2*nverts];

    if ( indices == NULL ||
         vertexdata == NULL ||
         normaldata == NULL ||
         tangendata == NULL ||
         binormdata == NULL ||
         texcoords == NULL )
    {
        return false;
    }

    memcpy( vertexdata, &(outputAtts[0].floatVector_[0]), nverts*3*sizeof(float) );
    memcpy( indices,    &(outputAtts[1].intVector_[0]), nindices*sizeof(int) );
    memcpy( normaldata, &(outputAtts[2].floatVector_[0]), nverts*3*sizeof(float) );
    memcpy( tangendata, &(outputAtts[4].floatVector_[0]), nverts*3*sizeof(float) );
    memcpy( binormdata, &(outputAtts[5].floatVector_[0]), nverts*3*sizeof(float) );

    // copy over texture coordinates skipping every 3rd float since (s,t,1)
    // is returned (for generated texture coordinates anyways)
    unsigned int ti = 0;
    for ( unsigned int t = 0; t < 3*nverts; t++ )
    {
        if ( t%3 == 2 ) continue;
        texcoords[ti] = outputAtts[3].floatVector_[t];
        ti++;
    }
 
    // normalize vectors just in case the reader wasn't smart enough to do it (ugh).
    for ( ti = 0; ti < nverts; ti++ )
    {
        normalize( &(tangendata[3*ti]) );
        normalize( &(binormdata[3*ti]) );
        normalize( &(normaldata[3*ti]) );
    }

    // Scale the geometry down into the appropriate range.
    scale_down( nverts, vertexdata );

    return true;
}

void scale_down( int nverts, float *&vertexdata )
{
    int i;
    float minmax[6];
    float center[3] = { 0.0, 0.0, 0.0 };
    float radius = 2.0;

    // Determine model extents.
    minmax[0] = 99999999.99f;    minmax[1] = -99999999.99f;
    minmax[2] = 99999999.99f;    minmax[3] = -99999999.99f;
    minmax[4] = 99999999.99f;    minmax[5] = -99999999.99f;
    for ( i = 0; i < nverts; i++ )
        {
        float *v = &(vertexdata[3*i]);
        if ( v[0] < minmax[0] ) minmax[0] = v[0];
        if ( v[0] > minmax[1] ) minmax[1] = v[0];
        if ( v[1] < minmax[2] ) minmax[2] = v[1];
        if ( v[1] > minmax[3] ) minmax[3] = v[1];
        if ( v[2] < minmax[4] ) minmax[4] = v[2];
        if ( v[2] > minmax[5] ) minmax[5] = v[2];
        }

    // Scale to shift to [-radius,radius]
    float rad[3];
    rad[0] = 0.5f*(minmax[1]-minmax[0]);
    rad[1] = 0.5f*(minmax[3]-minmax[2]);
    rad[2] = 0.5f*(minmax[5]-minmax[4]);

    float oldcenter[3];
    oldcenter[0] = minmax[0] + rad[0];
    oldcenter[1] = minmax[2] + rad[1];
    oldcenter[2] = minmax[4] + rad[2];

    float oldradius;
    float newradius;
    oldradius = sqrtf( rad[0]*rad[0] + rad[1]*rad[1] + rad[2]*rad[2] );
    newradius = radius / oldradius;
    
    for ( i = 0; i < nverts; i++ )
    {
        int id = 3*i;
        vertexdata[id+0] = center[0] + newradius*(vertexdata[id+0]-oldcenter[0]);
        vertexdata[id+1] = center[1] + newradius*(vertexdata[id+1]-oldcenter[1]);
        vertexdata[id+2] = center[2] + newradius*(vertexdata[id+2]-oldcenter[2]);
    }
}
*/