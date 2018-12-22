#include "main.h"

/***************************************************************/
Debug::Debug()
{
#ifdef DPRINTF
   AllocConsole();

   int32 cFile = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
   FILE * pFILE = _fdopen(cFile,"w");
   if (pFILE != NULL)
      *stdout = *pFILE;

   cFile = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);
   pFILE = _fdopen(cFile,"r");
   if (pFILE != NULL)
      *stdin = *pFILE;

   mpFile = fopen("debug.txt", "w");
   
#endif
}
/***************************************************************/
Debug::~Debug()
{
#ifdef DPRINTF
   fclose(mpFile);
#endif
}
/***************************************************************/
void Debug::Printf(char *fmt, ...)
{
#ifdef DPRINTF
   
   va_list marker;
   char buffer[2000];

   va_start(marker, fmt);

   // Catch bad dprintfs
   try
   {
      if (_vsnprintf(buffer, sizeof buffer, fmt, marker) < 0)
         throw;
   }
   catch (...)
   {
      _snprintf(buffer, sizeof buffer, "Bad dprintf, format string = %.*s",
                sizeof buffer - 100, fmt);
   }

   va_end(marker);

   printf("%s", buffer);

   fprintf(mpFile,"%s", buffer);
   
#endif
}
