#ifndef			CAPPLICATION_H_INCLUDED
#define			CAPPLICATION_H_INCLUDED

#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once
#endif 
#endif 

#include		"../glut/glut.h"
#include		"../glfont/GLFont.h"
#include		"../ik/CNode.h"
#include		"CTarget.h"
#include		"../ik/CIKSolverCyclicCoordinateDescent.h"
#include		"../ik/CIKSolverJacobianTranspose.h"

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	CApplication
//*****************************************************************************
//
//!	The application class using the Singleton design pattern
//
//*****************************************************************************
class CApplication
{

private:

	// Static instance

	static CApplication*				m_pApplication;			//!< The one and only instance of CApplication

	// OpenGL objects

	GLuint								m_uiListGrid;			//!< OpenGL display list identifier for the grid object
	GLuint								m_uiListAxis;			//!< OpenGL display list identifier for a coordinate axis object
	GLuint								m_uiListNode;			//!< OpenGL display list identifier for a node object

	GLUquadricObj*						m_Quadric;				//!< OpenGL quadric object used to draw a sphere

	GLFont								m_Font;					//!< Font used for rendering

	// "Camera"

	GLfloat								m_fDistance;			//!< Viewer's distance from the origin
	GLfloat								m_fAngleX;				//!< Viewer's rotation about the x-axis
	GLfloat								m_fAngleY;				//!< Viewer's rotation about the y-axis

	// Flags

	bool								m_bShowHelp;			//!< Flag whether to show help or not
	bool								m_bConstrainRotation;	//!< Flag whether to limit the node rotation or not

	// IK chain data and solver

	CNode								m_Root;					//!< Chain's root node
	CNode*								m_pEffector;			//!< Pointer to the node currently used as effector
	int									m_iLinkCount;			//!< Number of nodes in chain
	CTarget								m_Target;				//!< Object used as target for the effector

	CIKSolverCyclicCoordinateDescent	m_SolverCCD;			//!< Instance of a solver using the Cyclic Coordinate Descent method
	CIKSolverJacobianTranspose			m_SolverJT	;			//!< Instance of a solver using the Jacobian Transpose method
	IIKSolver*							m_pCurrentSolver;		//!< Pointer to the currently used solver
	std::string							m_strCurrentSolver;		//!< Name of the currently used solver

	// Con-/Destruction

							CApplication	();
							CApplication	(const CApplication& a_Other);	//!< Private declaration (only) to prevent automatic generation
							~CApplication	();
	CApplication&			operator=		(const CApplication& a_Other);	//!< Private declaration (only) to prevent automatic generation

	// Node processing

	void					AddNode			();
	void					RemoveNode		();
	void					DrawNode		(const CNode& a_Node) const;
	void					ResetNodes		(CNode& a_Node);
	void					ChangeLowerLimit(CNode& a_Node, float a_fChange);
	void					ChangeUpperLimit(CNode& a_Node, float a_fChange);

	// Event handling

	void					OnDraw			();
	void					OnResize		(int a_iWidth, int a_iHeight);
	void					OnKeyDown		(unsigned char a_ucKey, int a_iX, int a_iY);
	void					OnMouseDown		(int a_iButton, int a_iState, int a_iX, int a_iY);
	void					OnMouseMove		(int a_iX, int a_iY);
	void					OnSpecialKeyDown(int a_iKey, int a_iX, int a_iY);
	void					OnIdle			();

public:

	// Return application instance

	static CApplication&	Instance		();

	// Destroy instance

	static void				Destroy			();

	// Initialize application

	bool					Initialize		();

	// GLUT callbacks

	static void				Display			();
	static void				Reshape			(int a_iWidth, int a_iHeight);
	static void				Keyboard		(unsigned char a_ucKey, int a_iX, int a_iY);
	static void				Mouse			(int a_iButton, int a_iState, int a_iX, int a_iY);
	static void				Motion			(int a_iX, int a_iY);
	static void				Special			(int a_iKey, int a_iX, int a_iY);
	static void				Idle			();

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	Display
//*****************************************************************************
//
//!	\brief	GLUT "DisplayFunc" callback method
//
//*****************************************************************************
inline void CApplication::Display()
{
	CApplication::Instance().OnDraw();
}

//*****************************************************************************
//	Reshape
//*****************************************************************************
//
//!	\brief	GLUT "ReshapeFunc" callback method
//!	\param	[in] New viewport width
//!	\param	[in] New viewport height
//
//*****************************************************************************
inline void CApplication::Reshape(int a_iWidth, int a_iHeight)
{
	CApplication::Instance().OnResize(a_iWidth, a_iHeight);
}

//*****************************************************************************
//	Keyboard
//*****************************************************************************
//
//!	\brief	GLUT "KeyboardFunc" callback method
//!	\param	[in] Identifier of the pressed key
//!	\param	[in] Current cursor x-position
//!	\param	[in] Current cursor y-position
//
//*****************************************************************************
inline void CApplication::Keyboard(unsigned char a_ucKey, int a_iX, int a_iY)
{
	CApplication::Instance().OnKeyDown(a_ucKey, a_iX, a_iY);
}

//*****************************************************************************
//	Mouse
//*****************************************************************************
//
//!	\brief	GLUT "MouseFunc" callback method
//!	\param	[in] Identifier of the pressed mouse button
//!	\param	[in] State of the mouse button
//!	\param	[in] Current cursor x-position
//!	\param	[in] Current cursor y-position
//
//*****************************************************************************
inline void CApplication::Mouse(int a_iButton, int a_iState, int a_iX, int a_iY)
{
	CApplication::Instance().OnMouseDown(a_iButton, a_iState, a_iX, a_iY);
}

//*****************************************************************************
//	Motion
//*****************************************************************************
//
//!	\brief	GLUT "MotionFunc" callback method
//!	\param	[in] Current cursor x-position
//!	\param	[in] Current cursor y-position
//
//*****************************************************************************
inline void CApplication::Motion(int a_iX, int a_iY)
{
	CApplication::Instance().OnMouseMove(a_iX, a_iY);
}

//*****************************************************************************
//	Special
//*****************************************************************************
//
//!	\brief	GLUT "SpecialFunc" callback method
//!	\param	[in] Identifier of the pressed key
//!	\param	[in] Current cursor x-position
//!	\param	[in] Current cursor y-position
//
//*****************************************************************************
inline void CApplication::Special(int a_iKey, int a_iX, int a_iY)
{
	CApplication::Instance().OnSpecialKeyDown(a_iKey, a_iX, a_iY);
}

//*****************************************************************************
//	Idle
//*****************************************************************************
//
//!	\brief	GLUT "IdleFunc" callback method
//
//*****************************************************************************
inline void CApplication::Idle()
{
	CApplication::Instance().OnIdle();
}

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // CAPPLICATION_H_INCLUDED
