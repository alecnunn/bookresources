//
// BaseObject.h
//

#ifndef _REFLECTIVE_UI_H
#define _REFLECTIVE_UI_H

class CBaseObject;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CReflectiveUI
//
// This class builds and updates the property grid UI, as well as responding to its messages.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class CReflectiveUI
{

public :

	//----------------------------------------------------------------------------------------------
	CReflectiveUI();

	//----------------------------------------------------------------------------------------------
	// Builds a dynamic property grid for the specified object by looking up its reflective properties.
	void	BuildUIForObject( CBaseObject* pObject, const char* szName );

	//----------------------------------------------------------------------------------------------
	// Messages received from the property grid control are routed here
	LRESULT	OnGridNotify(WPARAM, LPARAM);

	// The property grid control itself
	//----------------------------------------------------------------------------------------------
	inline	CXTPPropertyGrid*	GetGrid();
	inline	void				SetGrid( CXTPPropertyGrid* pGrid );

private :

	CBaseObject*		m_pActiveObject;		// Object currently being explored in the property grid control
	CXTPPropertyGrid*	m_pGrid;				// Property grid control

};

inline CXTPPropertyGrid* CReflectiveUI::GetGrid()
{
	return m_pGrid;
}

inline void CReflectiveUI::SetGrid( CXTPPropertyGrid* pGrid )
{
	m_pGrid = pGrid;
}

#endif	// _REFLECTIVE_UI_H