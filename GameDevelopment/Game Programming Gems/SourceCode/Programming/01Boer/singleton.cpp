/* Copyright (C) James Boer, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Boer, 2000"
 */
#include <iostream.h>
#include <assert.h>

class SingletonBase
{
public:
	SingletonBase()		
	{  cout << "SingletonBase created!" << endl;  }
	virtual ~SingletonBase()				
	{  cout << "SingletonBase destroyed!" << endl; }
	virtual void Access()	
	{  cout << "SingletonBase accessed!" << endl;  }
	static SingletonBase* GetObj()	
	{  return m_pObj;  }
	static void SetObj(SingletonBase* pObj)
	{  m_pObj = pObj;  }
protected:
	static SingletonBase* m_pObj;
};

SingletonBase* SingletonBase::m_pObj;

inline SingletonBase* Base()
{  
	assert(SingletonBase::GetObj());  
	return SingletonBase::GetObj();  
}

// Create a derived singleton-type class
class SingletonDerived : public SingletonBase
{
public:
	SingletonDerived()		
	{  cout << "SingletonDerived created!" << endl;  }
	virtual ~SingletonDerived()				
	{  cout << "SingletonDerived destroyed" << endl;  }
	virtual void Access()	
	{  cout << "SingletonDerived accessed!" << endl;  }
protected:
};

inline SingletonDerived* Derived()
{  
	assert(SingletonDerived::GetObj());
	return (SingletonDerived*)SingletonDerived::GetObj();
}

// Using the code...
// The complex singleton requires more work to use, but is
// more flexible.  It also allows more control over object
// creation, which is sometimes desirable.
SingletonDerived::SetObj(new SingletonDerived);

// Notice that the functionality has been overridden by the new
// class, even though accessing it through the original method.
Base()->Access();
Derived()->Access();

// This variation on a singleton unfortunately requires both
// explicit creation and deletion.
delete SingletonDerived::GetObj();

