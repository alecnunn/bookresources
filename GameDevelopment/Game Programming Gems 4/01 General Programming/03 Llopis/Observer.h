
#ifndef OBSERVER_H_
#define OBSERVER_H_


class IObserver
{
public:
    virtual ~IObserver() {}
    virtual void Notify() = 0;
};


#endif
