// +-----------------------------------------------------------------------
// |
// |                         File Name: GPthreadObject.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPthreadObject Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPTHREADOBJECT_H
#define GPTHREADOBJECT_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPdefs.h>
#include <windows.h>

// +---------------------------
// |	Forward Declarations
// +---------------------------
class GPthread;

// +-----------------------------------------------------------------------
// |
// |  Class       : GPthreadObject
// |  Description : 
// |								This class provides an easy way to implement a Thread Object
// |								with its own process loop. It also provides the methods
// |								to control it
// |						
// |								The user of GPthreadObject only have to implement the method:
// |												virtual void Loop() = 0;									
// |								which should contain the code to be execute in each loop. The loop
// |								control is already implemented in the method ProcessLoop.
// |								
// +-----------------------------------------------------------------------
class _GP GPthreadObject
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPthreadObject(); 
		virtual ~GPthreadObject();
  
		// +---------------------------
		// |	GPthread
		// +---------------------------
		GPthread* GetThread() const;

		// +---------------------------
		// |	Priority
		// +---------------------------
		void SetPriority(int priority);
		int  GetPriority() const ;

		// +---------------------------
		// |	Control
		// +---------------------------
		void Start();
		void Pause();
		void Finish();
		bool IsFinishing() const;
		void ProcessLoop();
		virtual void Init();
		virtual void End();
	
		// +--------------------------------------------------
		// |	Virtual Method to Implement by a derived class
		// +--------------------------------------------------
		virtual void Loop() = 0;

	protected: 
		// +---------------------------
		// |	GPthread Function
		// +---------------------------
		static DWORD WINAPI GPthreadObjectFuncPtr(void *pArg);
  	GPthread*     thread_;

		// +---------------------------
		// |	Control
		// +---------------------------
		bool isFinishing_;
		bool exitFlag_;

	private:
		// +---------------------------
		// |	Copy Constructor
		// +---------------------------
		GPthreadObject(const GPthreadObject& ref); //Not Implemented
		
};

#endif //GPTHREADOBJECT_H