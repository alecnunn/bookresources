#include "DataLoader.h"

/* ***************************************************************** *\
|  CONSTRUCTION / DESTRUCTION
\* ***************************************************************** */
cDataLoader::cDataLoader()
{
}


cDataLoader::~cDataLoader()
{
}


/* ***************************************************************** *\
|  readPlyFiles
|  *****************************************************************
|  Reads the data from all ply files in the data directory.
\* ***************************************************************** */
bool cDataLoader::readPlyFiles()
{
	TCHAR			szCurDir[MAX_PATH];

	if(!GetCurrentDirectory(sizeof(szCurDir), szCurDir))
	{
		printf("Couldn't get current work directory!\n");
		return false;
	}

	// search for all files ...
	r_scanDirectory(String(szCurDir) + "\\data");

	// now we should have a list of ply files available ...
	if(m_plyFiles.size() <= 0)
	{
		printf("No .ply files found!\n");
		return false;
	}

	// determine the total amount of data to read ....
	uint32 numTriangles = 0;
	for(uint32 i = 0; i < m_plyFiles.size(); i++)
	{
		// open ply file
		p_ply ply = ply_open(m_plyFiles[i].c_str(), NULL);
		if(ply)
		{
			// read header
			if(ply_read_header(ply))
			{
				// determine number of triangles
				numTriangles += ply_set_read_cb(ply, "face", "vertex_indices", face_cb, NULL, 0);
			}else
				printf("Failed to read header of ply %s\n", m_plyFiles[i].c_str());

			// close the file
			ply_close(ply);
		}else
			printf("Couldn't open ply file %s\n", m_plyFiles[i].c_str());
	}

	// print summary of data we'r going to load
	printf("Loading %u triangles from %d ply files ...\n", numTriangles, m_plyFiles.size());

	// resize the vertex data array
	m_Vertices.reserve(numTriangles * 3);

	// read the data
	for(uint32 i = 0; i < m_plyFiles.size(); i++)
	{
		// open ply file
		p_ply ply = ply_open(m_plyFiles[i].c_str(), NULL);
		if(ply)
		{
			// read header
			if(ply_read_header(ply))
			{
				// temporary vertex object used to read in the data
				Vertex plyTmpVertex;

				// set vertex and face processing functions
				uint32 numVertices = ply_set_read_cb(ply, "vertex", "x", vertex_cb_x, &plyTmpVertex, 0);
				ply_set_read_cb(ply, "vertex", "y", vertex_cb_y, &plyTmpVertex, 0);
				// casting a vector* to a long is a a little bit dirty but it works for us here ... 
				ply_set_read_cb(ply, "vertex", "z", vertex_cb_z, &plyTmpVertex, (long)&m_PlyVertexList);
				ply_set_read_cb(ply, "vertex", "nx", vertex_cb_nx, &plyTmpVertex, 0);
				ply_set_read_cb(ply, "vertex", "ny", vertex_cb_ny, &plyTmpVertex, 0);
				ply_set_read_cb(ply, "vertex", "nz", vertex_cb_nz, &plyTmpVertex, (long)&m_PlyVertexList);
				ply_set_read_cb(ply, "face", "vertex_indices", face_cb, &m_PlyVertexList, (long)&m_Vertices);

				// reset the vertex vector
				m_PlyVertexList.clear();
				m_PlyVertexList.reserve(numVertices);

				// start reading ...
				if(!ply_read(ply))
					printf("Error reading ply file %s!\n", m_plyFiles[i].c_str());
			}else
				printf("Failed to read header of ply %s\n", m_plyFiles[i].c_str());

			// close the file
			ply_close(ply);
		}else
			printf("Couldn't open ply file %s\n", m_plyFiles[i].c_str());
	}

	// clear plyVertexList (not needed longer ..)
	m_PlyVertexList.clear();

	// clear file list
	m_plyFiles.clear();

	// everything went ok!
	return true;
}

/* ***************************************************************** *\
|  getVertexData
|  *****************************************************************
|  returns the vertex data
\* ***************************************************************** */
vector<Vertex>* cDataLoader::getVertexData()
{
	return &m_Vertices;
}

