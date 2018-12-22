/* Copyright (C) Herb Marselas, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Herb Marselas, 2001"
 */
#ifdef _BUILDING_MY_DLL
#define DLLFUNCTION __declspec(dllexport)
#else
#define DLLFUNCTION __declspec(dllimport)
#endif

class CMyExportedClass
{
   public:
      CMyExportedClass(void) : mdwValue(0)  {  }

      DLLFUNCTION void setValue(long dwValue) { mdwValue = dwValue; }
      DLLFUNCTION long getValue(void) { return mdwValue; }
      long clearValue(void);

   private:
      long mdwValue;
};

CMyExportedClass *createMyExportedClass(void) { return new CMyExportedClass; }
void deleteMyExportedClass(CMyExportedClass *pclass) { delete pclass; }
