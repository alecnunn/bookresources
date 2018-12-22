#pragma once
#ifndef BASE_CLASS_HPP
#define BASE_CLASS_HPP

//************************************************************
class CBaseClass
{
public:
	CBaseClass(void) { };
	virtual ~CBaseClass(void) {};
};

//************************************************************
class CBaseClassNonCopyable : public CBaseClass
{
protected:
	CBaseClassNonCopyable(){}
	virtual ~CBaseClassNonCopyable(){}
private:  // emphasize the following members are private
	CBaseClassNonCopyable( const CBaseClassNonCopyable& );
	const CBaseClassNonCopyable& operator=( const CBaseClassNonCopyable& );


};

//************************************************************
//The base class from which all major objects should be inheritted.
//Provides interface for -
//- Intelligent Pointers
//- Validation
#include <list>

class CBaseObject : public CBaseClass
{
friend class CBaseIntelligentPointer;
private:
	mutable std::list<CBaseIntelligentPointer*> IntelligentPointers;

protected:
	CBaseObject(void);
	~CBaseObject(void);

	void	Clear(void);

public:
	void	AddIntelligentPointer(CBaseIntelligentPointer *p) const;
	void	RemoveIntelligentPointer(CBaseIntelligentPointer *p) const;
	
	virtual void	Validate(void) const {};
};




#endif