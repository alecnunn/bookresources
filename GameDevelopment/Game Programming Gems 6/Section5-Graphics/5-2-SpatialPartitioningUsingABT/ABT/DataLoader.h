/*******************************************************************\
| DataLoader.h
|
|--------------------------------------------------------------------
| Copyright 2004/2005. Martin Fleisz.
| All rights reserved.
|
|--------------------------------------------------------------------
| CREATED:		2005/8/10
| AUTHOR:		Martin Fleisz
|
|--------------------------------------------------------------------
| DESCRIPTION:
| 
| Helper class to load data from a .ply file (or multiple .ply files)
\********************************************************************/
#ifndef __DATALOADER_H
#define __DATALOADER_H

#include <Windows.h>	// for the FindFirstFile/FindNextFile API functions
#include "rply.h"
#include "Types.h"




// define the structure of a ply vertex
class cDataLoader
{
	public:

		cDataLoader();
		~cDataLoader();

		bool	readPlyFiles();	// read all ply files from the \data directory
		vector<Vertex>*	getVertexData();
	
	private:
		
		void r_scanDirectory(String p_Dir);

		// ply reading functions
		static int vertex_cb_x(p_ply_argument p_Arg);
		static int vertex_cb_y(p_ply_argument p_Arg);
		static int vertex_cb_z(p_ply_argument p_Arg);
		static int vertex_cb_nx(p_ply_argument p_Arg);
		static int vertex_cb_ny(p_ply_argument p_Arg);
		static int vertex_cb_nz(p_ply_argument p_Arg);
		static int face_cb(p_ply_argument p_Arg);
		

		vector<string>	m_plyFiles;	// vector containing our input ply files
		vector<Vertex>	m_Vertices;

		
		vector<Vertex>	m_PlyVertexList;	// buffers for the ply reading functions
		uint32			m_VertexInsertCounter;
		uint32			m_FaceInsertCounter;
};


#endif // __DATALOADER_H
