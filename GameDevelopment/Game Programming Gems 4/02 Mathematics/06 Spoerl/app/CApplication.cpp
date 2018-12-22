#include	"CApplication.h"
#include	"../math/CAxisAngle.h"
#include	<gl\glaux.h>
#include	<iostream>

#pragma comment( lib, "glaux.lib" )

//*********************************************************************************************************************
//*********************************************************************************************************************
//	Static initialization
//*********************************************************************************************************************
//*********************************************************************************************************************

CApplication* CApplication::m_pApplication = NULL;

//*********************************************************************************************************************
//*********************************************************************************************************************
//	Private methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//*****************************************************************************
//	Default constructor
//*****************************************************************************
//
//!	\brief	Default constructor
//
//*****************************************************************************
CApplication::CApplication()
: m_fDistance(12.0f), m_fAngleX(40.0f), m_fAngleY(10.0f), m_bShowHelp(false)
, m_bConstrainRotation(false), m_pEffector(&m_Root), m_iLinkCount(1)
, m_pCurrentSolver(&m_SolverJT), m_strCurrentSolver("Jacobian Transpose")
{}

//*****************************************************************************
//	Destructor
//*****************************************************************************
//
//!	\brief	Destructor
//
//*****************************************************************************
CApplication::~CApplication()
{
	// Delete display lists

	glDeleteLists(m_uiListGrid, 1);
	glDeleteLists(m_uiListAxis, 1);
	glDeleteLists(m_uiListNode, 1);

	// Delete quadric object

	gluDeleteQuadric(m_Quadric);

	// Delete font

	m_Font.KillFont();
}

//*****************************************************************************
//	AddNode
//*****************************************************************************
//
//!	\brief	Adds a new node to the chain and sets it as the new effector
//
//*****************************************************************************
void CApplication::AddNode()
{
	// Create new node

	CNode* pNode = new CNode;

	// Add node and set new effector

	m_pEffector->AddChild(pNode);
	m_pEffector = pNode;

	// Increase node count

	++m_iLinkCount;

	// Compute transformation matrices

	m_Root.CalculateMatrix(true);
}

//*****************************************************************************
//	RemoveNode
//*****************************************************************************
//
//!	\brief	Removes a node from the chain and sets its parent as the new effector
//
//*****************************************************************************
void CApplication::RemoveNode()
{
	// Keep at least two nodes

	if( 2 == m_iLinkCount )
		return;

	// The effector is going to be deleted,
	// so save it's parent pointer

	CNode* pParent = m_pEffector->AccessParent();

	if( NULL == pParent )
		return;

	// Remove last node and set new effector

	pParent->RemoveChild(m_pEffector, true);
	m_pEffector = pParent;

	// Decrease node count

	--m_iLinkCount;

	// Compute transformation matrices

	m_Root.CalculateMatrix(true);
}

//*****************************************************************************
//	DrawNode
//*****************************************************************************
//
//!	\brief	Draws the given node and all of its children
//!	\param	[in] The node to draw
//
//*****************************************************************************
void CApplication::DrawNode(const CNode& a_Node) const
{
	// Convert the rotation quaternion to an axis/angle representation

	CAxisAngle rotation(a_Node.GetRotation());

	// Position and draw the node and its axis

	glPushMatrix();
		
		glTranslatef(a_Node.GetTranslation().GetX(), a_Node.GetTranslation().GetY(), a_Node.GetTranslation().GetZ());
		glRotatef(rotation.GetAngle() * g_fRadToDeg, rotation.GetAxis().GetX(), rotation.GetAxis().GetY(), rotation.GetAxis().GetZ());
		glCallList(m_uiListAxis);

		// Don't draw a node for the end effector

		if( &a_Node != m_pEffector )
			glCallList(m_uiListNode);

		// Draw node's children

		for( int iChild = 0; iChild < a_Node.GetChildCount(); ++iChild )
			DrawNode(*a_Node.GetChild(iChild));

	glPopMatrix();
}

