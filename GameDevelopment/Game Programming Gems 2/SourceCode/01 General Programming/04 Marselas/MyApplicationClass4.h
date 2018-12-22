/* Copyright (C) Herb Marselas, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Herb Marselas, 2001"
 */
class CMyApplicationClass : public CMyExportedClass
{
   public:
      CMyApplicationClass(void)  {  }

      virtual void setValue(long dwValue);
      virtual long getValue(void);
};
