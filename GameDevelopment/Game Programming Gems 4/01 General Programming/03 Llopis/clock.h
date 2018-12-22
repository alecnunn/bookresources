
#ifndef CLOCK_H_
#define CLOCK_H_


#include <cstddef>
#include <vector>
#include <deque>

class TimeSource;
class IObserver;


class Clock
{
public:
    Clock(const TimeSource * pSource = NULL);
    ~Clock();

    void SetTimeSource(const TimeSource * pSource);

    void FrameStep();

    double GetTime() const { return m_currentTime; }
    double GetFrameDuration() const { return m_frameTime; }
    int GetFrameNumber() const { return m_frameNumber; }
    float GetFrameRate() const;

    void AddObserver (IObserver * pObserver);
    void RemoveObserver (IObserver * pObserver);

    void SetFiltering (int frameWindow, double frameDefault = 0.030);

private:
    double GetExactLastFrameDuration ();
    void AddToFrameHistory (double exactFrameDuration);
    double GetPredictedFrameDuration () const;


    const TimeSource * m_pTimeSource;
    double m_currentTime;
    double m_frameTime;
    int m_frameNumber;

    double m_sourceStartValue;
    double m_sourceLastValue;

    int    m_frameFilteringWindow;
    double m_frameDefaultTime;
    std::deque<double> m_frameDurationHistory;

    typedef std::vector<IObserver *> ObserverList;
    ObserverList m_observers;
};



#endif