/*
   STL Queue Demo - Chapter 2
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<vector>
#include<queue>

using namespace std;


template<class T>
class BigToSmall
{
   public:
	   inline bool operator()(T left, T right)
	   {
	      return (left < right);
      }
};


int main(int args, char **argc)
{
   queue<int> normalQueue;
   priority_queue<int, vector<int>,
                  BigToSmall<int> > priorityQueue;

   normalQueue.push(20);
   normalQueue.push(10);
   normalQueue.push(30);

   priorityQueue.push(99);
   priorityQueue.push(50);
   priorityQueue.push(2035);
   priorityQueue.push(35);
   priorityQueue.push(10);

   cout << "Queue contents (" << "Size: " <<
           (int)normalQueue.size() <<") - ";

   while(normalQueue.empty() == false)
      {
         cout << normalQueue.front() << " ";
         normalQueue.pop();
      }

   cout << endl;

   cout << "Priority Queue contents (" << "Size: " <<
           (int)priorityQueue.size() <<") - ";

   while(priorityQueue.empty() == false)
      {
         cout << priorityQueue.top() << " ";
         priorityQueue.pop();
      }

   cout << endl << endl;

   return 1;
}