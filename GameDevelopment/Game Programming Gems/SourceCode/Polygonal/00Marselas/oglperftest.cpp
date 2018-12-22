//==============================================================================
// oglperftest.cpp
//
// Copyright (c) 2000, herbert marselas (hmarselas@ensemblestudios.com)
//
// Permission is given to freely use and incorporate 
// this source code with attribution
//
//==============================================================================

#include "stdafx.h"
#include <GL/gl.h>
#include <GL/glaux.h>
#include "oglperftest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//==============================================================================
//
//==============================================================================
CWinApp theApp;

using namespace std;

FILE *pfile;

GLuint texname;

//==============================================================================
//
//==============================================================================
GLenum gerrcode;
#ifdef _DEBUG
#define CheckOGLError { ASSERT((gerrcode = glGetError()) == GL_NO_ERROR); }
#else
#define CheckOGLError
#endif

//==============================================================================
//
//==============================================================================
extern "C" typedef void (APIENTRY *LOCKARRAYS_PROC)(int first, int count);
extern "C" typedef void (APIENTRY *UNLOCKARRAYS_PROC)(void);

LOCKARRAYS_PROC lpfnLockArrays;
UNLOCKARRAYS_PROC lpfnUnlockArrays;

//==============================================================================
//
//==============================================================================
DWORD dwWhite = 0x000000ff;
DWORD dwBlack = 0xffffffff;

DWORD dwMaxTiles = 50;

DWORD dwMaxIterations = 5;
DWORD dwCurrentIteration;

DWORD dwXRes = 800;
DWORD dwYRes = 600;

//==============================================================================
// GL_T2F_C3F_V3F
//==============================================================================
class CInterleavedVertex 
{
   public:
      float mTexCoord[2];  // s r
      float mColor[3];     // r g b
      float mVertex[3];    // x y z

}; // CInterleavedVertex

CInterleavedVertex *pvertices;

//==============================================================================
// ptrs for stream data
//==============================================================================
float *pTexCoordStream;
float *pColorStream;
float *pVertexStream;

//==============================================================================
// trace
//==============================================================================
void trace(const char *lpszFormat, ...)
{
   long nBuf;
   const long cMaxBytes = 8192;
   char szBuffer[cMaxBytes];

   va_list args;
   va_start(args, lpszFormat);

   nBuf = vsprintf(szBuffer, lpszFormat, args);

   ASSERT(nBuf >= 0);
   ASSERT(nBuf < cMaxBytes);

   if (nBuf)
      if (szBuffer[nBuf-1] != '\n') 
      {
         szBuffer[nBuf] = '\n';
         szBuffer[nBuf+1] = 0;
      }

   ASSERT(nBuf+1 < cMaxBytes);

   OutputDebugString(szBuffer);

   va_end(args);

} // trace

//==============================================================================
// getTimeStamp
//==============================================================================
void getTimeStamp(__int64 &qtime)
{
    DWORD   dwLoDword;
    DWORD   dwHiDword;
        
    _asm 
    {
        _emit 0x0f                      // "Fake" RDTSC instruction (0x0f31)
        _emit 0x31

        mov dwLoDword, eax              // Move Lo Timestamp into mem
        
        mov dwHiDword, edx              // Move Hi Timestamp into mem
    }

   qtime = dwHiDword;
   qtime = qtime << 32;
   qtime |= dwLoDword;

} // getTimeStamp

//==============================================================================
// getFrequency
//==============================================================================
double getFrequency(void)
{
   static double ffreq = 0.0;
   static bool bcalculated = false;

   HANDLE hSemaphore;
   __int64 mqwStart, mqwEnd;
   HANDLE hthread;
   HANDLE hprocess;
   long nOldPriority;
   DWORD dwOldClass;
   HCURSOR hcursor;

   if (bcalculated)
      if (ffreq > 0.0)
         return ffreq;

   hcursor = SetCursor(LoadCursor(0, IDC_WAIT));

   hthread = GetCurrentThread();

   hprocess = GetCurrentProcess();

   hSemaphore = CreateSemaphore(0, 1, 1, 0);

   WaitForSingleObject(hSemaphore, 1000);

 

   nOldPriority = GetThreadPriority(hthread);

   dwOldClass = GetPriorityClass(hprocess);


   SetPriorityClass(hprocess, REALTIME_PRIORITY_CLASS);

   SetThreadPriority(hthread, THREAD_PRIORITY_TIME_CRITICAL);


   getTimeStamp(mqwStart);

   WaitForSingleObject(hSemaphore, 5000);

   getTimeStamp(mqwEnd);


   SetPriorityClass(hprocess, dwOldClass);

   SetThreadPriority(hthread, nOldPriority);


   ReleaseSemaphore(hSemaphore, 0, 0);

   CloseHandle(hSemaphore);

   ffreq = (double) (mqwEnd - mqwStart) / 5000000.0;

   bcalculated = true;

   SetCursor(hcursor);

   return ffreq;

} // GetFrequency

