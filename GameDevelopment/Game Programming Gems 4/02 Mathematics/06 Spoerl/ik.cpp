#include	"glut/glut.h"
#include	"app/CApplication.h"
#include	<iostream>

//*****************************************************************************
//	OnExit
//*****************************************************************************
//
//!	\brief	Function called to destroy the Singleton instance before the application exits
//
//*****************************************************************************
void __cdecl OnExit()
{
	CApplication::Destroy();
}

//*****************************************************************************
//	main
//*****************************************************************************
//
//!	\brief	The appplication's entry point
//!	\param	[in] Number of command line arguments
//!	\param	[in] List of command line arguments
//!	\return	The application's exit code
//
//*****************************************************************************
int main(int argc, char* argv[])
{
	// Register ANSI C exit function

	atexit(OnExit);

	// Init GLUT

	std::cout << "Starting..." << std::endl;
	std::cout << "Initializing GLUT..." << std::endl;

	glutInit				(&argc, argv);
	glutInitWindowSize		(640, 480);
	glutInitWindowPosition	(0, 0);
	glutInitDisplayMode		(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// Create render window

	glutCreateWindow		("Inverse Kinematics");

	// Register callbacks

	glutDisplayFunc			(CApplication::Display);
	glutReshapeFunc			(CApplication::Reshape);
	glutKeyboardFunc		(CApplication::Keyboard);
//	glutMouseFunc			(CApplication::Mouse);
//	glutMotionFunc			(CApplication::Motion);
	glutSpecialFunc			(CApplication::Special);
//	glutIdleFunc			(CApplication::Idle);

	// Init application and OpenGL

	std::cout << "Initializing application..." << std::endl;

	if( false == CApplication::Instance().Initialize() )
	{
		std::cerr << "Application could not be initialized!" << std::endl;
		return 0;
	}

	// Enter main loop

	std::cout << "Entering GLUT main loop..." << std::endl;

	glutMainLoop();

	return 0;
}
