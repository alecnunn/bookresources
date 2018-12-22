//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE NODE CLASS                                                   //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINENODE_INL_
#define   _GPG4_ENGINENODE_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                TRFS GET/SET                                                        //
//--------------------------------------------------------------------------------------------------------------------//

INLINE const CVect3D& CEngineNode::GetPosition() const
  {
  return m_v3Pos;
  }

INLINE const CQuaternion& CEngineNode::GetRotation() const
  {
  return m_qRot;
  }

INLINE const CVect3D& CEngineNode::GetScale() const
  {
  return m_v3Scale;
  }

//

INLINE CVect3D CEngineNode::GetWorldPosition()
  {
  if(!m_pParentNode) return m_v3Pos;
  return CVect3D(GetWorldTrf().Col[POS],W);                 // or m_pParentNode->GetWorldTrf()*m_v3Pos
  }

INLINE CQuaternion CEngineNode::GetWorldRotation()
  {
  if(!m_pParentNode) return m_qRot;
  return m_qRot*m_pParentNode->GetWorldRotation();
  }

INLINE CVect3D CEngineNode::GetWorldScale()
  {
  if(!m_pParentNode) return m_v3Scale;
  return Prod(m_v3Scale,m_pParentNode->GetWorldScale());
  }

//

INLINE const CMat4x4& CEngineNode::GetLocalTrf()
  {
  return UpdateLocalTrf();
  }

INLINE const CMat4x4& CEngineNode::GetWorldTrf()
  {
  return UpdateWorldTrf();
  }

INLINE bool CEngineNode::IsLocalDirty() const
  {
  return m_boLocalDirty;
  }

INLINE bool CEngineNode::IsWorldDirty() const
  {
  return m_boWorldDirty;
  }

//

INLINE bool CEngineNode::SetPosition(const CVect3D& v3Pos)
  {
  m_v3Pos = v3Pos;
  SetLocalDirty();
  return true;
  }

INLINE bool CEngineNode::SetRotation(const CQuaternion& qRot)
  {
  m_qRot = qRot;
  SetLocalDirty();
  return true;
  }

INLINE bool CEngineNode::SetScale(const CVect3D& v3Scale)
  {
  m_v3Scale = v3Scale;
  SetLocalDirty();
  return true;
  }

INLINE const CMat4x4& CEngineNode::SetLocalTrf(const CVect3D& v3Pos,const CQuaternion& qRot,const CVect3D& v3Scale)
  {
  m_v3Pos   = v3Pos;
  m_qRot    = qRot;
  m_v3Scale = v3Scale;
  SetLocalDirty();
  return UpdateLocalTrf();
  }

INLINE const CMat4x4& CEngineNode::SetLocalTrf(const CVect3D& v3Pos,const CQuaternion& qRot,const CVect3D& v3Scale,const CMat4x4& m4Trf)
  {
  m_v3Pos   = v3Pos;
  m_qRot    = qRot;
  m_v3Scale = v3Scale;
  m_m4Local = m4Trf;

  m_boLocalDirty = false;
  SetWorldDirty();
  return m4Trf;
  }

//

INLINE void CEngineNode::SetLocalDirty()
  {
  m_boLocalDirty = true;
  SetWorldDirty();
  }

INLINE void CEngineNode::SetWorldDirty()
  {
  m_boWorldDirty = true;

  CEngineNode* pNode = GetFirstSubnode();
  while(pNode)
    {
    pNode->SetWorldDirty();                                 // propagate
    pNode = GetNextSubnode();
    }
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                FLAGS GET/SET                                                       //
//--------------------------------------------------------------------------------------------------------------------//

INLINE bool CEngineNode::GetDrawNode() const
  {
  return m_boDrawNode;
  }

INLINE void CEngineNode::SetDrawNode(const bool boDraw)
  {
  m_boDrawNode = boDraw;
  }

INLINE bool CEngineNode::GetDrawMesh() const
  {
  return m_boDrawMesh;
  }

INLINE void CEngineNode::SetDrawMesh(const bool boDraw)
  {
  m_boDrawMesh = boDraw;
  }

INLINE bool CEngineNode::GetCollide() const
  {
  return m_boCollide;
  }

INLINE void CEngineNode::SetCollide(const bool boColl)
  {
  m_boCollide = boColl;
  }

INLINE CStdString CEngineNode::GetEditName() const
  {
  return "Node";
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                SUBNODES                                                            //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEngineNode* CEngineNode::GetParentNode() const
  {
  return m_pParentNode;
  }

INLINE void CEngineNode::SetParentNode(CEngineNode* pParent)
  {
  m_pParentNode = pParent;
  SetWorldDirty();
  }

//

INLINE u32 CEngineNode::GetNbSubnodes() const
  {
  return m_Subnodes.size();
  }

INLINE CEngineNode* CEngineNode::GetFirstSubnode()
  {
  m_CurrSubnode = m_Subnodes.begin();
  return GetNextSubnode();
  }

INLINE CEngineNode* CEngineNode::GetNextSubnode(CEngineNode* pCurrent)
  {
  // using pCurrent is slow, but enables multiple GetFirst/GetNext at the same time with only 1 m_CurrSubnode

  if(pCurrent)
    {
    m_CurrSubnode = std::find(m_Subnodes.begin(),m_Subnodes.end(),pCurrent);
    if(m_CurrSubnode == m_Subnodes.end()) return NULL;
    ++m_CurrSubnode;
    }

  //

  if(m_CurrSubnode == m_Subnodes.end()) return NULL;
  CEngineNode* pNode = *m_CurrSubnode;
  ++m_CurrSubnode;
  return pNode;
  }

//

INLINE bool CEngineNode::AddSubnode(CEngineNode* pNode)
  {
  if(!pNode) return false;
  m_Subnodes.push_back(pNode);
  pNode->SetParentNode(this);
  return true;
  }

INLINE bool CEngineNode::RemoveSubnode(CEngineNode* pNode)
  {
  if(!pNode) return false;
  m_Subnodes.remove(pNode);
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_ENGINENODE_INL_
