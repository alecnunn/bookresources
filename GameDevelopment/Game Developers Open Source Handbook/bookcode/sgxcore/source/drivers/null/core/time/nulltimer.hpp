#ifndef SGX_NULL_TIMER_HPP
#define SGX_NULL_TIMER_HPP	1

class CNullHighTimer : public CHighTimer {
public:
    static  CHighTimer     *Get();

    CNullHighTimer();
    virtual ~CNullHighTimer();

    void                    Start();
    tREAL32                 GetTime();

private:

};

#endif /* SGX_NULL_TIMER_HPP */
