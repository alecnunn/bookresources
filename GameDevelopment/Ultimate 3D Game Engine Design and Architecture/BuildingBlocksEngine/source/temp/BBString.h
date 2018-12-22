/*

*/


#ifndef _BBE_STRING_H_
#define _BBE_STRING_H_


class CString
{
   public:
      CString(char *str) : m_size(0)
      {
         CreateString(str);
      }
      
      CString(CString &str)
      {
         CreateString(str.GetString());
      }

      virtual ~CString()
      {
         if(m_string) delete[] m_string;
         m_string = NULL;
         m_size = 0;
      }

      void CreateString(char *str)
      {
         int size = 0;

         if(str != 0)
            {
               if(m_string) delete[] m_string;
               size = strlen(str);
               m_string = ((size + 1) > 0) ? new char[size + 1] : 0;

               if(m_string)
                  {
                     m_size = size;
                     memcpy(m_string, str, size);
                     m_string[m_size] = '\0';
                  }
            }
      }

      void operator=(CString &str) { CreateString(str.GetString()); }

      int GetSize() { return m_size; }
      char *GetString() { return m_string; }

   private:
      char *m_string;
      int m_size;
};

#endif