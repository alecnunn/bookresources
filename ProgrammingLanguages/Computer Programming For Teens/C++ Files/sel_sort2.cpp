//Chapter 19 : Selection Sort

#include <iostream.h>
void load_array(int group[10]); // this is the function heading

void load_array(int group[10])
{
int m;
for ( m = 0; m < 10; m++)  //  m++ is the same as m= m + 1
{
cout << "Please type a number. \n"; // '\n' within a cout statement causes a line feed.
cin >> group[m];
}
cout << endl<<endl;
}



int main ()
{

int i,j,k, min_index, temp, min, m;

j = 9;

int list[10];

load_array (list);

cout << "This is the array as it looks unsorted."<< endl;

for ( m = 0; m < 10; m++)  
{
cout << list[m]<< endl;
}

// Now we begin the selection sort


for (i = 0; i < j; i++)
{
 min = list[i];
min_index = i;

for ( k = i+1; k <= j; k = k + 1)
{ if (list[k] < min )
{min = list[k];
min_index = k;
}
/* this loop finds the smallest number from the rest of the array starting at 
position k.  min_index is the slot of the smallest number.*/
}
 // the smallest number will be swapped into the selected position.
temp= list[i];
list[i]= list[min_index];
list[min_index] = temp;

}

cout<< "This is the array after the selection sort is performed."<< endl;

for ( m = 0; m < 10; m++)  
{
cout << list[m]<< endl;
}



return 0;

}//end of selection sort