//*****************************************************************************
//	ResetNodes
//*****************************************************************************
//
//!	\brief	Resets the rotation of all nodes
//!	\param	[in] The node to reset
//
//*****************************************************************************
void CApplication::ResetNodes(CNode& a_Node)
{
	// Reset specified node

	a_Node.SetRotation(CQuaternion(0.0f, 0.0f, 0.0f, 1.0f));

	// Reset node's children

	for( int iChild = 0; iChild < a_Node.GetChildCount(); ++iChild )
		ResetNodes(*a_Node.AccessChild(iChild));
}

//*****************************************************************************
//	ChangeLowerLimit
//*****************************************************************************
//
//!	\brief	Changes the negative rotational limits
//!	\param	[in] The node to change
//!	\param	[in] The change to the limit
//
//*****************************************************************************
void CApplication::ChangeLowerLimit(CNode& a_Node, float a_fChange)
{
	// Manipulate specified node

	float	fAngle = a_Node.GetNegativeLimits().GetX() * g_fRadToDeg;
			fAngle = Clamp( fAngle + a_fChange, -170.0f, -5.0f);
			fAngle *= g_fDegToRad;

	a_Node.SetNegativeLimits( CVector3(fAngle, fAngle, fAngle) );

	// Manipulate node's children

	for( int iChild = 0; iChild < a_Node.GetChildCount(); ++iChild )
		ChangeLowerLimit(*a_Node.AccessChild(iChild), a_fChange);
}

//*****************************************************************************
//	ChangeUpperLimit
//*****************************************************************************
//
//!	\brief	Changes the positive rotational limits
//!	\param	[in] The node to change
//!	\param	[in] The change to the limit
//
//*****************************************************************************
void CApplication::ChangeUpperLimit(CNode& a_Node, float a_fChange)
{
	// Manipulate specified node

	float	fAngle = a_Node.GetPositiveLimits().GetX() * g_fRadToDeg;
			fAngle = Clamp( fAngle + a_fChange, 5.0f, 170.0f);
			fAngle *= g_fDegToRad;

	a_Node.SetPositiveLimits( CVector3(fAngle, fAngle, fAngle) );

	// Manipulate node's children

	for( int iChild = 0; iChild < a_Node.GetChildCount(); ++iChild )
		ChangeUpperLimit(*a_Node.AccessChild(iChild), a_fChange);
}

