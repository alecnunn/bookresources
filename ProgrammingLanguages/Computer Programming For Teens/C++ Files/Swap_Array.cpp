
// Chapter 10: swap array 

#include <iostream.h>


int main ()
{

int group[10];
int list[10];

int m;  int temp;

// this for loop will initialize all the members to zero.

for ( m = 0; m < 10; m++)  //  m++ is the same as m= m + 1
{

 group[m] = 0;
}

// this for loop allows the user to load the array list 

for ( m = 0; m < 10; m++) 
{ 
cout << "Please type a number. \n";
cin >> list[m];  
}
cout << "This list shows what each array contains. "<< endl;
cout <<" Group        "<< "List              "<< endl;

for ( m = 0; m < 10; m++)  
{
cout <<group[m]<<"             "<< list[m] << endl;
 
}

cout<<"Now we will swap the members between the two arrays."<<endl;

// this for loop will allow us to swap all the values between
// list and group - one slot (member) at a time.
for ( m = 0; m < 10; m++)  
{
temp =  group[m];  // temp gets a copy of group1, for example.
group[m] = list[m]; // group1 then gets list1, for example
list[m] = temp; // list1 gets the value that was in temp (the former group1)
}

cout << "This list shows what each array contains after swapping. "<< endl;
cout <<" Group        "<< "List              "<< endl;

for ( m = 0; m < 10; m++)  
{
cout <<group[m]<<"             "<< list[m] << endl;
 
}
 
return 0;
}//end of program


