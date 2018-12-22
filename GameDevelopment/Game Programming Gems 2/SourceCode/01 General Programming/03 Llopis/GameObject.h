/* Copyright (C) Noel Llopis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Noel Llopis, 2001"
 */
class GameObject {
  public:
    enum GameInterfaceType {
      IRENDERABLE,
      IOTHERINTERFACE
    };

    virtual bool QueryInterface (const GameInterfaceType type,
                                 void ** pObj );
    // The rest of the GameObject declaration
};
