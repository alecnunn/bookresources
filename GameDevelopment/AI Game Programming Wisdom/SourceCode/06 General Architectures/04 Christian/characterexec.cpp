
#include "characterexec.h"


CharacterExec::CharacterExec ()
{
    m_owner = NULL;
}

void CharacterExec::init ()
{
    return;
}

bool CharacterExec::start  ()
{
    return false;
}

bool CharacterExec::finish ()
{
}

void CharacterExec::reset ()
{
}

IEOwner * CharacterExec::getOwner ()
{
    return m_owner;
}

const char * getName()
{