//*****************************************************************************
//	OnDraw
//*****************************************************************************
//
//!	\brief	Draws the scene
//
//*****************************************************************************
void CApplication::OnDraw()
{
	// Set modelview matrix

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, m_fDistance, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Clear

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Rotate scene (fake camera movement)

	glPushMatrix();
		glRotatef(m_fAngleX, 1.0f, 0.0f, 0.0f);
		glRotatef(m_fAngleY, 0.0f, 1.0f, 0.0f);

		// Draw grid

		glCallList(m_uiListGrid);

		// Draw nodes

		DrawNode(m_Root);

		// Position and draw the target (a sphere)

		glPushMatrix();
			glTranslatef(m_Target.GetPosition().GetX(), m_Target.GetPosition().GetY(), m_Target.GetPosition().GetZ());
			glPushMatrix();
				glScalef(2.0f, 2.0f, 2.0f);
				glCallList(m_uiListAxis);
			glPopMatrix();
			glColor3f(0.5f, 0.5f, 1.0f);
			gluSphere(m_Quadric, m_Target.GetThreshold(), 8, 8);
		glPopMatrix();

	glPopMatrix();

	// Init text

	int			iTextLine	= 5;
	const int	iLineHeight = 15;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glColor3f(1.0f, 1.0f, 1.0f);

	m_Font.SetBase(32);

	// Print current solver

	iTextLine += iLineHeight; m_Font.Print(5, iTextLine, m_strCurrentSolver.c_str());
	iTextLine += iLineHeight;

	// Print constraint information

	if( true == m_bConstrainRotation )
	{
		m_Font.Print(5, iTextLine, "Rotation is constrained");

		char carrBuffer[256];
		sprintf(carrBuffer, "Lower limit: %2.2f degrees", m_Root.GetNegativeLimits().GetX() * g_fRadToDeg);
		iTextLine += iLineHeight; m_Font.Print(5, iTextLine, carrBuffer);
		sprintf(carrBuffer, "Upper limit: %2.2f degrees", m_Root.GetPositiveLimits().GetX() * g_fRadToDeg);
		iTextLine += iLineHeight; m_Font.Print(5, iTextLine, carrBuffer);
	}
	else
	{
		m_Font.Print(5, iTextLine, "Rotation is not constrained");
	}

	// Print help hint

	iTextLine += iLineHeight;
	iTextLine += iLineHeight; m_Font.Print(5, iTextLine, "Press \'H\' to toggle help.");
	iTextLine += iLineHeight;

	// Print help

	if( true == m_bShowHelp )
	{
		iTextLine += iLineHeight; m_Font.Print(5, iTextLine, "Cursor keys .... : Camera rotation");
		iTextLine += iLineHeight; m_Font.Print(5, iTextLine, "Page up/down ... : Camera zoom");
		iTextLine += iLineHeight; m_Font.Print(5, iTextLine, "Numpad ......... : Target movement");
		iTextLine += iLineHeight; m_Font.Print(5, iTextLine, "/ and * ........ : Change lower rotational limits");
		iTextLine += iLineHeight; m_Font.Print(5, iTextLine, "- and + ........ : Change upper rotational limits");
		iTextLine += iLineHeight; m_Font.Print(5, iTextLine, "A .............. : Add node");
		iTextLine += iLineHeight; m_Font.Print(5, iTextLine, "D .............. : Delete node");
		iTextLine += iLineHeight; m_Font.Print(5, iTextLine, "R .............. : Reset node rotations");
		iTextLine += iLineHeight; m_Font.Print(5, iTextLine, "S .............. : Solve");
		iTextLine += iLineHeight; m_Font.Print(5, iTextLine, "C .............. : Change solver");
		iTextLine += iLineHeight; m_Font.Print(5, iTextLine, "L .............. : Toggle rotational limits");
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	// Display scene

	glFlush();
	glutSwapBuffers();
}

//*****************************************************************************
//	OnResize
//*****************************************************************************
//
//!	\brief	Resizes the current viewport
//!	\param	[in] New viewport width
//!	\param	[in] New viewport height
//
//*****************************************************************************
void CApplication::OnResize(int a_iWidth, int a_iHeight)
{
	// Constants used on resize

	const GLfloat fFov		= 45.0f;
	const GLfloat fZNear	= g_fZNear;
	const GLfloat fZFar		= 100.0f;
	const GLfloat fAspect	= static_cast<GLfloat>(a_iWidth) / static_cast<GLfloat>(a_iHeight);

	// Set viewport

	glViewport(0, 0, a_iWidth, a_iHeight);

	// Set projection matrix

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fFov, fAspect, fZNear, fZFar);

	// Resize font

	m_Font.SetDisplayMode(a_iWidth, a_iHeight);
}

