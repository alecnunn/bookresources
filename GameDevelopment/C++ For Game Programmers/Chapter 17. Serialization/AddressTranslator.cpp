

#include "AddressTranslator.h"


AddressTranslator::AddressTable AddressTranslator::s_table;


void AddressTranslator::AddAddress(void * oldAddress, void * newAddress)
{
    s_table[(int)oldAddress] = (int)newAddress;
}


void * AddressTranslator::TranslateAddress(void * oldAddress)
{
    AddressTable::iterator it = s_table.find((int)oldAddress);
    if (it == s_table.end())
        return NULL;
    return (void *)(*it).second;
}


void AddressTranslator::Reset()
{
    s_table.clear();
}