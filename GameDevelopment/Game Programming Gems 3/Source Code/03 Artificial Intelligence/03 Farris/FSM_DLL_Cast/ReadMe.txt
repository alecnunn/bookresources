FSM_DLL_Cast ReadMe

Description:
This project compiles an alternative version of the CFSM classes into a Win32 Dynamic Link 
Library (DLL).  The cast version of CFSM does not store the class instance with the 
CStateTemplate instance.  Instead, the class instance is passed to the execute function in
CState as a parameter and then cast (using static_cast) to the appropriate type before it is
used to execute the function pointers.  This modification is a memory optimization which
removes 4 bytes from each CStateTemplate instance.  In addition, CStateTemplate instances can
now be made static, another major memory optimization.  For example, consider a class called
game object which uses the cast version of CFSM:

	GameObject.h:

	class CGameObject : public CFSM
	{
	protected:
		static CStateTemplate<CGameObject> m_StateX;

		virtual void BeginStateX() {}
		virtual void StateX() {}
		virtual void EndStateX() {}
	};

	GameObject.cpp

	// Local Includes
	#include "GameObject.h"

	// Static Initializations
	CStateTemplate<CGameObject> CGameObject::m_StateX(BeginStateX,StateX,EndStateX);

	CGameObject::CGameObject() : public CFSM
	{
	}

By declaring m_StateX as static, all instances of CGameObject will now share a single instance 
of the state object.  For games with a large number of CGameObject instances, this can result
in a substantial memory usage reduction. 

Files:
	State.h			DLL Version of CState and CStateTemplate classes (cast version)
	State.cpp
	FSM.h				DLL Version of CFSM class (cast version)
	FSM.cpp
	FSM.dsp			Project File
	FSM.dsw			Project Workspace
	Main.h			DLL #defines
	Main.cpp			DLL Main Entry Point

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
