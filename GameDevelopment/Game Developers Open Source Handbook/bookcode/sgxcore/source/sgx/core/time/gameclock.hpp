#ifndef SGX_CORE_TIME_GAMECLOCK_HPP
#define SGX_CORE_TIME_GAMECLOCK_HPP	1

#include "sgx/core/types/types.hpp"

class CGameClock {
public:
    CGameClock();

    tREAL32		ApplyRealTimeToClock(tREAL32 current_time);
    void		SetModeRealtime();
    void		SetModeConstant(const tREAL32 period);
    void		SetModeConstantFPS(const tREAL32 fps);
    void		SetModeClamped(const tREAL32 min_fps, const tREAL32 max_fps=60);
	void		SetModeClampedTelaps(const tREAL32 min_telaps, const tREAL32 max_telaps);
    tREAL32		GetTime();
	tREAL32		GetLastTelaps();
	tREAL32		GetClampedMin();
	tREAL32		GetClampedMax();
    tREAL32		Update(const tREAL32 telaps);

    typedef enum {
        Realtime,
        AsGiven,
        Constant,
        Clamped
    } CClockMode;

    static	CGameClock * Get();

protected:
    static CGameClock      *sm_pSingleton;

private:
    tREAL32                 m_fLastTime;
    tREAL32                 m_fFractionalSpeed;
    tREAL32                 m_fGivenTime;
    tREAL32                 m_fConstantPeriod;
    tREAL32                 m_fClampedMin;
    tREAL32                 m_fClampedMax;
	tREAL32					m_fLastTelaps;
    CClockMode              m_ClockMode;
};

#endif /* SGX_CORE_TIME_GAMECLOCK_HPP */
