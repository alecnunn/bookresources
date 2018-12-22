#ifndef SAVEMGR_HPP
#define SAVEMGR_HPP

#include <stdio.h>		// for FILE* functions
#include <malloc.h>		// for malloc

//----------------------------------------------
// define ASSERT for the define challenged
//----------------------------------------------
#ifndef ASSERT
#define	ASSERT(A)
#endif

//----------------------------------------------
// forward references
//----------------------------------------------

class SAVEOBJ;
class SAVEMGR;

//----------------------------------------------
// the SAVERECORD object
//----------------------------------------------
typedef struct
{
	int		type;
	int		offset;
	int		offset2;
	int		length;

} SAVERECORD;

// the type defines
#define	SAVETYPE_END		(0)
#define	SAVETYPE_DATA		(1)
#define	SAVETYPE_PTR		(2)
#define	SAVETYPE_ALLOCMEM	(3)
#define	SAVETYPE_BASECLASS	(4)

// the SAVERECORD macros
#define SAVEDONE()							{	SAVETYPE_END,		0,								0,								0								}
#define	SAVEDATA(CLASS,DATANAME)			{	SAVETYPE_DATA,		((int)&((CLASS*)0)->DATANAME),	0,								sizeof(((CLASS*)0)->DATANAME)	}
#define	SAVEPTR(CLASS,PTRNAME)				{	SAVETYPE_PTR,		((int)&((CLASS*)0)->PTRNAME),	0,								sizeof(int)						}
#define SAVEALLOC(CLASS,SIZENAME,PTRNAME)	{	SAVETYPE_ALLOCMEM,	((int)&((CLASS*)0)->SIZENAME),	((int)&((CLASS*)0)->PTRNAME),	0								}
#define SAVEBASE(SAVETABLE)					{	SAVETYPE_BASECLASS,	((int)SAVETABLE),				0,								0								}

//----------------------------------------------
//	the SAVEOBJ class
//	all saveable objects are derived from this
//----------------------------------------------
class SAVEOBJ
{
public:
	virtual	int			GetSaveID() = 0;
	virtual	SAVERECORD	*GetSaveData() = 0;

	virtual	void		Save( SAVEMGR *psm );
	virtual	void		Load( SAVEMGR *psm );
	virtual	void		PostLoad( SAVEMGR *psm );
};

//----------------------------------------------
// the SAVEMGR class
//----------------------------------------------
class SAVEMGR
{
	friend class SAVEOBJ;
public:

	SAVEMGR();
	~SAVEMGR();

	void	AddSaveObject( SAVEOBJ *o );
	int		Save( const char *filename );
	int		Load( const char *filename );

protected:
	void	Clear();

	// this is a game-specific class-factory function
	SAVEOBJ	*MakeObject( int saveID );

	// this function writes the save records. It is called from SAVEOBJ::Save
	void	WriteData( SAVERECORD *list, void *base );

	// this function reads the save records. It is called from SAVEOBJ::Load
	void	ReadData( SAVERECORD *list, void *base );

	// this function corrects pointers in the save records. It is called from SAVEOBJ::PostLoad
	void	CorrectData( SAVERECORD *list, void *base );


	// these functions translate pointers using the saveObjList member
	int		DeReference( SAVEOBJ *po );
	SAVEOBJ	*ReReference( int id );

	int		nSaveObj;
	SAVEOBJ	**saveObjList;
	FILE	*saveFile;
};

#endif