//*****************************************************************************
//	OnKeyDown
//*****************************************************************************
//
//!	\brief	Handles actions for "standard" keys
//!	\param	[in] Identifier of the pressed key
//!	\param	[in] Current cursor x-position
//!	\param	[in] Current cursor y-position
//
//*****************************************************************************
void CApplication::OnKeyDown(unsigned char a_ucKey, int a_iX, int a_iY)
{
	switch( a_ucKey )
	{
		// Toggle help
	case 'h':
	case 'H':
		m_bShowHelp = !m_bShowHelp;
		break;

		// Toggle rotational limits
	case 'l':
	case 'L':
		m_bConstrainRotation = !m_bConstrainRotation;
		break;

		// Add new node
	case 'a':
	case 'A':
		AddNode();
		m_pCurrentSolver->Initialize(*m_pEffector, m_iLinkCount);
		break;

		// Delete last node
	case 'd':
	case 'D':
		RemoveNode();
		m_pCurrentSolver->Initialize(*m_pEffector, m_iLinkCount);
		break;

		// Reset node rotations
	case 'r':
	case 'R':
		ResetNodes(m_Root);
		m_Root.CalculateMatrix(true);
		break;

		// Solve manipulator
	case 's':
	case 'S':
		m_pCurrentSolver->Solve(m_bConstrainRotation);
		m_Root.CalculateMatrix(true);
		break;

		// Change IK solver
	case 'c':
	case 'C':
		ResetNodes(m_Root);
		m_Root.CalculateMatrix(true);

		if( m_pCurrentSolver == &m_SolverJT )
		{
			m_pCurrentSolver	= &m_SolverCCD;
			m_strCurrentSolver	= "Cyclic Coordinate Descent";
		}
		else
		{
			m_pCurrentSolver	= &m_SolverJT;
			m_strCurrentSolver	= "Jacobian Transpose";
		}

		m_pCurrentSolver->SetTargetPosition		(m_Target.GetPosition());
		m_pCurrentSolver->SetTargetThreshold	(m_Target.GetThreshold());
		m_pCurrentSolver->SetTryLimit			(100);
		m_pCurrentSolver->Initialize			(*m_pEffector, m_iLinkCount);

		break;

		// Move target along negative z-axis
	case '8':
		m_Target.Translate(CVector3(0.0f, 0.0f, -0.5f));
		m_pCurrentSolver->SetTargetPosition(m_Target.GetPosition());
		break;

		// Move target along positive z-axis
	case '2':
		m_Target.Translate(CVector3(0.0f, 0.0f, 0.5f));
		m_pCurrentSolver->SetTargetPosition(m_Target.GetPosition());
		break;

		// Move target along negative x-axis
	case '4':
		m_Target.Translate(CVector3(-0.5f, 0.0f, 0.0f));
		m_pCurrentSolver->SetTargetPosition(m_Target.GetPosition());
		break;

		// Move target along positive x-axis
	case '6':
		m_Target.Translate(CVector3(0.5f, 0.0f, 0.0f));
		m_pCurrentSolver->SetTargetPosition(m_Target.GetPosition());
		break;

		// Move target along positive y-axis
	case '7':
	case '9':
		m_Target.Translate(CVector3(0.0f, 0.5f, 0.0f));
		m_pCurrentSolver->SetTargetPosition(m_Target.GetPosition());
		break;

		// Move target along negative y-axis
	case '1':
	case '3':
		if( m_Target.GetPosition().GetY() > 0.0f )
			m_Target.Translate(CVector3(0.0f, -0.5f, 0.0f));
		m_pCurrentSolver->SetTargetPosition(m_Target.GetPosition());
		break;

		// Decrease lower rotational limit
	case '/':
		if( true == m_bConstrainRotation )
			ChangeLowerLimit(m_Root, -5.0f);
		break;

		// Increase lower rotational limit
	case '*':
		if( true == m_bConstrainRotation )
			ChangeLowerLimit(m_Root, 5.0f);
		break;

		// Decrease upper rotational limit
	case '-':
		if( true == m_bConstrainRotation )
			ChangeUpperLimit(m_Root, -5.0f);
		break;

		// Increase upper rotational limit
	case '+':
		if( true == m_bConstrainRotation )
			ChangeUpperLimit(m_Root, 5.0f);
		break;

	default:
		return;
	}

	glutPostRedisplay();
}

//*****************************************************************************
//	OnMouseDown
//*****************************************************************************
//
//!	\brief	Handles mouse button events
//!	\param	[in] Identifier of the pressed mouse button
//!	\param	[in] State of the mouse button
//!	\param	[in] Current cursor x-position
//!	\param	[in] Current cursor y-position
//
//*****************************************************************************
void CApplication::OnMouseDown(int a_iButton, int a_iState, int a_iX, int a_iY)
{}

//*****************************************************************************
//	OnMouseMove
//*****************************************************************************
//
//!	\brief	Handles mouse movements
//!	\param	[in] Current cursor x-position
//!	\param	[in] Current cursor y-position
//
//*****************************************************************************
void CApplication::OnMouseMove(int a_iX, int a_iY)
{}

