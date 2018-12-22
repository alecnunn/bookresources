//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR TEXTURE CLASS                                                //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EditorTexture.h"
#ifdef    _DEBUG
#include  "EditorTexture.inl"
#endif

#include  "EngineTexture.h"
#include  "Properties.h"
#include  "..\RTTIPropsDoc.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CEditorTexture,CEditorObj);

//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE                                                              //
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorTexture::CreateEngineObj()
  {
  CEngineObj* pEngineObj = CEngineObj::CreateObject(&CEngineTexture::m_RTTI);
  if(!pEngineObj) return false;

  pEngineObj->SetName(m_strName);
  pEngineObj->SetEditorObj(this);
  SetEngineObj(pEngineObj);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITING                                                             //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- SpecialEditing -----------------------------------------------------//
// called when a property needs a special dlg
// in : property,new value,document
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEditorTexture::SpecialEditing(CProperty* pProp,CStdString* pstrNew,CRTTIPropsDoc* pDoc)
  {
  if(!pProp || !pstrNew || !pDoc) return CEditorObj::SpecialEditing(pProp,pstrNew,pDoc);
  CStdString strRes = pProp->GetValue(m_spEngineObj);

  if(IsPropOfRTTI(pProp,&CEngineTexture::m_RTTI))
    {
    if(pProp->GetName() == "File")
      {
      CEngineTexture* pTex = DYNAMIC_CAST(CEngineTexture,m_spEngineObj);
      if(!pTex) return false;
      CStdString strFile = pTex->GetFileName();

	    CFileDialog dlgOpen(TRUE,"BMP/PCX/TGA",NULL,OFN_NOCHANGEDIR,"Texture Files (*.bmp; *.pcx; *.tga)|*.bmp; *.pcx; *.tga||");
      dlgOpen.m_ofn.lpstrInitialDir = CRTTIPropsDoc::GetTexturePath();
      if(dlgOpen.DoModal() != IDOK) return false;

      CStdString strChoosen = dlgOpen.GetFileName();
      if(strChoosen == strFile) return false;
      *pstrNew = strChoosen;
      return true;
      }
    }

  return CEditorObj::SpecialEditing(pProp,pstrNew,pDoc);
  }

//----------------------------------------------- SpecialGetValue ----------------------------------------------------//
// called when a property needs special displaying in the grid
// in : property,new value
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

CStdString CEditorTexture::SpecialGetValue(CProperty* pProp)
  {
  if(!pProp) return CEditorObj::SpecialGetValue(pProp);
  CStdString strRes = pProp->GetValue(m_spEngineObj);

  if(IsPropOfRTTI(pProp,&CEngineTexture::m_RTTI))
    {
    if(pProp->GetName() == "File")
      {
      CEngineTexture* pTex = DYNAMIC_CAST(CEngineTexture,m_spEngineObj);
      if(!pTex) return strRes;
      CStdString strFile = pTex->GetFileName();

      if(strFile.empty()) return "NO FILE DEFINED";
      return strFile;
      }
    }

  return strRes;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
