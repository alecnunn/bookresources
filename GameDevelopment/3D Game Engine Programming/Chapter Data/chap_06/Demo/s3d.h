// sample implementation for a loader

#ifndef ZFXMODELL_H
#define ZFXMODELL_H

#include <stdio.h>
#include "ZFX.h"
#include "ZFXRenderDevice.h"
#include "main.h"


typedef struct TRI_TYPE {
   WORD i0, i1, i2;  // original indices
   WORD n0, n1, n2;  // new indices
   UINT nMat;
   } TRI;

typedef int (*CMPFUNC) (const void *arg1, const void *arg2);
int SortTris(const TRI *arg1, const TRI *arg2);

class ZFXModell {
   protected:
      ZFXRenderDevice *m_pDevice;
      
      UINT    m_nNumSkins;
      UINT   *m_pSkins;

      UINT    m_nNumVertices;
      VERTEX *m_pVertices;
      
      VERTEX3T *m_pVertices3T;
      UINT     *m_pBufferID3T;

      UINT    m_nNumIndices;
      WORD   *m_pIndices;

      UINT   *m_pCount;    // indices per material
      UINT   *m_pBufferID; // static buffers
      
      FILE   *m_pFile;
      FILE   *m_pLog;
      bool    m_bReady;

      void ReadFile(void);

   public:
      ZFXModell(const char *chFile, ZFXRenderDevice *pDevice, FILE *pLog);
      ~ZFXModell(void);

      HRESULT Render(bool bStatic, bool b3T);
   };
/*----------------------------------------------------------------*/

#define SEEK(str, key) while (instr(str,key)==-1) fgets(str,80,m_pFile);
#define NEXT(str) fgets(str, 80, m_pFile);
int instr(const char *string, const char *substring);

#endif