//*****************************************************************************
//	OnSpecialKeyDown
//*****************************************************************************
//
//!	\brief	Handles actions for "special" keys
//!	\param	[in] Identifier of the pressed key
//!	\param	[in] Current cursor x-position
//!	\param	[in] Current cursor y-position
//
//*****************************************************************************
void CApplication::OnSpecialKeyDown(int a_iKey, int a_iX, int a_iY)
{
	switch( a_iKey )
	{
		// Rotate camera left
	case GLUT_KEY_LEFT:
		m_fAngleY += 2.0f;
		Mod(m_fAngleY, 360.0f);
		break;

		// Rotate camera right
	case GLUT_KEY_RIGHT:
		m_fAngleY -= 2.0f;
		Mod(m_fAngleY, 360.0f);
		break;

		// Rotate camera up
	case GLUT_KEY_UP:
		m_fAngleX += 2.0f;
		m_fAngleX = Clamp(m_fAngleX, 0.0f, 90.0f);
		break;

		// Rotate camera down
	case GLUT_KEY_DOWN:
		m_fAngleX -= 2.0f;
		m_fAngleX = Clamp(m_fAngleX, 0.0f, 90.0f);
		break;

		// Zoom in
	case GLUT_KEY_PAGE_UP:
		m_fDistance -= 2.0f;
		m_fDistance = Max(m_fDistance, 3.0f);
		break;

		// Zoom Out
	case GLUT_KEY_PAGE_DOWN:
		m_fDistance += 2.0f;
		m_fDistance = Min(m_fDistance, 100.0f);
		break;

	default:
		return;
	}
	
	glutPostRedisplay();
}

//*****************************************************************************
//	OnIdle
//*****************************************************************************
//
//!	\brief	Idle processing
//
//*****************************************************************************
void CApplication::OnIdle()
{}

//*********************************************************************************************************************
//*********************************************************************************************************************
//	Public methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//*****************************************************************************
//	Instance
//*****************************************************************************
//
//!	\brief	Returns the one and only instance of CApplication
//!	\return	The application instance
//
//*****************************************************************************
CApplication& CApplication::Instance()
{
	if( NULL == m_pApplication )
		m_pApplication = new CApplication;

	return *m_pApplication;
}

//*****************************************************************************
//	Destroy
//*****************************************************************************
//
//!	\brief	Destroys the one and only instance of CApplication
//
//*****************************************************************************
void CApplication::Destroy()
{
	if( NULL == m_pApplication )
		return;

	delete m_pApplication;
}

