// defines


#ifndef _atombase_defines_
#define _atombase_defines_

#ifndef interface
#define interface struct
#endif

//#define _countof(arr) (sizeof(arr) / sizeof(arr[0]))

//#define ATOM_CREATE(cls)  (atomos::I##cls*)g_pEngine->CreateObject(C##cls::CID);
#define ATOM_CREATE(cls)  (atomos::I##cls*)g_pEngine->CreateObject(CID_##cls);

#define ATOM_DELETE(x)	{if(x){delete x;x=NULL;}}
#define ATOM_RELEASE(x) {if(x!=NULL){x->Release();x=NULL;}}
#define ATOM_FREE(x) {if(x!=NULL){free((void*)x);x=NULL;}}


#define ATOM_UNIPOLAR	0
#define ATOM_BIPOLAR	1

#define ATOM_RDONLY       0x0000  /* open for reading only */
#define ATOM_WRONLY       0x0001  /* open for writing only */
#define ATOM_RDWR         0x0002  /* open for reading and writing */
#define ATOM_APPEND       0x0008  /* writes done at eof */

#define ATOM_CREAT        0x0100  /* create and open file */
#define ATOM_TRUNC        0x0200  /* open and truncate */
#define ATOM_EXCL         0x0400  /* open only if file doesn't already exist */


#define ATOM_TEXT         0x4000  /* file mode is text (translated) */
#define ATOM_BINARY       0x8000  /* file mode is binary (untranslated) */

#define ATOM_SEEK_SET        0               /* seek to an absolute position */
#define ATOM_SEEK_CUR        1               /* seek relative to current position */
#define ATOM_SEEK_END        2               /* seek relative to end of file */

#define ATOM_IREAD        0000400         /* read permission, owner */
#define ATOM_IWRITE       0000200         /* write permission, owner */

// simple types
#define CID_NONE	0
#define CID_INT8	1
#define CID_UINT8	2
#define CID_INT16	3
#define CID_UINT16	4
#define CID_INT32	5
#define CID_UINT32	6
#define CID_INT64	7
#define CID_UINT64	8
#define CID_FLOAT32	9
#define CID_FLOAT64	10
#define CID_POBJECT	11
#define CID_CHAR8	12
#define CID_CHAR16	13





#define MID_ATOMBASE		256

typedef enum eATOMBASE_CID{
	
	CID_BaseSystem=MID_ATOMBASE,
	CID_AttributeDescription,
	CID_System,
	CID_Systems,
	CID_Object,
	CID_Objects,
	CID_NamedObject,
	CID_Pin,
	CID_Pins,
	CID_Connection,
	CID_Connections,
	CID_Stream,
	CID_Engine,
	CID_Class,
	CID_Classes,
	CID_Node,
	CID_Nodes,
	CID_Graph,
	CID_PlatformObject,
	CID_Processor,
	CID_Processors,
	CID_Application,
	CID_Keyboard,
	CID_Mouse,
	CID_Window,
	CID_Renderer,
	CID_Image,
	CID_Shape,
	CID_Shapes,
	CID_Integers,
	CID_Floats,
	CID_Console,
	CID_Color,
	CID_Tuple4f,
	CID_Buffer,
	CID_CHAR8Scalar,
	CID_CHAR8Vector,
	CID_UINT32Scalar,
	CID_UINT8Vector,
	CID_INT16Vector,
	CID_UINT16Vector,
	CID_UINT32Vector,
	CID_FLOAT32Scalar,
	CID_FLOAT32Vector,
	CID_SignalGraph,
	CID_Signal,
	CID_Signals,
	CID_File,
	CID_Scalar,
	CID_GraphIterator,
	CID_String8,
	CID_Strings8,
	CID_FileLister,
	CID_Drawing,

	CID_DataLocation,
	CID_DataLocations,

} ATOMBASE_CID;

typedef void* (__stdcall * ATOM_CALL_CREATEOB)();

#define ATOM_IMPLEMENT_CLASS(cls, basecls)\
void* __stdcall Create##cls() { return( new cls); }\
	const CClass cls::class##cls(cls::CID,basecls::CID, #cls, Create##cls);\
const IClass*	cls##::GetClass() { return((const IClass*)&class##cls); }



// struct for operations on any external stored handle
typedef struct tagATOM_HANDLE
{
  tagATOM_HANDLE():pData(NULL){}

  template<typename T> tagATOM_HANDLE(const T& v)
   : pData ( &v ) {}

  template<typename T> operator T () const
  { return * reinterpret_cast<const T*>(pData); }

  const void* pData ;
} ATOM_HANDLE;



#define CMD_UPDATE	124
#define CMD_STEP	125




#endif // _atombase_defines_