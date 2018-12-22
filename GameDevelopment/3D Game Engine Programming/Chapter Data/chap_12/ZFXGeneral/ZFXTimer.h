// File: ZFXTimer.h
#ifndef  ZFXTIMER_H
#define  ZFXTIMER_H



class ZFXTimer {
   public:
      ZFXTimer(unsigned char nHH, unsigned char nMM);
      ~ZFXTimer();

      void  Update(void);

      void  SetClock(unsigned char nHH,
                     unsigned char nMM);

      char* GetClock(unsigned char *nHH, 
                     unsigned char *nMM,
                     char *pChar); 

      float GetTimeStamp(void)      { return m_fStamp; }
      float GetElapsed(void)        { return (m_fTime_elapsed*m_fFaktor); }
      float GetFPS(void)            { return (1/m_fTime_elapsed); }
      float GetScale(void)          { return m_fFaktor; }
      void  SetScale(float fFaktor) { m_fFaktor = fFaktor; }

   private:
	   LONGLONG m_nCur_time;         // current time
	   LONGLONG m_nPerf_cnt;         // performance timer frequency
	   bool     m_blnPerf_flag;      // flag for timer to use
	   LONGLONG m_nLast_time;	      // time of previous frame
	   float    m_fTime_elapsed;     // time elapsed since previous frame
	   float    m_fTime_scale;       // scaling factor for time
      UCHAR    m_nHH;               // clock time hours
      UCHAR    m_nMM;               // clock time minutes
      UCHAR    m_nSS;               // clock time seconds
      float    m_fClock;            // sum up milliseconds
      float    m_fFaktor;           // slowmo or speedup
      float    m_fStamp;            // unique timestamp
   };

#endif