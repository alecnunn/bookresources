//--------------------------------------------------------------------------------------------------------------------//
//                                                FILE ACCESS WITH C LIB                                              //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_FILEC_INL_
#define   _GPG4_FILEC_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CFileC::CFileC() : m_pFile(NULL)
  {}

//

INLINE CFileC::~CFileC()
  {
  if(m_pFile) Close();
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                OPERATIONS                                                          //
//--------------------------------------------------------------------------------------------------------------------//

INLINE u32 CFileC::Read(void* pvBuffer,const u32 u32Size)
  {
  if(!m_pFile) return 0;
  return fread(pvBuffer,1,u32Size,m_pFile);
  }

//

INLINE u32 CFileC::Write(const void* pvBuffer,const u32 u32Size)
  {
  if(!m_pFile) return 0;
  return fwrite(pvBuffer,1,u32Size,m_pFile);
  }

//

INLINE bool CFileC::Close()
  {
  if(!m_pFile) return true;                                 // already closed
  fclose(m_pFile);
  m_pFile = NULL;
  return true;
  }

//

INLINE u32 CFileC::Seek(const u32 u32Pos,const u32 u32Orig)
  {
  if(!m_pFile) return 1;                                    // !=0 : error
  return fseek(m_pFile,u32Pos,u32Orig);
  }

//

INLINE u32 CFileC::Tell()
  {
  if(!m_pFile) return -1;                                   // -1 : error
  return ftell(m_pFile);
  }

//

INLINE bool CFileC::Eof() const
  {
  if(!m_pFile) return true;                                 // no file => EOF
  return(feof(m_pFile) != 0);
  }

//

INLINE bool CFileC::Error() const
  {
  if(!m_pFile) return true;                                 // no file => error
  return(ferror(m_pFile) != 0);
  }

//

INLINE bool CFileC::Flush()
  {
  if(!m_pFile) return false;
  return(fflush(m_pFile) != EOF);
  }

//

INLINE u32 CFileC::GetChar()
  {
  if(!m_pFile) return EOF;
  return fgetc(m_pFile);
  }

//

INLINE bool CFileC::GetString(char* pszString,const u32 u32Len)
  {
  if(!m_pFile) return false;
  return(fgets(pszString,u32Len,m_pFile) != NULL);
  }

//

INLINE bool CFileC::PutChar(const u32 u32Char)
  {
  if(!m_pFile) return false;
  return(fputc(u32Char,m_pFile) != EOF);
  }

//

INLINE bool CFileC::PutString(const char* pszString)
  {
  if(!m_pFile) return false;
  return(fputs(pszString,m_pFile) != EOF);
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_FILEC_INL_
