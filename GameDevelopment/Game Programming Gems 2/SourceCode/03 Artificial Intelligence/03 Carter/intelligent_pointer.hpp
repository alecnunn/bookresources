#pragma once
#ifndef INTELLIGENT_POINTER_HPP__
#define INTELLIGENT_POINTER_HPP__


#include "base_class.hpp"

//********************************************************
#pragma pack(1)

class CBaseIntelligentPointer
{
friend class CBaseObject;
protected:
	virtual void ClearItem(void) = 0;


};						   

//********************************************************
template<class T> class CIntelligentPointer : public CBaseIntelligentPointer
{
private:
	T* Item;
	
private: //Functions
	T*	GetPointer(void) const
	{
		return(Item);
	}

	void	SetPointer(T *item)
	{
		if(Item!=NULL)
		{
			Item->RemoveIntelligentPointer(this);
			Item = NULL;
		}

		if(item!=NULL)
		{
			Item = item;
			Item->AddIntelligentPointer(this);
		}
		else
		{
			Item = NULL;
		}
	}
	
	void ClearItem(void)
	{
		Item = NULL;
	}

public:
	CIntelligentPointer(void)
	{
		Item = NULL;
	};

	CIntelligentPointer(T *item)
	{
		Item = NULL;
		SetPointer(item);
	};

	//This constructor allows us to convert from a non const pointer to a const pointer
	template<class U>
	CIntelligentPointer(const CIntelligentPointer<U> &item)
	{
		Item = NULL;
		SetPointer(item.Get());
	}

	template<class U>
	CIntelligentPointer(CIntelligentPointer<U> &item)
	{
		Item = NULL;
		SetPointer(item.Get());
	}

	//We need this one, because for some reason the one above is ignored for pointers of the same type
	CIntelligentPointer(const CIntelligentPointer<T> &item)
	{
		Item = NULL;
		SetPointer(item.GetPointer());
	}

	~CIntelligentPointer(void)
	{
		SetPointer(NULL);
	};
	
	T*	Get(void) const
	{
		return(GetPointer());
	}


	
	CIntelligentPointer<T> &operator=(T *item)
	{
		SetPointer(item);
		return(*this);
	}



	
	
	//This operator allows us to convert from a non const pointer to a const pointer
	template<class U>
	CIntelligentPointer<T> &operator=(const CIntelligentPointer<U> &item)
	{
		SetPointer(static_cast<U*>(item));
		return(*this);
	}

	//We need this one, because for some reason the one above is ignored for pointers of the same type
	CIntelligentPointer<T> &operator=(const CIntelligentPointer<T> &item)
	{
		SetPointer(item.GetPointer());
		return(*this);
	}

	int operator==(const T *p) const
	{
		return(p==Item);
	}

	int operator!=(const T *item) const
	{
		return(!operator==(item));
	}
	
	T* operator->(void) const
	{
		//LIB_ASSERT_MSG(Item!=NULL, "Dereferencing a null");
		return(GetPointer());
	};

	T& operator* (void) const
	{
		return(*GetPointer());
	};

	bool IsNull(void) const { return(Item==NULL); };
};

#pragma pack()

#endif //__LIB_INTELLIGENT_POINTER_HPP__