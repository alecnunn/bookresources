

#ifndef RESOURCEPTRHOLDER_H_
#define RESOURCEPTRHOLDER_H_


class IResource;


class ResourcePtrHolder
{
public:
    ResourcePtrHolder (IResource * p) : pRes(p) {}
    ~ResourcePtrHolder();
    IResource * pRes;
};



#endif
