

#ifndef FACTORY_H_
#define FACTORY_H_

#pragma warning (disable : 4786)  

#include <map>
#include <string>


typedef std::string ObjectType;


template<class Base>
class CreatorBase
{
public:
    virtual ~CreatorBase() {}
    virtual Base * Create() const = 0;
};



template<class Product, class Base>
class Creator : public CreatorBase<Base>
{
public:
    virtual Base * Create() const {return new Product;}
};




template<class Base>
class Factory
{
public:
    Base * Create(ObjectType type);
    bool Register(ObjectType type, CreatorBase<Base> * pCreator);
private:
    typedef std::map<ObjectType, CreatorBase<Base> *> CreatorMap;
    CreatorMap m_creatorMap;
};


template<class Base>
bool Factory<Base>::Register(ObjectType type, CreatorBase<Base> * pCreator)
{
    CreatorMap::iterator it = m_creatorMap.find(type);
    if (it != m_creatorMap.end()) {
        delete pCreator;
        return false;
    }
    m_creatorMap[type] = pCreator;
    return true;
}


template<class Base>
Base * Factory<Base>::Create(ObjectType type)
{
    CreatorMap::iterator it = m_creatorMap.find(type);
    if (it == m_creatorMap.end()) 
        return NULL;

    CreatorBase<Base> * pCreator = (*it).second;
    return pCreator->Create();
}



#endif