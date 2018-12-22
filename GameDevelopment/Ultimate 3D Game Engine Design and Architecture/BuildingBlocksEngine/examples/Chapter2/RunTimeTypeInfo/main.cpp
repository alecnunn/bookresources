/*
   Run-Time Type Information Demo - Chapter 2
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<RTTI.h>

using namespace std;
using namespace bbe;


class Base
{
   BB_RTTI_EXPOSE_TYPE;

   public:
      Base() {}
      virtual ~Base() {}
};

RTTI Base::m_RTTIType("Base", NULL);


class Derived : public Base
{
   BB_RTTI_EXPOSE_TYPE;
   BB_RTTI_DEFINE_SUPER(Base);

   public:
      Derived() {}
      virtual ~Derived() {}
};

RTTI Derived::m_RTTIType("Derived", &Base::m_RTTIType);


int main(int args, char **argc)
{
   Base *ptr = NULL;
   Base *base1 = new Base();
   Derived *derived1 = new Derived();

   if(!base1) cout << "Error creating base1 object.\n";
   if(!derived1) cout << "Error creating derived1 object.\n";

   cout << endl;
   
   if(!base1 || !derived1) return 0;


   cout << "base1 is of type: " << base1->m_RTTIType.GetName() << ".\n";
   cout << "derived1 is of type: " << derived1->m_RTTIType.GetName() << ".\n";

   cout << "base1 is derived from Derived: ";
   if(base1->m_RTTIType.IsOfType(&Derived::m_RTTIType)) cout << "Yes." << "\n";
   else cout << "No." << "\n";

   cout << "derived1 is derived from Base: ";
   if(derived1->m_RTTIType.IsOfType(&Base::m_RTTIType)) cout << "Yes." << "\n";
   else cout << "No." << "\n";


   cout << endl;


   ptr = BB_RTTI_SAFE_CAST(base1, Derived);

   if(ptr) cout << "Cast base1 to Derived was successfull.\n";
   else cout << "Cast base1 to Derived failed.\n";
   
   ptr = BB_RTTI_SAFE_CAST(derived1, Base);

   if(ptr) cout << "Cast derived1 to Base was successfull.\n";
   else cout << "Cast derived1 to Base failed.\n";


   cout << endl;


   delete base1;
   delete derived1;

   return 0;
}