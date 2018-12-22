/* Copyright (C) Noel Llopis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Noel Llopis, 2001"
 */
bool 3DObject::QueryInterface (const GameInterfaceType type,
                               void ** pObj ) {
  bool bSuccess = false;
  if ( type == GameObject::IRENDERABLE ) {
    *pObj = static_cast<IRenderable *>(this);
    bSuccess = true;
  }
  return bSuccess;
}
