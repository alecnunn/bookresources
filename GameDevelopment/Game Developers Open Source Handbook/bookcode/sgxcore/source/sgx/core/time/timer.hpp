#ifndef SGX_CORE_TIME_TIMER_HPP
#define SGX_CORE_TIME_TIMER_HPP	1

#include "sgx/core/types/types.hpp"

class CHighTimer {
public:
    static  CHighTimer     *Get();

    virtual                ~CHighTimer() {}

    virtual void            Start()      {}
    virtual tREAL32         GetTime()    { return 0; }

protected:
    static  CHighTimer     *ms_pSingleton;

};

#endif /* SGX_CORE_TIME_TIMER_HPP */
