/*
Copyright (c) 2001 by Justin Randall.  All rights reserved.
This copyright applies only to this particular software distribution 
package, and does not imply a copyright on any particular module in 
the library.

Permission to use, copy, modify, and distribute this compilation for
non-commercial use is hereby granted without fee, subject to 
the following license:

1. Any copy or modification of this compilation must include the above
copyright notice and this license.

2. Users of the software included in this compilation agree to use
their best efforts to provide Justin Randall with any modifications containing
improvements or extensions and hereby grant Justin Randall a perpetual,
royalty-free license to use and distribute such modifications under
the terms of this license.

3. Justin Randall makes no warranty or representation that the operation of the
software in this compilation will be error-free, and Justin Randall is under no
obligation to provide any services, by way of maintenance, update, or
otherwise.  THE SOFTWARE AND ANY DOCUMENTATION ARE PROVIDED 'AS IS'
WITHOUT EXPRESS OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. IN NO EVENT WILL JUSTIN RANDALL OR ANY OTHER CONTRIBUTOR BE LIABLE FOR
DIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

4. Users will not use Justin Randall or any other contributor's name in any 
publicity or advertising, without prior written consent in each case.

5. Export of this software from the United States may require a
specific license from the United States Government.  It is the
responsibility of any person or organization contemplating export
to obtain such a license before exporting.

6. Certain parts of this software may be protected by patents.  It
is the users' responsibility to obtain the appropriate
licenses before using those parts.
*/

#ifndef	_SmartPointer_H
#define	_SmartPointer_H

namespace SmartPointer {

template<typename ObjectType> 
class Subject;

//---------------------------------------------------------------------

template<class ObjectType>
class Observer
{
public:
	                    operator ObjectType *        ();
	                    operator const ObjectType *  () const;
	ObjectType *        getValue  ();
	const ObjectType *  getValue  () const;
	void                release                      ();

protected:
	friend class Subject<ObjectType>;

	          Observer   (Subject<ObjectType> * owner, ObjectType * object);
	          ~Observer  ();

	void      observe  ();
	void      onSubjectDestroyed  ();
private:
	ObjectType *          object;
	unsigned long         refCount;
	Subject<ObjectType> * subject;
};

//---------------------------------------------------------------------

template<typename ObjectType>
inline Observer<ObjectType>::Observer(Subject<ObjectType> * s, ObjectType * o) :
object(o),
refCount(1),
subject(s)
{
}

//---------------------------------------------------------------------

template<typename ObjectType>
inline Observer<ObjectType>::~Observer()
{
	if(subject)
		subject->view = 0;
}

//---------------------------------------------------------------------

template<typename ObjectType>
inline ObjectType * Observer<ObjectType>::getValue()
{
	return object;
}

//---------------------------------------------------------------------

template<typename ObjectType>
inline const ObjectType * Observer<ObjectType>::getValue() const
{
	return object;
}

//---------------------------------------------------------------------

template<typename ObjectType>
inline void Observer<ObjectType>::observe()
{
	refCount++;
}

//---------------------------------------------------------------------

template<typename ObjectType>
inline void Observer<ObjectType>::onSubjectDestroyed()
{
	subject = 0;
	object = 0;
	refCount--;
}

//---------------------------------------------------------------------
template<typename ObjectType>
inline void Observer<ObjectType>::release()
{
	if(refCount > 1)
		refCount--;
	else
		delete this;
}

//---------------------------------------------------------------------

template<typename ObjectType>
class Subject
{
public:
	Subject();
	~Subject();

	Observer<ObjectType> *  getTarget  ();

protected:
	friend ObjectType;
	friend class Observer<ObjectType>;

	void  observing  (ObjectType * target);

private:
	Observer<ObjectType> *  view;
};

//---------------------------------------------------------------------

template<typename ObjectType>
inline Subject<ObjectType>::Subject() :
view(0)
{
}

//---------------------------------------------------------------------

template<typename ObjectType>
inline Subject<ObjectType>::~Subject()
{
	if(view)
		view->onSubjectDestroyed();
}

//---------------------------------------------------------------------

template<typename ObjectType>
inline Observer<ObjectType> * Subject<ObjectType>::getTarget()
{
	if(view)
		view->observe();

	return view;
}

//---------------------------------------------------------------------

template<typename ObjectType>
inline void Subject<ObjectType>::observing(ObjectType * target)
{
	view = new Observer<ObjectType>(this, target);
}

//---------------------------------------------------------------------

}//namespace SmartPointer

#endif	// _SmartPointer_H
