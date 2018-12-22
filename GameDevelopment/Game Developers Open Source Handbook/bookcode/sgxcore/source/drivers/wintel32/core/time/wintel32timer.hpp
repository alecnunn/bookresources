#ifndef SGX_WINTEL32_TIMER_HPP
#define SGX_WINTEL32_TIMER_HPP	1

class CPCHighTimer : public CHighTimer {
public:
    static  CHighTimer     *Get();

    CPCHighTimer();
    virtual ~CPCHighTimer();

    void                    Start();
    tREAL32                 GetTime();

private:
    LARGE_INTEGER           m_Frequency;
    tREAL64                 m_FrequencyTime;

    LARGE_INTEGER           m_Start;
};

#endif /* SGX_WINTEL32_TIMER_HPP */
