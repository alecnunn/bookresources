// -----------------------------------------------------------------
// CZFXModel
// -----------------------------------------------------------------
// http://www.zfx.info                       c. by Oliver Düvel 2003
// -----------------------------------------------------------------

// -----------------------------------------------------------------
// Include
// -----------------------------------------------------------------
#include "include/CZFXModel.h"                        // Model Header
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: Init( void )
// Info: Initalization
// -----------------------------------------------------------------
void CZFXModel::Init( void )
{
   // Settings
   m_pFile              = NULL;                       // File
   m_pVertices          = NULL;                       // Vertices
   m_pVertices_Orig     = NULL;                       // Vertices
   m_pFaces             = NULL;                       // Faces
   m_pMeshs             = NULL;                       // Meshs
   m_pMaterials         = NULL;                       // Materials
   m_ppIndices          = NULL;                       // Indices
   m_pIndices           = NULL;                       // Indices
   m_pAnimations        = NULL;                       // Animations
   m_pRenderDevice      = NULL;                       // Renderdevice
   m_uiStartFaceID      = 0;                          // Offset Faces
   m_uiStartVertexID    = 0;                          // Offset Vertices
   strcpy( m_cLogFileName, "MODELLOG.TXT" );          // Logfilename
   m_bLog               = true;                       // Logging on
   m_uiLogLevel         = 10;                         // Loglevel
   m_fTime              = 0.0;                        // Time
   m_fStartTime         = -1.0f;                      // Time
   m_uiCurrentAnimation	= 8;                          // Animation   
   m_bAnimationComplete = false;                      // Animationcompleted
   m_bAnimationRunOnce  = true;                       // Singleanimation
   m_bRenderBones       = false;                      // Renderbones
   m_bRenderNormals     = false;                      // Normalvectoren
   m_fAnimationSpeed    = 2048;                       // Animationsspeed
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: Destroy( void )
// Info: Shutdownsequence
// -----------------------------------------------------------------
void CZFXModel::Destroy( void )
{ 
	// Variablen init
	WORD          wLoop	   = 0;                        // Counter
	LPJOINT       pJoint   = NULL;                     // TempJoint
	PVOID         pIndices = NULL;                     // TempIndices

	// close file
	if( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = NULL;
	}

	// Vertices
	if( m_pVertices )
	{
		delete [] m_pVertices;
		m_pVertices = NULL;
	}

	// Vertices
	if( m_pVertices_Orig )
	{
		delete [] m_pVertices_Orig;
		m_pVertices_Orig = NULL;
	}

	// Indices
	if( m_pIndices )
	{
		delete [] m_pIndices;
		m_pIndices = NULL;
	}

	// Indexbuffer
	for( wLoop = 0; wLoop < m_sHeader.uiNumMaterials; wLoop++ )
	{
		pIndices = m_ppIndices[ wLoop ];
		delete [] pIndices;
		pIndices = NULL;
	}

	// Faces
	if( m_pFaces )
	{
		delete [] m_pFaces;
		m_pFaces = NULL;
	}

	// Meshs
	if( m_pMeshs )
	{
		delete [] m_pMeshs;
		m_pMeshs = NULL;
	}

	// Joints have some more data
	if( m_pJoints )
	{
		// get the single Joint
		for( wLoop = 0; wLoop < m_sHeader.uiNumJoints; wLoop++ )
		{
			// get the joint
			pJoint = &m_pJoints[ wLoop ];

			// the KF Rotations
			if( pJoint->wNumKF_Rotation > 0 )
				delete [] pJoint->pKF_Rotation;

			// the KF Positions
			if( pJoint->wNumKF_Position > 0 )
				delete [] pJoint->pKF_Position;
		}

		// delete the joints
		delete [] m_pJoints;
	}

	// Materials
	if( m_pMaterials )
	{
		delete [] m_pMaterials;
		m_pMaterials = NULL;
	}

	// Animations
	if( m_pAnimations )
	{
		delete [] m_pAnimations;
		m_pAnimations = NULL;
	}

	// Filename
	if( m_pcFileName )
	{
		delete [] m_pcFileName;
		m_pcFileName = NULL;
	}
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: CZFXModel( const PCHAR pcName, ZFXRenderDevice *pDevice, float fSize  )
// Info: Construktor
// -----------------------------------------------------------------
CZFXModel::CZFXModel( const PCHAR pcName, ZFXRenderDevice *pDevice, float fSize )
{
   // Initalization
   Init();

   // set device
   m_pRenderDevice = pDevice;

   // load the model
   if( !Execute( pcName, fSize ) )
   {
	   LOG( 1, true, "ERROR: Execute() faild for '%s'", pcName );
	   return;
   }
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: ~CZFXModel( )
// Info: Destructor
// -----------------------------------------------------------------
CZFXModel::~CZFXModel(){ Destroy(); }
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: LOG( UINT iLevel, bool bCR, PCHAR pcText, ... )
// Info: writes a line into the logfile
//
// iLevel    = (UINT)   our current loglevel
// bCR       = (bool)   Carriage Return wanted?
// pcText    =	        text to write
// ...       =          variable argumentelist
// -----------------------------------------------------------------
void CZFXModel::LOG( UINT iLevel, bool bCR, PCHAR pcText, ... )
{
   // Variablen init
   va_list  Argumente;                                // Arguments
   char     cBuffer[4096];                            // linebuffer
   FILE     *pLog;                                    // file

   // do logging?
   if( !m_bLog ) return;

   // correct loglevel?
   if( iLevel <= m_uiLogLevel )
	{
       // something to log?
       if( pcText )
		{
           // set the pointer to the beginning
           va_start( Argumente, pcText );

           // copy all data to cBuffer
           vsprintf( cBuffer, pcText, Argumente );

           // reset the list
           va_end( Argumente );
		}

        // open the logfile
       pLog = fopen( m_cLogFileName, "a" );

        // file opened?
        if( !pLog ) return;

        // write the wanted text into the logfile
        if( bCR )
            fprintf( pLog, "%s\n", cBuffer );
        else
            fprintf( pLog, "%s", cBuffer );

        // file close
        fclose( pLog );
	}
}
//------------------------------------------------------------------


// -----------------------------------------------------------------
// Name: CloseFile( bool rBool )
// Info: closes the file
//
// Return     = (bool)  true == OK / false == nIO
//
// rBool      = (bool)  return
// -----------------------------------------------------------------
bool CZFXModel::CloseFile( bool rBool )
{
	// close file
	if( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = NULL;
	}

   // bye
   return rBool;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: GetFileName( PCHAR pcName, PCHAR pcModelName )
// Info: extracts the filename from the whole path
//
// pcName				= (PCHAR) Pfadname
// pcModelName			= (PCHAR) Dateiname
// -----------------------------------------------------------------
void CZFXModel::GetFileName( PCHAR pcName, PCHAR pcModelName )
{
   // Variablen init
   char *Seperatoren   = { " {;,/\\." };              // seperators
   char cName[ 64 ];                                  // filename
   char cTypus[ 6 ];                                  // Typ / Ending
   int  iLength_A;                                    // Temp
	
   // convert to upper letters
   strupr( pcName );
	
   // reverse the whole name
   strrev( pcName );
	
   // find the ending
   strcpy( cTypus, strtok( pcName, Seperatoren ) );
	
   // attach the point for the ending
   strcat( cTypus, "." );
	
   // get the next part from the point to the next seperator. wich is our
   // wanted filename
   strrev( strcpy( cName, strtok( NULL, Seperatoren ) ) );
   iLength_A = (int) strlen( cName );                 // Länge ermitteln
   strcpy( pcModelName, cName );                      // ins Objekt kopieren
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: Execute( const PCHAR pcName, float fSize )
// Info: Executes the PlugIn
//
// Return       = (bool)      true == OK / false == nIO
//
// pcName       = (PCHAR)     filename
// fSize        = (float)     Size of the model
// -----------------------------------------------------------------
bool CZFXModel::Execute( const PCHAR pcName, float fSize )
{
   // copy the filename for further use
   m_pcFileName = new char[ strlen( pcName ) + 1 ];
   strcpy( m_pcFileName, pcName );

	// open the file
   m_pFile = fopen( pcName, "rb" );
   if( !m_pFile ) return false;

   // read the cunks...
   CheckForChunks();

   // scale the model
   SetScaling( fSize );

   // prepare the model
   Prepare();

   // bye
   return CloseFile( true );
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: GetNextChunk( CHUNK_S &pChunk )
// Info: Reads the the next ChunkID
//
// Return       = (WORD)      next chunk id
//
// pChunk       = (CHUNK_S&)  Pointer to Chunk
// -----------------------------------------------------------------
WORD CZFXModel::GetNextChunk( CHUNK_S &pChunk )
{
   // read next chunk
   fread( &pChunk, sizeof( CHUNK_S ), 1, m_pFile );
	
   // bye with Identifier
   return pChunk.wIdentifier;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: CheckForChunks( void )
// Info: Checks for Chunks
//
// Return       = (HRESULT)   Status
// -----------------------------------------------------------------
HRESULT CZFXModel::CheckForChunks( void )
{
   // Variablen init
   bool   bLoop  = true;                              // LoopFlag

   // loop until dawn...
   do{
      // find the next chunk
      switch( GetNextChunk( m_sChunk ) )
		{
        case V1_HEADER:       ReadHeader();       break;// read Header
        case V1_VERTEX:       ReadVertices();     break;// read Vertices
        case V1_FACE:         ReadFaces();        break;// read Faces
		case V1_MESH:         ReadMesh();         break;// read Mesh
        case V1_MATERIAL:     ReadMaterials();    break;// read Materials
		case V1_JOINT:        ReadJoints();       break;// read Joints
		case V1_ANIMATION:    ReadAnimations();   break;// read Animations
        case V1_END:          bLoop = false;      break;// end
		}
	}while( bLoop );
   
   // do we have some animations?
   if( m_sHeader.uiNumJoints == 0 )
   {
	   // we don't need the secondary vertices
	   delete [] m_pVertices_Orig;
	   m_pVertices_Orig = NULL;
   }

   // bye with OK
   return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: ReadHeader( void )
// Info: Liest den Header aus dem geöffneten File
// Info: Reads the Header from the opened File
//
// Return				= (HRESULT)		Status
// -----------------------------------------------------------------
HRESULT CZFXModel::ReadHeader( void )
{
   // start reading header
   LOG( 20, false, "Reading Header..." );

   // clear header
   ZeroMemory( &m_sHeader, sizeof( CHUNKHEAD_S ) );

   // read header
   fread( &m_sHeader, sizeof( CHUNKHEAD_S ), 1, m_pFile );

   // seek for endchunk
   if( GetNextChunk( m_sChunk ) == V1_END ) {
       LOG( 20, true, " OK" );                        // logit
       return S_OK;                                   // bye
	}

   // log failure
   LOG( 1, true, " FAILED [Header]" );

   // bye with failure
   return E_FAIL;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: ReadVertices( void )
// Info: Reads the Vertices from the opened File
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
HRESULT CZFXModel::ReadVertices( void )
{
   // variablen init
   ULONG   ulNumVertices = m_sHeader.ulNumVertices;   // temp Var
   LPVERTEX_3F pVertices = NULL;                      // Vertices

   // start reading vertices
   LOG( 20, false, "Reading Vertices [%d]...", ulNumVertices );

   // allocate memory
   pVertices = new VERTEX_3F_S[ ulNumVertices ];
   if( !pVertices ){
    LOG( 1, true, " FAILED [VERTICES]" );             // logit
    return E_FAIL;                                    // bye
   }

   // clear memory
   ZeroMemory( pVertices, sizeof( VERTEX_3F_S ) * ulNumVertices );
 
   // read all of the vertices at once
   fread( pVertices, sizeof( VERTEX_3F_S ), ulNumVertices, m_pFile );

   // get max memory for the indices and vertices
   m_pVertices      = new CVERTEX[ ulNumVertices ];
   m_pVertices_Orig = new CVERTEX[ ulNumVertices ];
   ZeroMemory( m_pVertices,      sizeof( CVERTEX ) * ulNumVertices );
   ZeroMemory( m_pVertices_Orig, sizeof( CVERTEX ) * ulNumVertices );

   // convert all vertices
   for( ULONG ulCounter = 0; ulCounter < m_sHeader.ulNumVertices; ulCounter++ )
   {
	   // copy the vertex
	   memcpy( &m_pVertices[ ulCounter ].x,		    &pVertices[ ulCounter ].fXYZ, sizeof( float ) * 3 );
	   memcpy( &m_pVertices[ ulCounter ].vcN,		&pVertices[ ulCounter ].fNormal, sizeof( float ) * 3 );
	   memcpy( &m_pVertices[ ulCounter ].tu,		&pVertices[ ulCounter ].fUV0, sizeof( float ) * 2 );
	   m_pVertices[ ulCounter ].fBone1	    = (float) pVertices[ ulCounter ].uiBoneID_A;
	   m_pVertices[ ulCounter ].fWeight1	= (float) pVertices[ ulCounter ].fWeight_A;
	   m_pVertices[ ulCounter ].fBone2	    = (float) pVertices[ ulCounter ].uiBoneID_B;
	   m_pVertices[ ulCounter ].fWeight2	= (float) pVertices[ ulCounter ].fWeight_B;
   }

   // Free Memory
   delete [] pVertices;

   // seek for endchunk
   if( GetNextChunk( m_sChunk ) == V1_END ) {
       LOG( 20, true, " OK" );                        // logit
       return S_OK;                                   // bye
	}

   // log failure
   LOG( 1, true, " FAILED [VERTICES]" );

   // bye
   return E_FAIL;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: ReadFaces( void )
// Info: Reads the Faces in to the opened File
//
// Return        = (HRESULT)  Status
// -----------------------------------------------------------------
HRESULT CZFXModel::ReadFaces( void )
{
   // variablen init
   ULONG   ulNumFaces = m_sHeader.ulNumFaces;         // temp Var

   // start reading materials
   LOG( 20, false, "Reading Faces [%d]...", ulNumFaces );

   // allocate memory
   m_pFaces = new FACE_S[ ulNumFaces ];
   if( !m_pFaces ){
    LOG( 1, true, " FAILED [FACES]" );                // logit
    return E_FAIL;                                    // bye
   }

   // clear memory
   ZeroMemory( m_pFaces, sizeof( FACE_S ) * ulNumFaces );

   // read all of the faces at once
   fread( m_pFaces, sizeof( FACE_S ), ulNumFaces, m_pFile );

   // seek for endchunk
   if( GetNextChunk( m_sChunk ) == V1_END ) {
       LOG( 20, true, " OK" );                        // logit
       return S_OK;                                   // bye
	}

   // log failure
   LOG( 1, true, " FAILED [FACES]" );

   // bye
   return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: ReadMesh( void )
// Info: Reads the Mesh in to the opened File
//
// Return        = (HRESULT)  Status
// -----------------------------------------------------------------
HRESULT CZFXModel::ReadMesh( void )
{
   // variablen init
   ULONG   ulNumMesh = m_sHeader.ulNumMeshs;         // temp Var

   // start reading materials
   LOG( 20, false, "Reading Meshs [%d]...", ulNumMesh );

   // allocate memory
   m_pMeshs = new MESH_S[ ulNumMesh ];
   if( !m_pMeshs ){
    LOG( 1, true, " FAILED [MESH]" );                 // logit
    return E_FAIL;                                    // bye
   }

   // clear memory
   ZeroMemory( m_pMeshs, sizeof( MESH_S ) * ulNumMesh );

   // read all of the meshs at once
   fread( m_pMeshs, sizeof( MESH_S ), ulNumMesh, m_pFile );

   // seek for endchunk
   if( GetNextChunk( m_sChunk ) == V1_END ) {
       LOG( 20, true, " OK" );                        // logit
       return S_OK;                                   // bye
	}

   // log failure
   LOG( 1, true, " FAILED [MESH]" );

   // bye
   return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: ReadMaterials( void )
// Info: Reads the Materials from to the opened File
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
HRESULT CZFXModel::ReadMaterials( void )
{
   // variablen init
   UINT   uiNumMat = m_sHeader.uiNumMaterials;        // temp Var

   // start reading materials
   LOG( 20, false, "Reading Materials [%d]...", uiNumMat );

   // allocate memory
   m_pMaterials = new MATERIAL_S[ uiNumMat ];
   if( !m_pMaterials ){
    LOG( 1, true, " FAILED [MATERIALS]" );            // logit
    return E_FAIL;                                    // bye
   }

   // clear memory
   ZeroMemory( m_pMaterials, sizeof( MATERIAL_S ) * uiNumMat );

   // read all of the materials at once
   fread( m_pMaterials, sizeof( MATERIAL_S ), uiNumMat, m_pFile );

   // seek for endchunk
   if( GetNextChunk( m_sChunk ) == V1_END ) {
       LOG( 20, true, " OK" );                        // logit
       return S_OK;                                   // bye
	}

   // log failure
   LOG( 1, true, " FAILED [MATERIALS]" );

   // bye
   return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: ReadJoints( void )
// Info: Reads the Joints from to the opened File
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
HRESULT CZFXModel::ReadJoints( void )
{
	// Variablen init
	bool    bLoop  = true;                            // LoopFlag
	UINT    uiLoop = 0;                               // Counter
	UINT    uiNumJoints = m_sHeader.uiNumJoints;      // temp Var
	LPJOINT pJoint = NULL;                            // Temp Joint

	// reserver some memory for the joints
	LOG( 20, false, "Reading Joints [%d]...", uiNumJoints );

	// allocate memory
	m_pJoints = new JOINT_S[ uiNumJoints ];
	if( !m_pJoints ){
		LOG( 1, true, " FAILED [JOINTS]" );
		return E_FAIL;                                // bye
	}

	// loop until dawn...
	do{
		// find the next chunk
		switch( GetNextChunk( m_sChunk ) )
		{
		case V1_JOINT_MAIN:
			 pJoint = &m_pJoints[ uiLoop ];           // set Joint
			 ReadJoint_Main( pJoint );                // read Joint
			 uiLoop++;                                // next Joint
			 break;

		case V1_JOINT_KEYFRAME_ROT:
			 ReadJoint_KeyFrame_Rot( pJoint ); break;

		case V1_JOINT_KEYFRAME_POS:
			 ReadJoint_KeyFrame_Pos( pJoint ); break;

		case V1_END:
			 bLoop = false;                    break;
		}

	}while( bLoop );

	// seek for endchunk
	if( !bLoop ) {
		LOG( 20, true, " OK" );                        // logit
		return S_OK;                                   // bye
	}

	// log failure
	LOG( 1, true, " FAILED [JOINTS]" );

	// bye
	return E_FAIL;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: ReadJoint_Main( LPJOINT pJoint )
// Info: Reads the Joints Mainpart from to the opened File
//
// Return       = (HRESULT)    Status
//
// pJoint       = (LPJOINT)    Parentjoint
// -----------------------------------------------------------------
HRESULT CZFXModel::ReadJoint_Main( LPJOINT pJoint )
{
	// Start
	LOG( 20, false, "Reading Joint " );

	// read the joint
	fread( pJoint, sizeof( JOINT_S ), 1, m_pFile );

	// seek for endchunk
	if( GetNextChunk( m_sChunk ) == V1_END ) {
		LOG( 20, true, " OK" );                        // logit
		return S_OK;                                   // bye
	}

	// log failure
	LOG( 1, true, " FAILED [JOINT_MAIN]" );

	// bye
	return E_FAIL;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: ReadJoint_KeyFrame_Rot( LPJOINT pJoint )
// Info: Reads the Keyframe_Rotation from to the opened File
//
// Return       = (HRESULT)    Status
//
// pJoint       = (LPJOINT)    Parentjoint
// -----------------------------------------------------------------
HRESULT CZFXModel::ReadJoint_KeyFrame_Rot( LPJOINT pJoint )
{
   // variablen init
   UINT   uiNumKeys = pJoint->wNumKF_Rotation;

   // start reading materials
   LOG( 20, false, "Reading KeyFrame Rotation [%d]...", uiNumKeys );

   // allocate memory
   pJoint->pKF_Rotation = new KF_ROT_S[ uiNumKeys ];
   if( !pJoint->pKF_Rotation ){
    LOG( 1, true, " FAILED [JOINT_KEYFRAME_ROTATIONS]" );
    return E_FAIL;                                    // bye
   }

   // clear memory
   ZeroMemory( pJoint->pKF_Rotation, sizeof( KF_ROT_S ) * uiNumKeys );

   // read all of the Rotations at once
   fread( pJoint->pKF_Rotation, sizeof( KF_ROT_S ), uiNumKeys, m_pFile );

   // seek for endchunk
   if( GetNextChunk( m_sChunk ) == V1_END ) {
       LOG( 20, true, " OK" );                        // logit
       return S_OK;                                   // bye
	}

   // log failure
   LOG( 1, true, " FAILED [JOINT_KEYFRAME_ROTATIONS]" );

   // bye
   return E_FAIL;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: ReadJoint_KeyFrame_Pos( LPJOINT pJoint )
// Info: Reads the Keyframe_Position from to the opened File
//
// Return       = (HRESULT)    Status
//
// pJoint       = (LPJOINT)    Parentjoint
// -----------------------------------------------------------------
HRESULT CZFXModel::ReadJoint_KeyFrame_Pos( LPJOINT pJoint )
{
   // variablen init
   UINT   uiNumKeys = pJoint->wNumKF_Position;

   // start reading materials
   LOG( 20, false, "Reading KeyFrame Positions [%d]...", uiNumKeys );

   // allocate memory
   pJoint->pKF_Position = new KF_POS_S[ uiNumKeys ];
   if( !pJoint->pKF_Position ){
    LOG( 1, true, " FAILED [JOINT_KEYFRAME_POSITIONS]" );
    return E_FAIL;                                    // bye
   }

   // clear memory
   ZeroMemory( pJoint->pKF_Position, sizeof( KF_POS_S ) * uiNumKeys );

   // read all of the Positions at once
   fread( pJoint->pKF_Position, sizeof( KF_POS_S ), uiNumKeys, m_pFile );

   // seek for endchunk
   if( GetNextChunk( m_sChunk ) == V1_END ) {
       LOG( 20, true, " OK" );                        // logit
       return S_OK;                                   // bye
	}

   // log failure
   LOG( 1, true, " FAILED [JOINT_KEYFRAME_POSITIONS]" );

   // bye
   return E_FAIL;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: ReadAnimations( void )
// Info: Reads the Animations
//
// Return        = (HRESULT)  Status
// -----------------------------------------------------------------
HRESULT CZFXModel::ReadAnimations( void )
{
	// variablen init
	UINT    uiNumAnim = m_sHeader.uiNumAnimations;     // temp Var

	// start reading materials
	LOG( 20, false, "Reading Animations [%d]...", uiNumAnim );

	// allocate memory
	m_pAnimations = new ANIMATION_S[ uiNumAnim ];
	if( !m_pAnimations ){
		LOG( 1, true, " FAILED [ANIMATIONS]" );        // logit
		return E_FAIL;                                 // bye
	}

	// clear memory
	ZeroMemory( m_pAnimations, sizeof( ANIMATION_S ) * uiNumAnim );

	// read all of the faces at once
	fread( m_pAnimations, sizeof( ANIMATION_S ), uiNumAnim, m_pFile );

	// seek for endchunk
	if( GetNextChunk( m_sChunk ) == V1_END ) {
		LOG( 20, true, " OK" );                        // logit
		return S_OK;                                   // bye
	}

	// log failure
	LOG( 1, true, " FAILED [ANIMATIONS]" );

	// bye
	return E_FAIL;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: Prepare( void )
// Info: Prepare the model
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
HRESULT CZFXModel::Prepare( void )
{
   // Variablen init
   ULONG      ulNumIndices  = 0;                      // counter
   ULONG      ulNumVertices = 0;                      // counter
   UINT       uiCurrentMat  = 0;                      // Material
   PWORD      pIndex        = NULL;                   // Index
   ULONG      ulCounter     = 0;                      // counter
   LPMATERIAL pMaterial     = NULL;                   // Material
   char       cTexture[256] = { 0 };                  // Texturepath
   PCHAR      pcSeperator   = NULL;                   // Seperator
   ULONG      ulIndexCount  = 0;                      // Counter

   // 1. we have to setup the Bones
   SetupBones();

   // log it
   LOG( 20, false, "Sort Indices by Material [%d]", m_sHeader.uiNumMaterials );

   // get max memory for the indices and vertices
   m_sHeader.ulNumIndices = m_sHeader.ulNumFaces * 3;
   pIndex          = new WORD[ m_sHeader.ulNumIndices ];

   // prepare pointers
   m_ppIndices     = new PVOID[ m_sHeader.uiNumMaterials ];
   ZeroMemory( m_ppIndices, sizeof( PVOID ) * m_sHeader.uiNumMaterials );

   m_pIndices      = new WORD[ m_sHeader.ulNumIndices ];
   ZeroMemory( m_pIndices, sizeof( WORD ) * m_sHeader.ulNumIndices );

   m_puiNumIndices = new UINT[ m_sHeader.uiNumMaterials ];
   ZeroMemory( m_puiNumIndices, sizeof( UINT ) * m_sHeader.uiNumMaterials );
   
   m_puiSkinBuffer = new UINT[ m_sHeader.uiNumMaterials ];
   ZeroMemory( m_puiSkinBuffer, sizeof( UINT ) * m_sHeader.uiNumMaterials );

   // Sort all the Faces into the Indexarrays by MaterialID
   do{
    
	// clear indexarray
	ZeroMemory( pIndex, sizeof(WORD) * m_sHeader.ulNumIndices );
	
	// reset index counter
	ulNumIndices = 0;

    // loop through all the faces
	for( ulCounter = 0; ulCounter < m_sHeader.ulNumFaces; ulCounter++ )
	{
	   // same material as the current one?
	   if( m_pFaces[ ulCounter ].uiMaterialID == uiCurrentMat )
	   {
		   // copy index
		   m_pIndices[ ulIndexCount++ ] = pIndex[ ulNumIndices++ ] = (WORD)m_pFaces[ ulCounter ].ulIndices[ 0 ];
		   m_pIndices[ ulIndexCount++ ] = pIndex[ ulNumIndices++ ] = (WORD)m_pFaces[ ulCounter ].ulIndices[ 1 ];
		   m_pIndices[ ulIndexCount++ ] = pIndex[ ulNumIndices++ ] = (WORD)m_pFaces[ ulCounter ].ulIndices[ 2 ];
	   }
	}

	// enough Indices
	if( !ulNumIndices )
	{
		// set new Material
	    uiCurrentMat++;
		
		// Stop Error
		LOG( 1, true, "STOP Error: Not enough Indices..." );

		// goon
		continue;
	}

	// copy the indexarray
	m_puiNumIndices[ uiCurrentMat ] = ulNumIndices;
	m_ppIndices[ uiCurrentMat ] = new WORD[ ulNumIndices ];
	memcpy( m_ppIndices[ uiCurrentMat ], pIndex, sizeof(WORD) * ulNumIndices );

	// set current material
	pMaterial = &m_pMaterials[ uiCurrentMat ];

	// Readin the Materials
    if( FAILED( m_pRenderDevice->GetSkinManager()->AddSkin( 
 	                             (ZFXCOLOR*)&pMaterial->fAmbient,
                                 (ZFXCOLOR*)&pMaterial->fDiffuse,
                                 (ZFXCOLOR*)&pMaterial->fEmissive,
                                 (ZFXCOLOR*)&pMaterial->fSpecular,
                                 pMaterial->fSpecularPower,
                                 &m_puiSkinBuffer[ uiCurrentMat ])))
	{
	   // logit
	   LOG( 1, true, " FAILED [LOAD SKIN %d]", uiCurrentMat );
	}

	// prepare texture
	ZeroMemory( cTexture, sizeof( char ) * 256 );
	pcSeperator = strchr( strrev( strdup( m_pcFileName ) ), '/' );
	if( !pcSeperator ) pcSeperator = strchr( strrev( strdup( m_pcFileName ) ), 92 );
	if( pcSeperator )	strcpy( cTexture, strrev( pcSeperator ) );
	strcat( cTexture, pMaterial->cTexture_1 );

	// load texture
	if( FAILED( m_pRenderDevice->GetSkinManager()->AddTexture(	
                                 m_puiSkinBuffer[ uiCurrentMat ],
                                 cTexture, false,
								 0, NULL, 0 ) ) )
	{
	   // logit
	   LOG( 1, true, " FAILED [LOAD TEXTURE %s]", pMaterial->cTexture_1 );
	}

	// set new Material
	uiCurrentMat++;    

   }while( uiCurrentMat != m_sHeader.uiNumMaterials );

   // now release old memory
   delete [] pIndex;

   // log it
   LOG( 20, true, " done" );

   // bye
   return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: SetupBones( void )
// Info: Prepare the bones from the Model
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
HRESULT CZFXModel::SetupBones( void )
{
	// Variablen init
   LPJOINT     pJoint       = NULL;                    // Joint
	ULONG	    ulCounter    = 0;                       // Counter
	UINT	    uiLoop       = 0;                       // Counter
	UINT        uiParentID   = 0;                       // Parent ID
	ZFXVector   sVector_A;                              // Vector
	ZFXVector   sVector_B;                              // Vector
	CVERTEX     *pVertex     = NULL;                    // TempVertex
	ZFXMatrix   matTemp; matTemp.Identity();            // Temp Matrix

	// If we have no Bones we can leave at this point
	if( m_sHeader.uiNumJoints == 0 ) return S_OK;

	// empower the matrices
	for( ulCounter = 0; ulCounter < m_sHeader.uiNumJoints; ulCounter++ )
	{
		// Get the current joint
		pJoint = &m_pJoints[ ulCounter ];

		// Aktuelle Rotation in der Relative Matrix erzeugen
		pJoint->sMatrix_relative = CreateRotationMatrix( &pJoint->vRotation );

		// Position setzen
		pJoint->sMatrix_relative._14 = pJoint->vPosition.x;
		pJoint->sMatrix_relative._24 = pJoint->vPosition.y;
		pJoint->sMatrix_relative._34 = pJoint->vPosition.z;

		// Find our Parent...
		for( uiLoop = 0; uiLoop < m_sHeader.uiNumJoints; uiLoop++ )
		{
			// set the parent id, for later checks
			uiParentID = 255;

			if( strcmp( m_pJoints[ uiLoop ].cName, pJoint->cParentName ) == 0 )
			{
				// We have found our parent, so lets stop
				uiParentID = uiLoop;
				break;
			}
		}

		// Store the known parentid
		pJoint->wParentID = uiParentID;

		// Did we found our parent?
		if( uiParentID != 255 )
		{
			// We found our parents, so we have to multiply the parent absolut-
			// matrix with our relativmatrix. So we get a new and polished
			// absolutmatrix. Straight forward, eh?
			pJoint->sMatrix_absolute = m_pJoints[ uiParentID ].sMatrix_absolute *
				              pJoint->sMatrix_relative;
		} else {
			// We stay on our own.... what a pity...
			// So our relativmatrix is our absolutmatrix :)
			pJoint->sMatrix_absolute = pJoint->sMatrix_relative;

		}
		// get the final matrix
		pJoint->sMatrix.TransposeOf( pJoint->sMatrix_absolute );

		// Transponieren der Matrizen
		matTemp = pJoint->sMatrix_relative;
		pJoint->sMatrix_relative.TransposeOf( matTemp );
	}
	
	// Setup all Vertices
	for( ulCounter = 0; ulCounter < m_sHeader.ulNumVertices; ulCounter++ )
	{
		// get the current vertex
		pVertex = &m_pVertices_Orig[ ulCounter ];

		// process only, when there is a bone
		if( pVertex->fBone1 != 255.0f )
		{
			// Get current matrix
			matTemp.Identity();
			matTemp = m_pJoints[ (UINT)pVertex->fBone1 ].sMatrix;
			
			// 1. rotate the vertices
			sVector_A.x = pVertex->x;
			sVector_A.y = pVertex->y;
			sVector_A.z = pVertex->z;
			sVector_A -= matTemp.GetTranslation();
			sVector_A.InvRotateWith( matTemp );
			pVertex->x = sVector_A.x;
			pVertex->y = sVector_A.y;
			pVertex->z = sVector_A.z;

			// 2. rotate the normals
			sVector_A.x = pVertex->vcN[ 0 ];
			sVector_A.y = pVertex->vcN[ 1 ];
			sVector_A.z = pVertex->vcN[ 2 ];
			sVector_A.InvRotateWith( matTemp );
			pVertex->vcN[ 0 ] = sVector_A.x;
			pVertex->vcN[ 1 ] = sVector_A.y;
			pVertex->vcN[ 2 ] = sVector_A.z;
		}
	}
		
	// bye
	return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: SetAnimationSpeed( float fSpeed )
// Info: Sets the Animationsspeed
//
// fSpeed       = (float)      Speed
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
void CZFXModel::SetAnimationSpeed( float fSpeed )
{ m_fAnimationSpeed = fSpeed; }
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: SetAnimation( UINT uiAnim )
// Info: Sets the Animationssequenze
//
// uiAnim       = (UINT) Animationssquenze
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
void CZFXModel::SetAnimation( UINT uiAnim )
{
	// In Range?
	if( uiAnim > m_sHeader.uiNumAnimations ) uiAnim = 0;
	if( uiAnim < 0 ) uiAnim = m_sHeader.uiNumAnimations;

	// Setz Animation
	m_uiCurrentAnimation  = uiAnim;

	// Multiply Animations
	m_bAnimationRunOnce   = false;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: SetAnimation( UINT uiAnim, float fSpeed )
// Info: Sets the Animationssequenze
//
// uiAnim       = (UINT)       Animationssquenze
// fSpeed       = (float)      Speed
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
void CZFXModel::SetAnimation( UINT uiAnim, float fSpeed )
{
	// In Range?
	if( uiAnim > m_sHeader.uiNumAnimations ) uiAnim = 0;
	if( uiAnim < 0 ) uiAnim = m_sHeader.uiNumAnimations;

	// Setz Animation
	m_uiCurrentAnimation  = uiAnim;

	// Multiply Animations
	m_bAnimationRunOnce   = false;

	// Set the Animationsspeed
	SetAnimationSpeed( fSpeed );
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: SetAnimation( bool bSingle, UINT uiAnim )
// Info: Sets the Animationssequenze
//
// bSingle      = (bool)       Singleanimation
// uiAnim       = (UINT)       Animationssquenze
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
void CZFXModel::SetAnimation( bool bSingle, UINT uiAnim )
{
	// Set the wanted Animation
	SetAnimation( uiAnim );

	// Multiply Animations
	m_bAnimationChanged   = true;
	m_bAnimationRunOnce   = bSingle;
	m_bAnimationComplete  = false;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: SetAnimation( bool bSingle, UINT uiAnim, float fSpeed )
// Info: Sets the Animationssequenze
//
// bSingle      = (bool)       Singleanimation
// uiAnim       = (UINT)       Animationssquenze
// fSpeed       = (float)      Speed
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
void CZFXModel::SetAnimation( bool bSingle, UINT uiAnim, float fSpeed )
{
	// Set the wanted Animation
	SetAnimation( uiAnim );

	// Multiply Animations
	m_bAnimationChanged   = true;
	m_bAnimationRunOnce   = bSingle;
	m_bAnimationComplete  = false;

	// Set the Animationsspeed
	SetAnimationSpeed( fSpeed );
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: SetRenderBones( void )
// Info: Render the bones?
// -----------------------------------------------------------------
void CZFXModel::SetRenderBones( void )
{ m_bRenderBones = !m_bRenderBones; }
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: SetRenderNormals( void )
// Info: Render the Normals?
// -----------------------------------------------------------------
void CZFXModel::SetRenderNormals( void )
{ m_bRenderNormals = !m_bRenderNormals; }
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: SetScaling( float fScale /* = 0.0f  */ )
// Info: Sets the scaling of the model. 0.0f for no scaling.
//
// fScale       = (float)      Scaling
// -----------------------------------------------------------------
void CZFXModel::SetScaling( float fScale /* = 0.0f  */ )
{
	// Variablen init
	ULONG	    ulCounter     = 0;                      // Counter
	ULONG       ulInner       = 0;                      // Counter
	CVERTEX     *pVertex      = NULL;                   // TempVertex
	float       fScaling      = 0.0f;                   // Scaling
	LPJOINT     pJoint        = NULL;                   // Joint
	
	// shall we scale the model?
	if( fScale == 0.0f ) return;

	// get the bounding box
	// reset the min and max vertex of the BBox
	m_sBBoxMin.x =  999999.0f; m_sBBoxMin.y =  999999.0f;
	m_sBBoxMin.z =  999999.0f; m_sBBoxMax.x = -999999.0f;
	m_sBBoxMax.y = -999999.0f; m_sBBoxMax.z = -999999.0f;

	// Now setup all known vertices
	for( ulCounter = 0; ulCounter < m_sHeader.ulNumVertices; ulCounter++ )
	{
		// get the current Vertex
		pVertex    = &m_pVertices[ ulCounter ];

		// 2. Calc BBox
		m_sBBoxMax.x = MAX( m_sBBoxMax.x, pVertex->x );
		m_sBBoxMax.y = MAX( m_sBBoxMax.y, pVertex->y );
		m_sBBoxMax.z = MAX( m_sBBoxMax.z, pVertex->z );
		m_sBBoxMin.x = MIN( m_sBBoxMin.x, pVertex->x );
		m_sBBoxMin.y = MIN( m_sBBoxMin.y, pVertex->y );
		m_sBBoxMin.z = MIN( m_sBBoxMin.z, pVertex->z );
	}

	// Calc scaling
	fScaling = ( m_sBBoxMax.y - m_sBBoxMin.y ) / fScale;
	
	// Now scale all known vertices
	for( ulCounter = 0; ulCounter < m_sHeader.ulNumVertices; ulCounter++ )
	{
		// get the current vertex
		pVertex      = &m_pVertices[ ulCounter ];

		// scale the vertex
		pVertex->x   /= fScaling;
		pVertex->y   /= fScaling;
		pVertex->z   /= fScaling;	
	}

	// copy the vertices if we have animations...
	if( m_sHeader.uiNumJoints > 0 )
	   memcpy( m_pVertices_Orig, m_pVertices, sizeof( CVERTEX ) * m_sHeader.ulNumVertices );

	// Now scale all known bones
	for( ulCounter = 0; ulCounter < m_sHeader.uiNumJoints; ulCounter++ )
	{
		// get the current vertex
		pJoint       = &m_pJoints[ ulCounter ];

		// scale the vertex
		pJoint->vPosition.x   /= fScaling;
		pJoint->vPosition.y   /= fScaling;
		pJoint->vPosition.z   /= fScaling;

		// scale the keyframe position
		for( ulInner = 0; ulInner < pJoint->wNumKF_Position; ulInner++ )
		{
			pJoint->pKF_Position[ ulInner ].vPosition.x /= fScaling;
			pJoint->pKF_Position[ ulInner ].vPosition.y /= fScaling;
			pJoint->pKF_Position[ ulInner ].vPosition.z /= fScaling;
		}
	}

	// create Aabb
	m_sAabb.vcMin.x = m_sBBoxMin.x;
	m_sAabb.vcMin.y = m_sBBoxMin.y;
	m_sAabb.vcMin.z = m_sBBoxMin.z;
	m_sAabb.vcMax.x = m_sBBoxMax.x;
	m_sAabb.vcMax.y = m_sBBoxMax.y;
	m_sAabb.vcMax.z = m_sBBoxMax.z;
	m_sAabb.vcCenter.x = ( m_sBBoxMax.x - m_sBBoxMin.x ) / 2;
	m_sAabb.vcCenter.y = ( m_sBBoxMax.y - m_sBBoxMin.y ) / 2;
	m_sAabb.vcCenter.z = ( m_sBBoxMax.z - m_sBBoxMin.z ) / 2;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: GetAnimation( void )
// Info: Gets the Animationssequenze
//
// Return       = (HRESULT)    Animationssquenze
// -----------------------------------------------------------------
UINT CZFXModel::GetAnimation( void ){ return m_uiCurrentAnimation; }
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: GetAnimationSpeed( void )
// Info: Gets the Animationsspeed
//
// Return       = (HRESULT)    Animationssquenze
// -----------------------------------------------------------------
float CZFXModel::GetAnimationSpeed( void ){ return m_fAnimationSpeed; }
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: GetVertices( CVERTEX* *pVertices )
// Info: Gets the whole Data out of the model
//
// Return       = (ULONG)      Number of Vertices
//
// pVertices    = (CVERTEX*)   Pointer for the Vertices
// -----------------------------------------------------------------
ULONG CZFXModel::GetVertices( CVERTEX* *pVertices )
{ 
	// return the Pointer
	if( pVertices ) *pVertices = m_pVertices;
	
	// return the number of Vertices
	return m_sHeader.ulNumVertices;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: GetIndices( PWORD *pIndices )
// Info: Gets the whole Data out of the model
//
// Return       = (ULONG)      Number of Vertices
//
// pIndices     = (PWORD)      Pointer for the Indices
// -----------------------------------------------------------------
ULONG CZFXModel::GetIndices( PWORD *pIndices )
{ 
	// return the Pointer
	if( pIndices ) *pIndices = m_pIndices;

	// return the number of Vertices
	return m_sHeader.ulNumIndices;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: GetBBoxMin( void )
// Info: Returns the BBox MinVertex
//
// Return       = (VERTEX)  MinVertex
// -----------------------------------------------------------------
VERTEX CZFXModel::GetBBoxMin( void ){ return m_sBBoxMin; }
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: GetBBoxMax( void )
// Info: Returns the BBox MaxVertex
//
// Return       = (VERTEX)  MaxVertex
// -----------------------------------------------------------------
VERTEX CZFXModel::GetBBoxMax( void ){ return m_sBBoxMax; }
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: GetBBoxMax( void )
// Info: Returns the BBox MaxVertex
//
// Return       = (ZFXAabb)  Aabb
// -----------------------------------------------------------------
ZFXAabb CZFXModel::GetAabb( void ){ return m_sAabb; }
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: IsAnimationCompleted( void )
// Info: Gets the Animation completeflag
//
// Return       = (bool)    Animation Complete Flag
// -----------------------------------------------------------------
bool CZFXModel::IsAnimationCompleted( void ){ return m_bAnimationComplete; }
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: Animation( void )
// Info: Animate the model
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
HRESULT CZFXModel::Animation( void )
{
	// Variablen init
	float		fElapsed	= -1.0f;						// elapsed time
	float		fStart		= -1.0f;						// Start
	float		fEnd		= -1.0f;						// End
	LPANIMATION pAnimation  = NULL;                         // Animation

	// Do we have some animations?
	if( m_sHeader.uiNumJoints == 0 )                     return S_OK;

	// Shall we run the Animation only one time?
	if( m_bAnimationRunOnce  &&
		m_bAnimationComplete &&
		!m_bAnimationChanged )
		return S_OK;

	// Get Time
	m_fTime = (float)GetTickCount();
	
	// New, then set the time as Starttime
	if( m_fStartTime == -1.0f )	m_fStartTime = m_fTime;

	// calc elapsed Time
	fElapsed = m_fTime - m_fStartTime;


	// get the current animation ----------------------------------------------
	pAnimation = &m_pAnimations[ m_uiCurrentAnimation ];

	fStart	= pAnimation->fStartFrame;						// Startzeit
	fEnd	= pAnimation->fEndFrame;						// Endzeit

	// Calculate the current frameposition
	m_fFrame = fStart + (m_sHeader.fAnimationFPS / m_fAnimationSpeed) * fElapsed;
	
	// Set the startframe if we are new
	if( m_fFrame <= fStart )	m_fFrame = fStart;

	// reached the end? if so, then tweak the time
	if( m_fFrame >= fEnd )
	{
		m_fStartTime = m_fTime;   			                // Zeit setzen
		m_fFrame     = fStart;
		m_bAnimationComplete	= true;						// Flag setzen
	}
	else
	{
		// Prepare the animation
		AnimationPrepare();

		// Setup all Vertices
		AnimationVertices();
		m_bAnimationComplete	= false;					// Flag setzen
		m_bAnimationChanged     = false;                    // Animation changed

	}

	// bye
	return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: AnimationPrepare( void )
// Info: Prepares the animationsequence
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
HRESULT CZFXModel::AnimationPrepare( void )
{
	// Variablen init
	LPJOINT     pJoint       = NULL;                    // Joint
	ULONG	    ulCounter    = 0;                       // Counter
	UINT	    uiLoop       = 0;                       // Counter
	ZFXVector   sPosition;                              // Vector
	ZFXVector   sRotation;                              // Vector
	UINT        uiKeyPos     = 0;                       // KeyPositions
    UINT        uiKeyRot     = 0;                       // KeyRotations
	LPKF_ROT    pLastRot     = NULL;                    // Rotation
	LPKF_ROT    pThisRot     = NULL;                    // Rotation
	LPKF_ROT    pKeyRot      = NULL;                    // Rotation
	LPKF_POS    pLastPos     = NULL;                    // Position
	LPKF_POS    pThisPos     = NULL;                    // Position
	LPKF_POS    pKeyPos      = NULL;                    // Position
	float       fScale       = 0.0f;                    // Scaling
	ZFXMatrix   matTemp; matTemp.Identity();            // Temp Matrix
	ZFXMatrix   matFinal; matFinal.Identity();          // Temp Matrix

	// clip the animation
	if( m_fFrame > m_sHeader.uiNumFrames ) m_fFrame = 0;

	// empower the matrices
	for( ulCounter = 0; ulCounter < m_sHeader.uiNumJoints; ulCounter++ )
	{
		// Get the current joint
		pJoint   = &m_pJoints[ ulCounter ];

		// Get some other data
		uiKeyPos =  pJoint->wNumKF_Position;            // Position
		uiKeyRot =  pJoint->wNumKF_Rotation;            // Rotation

		// do we have to calculate a new matrix?
		if( ( uiKeyRot + uiKeyPos )  != 0 )
		{
			// yes... there is a new Position or Rotation

			// process the positions ------------------------------------------
			pLastPos = NULL; pThisPos = NULL; pKeyPos = NULL;

			for( uiLoop = 0; uiLoop < uiKeyPos; uiLoop++ )
			{
				// get current position
				pKeyPos = &pJoint->pKF_Position[ uiLoop ];

				// check with the time
				if( pKeyPos->fTime >= m_fFrame )
				{
					// we've found the current Time
					pThisPos = pKeyPos;

					// bye
					break;
				}

				// nothing found
				pLastPos = pKeyPos;
			} // all positions

			// now interpolate the to positions
			if( pLastPos && pThisPos )
			{
				// calc scaling
				fScale = ( m_fFrame - pLastPos->fTime ) / ( pThisPos->fTime - pLastPos->fTime );

				// Interpolate the positions
				sPosition = pLastPos->vPosition + ( pThisPos->vPosition - pLastPos->vPosition ) * fScale;
			} else if( !pLastPos )
			{
				// copy the position
				sPosition = pThisPos->vPosition;
			} else {
				// copy the position
				sPosition = pLastPos->vPosition;
			}

			// process the rotations ------------------------------------------
			pLastRot = NULL; pThisRot = NULL; pKeyRot = NULL;

			for( uiLoop = 0; uiLoop < uiKeyRot; uiLoop++ )
			{
				// get current Rotition
				pKeyRot = &pJoint->pKF_Rotation[ uiLoop ];

				// check with the time
				if( pKeyRot->fTime >= m_fFrame )
				{
					// we've found the current Time
					pThisRot = pKeyRot;

					// bye
					break;
				}

				// nothing found
				pLastRot = pKeyRot;
			} // all Rotitions

			// now interpolate the to Rotations
			if( pLastRot && pThisRot )
			{
				// Interpolate the Rotitions
				sRotation = pLastRot->vRotation + ( pThisRot->vRotation - pLastRot->vRotation ) * fScale;
			} else if( !pLastRot )
			{
				// copy the Rotition
				sRotation = pThisRot->vRotation;
			} else {
				// copy the Rotition
				sRotation = pLastRot->vRotation;
			}

			// setup the joint matrix
			matTemp.SetTranslation( sPosition );        // Position
			matTemp.Rota( sRotation );                  // Rotation

			// calc the relative matrix
			matFinal = matTemp * pJoint->sMatrix_relative;

			// Did we found our parent?
			if( pJoint->wParentID != 255 )
			{
				// We found our parents, so we have to multiply the parent absolut-
				// matrix with our relativmatrix. So we get a new and polished
				// absolutmatrix. Straight forward, eh?
				pJoint->sMatrix = matFinal * m_pJoints[ pJoint->wParentID ].sMatrix;
			} else {
				// We stay on our own.... what a pity...
				// So our relativmatrix is our absolutmatrix :)
				pJoint->sMatrix = matFinal;
			}           

		} else {

			// We have no new Matrix, so copy the old one....
			pJoint->sMatrix = pJoint->sMatrix_relative;
		} // change Matrix
	} // all Joints

	// bye
	return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: AnimationVertices( void  )
// Info: Calculates the Vertices with the current animationsequence
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
HRESULT CZFXModel::AnimationVertices( void )
{
	// Variablen init
	ULONG	    ulCounter     = 0;                      // Counter
	CVERTEX     *pVertex      = NULL;                   // TempVertex
	CVERTEX     *pVertex_Orig = NULL;                   // TempVertex
	ZFXVector   sVector_A, sVector_B;                   // Vector
	
	// reset the min and max vertex of the BBox
	m_sBBoxMin.x =  999999.0f; m_sBBoxMin.y =  999999.0f;
	m_sBBoxMin.z =  999999.0f; m_sBBoxMax.x = -999999.0f;
	m_sBBoxMax.y = -999999.0f; m_sBBoxMax.z = -999999.0f;

	// Now setup all known vertices
	for( ulCounter = 0; ulCounter < m_sHeader.ulNumVertices; ulCounter++ )
	{
		// get the current vertex
		pVertex      = &m_pVertices[ ulCounter ];
		pVertex_Orig = &m_pVertices_Orig[ ulCounter ];

		// process only, when there is a bone
		if( pVertex->fBone1 != 255.0f )
		{
			// 1. get the current orig vertex
			sVector_A.x = pVertex_Orig->x;
			sVector_A.y = pVertex_Orig->y;
			sVector_A.z = pVertex_Orig->z;

			// 2. rotate the vertex
			sVector_A.RotateWith( m_pJoints[ (UINT)pVertex_Orig->fBone1 ].sMatrix );

			// 3. get the Position
			sVector_A  += m_pJoints[ (UINT)pVertex_Orig->fBone1 ].sMatrix.GetTranslation();

            // 4. save and calc the new position
			pVertex->x = sVector_A.x;
			pVertex->y = sVector_A.y;
			pVertex->z = sVector_A.z;

			// 5. skin the normals
			sVector_A.x = pVertex_Orig->vcN[ 0 ];
			sVector_A.y = pVertex_Orig->vcN[ 1 ];
			sVector_A.z = pVertex_Orig->vcN[ 2 ];
			sVector_A.RotateWith( m_pJoints[ (UINT)pVertex_Orig->fBone1 ].sMatrix );
			pVertex->vcN[ 0 ] = sVector_A.x;
			pVertex->vcN[ 1 ] = sVector_A.y;
			pVertex->vcN[ 2 ] = sVector_A.z;

			// 6. Calc BBox
			m_sBBoxMax.x = MAX( m_sBBoxMax.x, pVertex->x );
			m_sBBoxMax.y = MAX( m_sBBoxMax.y, pVertex->y );
			m_sBBoxMax.z = MAX( m_sBBoxMax.z, pVertex->z );
			m_sBBoxMin.x = MIN( m_sBBoxMin.x, pVertex->x );
			m_sBBoxMin.y = MIN( m_sBBoxMin.y, pVertex->y );
			m_sBBoxMin.z = MIN( m_sBBoxMin.z, pVertex->z );
		}
	}

	// create Aabb
	m_sAabb.vcMin.x = m_sBBoxMin.x;
	m_sAabb.vcMin.y = m_sBBoxMin.y;
	m_sAabb.vcMin.z = m_sBBoxMin.z;
	m_sAabb.vcMax.x = m_sBBoxMax.x;
	m_sAabb.vcMax.y = m_sBBoxMax.y;
	m_sAabb.vcMax.z = m_sBBoxMax.z;
	m_sAabb.vcCenter.x = ( m_sBBoxMax.x - m_sBBoxMin.x ) / 2;
	m_sAabb.vcCenter.y = ( m_sBBoxMax.y - m_sBBoxMin.y ) / 2;
	m_sAabb.vcCenter.z = ( m_sBBoxMax.z - m_sBBoxMin.z ) / 2;

	// bye
	return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: Update( float fTime  )
// Info: Updates the model
//
// fTime        = Deltatime
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
HRESULT CZFXModel::Update( float fTime )
{
	// Set time
	m_fTime = fTime;

	// Do the animation
	return Animation();
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: Render( void )
// Info: Render the model
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
HRESULT CZFXModel::Render( void )
{
   // Variablen init
   UINT     uiCounter = 0;                                // Counter

   // Set Culling
   m_pRenderDevice->SetBackfaceCulling(RS_CULL_CCW);

   // render the VB
   for( uiCounter = 0; uiCounter < m_sHeader.uiNumMaterials; uiCounter++ )
         if( FAILED( m_pRenderDevice->GetVertexManager()->Render( VID_CA, m_sHeader.ulNumVertices, m_puiNumIndices[ uiCounter ],
			                                                      m_pVertices, (PWORD)m_ppIndices[ uiCounter ], m_puiSkinBuffer[ uiCounter ] ) ) )
			 LOG( 1, true, "ERROR Failed to Render VB: %d [%d]", m_puiSkinBuffer[ uiCounter ], uiCounter );

   // render other stuff?
   if( m_bRenderBones )   RenderBones();                 // Bones
   if( m_bRenderNormals ) RenderNormals();               // Normals
  
   // bye
   return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: RenderBones( void )
// Info: Render the bones of the model
//
// fTime        = Deltatime
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
HRESULT CZFXModel::RenderBones( void )
{
	// Variablen init
	UINT     uiCounter = 0;                                // Counter
	LVERTEX  pLine[3];                                     // Joints
	WORD     pIndis[3] = { 0, 1, 2 };                      // Indices
	DWORD    dwColor   = 0x00ffff;                         // Bonecolor
	
	// got bones?
	if( m_sHeader.uiNumJoints == 0 ) return S_OK;

	// render the bones
	m_pRenderDevice->SetBackfaceCulling(RS_CULL_NONE);
	m_pRenderDevice->SetDepthBufferMode(RS_DEPTH_NONE);

	// render the bones
	for( uiCounter = 0; uiCounter < m_sHeader.uiNumJoints; uiCounter++ )
	{
		// First Vertex
		pLine[0].x = m_pJoints[ uiCounter ].sMatrix._41;
		pLine[0].y = m_pJoints[ uiCounter ].sMatrix._42;
		pLine[0].z = m_pJoints[ uiCounter ].sMatrix._43;
		pLine[0].Color  = dwColor;

		if( m_pJoints[ uiCounter ].wParentID != 255 )
		{
			// Second Vertex
			pLine[1].x = m_pJoints[ m_pJoints[
			uiCounter ].wParentID ].sMatrix._41;
			pLine[1].y = m_pJoints[ m_pJoints[
			uiCounter ].wParentID ].sMatrix._42;
			pLine[1].z = m_pJoints[ m_pJoints[
			uiCounter ].wParentID ].sMatrix._43;
			pLine[1].Color  = dwColor;

			// Third Vertex
			pLine[2].x = pLine[1].x + 1.0f;
			pLine[2].y = pLine[1].y + 1.0f;
			pLine[2].z = pLine[1].z + 1.0f;
			pLine[2].Color  = dwColor;

			// Render
			m_pRenderDevice->GetVertexManager()->Render(VID_UL, 3, 3, pLine, pIndis, 0);
		}
	}
	    
	// Settings for the engine
	m_pRenderDevice->SetDepthBufferMode(RS_DEPTH_READWRITE);
	m_pRenderDevice->SetBackfaceCulling(RS_CULL_CCW);

	// bye
	return S_OK;
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
// Name: RenderNormals( void )
// Info: Render the normals of the model
//
// fTime        = Deltatime
//
// Return       = (HRESULT)    Status
// -----------------------------------------------------------------
HRESULT CZFXModel::RenderNormals( void )
{
	// Variablen init
	ULONG    ulCounter = 0;                                // Counter
	float    fStart[3] = { 0,0,0 };                        // Start
	float    fEnd[3]   = { 0,0,0 };                        // End
	ZFXCOLOR sColor    = { 1.0f, 0, 0, 0 };                // Colors
	CVERTEX* pVertex   = NULL;                             // Vertex


	// render the bones
	for( ulCounter = 0; ulCounter < m_sHeader.ulNumVertices; ulCounter++ )
	{
		// get current Vertex
		pVertex = &m_pVertices[ ulCounter ];

		// set Startpoint
		memcpy( fStart, &pVertex->x, sizeof( float ) * 3 );

		// set the Endpoint
		fEnd[0] = fStart[0] + (pVertex->vcN[0] * 2.0f);
		fEnd[1] = fStart[1] + (pVertex->vcN[1] * 2.0f);
		fEnd[2] = fStart[2] + (pVertex->vcN[2] * 2.0f);

		// Render the normal
		m_pRenderDevice->GetVertexManager()->RenderLine( fStart, fEnd, &sColor );
	}

	// bye
	return S_OK;
}
// -----------------------------------------------------------------


// ----------------------------------------------------------------------------
// Name: CreateRotationMatrix( Vector_C *pVector )
// Info: Erstellt eine Rotationsmatrix aus dem Vector
//
// pVector			= Rotationsvector
// ----------------------------------------------------------------------------
ZFXMatrix CZFXModel::CreateRotationMatrix( ZFXVector *pVector )
{
	// Variablen init
	float	sr, sp, sy, cr, cp, cy;
	ZFXMatrix matRet;

	matRet.Identity();

	sy = (float)sin( pVector->z );
	cy = (float)cos( pVector->z );
	sp = (float)sin( pVector->y );
	cp = (float)cos( pVector->y );
	sr = (float)sin( pVector->x );
	cr = (float)cos( pVector->x );

	matRet._11	= cp*cy;
	matRet._21 = cp*sy;
	matRet._31 = -sp;
	matRet._12 = sr*sp*cy+cr*-sy;
	matRet._22 = sr*sp*sy+cr*cy;
	matRet._32 = sr*cp;
	matRet._13 = (cr*sp*cy+-sr*-sy);
	matRet._23 = (cr*sp*sy+-sr*cy);
	matRet._33 = cr*cp;
	matRet._14 = 0.0;
	matRet._24 = 0.0;
	matRet._34 = 0.0;
	
	return matRet;
}
// ----------------------------------------------------------------------------