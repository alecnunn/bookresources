
// Chapter 22 (online): Driver file for a square class. 

#include <iostream.h>
#include "Square.h"
#include "Square.cpp"


int main ()
{
double length, area, perim, y;
Square s1 (4); // constructs the square object

   length = s1.Get_Length();
   cout<< "Length of a side is "<< length<<endl;
   
   area= s1.Get_Area();
   cout << "Area of the square is  "<< area<< endl;
   
  
   perim = s1.Get_Perimeter();
   cout<< "Perimeter of the square is "<< perim<< endl;
   
   cout<< " What length would you like the side of the square to be?"<< endl;
   cin >> y;
   
   s1.Change_Side(y);
   
   cout<< "We can confirm that the side length changed by calling the Get_Length function."<<endl;
   
   length = s1.Get_Length();
   cout<< "The new length is " << length << endl;
   
   cout << "Now call some of the other functions." << endl;
   
   length = s1.Get_Length();
   cout<< "Length of a side is "<< length<<endl;
   
   area= s1.Get_Area();
   cout << "Area of the square is  "<< area<< endl;
   
   Square s2 (s1);
   
   cout<< " We just created another square with s1's information. "<< endl;
   cout <<"Let's call some functions to see if the values are the same. "<< endl;
   
    length = s2.Get_Length();
   cout<< "Length of a side is "<< length<<endl;
   
   area= s2.Get_Area();
   cout << "Area of the square is  "<< area<< endl;
   
  
   
   return 0;
   
   }
   
   
   
   
   
   

