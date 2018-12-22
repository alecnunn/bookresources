/*
   Deque Demo - Chapter 2
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<deque>

using namespace std;


int main(int args, char **argc)
{
   deque<int> normalQueue;

   normalQueue.push_front(20);
   normalQueue.push_front(10);
   normalQueue.push_front(80);
   normalQueue.push_front(74);
   normalQueue.push_front(15);
   normalQueue.push_front(320);

   normalQueue.push_back(3);
   normalQueue.push_back(63);
   normalQueue.push_back(21);
   normalQueue.push_back(87);

   cout << "Deque contents (" << "Size: " <<
           (int)normalQueue.size() <<") - ";

   while(normalQueue.empty() == false)
      {
         cout << normalQueue.front() << " ";
         normalQueue.pop_front();
      }

   cout << endl << endl;

   return 1;
}