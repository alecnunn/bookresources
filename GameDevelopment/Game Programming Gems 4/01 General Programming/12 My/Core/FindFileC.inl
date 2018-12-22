//--------------------------------------------------------------------------------------------------------------------//
//                                                FIND FILE/DIR WITH C LIB                                            //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_FINDFILEC_INL_
#define   _GPG4_FINDFILEC_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CFindFileC::CFindFileC() : m_boInFind(false)
  {}

//

INLINE CFindFileC::~CFindFileC()
  {
  if(m_boInFind) FindClose();
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                ATTRIBUTES                                                          //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CStdString CFindFileC::GetFileName() const
  {
  if(!m_boInFind) return "";
  if(m_pSubLevel) return m_pSubLevel->GetFileName();
  return m_FindData.name;
  }

//

INLINE CStdString CFindFileC::GetFilePath() const
  {
  if(!m_boInFind) return "";
  if(m_pSubLevel) return m_pSubLevel->GetFilePath();
  return GetFileRoot()+GetFileName();
  }

//

INLINE CStdString CFindFileC::GetFileRoot() const
  {
  if(!m_boInFind) return "";
  if(m_pSubLevel) return m_pSubLevel->GetFileRoot();
  return m_strFileRoot;
  }

//

INLINE u32 CFindFileC::GetFileSize() const
  {
  if(!m_boInFind) return 0;
  if(m_pSubLevel) return m_pSubLevel->GetFileSize();
  return m_FindData.size;
  }

//

INLINE bool CFindFileC::IsDirectory() const
  {
  if(!m_boInFind) return false;
  if(m_pSubLevel) return m_pSubLevel->IsDirectory();
  return((m_FindData.attrib & _A_SUBDIR) != 0);
  }

//

INLINE bool CFindFileC::IsDots() const
  {
  if(!m_boInFind)    return false;
  if(m_pSubLevel)    return m_pSubLevel->IsDots();

  if(!IsDirectory()) return false;
  return(m_FindData.name[0] == '.');
  }

//

INLINE bool CFindFileC::IsReadOnly() const
  {
  if(!m_boInFind) return false;
  if(m_pSubLevel) return m_pSubLevel->IsReadOnly();
  return((m_FindData.attrib & _A_RDONLY) != 0);
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_FINDFILEC_INL_
