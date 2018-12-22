/*
   STL Vectors 2 Demo - Chapter 2
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<vector>

using namespace std;


class DummyClass
{
   public:
      DummyClass(int val) : m_member(val)
      {
      
      }

      int GetMember()
      {
         return m_member;
      }

   private:
      int m_member;
};


int main(int args, char **argc)
{
   int i = 0;
   vector<DummyClass*> array;

   array.push_back(new DummyClass(10));
   array.push_back(new DummyClass(20));
   array.push_back(new DummyClass(30));
   array.push_back(new DummyClass(40));
   array.push_back(new DummyClass(50));


   cout << "Array contents(" << "Size: " << (int)array.size() <<
           " Capacity: " << (int)array.capacity() << ") - ";

   for(i = 0; i < (int)array.size(); i++)
      {
         cout << array[i]->GetMember() << " ";
      }

   cout << endl;


   for(vector<DummyClass*>::iterator it = array.begin(); it != array.end(); ++it)
      {
         if((*it) != NULL)
            {
               cout << "Deleting DummyClass(" <<
                       (*it)->GetMember() << ")" << endl;

               delete (*it);
               (*it) = NULL;
            }
      }

   cout << endl << endl;

   return 1;
}