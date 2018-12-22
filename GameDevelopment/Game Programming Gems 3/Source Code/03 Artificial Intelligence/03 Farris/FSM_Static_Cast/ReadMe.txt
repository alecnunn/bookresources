FSM_Static_Cast ReadMe

Description:
This project compiles an alternative version of the CFSM classes into a static library.  The 
cast version of CFSM does not store the class instance with the CStateTemplate instance.  
Instead, the class instance is passed to the execute function in CState as a parameter and then 
cast (using static_cast) to the appropriate type before it is used to execute the function 
pointers.  This modification is a memory optimization which removes 4 bytes from each 
CStateTemplate instance.  In addition, CStateTemplate instances can now be made static, another 
major memory optimization.  For example, consider a class called game object which uses the 
cast version of CFSM:

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
	State.h			CState and CStateTemplate classes (cast version)
	State.cpp
	FSM.h				CFSM class (cast version)
	FSM.cpp
	FSM.dsp			Project File
	FSM.dsw			Project Workspace

Release Notes:
None