//==============================================================================
//
//==============================================================================
extern "C" void CALLBACK renderStreamLocked(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   CheckOGLError
   static bool bFirst = true;

   __int64 qtime[2];

   // stuff to do the first time

   if (bFirst)
   {
      bFirst = false;

      // enable the arrays

      glEnableClientState(GL_COLOR_ARRAY);
      CheckOGLError

      glEnableClientState(GL_VERTEX_ARRAY);
      CheckOGLError

      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      CheckOGLError


      // set the array ptrs

      glVertexPointer(3, GL_FLOAT, 0, pVertexStream);
      CheckOGLError

      glTexCoordPointer(2, GL_FLOAT, 0, pTexCoordStream);
      CheckOGLError
   
      glColorPointer(3, GL_FLOAT, 0, pColorStream);
      CheckOGLError

      // lock the arrays

      if (lpfnLockArrays)
      {
         (*lpfnLockArrays)(0, 6 * dwMaxTiles);
         CheckOGLError
      }
      else
      {
         fprintf(pfile, "Warning! API missing CVA extension\n");
      }
   }
   // draw it!

   getTimeStamp(qtime[0]);
   glDrawArrays(GL_TRIANGLES, 0, 6 * dwMaxTiles);
   getTimeStamp(qtime[1]);

   CheckOGLError

   fprintf(pfile, "%I64u\t%I64u\n", qtime[0], qtime[1]);

   glFlush();
   CheckOGLError

   // invalidate the window for the next render
   InvalidateRect(auxGetHWND(), 0, TRUE);

   // keep going until we hit the max count

   dwCurrentIteration++;

   if (dwCurrentIteration > dwMaxIterations)
   {
      // unlock

      if (lpfnUnlockArrays)
      {
         (*lpfnUnlockArrays)();
         CheckOGLError
      }
      else
      {
         fprintf(pfile, "Warning! API missing CVA extension\n");
      }

      // for cleanliness (we don't know who else may use this) we disable states

      glDisableClientState(GL_COLOR_ARRAY);
      CheckOGLError

      glDisableClientState(GL_VERTEX_ARRAY);
      CheckOGLError

      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      CheckOGLError

      // close window
      auxCloseWindow();
   }

} // renderStreamLocked

//==============================================================================
//
//==============================================================================
extern "C" void CALLBACK renderStream(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   CheckOGLError

   __int64 qtime[5];


   // enable the arrays

   glEnableClientState(GL_COLOR_ARRAY);
   CheckOGLError

   glEnableClientState(GL_VERTEX_ARRAY);
   CheckOGLError

   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   CheckOGLError


   // set the array ptrs

   getTimeStamp(qtime[0]);
   glVertexPointer(3, GL_FLOAT, 0, pVertexStream);
   CheckOGLError

   getTimeStamp(qtime[1]);
   glTexCoordPointer(2, GL_FLOAT, 0, pTexCoordStream);
   CheckOGLError
   
   getTimeStamp(qtime[2]);
   glColorPointer(3, GL_FLOAT, 0, pColorStream);
   CheckOGLError

   // draw it!

   getTimeStamp(qtime[3]);
   glDrawArrays(GL_TRIANGLES, 0, 6 * dwMaxTiles);


   getTimeStamp(qtime[4]);
   CheckOGLError

   fprintf(pfile, "%I64u\t%I64u\t%I64u\t%I64u\t%I64u\n", qtime[0], qtime[1], qtime[2], qtime[3], qtime[4]);

   glFlush();
   CheckOGLError

   // for cleanliness (we don't know who else may use this) we disable states

   glDisableClientState(GL_COLOR_ARRAY);
   CheckOGLError

   glDisableClientState(GL_VERTEX_ARRAY);
   CheckOGLError

   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   CheckOGLError


   // invalidate the window for the next render
   InvalidateRect(auxGetHWND(), 0, TRUE);

   // keep going until we hit the max count

   dwCurrentIteration++;

   if (dwCurrentIteration > dwMaxIterations)
      auxCloseWindow();

} // renderStream

