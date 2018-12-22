#include "savemgr.hpp"

//--------------------------------------------------
//	SAVEOBJ base class functions
//--------------------------------------------------
void SAVEOBJ::Save( SAVEMGR *psm )
{
	psm->WriteData(GetSaveData(),this);
}

void SAVEOBJ::Load( SAVEMGR *psm )
{
	psm->ReadData(GetSaveData(),this);
}

void SAVEOBJ::PostLoad( SAVEMGR *psm )
{
	psm->CorrectData(GetSaveData(),this);
}

//--------------------------------------------------
//	SAVEMGR class
//--------------------------------------------------
SAVEMGR::SAVEMGR()
{
	nSaveObj = 0;
	saveObjList = 0;
	saveFile = 0;
}

SAVEMGR::~SAVEMGR()
{
	ASSERT(saveFile==0);
	Clear();
}

//--------------------------------------------------
//	clear the object list
//--------------------------------------------------
void SAVEMGR::Clear()
{
	ASSERT(saveFile==0);
	if( saveObjList!=0 )
	{
		nSaveObj = 0;
		free(saveObjList);
		saveObjList = 0;
	}
}

//--------------------------------------------------
//	add an object to be saved
//--------------------------------------------------
void SAVEMGR::AddSaveObject( SAVEOBJ *o )
{
	if( (nSaveObj&15)==0 )
		saveObjList = (SAVEOBJ**)realloc(saveObjList,(nSaveObj+16)*sizeof(saveObjList[0]));
	saveObjList[nSaveObj] = o;
	nSaveObj++;
}

//--------------------------------------------------
//	actually save the game
//--------------------------------------------------
int SAVEMGR::Save( const char *filename )
{
	int		i,id;

	ASSERT(saveFile==0);
	saveFile = fopen(filename,"wb");
	if( saveFile==0 )
		return -1;

	// TODO:
	//	write out header specific to game
	//	example information:
	//		level name,
	//		playing time,
	//		screen shot thumbnail


	// write out # objects to be saved
	fwrite(&nSaveObj,1,sizeof(nSaveObj),saveFile);

	// save each object
	for( i=0;i<nSaveObj;i++ )
	{
		// write out its SaveID
		id = saveObjList[i]->GetSaveID();
		fwrite(&id,1,sizeof(id),saveFile);

		// write out its data
		saveObjList[i]->Save(this);
	}

	// done!!
	fclose(saveFile);
	saveFile = 0;

	// free list of objects
	Clear();
	return 0;
}

//--------------------------------------------------
//	actually load the game
//--------------------------------------------------
int SAVEMGR::Load( const char *filename )
{
	int		i,nObj,id;
	SAVEOBJ	*obj;

	ASSERT(saveFile==0);

	// free list of objects
	Clear();
	saveFile = fopen(filename,"rb");
	if( saveFile==0 )
		return -1;

	// TODO:
	//	add game-specific header read

	// read in # objects
	fread(&nObj,1,sizeof(nObj),saveFile);

	for( i=0;i<nObj;i++ )
	{
		// read in its SaveID
		fread(&id,1,sizeof(id),saveFile);

		// make the object
		obj = MakeObject(id);
		ASSERT(obj!=0);

		// add it to the list for dereferencing pointers
		AddSaveObject(obj);

		// load the object
		obj->Load(this);
	}

	// now post-load each object
	for( i=0;i<nObj;i++ )
		saveObjList[i]->PostLoad(this);

	// done!!
	fclose(saveFile);
	saveFile = 0;

	// free list of objects
	Clear();
	return 0;
}

//---------------------------------------------------------------------
//	these functions form the core of the save manager
//	we define the SAVEDATAPTR union to avoid lots of casts
//---------------------------------------------------------------------
typedef union
{
	int				value;
	int				*pi;
	unsigned char	**ppuc;
	SAVEOBJ			**ppso;
	SAVERECORD		*ptable;

} SAVEDATAPTR;

