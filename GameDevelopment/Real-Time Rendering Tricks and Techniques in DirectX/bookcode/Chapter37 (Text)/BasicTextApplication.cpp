/***************************************************************
* BasicTextApplication.cpp                                     *
*                                                              *
* This file contains the implementation of the                 *
* CBasicTextApplication class.	        					   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
*                                                              *
***************************************************************/

#include "BasicTextApplication.h"

#define STATIC_TEXT   "Static Buffer"
#define STATIC_LENGTH strlen(STATIC_TEXT)

CBasicTextApplication::CBasicTextApplication()
{
	m_pStaticTextBuffer = NULL;
}

CBasicTextApplication::~CBasicTextApplication()
{
}

BOOL CBasicTextApplication::PostInitialize()
{	
	if (FAILED(EasyCreateWindowed(m_hWnd, D3DDEVTYPE_HAL, 
					  D3DCREATE_HARDWARE_VERTEXPROCESSING)))
		return FALSE;	
	
	CreateTextObjects();

	return TRUE;
}

BOOL CBasicTextApplication::PreReset()
{
	DestroyTextObjects();
	return TRUE;
}

BOOL CBasicTextApplication::PostReset()
{
	CreateTextObjects();
	return TRUE;
}

void CBasicTextApplication::Render()
{
	m_Font.DrawText(0.0f, 50.0f, "12345\nThis is a test.", 0xffffaaaa);

	//Sine constrains the value from -1 to 1. 
	//Adding 1 changes the range from 0 to 2.
	//Dividing by 2 changes the range from 0 to 1.
	//Multiplying by the string length gives us a valid character.
	long Character = (long)((sin((float)GetTickCount() / 200.0f) + 1.0) / 
		                    2.0f * (float)STATIC_LENGTH);
	m_Font.DrawStaticText(m_pStaticTextBuffer, Character, 1);

	m_Font.DrawDebug(0, 100);
}

BOOL CBasicTextApplication::PreTerminate()
{
	DestroyTextObjects();
	return TRUE;
}


void CBasicTextApplication::CreateTextObjects()
{
	m_Font.CreateFont(m_pD3DDevice, "Arial", 20);

	m_pStaticTextBuffer = m_Font.CreateStaticText(0.0f, 0.0f,
		                                          STATIC_TEXT, 0xff00ff00);
}

void CBasicTextApplication::DestroyTextObjects()
{
	m_Font.DestroyStaticText(m_pStaticTextBuffer);
	m_Font.DestroyFont();
}