//==============================================================================
//
//==============================================================================
extern "C" void CALLBACK renderInterleaved(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   CheckOGLError

   __int64 qtime[4];


   getTimeStamp(qtime[0]);
   glInterleavedArrays(GL_T2F_C3F_V3F, 0, pvertices);
   getTimeStamp(qtime[1]);
   CheckOGLError

   getTimeStamp(qtime[2]);
   glDrawArrays(GL_TRIANGLES, 0, 6 * dwMaxTiles);
   getTimeStamp(qtime[3]);
   CheckOGLError

   fprintf(pfile, "%I64u\t%I64u\t%I64u\t%I64u\n", qtime[0], qtime[1], qtime[2], qtime[3]);

   glFlush();
   CheckOGLError

   // invalidate the window for the next render
   InvalidateRect(auxGetHWND(), 0, TRUE);

   // keep going until we hit the max count

   dwCurrentIteration++;

   if (dwCurrentIteration > dwMaxIterations)
      auxCloseWindow();

} // renderInterleaved

//==============================================================================
// renderImmediateMode
//==============================================================================
extern "C" void CALLBACK renderImmediateMode(void)
{
   CInterleavedVertex *p = pvertices;

   glClear(GL_COLOR_BUFFER_BIT);
   CheckOGLError

   __int64 qtime[8];

   getTimeStamp(qtime[0]);
      glBegin(GL_TRIANGLES);
   getTimeStamp(qtime[1]);

   for (DWORD i = 0; i < (6 * dwMaxTiles); i++, p++)
   {
      getTimeStamp(qtime[2]);
         
         glTexCoord2f(p->mTexCoord[0], p->mTexCoord[1]);

      getTimeStamp(qtime[3]);

         glColor4f(p->mColor[0], p->mColor[1], p->mColor[2], 1);

      getTimeStamp(qtime[4]);

         glVertex3f(p->mVertex[0], p->mVertex[1], p->mVertex[2]);

      getTimeStamp(qtime[5]);

      fprintf(pfile, "%I64u\t%I64u\t%I64u\t%I64u\n", qtime[2], qtime[3], qtime[4], qtime[5]);
   }
  
   getTimeStamp(qtime[6]);
   glEnd();
   getTimeStamp(qtime[7]);

   fprintf(pfile, "%I64u\t%I64u\n", qtime[1] - qtime[0], qtime[7] - qtime[6]);

   CheckOGLError

   glFlush();
   CheckOGLError

   // invalidate the window for the next render
   InvalidateRect(auxGetHWND(), 0, TRUE);

   // keep going until we hit the max count

   dwCurrentIteration++;

   if (dwCurrentIteration > dwMaxIterations)
      auxCloseWindow();

} // renderImmediateMode

//==============================================================================
//
//==============================================================================
void createVertexData(void)
{
   float fxpixel = 1;
   float fypixel = 1;
   float dx = 2;
   float dy = 2;
   float fzvalue = 1.0f;

   // allocate

   CInterleavedVertex *p;

   pvertices = new CInterleavedVertex[dwMaxTiles * 6]; // 6 discrete vertices per tile

   p = pvertices;

   // fill

   for (DWORD i = 0; i < dwMaxTiles; i++, fxpixel += dx)
   {
      // if we're wider than the window, increment to the beginning of the next line
      if (fxpixel >= dwXRes)
      {
         fypixel += dy;
         fxpixel = 0;
      }

      //   . .    <- 2nd and 3rd verts
      //   .      <- 1st vert

      // 1st vert

      p->mTexCoord[0] = 0;
      p->mTexCoord[1] = 0;
      
      p->mColor[0] = 1;
      p->mColor[1] = 0;
      p->mColor[2] = 0;

      p->mVertex[0] = fxpixel;
      p->mVertex[1] = fypixel;
      p->mVertex[2] = fzvalue;

      p++;

      // 2nd vert

      p->mTexCoord[0] = 1;
      p->mTexCoord[1] = 0;
      
      p->mColor[0] = 0;
      p->mColor[1] = 1;
      p->mColor[2] = 0;

      p->mVertex[0] = fxpixel;
      p->mVertex[1] = fypixel + dy;
      p->mVertex[2] = fzvalue;

      p++;

      // 3rd vert

      p->mTexCoord[0] = 1;
      p->mTexCoord[1] = 1;
      
      p->mColor[0] = 0;
      p->mColor[1] = 0;
      p->mColor[2] = 1;

      p->mVertex[0] = fxpixel + dx;
      p->mVertex[1] = fypixel + dy;
      p->mVertex[2] = fzvalue;

      p++;

      //    .  <- 4th vert
      //  , ,  <- 6th and 5th verts

      // 4th vert

      p->mTexCoord[0] = 1;
      p->mTexCoord[1] = 1;
      
      p->mColor[0] = 0;
      p->mColor[1] = 0;
      p->mColor[2] = 1;

      p->mVertex[0] = fxpixel + dx;
      p->mVertex[1] = fypixel + dy;
      p->mVertex[2] = fzvalue;

      p++;

      // 5th vert

      p->mTexCoord[0] = 0;
      p->mTexCoord[1] = 1;
      
      p->mColor[0] = 0;
      p->mColor[1] = 1;
      p->mColor[2] = 0;

      p->mVertex[0] = fxpixel + dx;
      p->mVertex[1] = fypixel;
      p->mVertex[2] = fzvalue;

      p++;

      // 6th vert

      p->mTexCoord[0] = 0;
      p->mTexCoord[1] = 0;
      
      p->mColor[0] = 1;
      p->mColor[1] = 0;
      p->mColor[2] = 0;

      p->mVertex[0] = fxpixel;
      p->mVertex[1] = fypixel;
      p->mVertex[2] = fzvalue;

      p++;

   } // for (...)

   // allocate and copy into streams

   pTexCoordStream = new float[dwMaxTiles * 6 * 2]; // 2 tx coords

   pColorStream = new float[dwMaxTiles * 6 * 3]; // r g b
   
   pVertexStream  = new float[dwMaxTiles * 6 * 3]; // x y z


   // fill streams from seeded interleaved array

   for (i = 0, p = pvertices; i < (dwMaxTiles * 6); i++, p++)
   {
      memcpy(&pTexCoordStream[i * 2], &p->mTexCoord[0], 2 * sizeof(float));

      memcpy(&pColorStream[i * 3], &p->mColor[0], 3 * sizeof(float));

      memcpy(&pVertexStream[i * 3], &p->mVertex[0], 3 * sizeof(float));
   }

} // createVertexData

