/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_ENDIAN_H_
#define _BB_ENDIAN_H_


#include<NameSpace.h>
#include<Defines.h>


DECLARE_ENGINE_NAMESPACE


class Endian
{
   public:
      Endian();
      virtual ~Endian();

      void ResolveEndian(char *data, int size,
                         BB_ENDIAN_TYPE inputEndian);

      void SwapBytes(char *data, int size);
      void SwapBytes(char *data, int size, int number);

      BB_ENDIAN_TYPE GetEndianType()
      {
         return m_systemEndian;
      }


   private:
      BB_ENDIAN_TYPE m_systemEndian;
};


END_ENGINE_NAMESPACE

#endif