//---------------------------------------------------------------------
// this function writes the save records. It is called from SAVEOBJ::Save
//---------------------------------------------------------------------
void	SAVEMGR::WriteData( SAVERECORD *list, void *base )
{
	SAVEDATAPTR		src;
	SAVEDATAPTR		src2;
	int				saveID;

	while( list->type!=SAVETYPE_END )
	{
		src.value = ((int)base) + list->offset;
		src2.value = ((int)base) + list->offset2;

		switch( list->type )
		{
		case SAVETYPE_DATA:
			fwrite(src.pi,list->length,1,saveFile);
			break;
		case SAVETYPE_PTR:
			saveID = DeReference(src.ppso[0]);
			fwrite(&saveID,sizeof(saveID),1,saveFile);
			break;
		case SAVETYPE_ALLOCMEM:
			// write length of allocated data
			fwrite(src.pi,sizeof(src.pi[0]),1,saveFile);
			if( src.pi[0]!=0 )
			{
				// write out allocated data itself
				fwrite(src2.ppuc[0],src.pi[0],1,saveFile);
			}
			break;
		case SAVETYPE_BASECLASS:
			// call with same base, but ptr to savetable of base class
			WriteData(src.ptable,base);
			break;
		}
		list++;
	}
}

//---------------------------------------------------------------------
// this function reads the save records. It is called from SAVEOBJ::Load
//---------------------------------------------------------------------
void	SAVEMGR::ReadData( SAVERECORD *list, void *base )
{
	SAVEDATAPTR		src;
	SAVEDATAPTR		src2;

	while( list->type!=SAVETYPE_END )
	{
		src.value = ((int)base) + list->offset;
		src2.value = ((int)base) + list->offset2;

		switch( list->type )
		{
		case SAVETYPE_DATA:
			fread(src.pi,list->length,1,saveFile);
			break;
		case SAVETYPE_PTR:
			// store the index into the object array
			fread(src.pi,sizeof(src.pi[0]),1,saveFile);
			break;													
		case SAVETYPE_ALLOCMEM:										
			// read length of allocated data
			fread(src.pi,sizeof(src.pi[0]),1,saveFile);	
			// is there any data to read???
			if( src.pi[0]!=0 )
			{
				// allocate the required space
				src2.ppuc[0] = (unsigned char*)malloc(src.pi[0]);
				// read back allocated data
				fread(src2.ppuc[0],src.pi[0],1,saveFile);
			}
			break;
		case SAVETYPE_BASECLASS:
			// call with same base, but ptr to savetable of base class
			ReadData(src.ptable,base);
			break;
		}
		list++;
	}
}

//---------------------------------------------------------------------
// this function corrects pointers in the save records. It is called from SAVEOBJ::PostLoad
//---------------------------------------------------------------------
void	SAVEMGR::CorrectData( SAVERECORD *list, void *base )
{
	SAVEDATAPTR		src;
	while( list->type!=SAVETYPE_END )
	{
		src.value = ((int)base) + list->offset;
		switch( list->type )
		{
		case SAVETYPE_PTR:		// translate the index back into a pointer
			src.ppso[0] = ReReference(src.pi[0]);
			break;
		}
		list++;
	}
}


//---------------------------------------------------------------------
//	these functions translate pointers using the saveObjList member
//---------------------------------------------------------------------
int		SAVEMGR::DeReference( SAVEOBJ *po )
{
	int		i;
	if( po==0 )
		return 0;
	for( i=0;i<nSaveObj;i++ )
	{
		if( saveObjList[i]==po )
			return 1+i;
	}
	ASSERT(0);		// this object is not in the save list!!!
	return 0;
}

SAVEOBJ	*SAVEMGR::ReReference( int id )
{
	if( id==0 )		return 0;
	ASSERT(id>0 && id<=nSaveObj);
	return saveObjList[id-1];
}

//---------------------------------------------------------------------
//	this is a game-specific class-factory function
//	you need to instantiate an empty class of type saveID
//	ready to receive its load() call
//---------------------------------------------------------------------
//	this should be defined by each game...
//	so it is commented out here...
//---------------------------------------------------------------------
#if 0
SAVEOBJ	*SAVEMGR::MakeObject( int saveID )
{	// instantiate a class based upon saveID
	return 0;
}
#endif