//*****************************************************************************
//	Initializes
//*****************************************************************************
//
//!	\brief	Initializes the application
//!	\return	Returns either of the following values
//! \retval	true - Initialization succeeded
//! \retval	false - Initialization failed
//
//*****************************************************************************
bool CApplication::Initialize()
{
	// Initialize OpenGL

	std::cout << "Initializing OpenGL..." << std::endl;

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_FLAT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// Generate grid display list

	std::cout << "Generating grid display list..." << std::endl;

	const GLfloat fGridSize = 7.0f;

	m_uiListGrid = glGenLists(1);

	glNewList(m_uiListGrid, GL_COMPILE);
		glColor3f(0.6f, 0.6f, 0.6f);
		glBegin(GL_LINES);

			for( GLfloat fRow = -fGridSize; fRow <= fGridSize; fRow += 1.0f )
			{
				glVertex3f(-fGridSize, 0.0f, fRow);	
				glVertex3f(fGridSize, 0.0f, fRow);	
			}

			for( GLfloat fColumn = -fGridSize; fColumn <= fGridSize; fColumn += 1.0f )
			{
				glVertex3f(fColumn, 0.0f, -fGridSize);	
				glVertex3f(fColumn, 0.0f, fGridSize);	
			}

		glEnd();
	glEndList();

	// Generate axis display list

	std::cout << "Generating axis display list..." << std::endl;

	const GLfloat fAxisSize = 0.2f;
	const GLfloat fTopSize1 = 0.15f;
	const GLfloat fTopSize2 = 0.04f;

	m_uiListAxis = glGenLists(1);

	glNewList(m_uiListAxis, GL_COMPILE);
		glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(-fAxisSize,	0.0f,		0.0f);
			glVertex3f( fAxisSize,	0.0f,		0.0f);
			glVertex3f( fAxisSize,	0.0f,		0.0f);
			glVertex3f( fTopSize1,	fTopSize2,	0.0f);
			glVertex3f( fAxisSize,	0.0f,		0.0f);
			glVertex3f( fTopSize1,	-fTopSize2, 0.0f);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f( 0.0f,		fAxisSize,	0.0f);
			glVertex3f( 0.0f,		-fAxisSize,	0.0f);			
			glVertex3f( 0.0f,		fAxisSize,	0.0f);
			glVertex3f( fTopSize2,  fTopSize1,	0.0f);
			glVertex3f( 0.0f,		fAxisSize,	0.0f);
			glVertex3f( -fTopSize2,  fTopSize1, 0.0f);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f( 0.0f,	0.0f,		fAxisSize);
			glVertex3f( 0.0f,	0.0f,		-fAxisSize);
			glVertex3f( 0.0f,	0.0f,		fAxisSize);
			glVertex3f( 0.0f,	fTopSize2,	fTopSize1);
			glVertex3f( 0.0f,	0.0f,		fAxisSize);
			glVertex3f( 0.0f,	-fTopSize2, fTopSize1);
		glEnd();
	glEndList();

	// Generate node display list

	std::cout << "Generating node display list..." << std::endl;

	const GLfloat fNodeSize = 0.2f;

	m_uiListNode = glGenLists(1);

	glNewList(m_uiListNode, GL_COMPILE);
		glBegin(GL_LINE_STRIP);
			glColor3f(1.0f, 1.0f, 0.0f);
			glVertex3f( 0.0f,		-fNodeSize, 0.0f);
			glVertex3f(-fNodeSize,  0.0f,		-fNodeSize);
			glVertex3f( fNodeSize,  0.0f,		-fNodeSize);
			glVertex3f( 0.0f,		1.0f,		0.0f);
			glVertex3f(-fNodeSize,  0.0f,		-fNodeSize);
			glVertex3f(-fNodeSize,  0.0f,		fNodeSize);
			glVertex3f( 0.0f,		-fNodeSize,	0.0f);
			glVertex3f( fNodeSize,  0.0f,		-fNodeSize);
			glVertex3f( fNodeSize,  0.0f,		fNodeSize);
			glVertex3f( 0.0f,		-fNodeSize, 0.0f);
			glVertex3f(-fNodeSize,  0.0f,		fNodeSize);
			glVertex3f( 0.0f,		1.0f,		0.0f);
			glVertex3f( fNodeSize,  0.0f,		fNodeSize);
			glVertex3f(-fNodeSize,  0.0f,		fNodeSize);
		glEnd();
	glEndList();

	// Create quadric object

	std::cout << "Generating quadric object..." << std::endl;

	m_Quadric = gluNewQuadric();

	gluQuadricDrawStyle(m_Quadric, GLU_LINE);
	gluQuadricOrientation(m_Quadric, GLU_OUTSIDE);
	gluQuadricNormals(m_Quadric, GLU_NONE);
	gluQuadricTexture(m_Quadric, GL_FALSE);

	// Initialize font

	std::cout << "Generating font..." << std::endl;

	GLuint				uiFontTexture;
	AUX_RGBImageRec*	pFontTexture = auxDIBImageLoad("font.bmp");

	if( NULL == pFontTexture )
	{
		std::cerr << "Font texture couldn not be loaded!" << std::endl;
		return false;
	}

	glGenTextures(1, &uiFontTexture);
	glBindTexture(GL_TEXTURE_2D, uiFontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, pFontTexture->sizeX , pFontTexture->sizeY , 0, GL_RGB, GL_UNSIGNED_BYTE, pFontTexture->data);

	if( NULL != pFontTexture->data )
		free(pFontTexture->data);

	m_Font.SetTexture(uiFontTexture, 16, 16);
	m_Font.SetFontProperties(16, 16, 10);
	m_Font.SetDisplayMode(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	m_Font.BuildFont();

	// Initialize chain and solver

	std::cout << "Initializing IK chain and solver..." << std::endl;

	m_Root.SetTranslation(CVector3(0.0f, 0.0f, 0.0f));

	AddNode();
	AddNode();
	AddNode();
	AddNode();

	m_Target.SetPosition(CVector3(2.0f, 4.0f, 0.0f));

	m_pCurrentSolver->SetTargetPosition		(m_Target.GetPosition());
	m_pCurrentSolver->SetTargetThreshold	(m_Target.GetThreshold());
	m_pCurrentSolver->SetTryLimit			(100);
	m_pCurrentSolver->Initialize			(*m_pEffector, m_iLinkCount);

	return true;
}
