
#ifndef REFCOUNTED_H_
#define REFCOUNTED_H_


class RefCounted
{
public:
	RefCounted ();
	virtual ~RefCounted ();

	virtual int GetRefCount () const;
	virtual int AddRef ();
	virtual int Release ();

protected:
	int m_refCount;
};

#endif
