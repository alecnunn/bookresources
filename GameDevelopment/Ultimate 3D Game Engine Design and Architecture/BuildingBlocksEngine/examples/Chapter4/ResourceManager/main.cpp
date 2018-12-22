/*
   Resource Manager Demo - Chapter 4
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<ResourceManager.h>

using namespace std;


class TestTag { };

class TestResource
{
   public:
      TestResource(int i) : m_int(i) {}
      ~TestResource() { }

      void Set(int i) { m_int = i; }
      int Get() { return m_int; }

   private:
      int m_int;
};


int main(int args, char **argc)
{
   bbe::ResourceManager<TestResource, bbe::Handle<TestTag> > resManager;
   bbe::Handle<TestTag> handle, handle2;
   TestResource *res = NULL;

   resManager.Create(new TestResource(10), "NULL", &handle);
   resManager.Release(handle);
   resManager.Create(new TestResource(20), "NULL", &handle);
   handle2.Initialize(handle.GetIndex());
   resManager.Release(handle);
   res = resManager.GetResource(handle2);

   cout << "Total objects: " << resManager.GetSize() << ".\n";
   cout << "Handle = " << handle.GetIndex() << ".\n";
   cout << "Handle2 = " << handle2.GetIndex() << ".\n";
   if(res) cout << "res = " << res->Get() << ".\n";
   else cout << "res == NULL,\n";

   return 0;
}