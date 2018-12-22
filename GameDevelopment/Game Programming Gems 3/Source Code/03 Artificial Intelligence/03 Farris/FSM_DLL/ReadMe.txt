FSM_DLL ReadMe

Description:
This project compiles the CFSM classes into a Win32 Dynamic Link Library (DLL).  

Files:
	State.h		DLL Version of CState and CStateTemplate classes
	State.cpp
	FSM.h			DLL Version of CFSM class
	FSM.cpp
	FSM.dsp		Project File
	FSM.dsw		Project Workspace
	Main.h		DLL #defines
	Main.cpp		DLL Main Entry Point

Release Notes:
Template classes do not work well with DLLs due to the way DLLs handle function exporting. 
In order to make the CFSM classes work from a DLL, the CStateTemplate instance must be 
instantiated and exported.  The code fragment from FSM.h below handles this instantiation.
	
	FSM.h:

	// Forward Declarations
	class CFSM;

	// Template Instantiation
	#pragma warning( disable : 4231 )  
	extern template class FSM_API CStateTemplate<CFSM>;

For any class which uses the CStateTemplate class and is exported from a DLL, the developer 
must also instantiate and export the CStateTemplate instance for that class.  Only one 
instantiation is necessary for a given class.  For example,  class CGameObject uses the CFSM 
base class and is exported from a class called Game.dll.  In GameObject.h, the CStateTemplate 
instance is instantiated and exported.

	GameObject.h:

	// Forward Declarations
	class CGameObject;

	// Template Instantiation
	#pragma warning( disable : 4231 )  
	extern template class GAME_API CStateTemplate<CGameObject>;

	class GAME_API CGameObject : public CFSM
	{
	protected:
		CStateTemplateObject<CGameObject> m_StateX;
		CStateTemplateObject<CGameObject> m_StateY;
		CStateTemplateObject<CGameObject> m_StateZ;
	};
	
Classes in static libraries or executables don't require the instantiation and export of the 
CStateTemplate instance.