/* ***************************************************************** *\
|  r_scanDirectory
|  *****************************************************************
|  Scans a directory (recursively) for ply files.
\* ***************************************************************** */
void cDataLoader::r_scanDirectory(String p_Dir)
{
	// search for all files ...
	HANDLE			hSearch;
	WIN32_FIND_DATA	wfd;
	String			strSearch = p_Dir + "\\*";

	hSearch = FindFirstFile(strSearch.c_str(), &wfd);
	if(hSearch != INVALID_HANDLE_VALUE)
	{
		// process 
		do
		{
			// if we find a directory - scan it
			if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(wfd.cFileName[0] != '.')
					r_scanDirectory(p_Dir + String("\\") + wfd.cFileName);
			}else
			{					
				// if the filename has a .ply extension - add it to the list
				String strFileName(wfd.cFileName);
				if(strFileName.find(".ply", strFileName.length() - 4) != String::npos)
					m_plyFiles.push_back(p_Dir + "\\" + strFileName);
			}

		}while(FindNextFile(hSearch, &wfd));

		// close search handle
		FindClose(hSearch);
	}
}

/* ***************************************************************** *\
|  vertex_cb_*
|  *****************************************************************
|  Reads a ply vertex arguments
\* ***************************************************************** */
int cDataLoader::vertex_cb_x(p_ply_argument p_Arg)
{
	Vertex *plyVertex;

	ply_get_argument_user_data(p_Arg, (void**)&plyVertex, NULL);
	plyVertex->xyz[0] = (float32)ply_get_argument_value(p_Arg);
	return 1;
}

int cDataLoader::vertex_cb_y(p_ply_argument p_Arg)
{
	Vertex *plyVertex;

	ply_get_argument_user_data(p_Arg, (void**)&plyVertex, NULL);
	plyVertex->xyz[1] = (float32)ply_get_argument_value(p_Arg);
	return 1;
}

int cDataLoader::vertex_cb_z(p_ply_argument p_Arg)
{
	Vertex *plyVertex;
	vector<Vertex> *pVector;

	ply_get_argument_user_data(p_Arg, (void**)&plyVertex, (long*)&pVector);
	plyVertex->xyz[2] = (float32)ply_get_argument_value(p_Arg);
	pVector->push_back(*plyVertex);
	return 1;
}

int cDataLoader::vertex_cb_nx(p_ply_argument p_Arg)
{
	Vertex *plyVertex;

	ply_get_argument_user_data(p_Arg, (void**)&plyVertex, NULL);
	plyVertex->nxyz[0] = (float32)ply_get_argument_value(p_Arg);
	return 1;
}

int cDataLoader::vertex_cb_ny(p_ply_argument p_Arg)
{
	Vertex *plyVertex;

	ply_get_argument_user_data(p_Arg, (void**)&plyVertex, NULL);
	plyVertex->nxyz[1] = (float32)ply_get_argument_value(p_Arg);
	return 1;
}

int cDataLoader::vertex_cb_nz(p_ply_argument p_Arg)
{
	Vertex *plyVertex;
	vector<Vertex> *pVector;

	ply_get_argument_user_data(p_Arg, (void**)&plyVertex, (long*)&pVector);
	plyVertex->nxyz[2] = (float32)ply_get_argument_value(p_Arg);
	if(pVector->size() > 0)
		(*pVector)[pVector->size() - 1] = 	(*plyVertex);
	return 1;
}

/* ***************************************************************** *\
|  face_cb
|  *****************************************************************
|  Reads a ply face argument
\* ***************************************************************** */
int cDataLoader::face_cb(p_ply_argument p_Arg)
{
	vector<Vertex> *pSourceVec;
	vector<Vertex> *pDestVec;
	long length, value_index;
	int32 index;

	ply_get_argument_user_data(p_Arg, (void**)&pSourceVec, (long*)&pDestVec);
	ply_get_argument_property(p_Arg, NULL, &length, &value_index);
	index = (int32)ply_get_argument_value(p_Arg);

	// copy the face in our vertices list
	if(index >= 0)
	{
		if(value_index >= 0)
		{
			pDestVec->push_back((*pSourceVec)[index]);
		}
	}else
	{
		printf("Invalid index in ply file!\n");
		return 0;
	}
	
	return 1;
}
