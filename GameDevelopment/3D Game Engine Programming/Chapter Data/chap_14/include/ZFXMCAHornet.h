// File: ZFXMCAHornet.h
#ifndef ZFXMCAHORNET_H
#define ZFXMCAHORNET_H

#include "ZFXGeneral.h"

class ZFXMCAHornet : public ZFXMCAircraft {
   public:
   	ZFXMCAHornet(void);
   	virtual ~ZFXMCAHornet(void);

      virtual void Update(float fElapsedTime);

      virtual void SetStartParams(ZFXVector vcPos, float fPower);

   protected:
      virtual void Init(void);

      virtual void SetAllTables(void);

      void SetTableCx(void);
      void SetTableCz(void);
      void SetTableCm(void);
      void SetTableCl(void);
      void SetTableCn(void);
      void SetTableClAilr(void);
      void SetTableClRddr(void);
      void SetTableCnAilr(void);
      void SetTableCnRddr(void);
      void SetTableDamp(void);
      void SetTableThrust(void);
   };

#endif
