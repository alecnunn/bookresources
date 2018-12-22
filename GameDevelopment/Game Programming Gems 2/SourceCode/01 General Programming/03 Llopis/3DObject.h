/* Copyright (C) Noel Llopis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Noel Llopis, 2001"
 */
class 3DObject : public GameObject, public IRenderable {
  public:
    virtual ~3DObject();
    virtual bool QueryInterface (const GameInterfaceType type,
                                 void ** pObj );
    virtual bool Render();
    // Some more functions if needed
};

