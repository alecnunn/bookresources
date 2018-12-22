#ifndef GPL_OBJECT_H
#define GPL_OBJECT_H

/******************************************************************************

Coral Tree gpl/object.h
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

#include <gpl/factory.h>

/***********************************************************************
Should this declare the virtual functions as pure?
***********************************************************************/
#define GPL_OBJECT_ABSTRACT_INTERFACE(className)	public:	\
	virtual GPL_ClassId GetClassId(void) const { return 0; }	\
	virtual const char * GetClassName(void) const { return (const char *) 0; }	\
	virtual GPL_Object * NewInstance(void) const { return (GPL_Object *) 0; }	\
	virtual GPL_Object * Clone(void) const { return (GPL_Object *) 0; }

#define GPL_OBJECT_INTERFACE(className)	public:	\
	virtual GPL_ClassId GetClassId(void) const;	\
	static GPL_ClassId GetClassIdStatic(void);	\
	virtual const char * GetClassName(void) const;	\
	static const char * GetClassNameStatic(void);	\
	virtual GPL_Object * NewInstance(void) const;	\
	virtual GPL_Object * Clone(void) const

/***********************************************************************
If string concatenation via ## fails because some preprocessor does
not recoginze its syntax, try the name2 macro, e.g., change
'GPL_ObjInit##className' to 'name2(GPL_ObjInit,className)'.
***********************************************************************/
#define GPL_ObjectInit(className)		GPL_ObjInit##className
#define GPL_ObjectCreator(className)	gplObjCreat##className
#define GPL_ObjectDummy(className)		gplObjDummy##className

#define GPL_OBJECT_IMPLEMENTATION(className, id, name)	\
	GPL_ClassId className::GetClassId(void) const { return id; }	\
	GPL_ClassId className::GetClassIdStatic(void) { return id; }	\
	const char * className::GetClassName(void) const { return name; }	\
	const char * className::GetClassNameStatic(void) { return name; }	\
	GPL_Object * className::NewInstance(void) const { return (GPL_Object *) new className; }	\
	GPL_Object * className::Clone(void) const { return (GPL_Object *) new className (*this); }	\
	GPL_Object * GPL_ObjectCreator(className)(void) { return (GPL_Object *) new className; }	\
	struct GPL_ObjectInit(className)\
	{	\
		GPL_ObjectInit(className)(void);	\
		~GPL_ObjectInit(className)(void);	\
	};	\
	GPL_ObjectInit(className)::GPL_ObjectInit(className)(void)	\
	{	\
		GPL_ObjectFactory::Instance()->Add(name, id, GPL_ObjectCreator(className));	\
	}	\
	GPL_ObjectInit(className)::~GPL_ObjectInit(className)(void) {	\
		GPL_ObjectFactory::Instance()->Remove(name);	\
	}	\
	GPL_ObjectInit(className) GPL_ObjectDummy(className)

/***********************************************************************
class GPL_Object

Base object for all objects.
***********************************************************************/
class GPL_Object {

	public:
		virtual GPL_ClassId GetClassId(void) const
		{
			return 0;
		}
		virtual const char * GetClassName(void) const
		{
			return (char *) GPL_BAD_ADDRESS;
		}

		virtual GPL_Object * NewInstance(void) const
		{
			return (GPL_Object *) GPL_BAD_ADDRESS;
		}
		virtual GPL_Object * Clone(void) const
		{
			return (GPL_Object *) GPL_BAD_ADDRESS;
		}
};

#endif /* GPL_OBJECT_H */