//==============================================================================
// initScene
//==============================================================================
void initScene(void)
{
   glClearColor(0, 0, 0, 1);
   CheckOGLError

   glClear(GL_COLOR_BUFFER_BIT);
   CheckOGLError

   glDisable(GL_CULL_FACE);

   glDisable(GL_DEPTH_TEST);

   glShadeModel(GL_SMOOTH);

   // build checkerboard texture

   const long dwSpan = 8;

   DWORD dwTexture[dwSpan][dwSpan];

   for (DWORD i = 0; i < dwSpan; i++)
      for (DWORD j = 0; j < dwSpan; j++)
         if (j % 2)
            dwTexture[i][j] = dwWhite;
         else
            dwTexture[i][j] = dwBlack;

   // load checkerboard texture

   glGenTextures(1, &texname);
   CheckOGLError

   glBindTexture(GL_TEXTURE_2D, texname);
   CheckOGLError

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dwSpan, dwSpan, 0, GL_RGBA, GL_UNSIGNED_BYTE, dwTexture);
   CheckOGLError

   // see if we can map the CVA extension

   lpfnLockArrays = (LOCKARRAYS_PROC) wglGetProcAddress("glLockArraysEXT");

   lpfnUnlockArrays = (UNLOCKARRAYS_PROC) wglGetProcAddress("glUnlockArraysEXT");

   if (!lpfnLockArrays || !lpfnUnlockArrays)
   {
      AfxMessageBox("Warning - CVA NOT Available !!!");
   }


} // initScene

//==============================================================================
// _tmain
//==============================================================================
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
   double ffreq = 450;
	int nRetCode = 0;

   // let it init MFC so we can use it's container classes if we want

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
      return 1;
	}

   // setup window

   auxInitDisplayMode(AUX_SINGLE | AUX_RGBA);

   auxInitPosition(0, 0, dwXRes, dwYRes);
   
   auxInitWindow("Simple OpenGL Performance Test");

   // output basic info

   pfile = fopen("output.txt", "wt");

   if (!pfile)
   {
      AfxMessageBox("error - unable to open output.txt");
      return 1;
   }
   
//   ffreq = getFrequency();

   fprintf(pfile, "cpu\t%f\n", ffreq);

   // create list of data

   createVertexData();

   // init scene

   initScene();

   // main loop

   // uncomment the test you want to run - supa haX0r

//   dwMaxIterations = 5;
//   auxMainLoop(renderImmediateMode); 

//   dwMaxIterations = 50;
//   auxMainLoop(renderInterleaved);

//   dwMaxIterations = 50;
//   auxMainLoop(renderStream);

   dwMaxIterations = 50;
   auxMainLoop(renderStreamLocked);

   // all done

   delete [] pvertices;

   delete [] pTexCoordStream;
   delete [] pColorStream;
   delete [] pVertexStream;

   fclose(pfile);

	return nRetCode;

} // _tmain

//==============================================================================
// eof: oglperftest.cpp
//==============================================================================
