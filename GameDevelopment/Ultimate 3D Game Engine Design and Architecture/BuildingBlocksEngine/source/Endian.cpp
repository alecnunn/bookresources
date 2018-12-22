/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<Endian.h>
#include<assert.h>


DECLARE_ENGINE_NAMESPACE


Endian::Endian()
{
   m_systemEndian = BB_ENDIAN_UNKNOWN;

   unsigned long data = 0x12345678;
   unsigned char *ptr = (unsigned char*)&data;

   if(*ptr == 0x12 && *(ptr+1) == 0x34 &&
      *(ptr+2) == 0x56 && *(ptr+3) == 0x78)
      {
         m_systemEndian = BB_ENDIAN_BIG;
      }
   else if(*ptr == 0x78 && *(ptr + 1) == 0x56 &&
           *(ptr + 2) == 0x34 && *(ptr + 3) == 0x12)
      {
         m_systemEndian = BB_ENDIAN_LITTLE;
      }
   else if(*ptr == 0x34 && *(ptr + 1) == 0x12 &&
           *(ptr + 2) == 0x78 && *(ptr + 3) == 0x56)
      {
         m_systemEndian = BB_ENDIAN_MIDDLE;
      }
}


Endian::~Endian()
{

}


void Endian::ResolveEndian(char *data, int size, BB_ENDIAN_TYPE inputEndian)
{
   if(m_systemEndian == BB_ENDIAN_UNKNOWN ||
      inputEndian == BB_ENDIAN_UNKNOWN ||
      m_systemEndian == inputEndian)
      {
         return;
      }


   int half = size / 2;


   // Test for middle vs big else middle vs little.
   if((m_systemEndian == BB_ENDIAN_MIDDLE &&
      inputEndian == BB_ENDIAN_BIG) ||
      (m_systemEndian == BB_ENDIAN_BIG &&
      inputEndian == BB_ENDIAN_MIDDLE))
      {
         SwapBytes(data, half);
         SwapBytes(data + half, half);

         return;
      }
   else if((m_systemEndian == BB_ENDIAN_MIDDLE &&
           inputEndian == BB_ENDIAN_LITTLE) ||
           (m_systemEndian == BB_ENDIAN_LITTLE &&
           inputEndian == BB_ENDIAN_MIDDLE))
      {
         // Switch little to big then middle.
         SwapBytes(data, size);
         SwapBytes(data, half);
         SwapBytes(data + half, half);

         return;
      }


   // Little to Big or Big to Little Swap (NO MIDDLE).
   SwapBytes(data, size);
}


void Endian::SwapBytes(char *data, int size)
{
	assert((size & 1) == 0);

	char *ptr = data;
	char temp = 0;

	for(int i = 0, j = size - 1; i < size / 2; i++, j--)
	   {
		   temp = ptr[i];
		   ptr[i] = ptr[j];
		   ptr[j] = temp;
	   }
}


void Endian::SwapBytes(char *data, int size, int number)
{
	assert((size & 1) == 0);

	char *ptr = data;

	for(int n = 0; n < number; n++, ptr += size)
	   {
		   SwapBytes(ptr, size);
      }
}


END_ENGINE_NAMESPACE