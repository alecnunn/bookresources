
// Chapter 10: array manipulation

#include <iostream.h>


int main ()
{

int group[10];

int m;
// this for loop will initialize all the members to zero.
for ( m = 0; m < 10; m++)  //  m++ is the same as m= m + 1
{

 group[m] = 0;
}
 
for ( m = 0; m < 10; m++)  
{
cout<<  group[m] << endl;  // prints the array members
}

for ( m = 0; m < 10; m++) 
{ 
cout << "Please type a number. \n";
cin >> group[m];  // allows the user to type values at the keyboard
}
cout<<endl; // leaves a blank line
for ( m = 0; m < 10; m++)  
{
cout<<  group[m] << endl;  // prints the array members again
}

 
return 0;
}//end of program


