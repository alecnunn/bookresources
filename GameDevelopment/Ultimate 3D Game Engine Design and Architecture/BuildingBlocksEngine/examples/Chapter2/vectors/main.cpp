/*
   STL Vectors Demo - Chapter 2
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<vector>

using namespace std;


int main(int args, char **argc)
{
   int i = 0;
   vector<int> array;

   array.push_back(10);
   array.push_back(20);
   array.push_back(30);
   array.push_back(40);
   array.push_back(50);

   cout << "Array contents(" << "Size: " << (int)array.size() <<
           " Capacity: " << (int)array.capacity() << ") - ";

   for(i = 0; i < (int)array.size(); i++)
      {
         cout << array[i] << " ";
      }

   cout << endl;

   array.pop_back();
   array.pop_back();

   cout << "Array contents(" << "Size: " << (int)array.size() <<
           " Capacity: " << (int)array.capacity() <<
           ") after 2 pop_back() - ";

   for(i = 0; i < (int)array.size(); i++)
      {
         cout << array[i] << " ";
      }

   cout << endl;

   array.clear();

   cout << "Array stats(" << "Size: " << (int)array.size() <<
           " Capacity: " << (int)array.capacity() <<
           ") after clear. ";

   cout << endl << endl;

   return 1;
}