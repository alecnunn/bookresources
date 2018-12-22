// Chapter 16 : Displays contents of an external file

#include <iostream.h>
#include <fstream.h>
#include<string.h>

int main ()
{

int x; char  phone[8];
const char * file = "Phone_Numbers.dat";
ifstream fin;
 fin.open(file , ios::in|ios::binary); 
 
 if( fin.is_open())
 { 
    for (x = 0; x < 10; x++)
{
fin>>phone;
cout << phone<<endl; 
}

cout<<endl;

  }
 
fin.close (); 

cout<<"This worked."<<endl; 
return 0;
}
