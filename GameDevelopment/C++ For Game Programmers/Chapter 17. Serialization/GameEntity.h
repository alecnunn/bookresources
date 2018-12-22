
#ifndef GAMEENTITY_H_
#define GAMEENTITY_H_


#include "Serializable.h"
#include <string>
#include <vector>


class GameEntity: public ISerializable
{
public:
    GameEntity() {} ;
    GameEntity(const std::string & name) : m_name(name) {};

    const std::string & GetName () const;
    void SetName(const std::string & name);

    // Implement ISerializable members
    virtual bool Write(IStream & stream) const;
    virtual bool Read(IStream & stream);
    virtual void Fixup();

    // Functions specific to GameEntity
    virtual const std::string GetClass() const;
    virtual void  Print(int indent) const;

    int GetNumChildren() const;
    GameEntity * GetChild(int index);
    void AddChild(GameEntity * pChild);
    void RemoveChild (int index);

private:
    std::string               m_name;
    std::vector<GameEntity *> m_children;
};



#endif