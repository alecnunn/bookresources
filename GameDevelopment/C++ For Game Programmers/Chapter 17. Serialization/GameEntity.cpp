
#include "GameEntity.h"
#include "Stream.h"
#include "AddressTranslator.h"
#include <assert.h>
#include <iostream>


const std::string & GameEntity::GetName () const
{
    return m_name;
}


void GameEntity::SetName(const std::string & name)
{
    m_name = name;
}


bool GameEntity::Write(IStream & stream) const
{
    WriteString (stream, m_name);
    WriteInt (stream, GetNumChildren());

    if (GetNumChildren() > 0)
    {
        std::vector<GameEntity *>::const_iterator it;
        for (it=m_children.begin(); it!=m_children.end(); ++it)
            WriteInt(stream, (int)(*it));
    }

    return true;
}


bool GameEntity::Read(IStream & stream)
{
    m_name = ReadString(stream);

    m_children.clear();
    int numChildren = ReadInt (stream);
    if (numChildren > 0)
    {
        for (int i=0; i<numChildren; ++i)
            m_children.push_back((GameEntity *)ReadInt(stream));
    }
    return true;
}


void GameEntity::Fixup()
{
    int numChildren = GetNumChildren();
    for (int i=0; i<numChildren; ++i)
        m_children[i] = (GameEntity *)AddressTranslator::TranslateAddress(m_children[i]);
}


const std::string GameEntity::GetClass() const
{
    return std::string("GameEntity");
}

int GameEntity::GetNumChildren() const
{
    return m_children.size();
}

GameEntity * GameEntity::GetChild(int index)
{
    assert (index >= 0 && index < GetNumChildren());
    return m_children[index];
}


void GameEntity::AddChild(GameEntity * pChild)
{
    assert (pChild != NULL);
    m_children.push_back(pChild);
}


void GameEntity::RemoveChild (int index)
{
    assert (index >= 0 && index < GetNumChildren());
    m_children.erase(m_children.begin()+index);
}


void GameEntity::Print(int indent) const
{
    for (int col=0; col<indent; ++col)
        std::cout << "  ";

    std::cout << GetName() << "(" << GetClass() << ")";
    std::cout << " Children: " << GetNumChildren() << std::endl;
}

