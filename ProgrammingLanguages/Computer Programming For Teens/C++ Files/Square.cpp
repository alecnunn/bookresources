
// Chapter 22 (online): Inplementation file for a square class. 

#include <iostream.h>
#include "Square.h"


Square:: Square ( )  // a default constructor which is a constructor that sets
// all attributes of the object to 0 or null.
{
side = 0;

}

Square::Square ( double s)  // a constructor
{

side = s;


}


double Square::Get_Length ( )  // an accessor because it accesses an attribute of the object
{
return side;

}

double Square::Get_Area ( )
{
double ans;
ans = side * side;
return ans;
}


double Square::Get_Perimeter ()
{
double ans ;
ans = 4 * side;
return ans;

}

 void Square::Change_Side ( double s ) // a modifier because it alters an attribute of the object
 {
 side = s;
 // now the side has the value in s.
 
 }

// end of implementation file
