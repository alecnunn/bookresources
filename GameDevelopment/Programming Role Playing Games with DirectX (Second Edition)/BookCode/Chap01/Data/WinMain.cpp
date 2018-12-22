/**************************************************
WinMain.cpp
Chapter 1 Data Storage Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
**************************************************/

// Include files
#include <windows.h>
#include <stdio.h>

class cDataPackage
{
  protected:
    // Data buffer and size
    void *m_Buf;
    unsigned long m_Size;

  public:
    cDataPackage()  { m_Buf = NULL; m_Size = 0; }
    ~cDataPackage() { Free();                   }

    void *Create(unsigned long Size)
    {
      // Free a previously created buffer
      Free();
  
      // Allocate some memory and return a pointer
      return (m_Buf = (void*)new char[(m_Size = Size)]);
    }

    // Free the allocated memory
    void Free() { delete m_Buf; m_Buf = NULL; m_Size = 0; }

    BOOL Save(char *Filename)
    {
      FILE *fp;

      // Make sure there's something to write
      if(m_Buf != NULL && m_Size) {
        // Open file, write size and data
        if((fp=fopen(Filename, "wb")) != NULL) {
          fwrite(&m_Size, 1, 4, fp);
          fwrite(m_Buf, 1, m_Size, fp);
          fclose(fp);
          return TRUE;
        }
      }
 
      return FALSE;
    }

    void *Load(char *Filename, unsigned long *Size)
    {
      FILE *fp;
     
      // Free a prior buffer
      Free();
 
      if((fp=fopen(Filename, "rb"))!=NULL) {
        // Read in size and data
        fread(&m_Size, 1, 4, fp);
        if((m_Buf = (void*)new char[m_Size]) != NULL)
          fread(m_Buf, 1, m_Size, fp);
        fclose(fp);
 
        // Store size to return
        if(Size != NULL)
          *Size = m_Size;
 
        // return pointer
          return m_Buf;
      }

      return NULL;
    }
};

// A structure to contain a name
typedef struct {
  char Name[32];
} sName;

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,           \
                   LPSTR szCmdLine, int nCmdShow)
{
  cDataPackage DP;
  DWORD Size;

  // Create the data package (w/64 bytes) and get the
  // pointer, casting it to an sName structure type.
  sName *Names = (sName*)DP.Create(64);

  // Since there are 64 bytes total, and each name uses
  // 32 bytes, then I can have 2 names stored.
  strcpy(Names[0].Name, "Jim");
  strcpy(Names[1].Name, "Adams");

  // Save the names to disk and free the data buffer
  DP.Save("names.dat");
  DP.Free();

  // Load the names from disk. Size will equal 64
  // when the load function returns.
  Names = (sName*)DP.Load("names.dat", &Size);

  // Display the names
  MessageBox(NULL, Names[0].Name, "1st Name", MB_OK);
  MessageBox(NULL, Names[1].Name, "2nd Name", MB_OK);

  // Free up the data package
  DP.Free();

  return 0;
}
