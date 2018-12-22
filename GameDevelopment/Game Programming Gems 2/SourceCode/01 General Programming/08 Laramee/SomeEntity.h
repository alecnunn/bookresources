/* Copyright (C) François Dominic Laramée, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) François Dominic Laramée, 2001"
 */
class SomeEntity : public Entity
{
  // The function pointer
	void ( * friendptr )( Entity * me, Entity * target );

public:
  // Declare one or more strategy functions as friends
  friend void Strategy1( Entity * me, Entity * target );
  ...

  // The actual operation
  void HandleInteractions( Entity * target ) 
  {
    (*friendptr) ( this, target );
  }
};
