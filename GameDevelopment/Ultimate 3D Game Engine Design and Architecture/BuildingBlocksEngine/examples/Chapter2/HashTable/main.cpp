/*
   Hash Table Demo - Chapter 2
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<string>
#include<HashTable.h>

using namespace std;


int main(int args, char **argc)
{
   bbe::HashTable<int> hashTable(21);
   bbe::HashItem<int> item;

   item.SetKey(112);
   item.SetObj(348);
   hashTable.Insert(item);

   item.SetKey(87);
   item.SetObj(841);
   hashTable.Insert(item);

   item.SetKey(24);
   item.SetObj(654);
   hashTable.Insert(item);

   item.SetKey(66);
   item.SetObj(11);
   hashTable.Insert(item);

   item.SetKey(222);
   item.SetObj(156);
   hashTable.Insert(item);

   item = hashTable.Find(87);
   cout << "Item: " << item.GetKey() << " has a value of " << item.GetObject() << ".\n";

   item = hashTable.Find(112);
   cout << "Item: " << item.GetKey() << " has a value of " << item.GetObject() << ".\n";
   
   hashTable.Delete(66);
   item = hashTable.Find(66);
   cout << "Item: " << item.GetKey() << " has a value of " << item.GetObject() << ".\n";
   
   item = hashTable.Find(11);
   cout << "Item: " << item.GetKey() << " has a value of " << item.GetObject() << ".\n";

   cout << "\n";
   
   string str("cats");
   int stringHash = hashTable.HashFunction(str);

   cout << "The string cats hash to " << stringHash << ".\n\n";
   
   return 1;
}