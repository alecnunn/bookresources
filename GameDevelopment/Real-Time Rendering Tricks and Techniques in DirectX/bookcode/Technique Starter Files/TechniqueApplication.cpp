/***************************************************************
* TechniqueApplication.cpp                                     *
*                                                              *
* This file contains the implementation of the                 *
* TechniqueApplication class.    	        				   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
*                                                              *
***************************************************************/

#include "TechniqueApplication.h"

CTechniqueApplication::CTechniqueApplication()
{
}

CTechniqueApplication::~CTechniqueApplication()
{
}

BOOL CTechniqueApplication::PostInitialize()
{	
	if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
					  D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return FALSE;

	return TRUE;
}

void CTechniqueApplication::Render()
{
}