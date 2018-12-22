//Chapter 18: Binary Search    

#include <iostream.h>
void load_array(int group[20]); // this is the function heading

void load_array(int group[20])
{
int m;
for ( m = 0; m < 20; m++)  //  m++ is the same as m= m + 1
{
cout << "Please type a number. \n";
cin >> group[m];
}
cout << endl<<endl;
}



int main ()
{

int left, mid, right, num;
left = 0;
right = 19;

int list[20];

load_array (list);

cout << "For what number would you like to search?"<< endl;
cin>> num;

mid = (left + right)/2;

while  (list[mid] != num && right >= left)
{
if (num < list[mid])
right= mid - 1;
else 
left = mid + 1;
mid = (left + right) /2; // we reestablish the middle position

}

if (list[mid] == num)
cout << num <<  "  was found in the "<< mid << " slot of the array."<< endl;
else cout << num << "  was not found in the list.\n";
cout<<"Remember that the array's first slot is the zero slot.";
return 0;

}//end of binary search


