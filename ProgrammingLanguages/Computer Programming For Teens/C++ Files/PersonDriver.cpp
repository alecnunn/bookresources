// Chapter 22 ( online) : the driver for the person object
// This program does not use the more efficient string type.
//We are using arrays-typing one character at a time.

#include <iostream.h>

#include "Person.h"
#include "Person.cpp"



int main ()
{

int x;

char n[10];
char location[10];
 char phon[8];
  int num;

for (x = 0; x < 10; x++) // initializing all slots in the array
{
n[x]=' ';
location[x] =' ';

}

for (x = 0; x < 8; x++) // initializing
{
phon[x]=' ';
}
 
 cout << "Please type your first name one character at a time.\n ";
 
 for (x = 0; x < 10; x++)
{
cin >> n[x];
}
cout<<endl;
 cout << "Please type your address one character at a time.\n ";
 
 for (x = 0; x < 10; x++)
{
cin >> location[x];
}
cout<<endl;
cout << "Please type your phone number one character at a time.\n ";
 
 for (x = 0; x < 8; x++)
{
cin >> phon[x];
}
cout<<endl;
cout << "Please type your age.\n ";

cin>> num;


Person p(n, location, phon, num); // constructing a person object

p. Print_Info();



for (x = 0; x < 10; x++) // initializing all slots in the array
{
location[x] =' ';
}

cout << "Please type your new address one character at a time.\n  ";

 
 for (x = 0; x < 10; x++)
{
cin >> location[x];
}
cout<<endl;
p.Change_Address(location);

cout << " Change the age by typing a new number. \n ";
cin >> num;

p. Change_Age (num);

p. Print_Info();

return 0;

}














