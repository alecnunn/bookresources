

#ifndef ADDRESSTRANSLATOR_H_
#define ADDRESSTRANSLATOR_H_



#pragma warning (disable : 4786)  
#include <hash_map>


class AddressTranslator
{
public:
    static void AddAddress(void * oldAddress, void * newAddress);
    static void * TranslateAddress(void * oldAddress);
    static void Reset();

private:
    typedef std::hash_map<int, int> AddressTable;
    static AddressTable s_table;
};



#endif