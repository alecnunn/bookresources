/*
   STD Lists Demo - Chapter 2
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<list>

using namespace std;


int main(int args, char **argc)
{
   int i = 0;
   list<int> linkList;
   list<int>::iterator it;

   linkList.push_back(10);
   linkList.push_back(20);
   linkList.push_back(30);
   linkList.push_back(40);
   linkList.push_back(50);

   cout << "Contents of the link list: ";

   for(it = linkList.begin(); it != linkList.end(); it++)
      {
         cout << *it << " ";
      }

   cout << endl;

   linkList.pop_back();
   linkList.pop_back();

   cout << "Contents of the link list: ";

   for(it = linkList.begin(); it != linkList.end(); it++)
      {
         cout << *it << " ";
      }

   cout << endl << endl;

   return 1;
}