
// Chapter 22 (online): Header file for a square class. 

#include <iostream.h>
#ifndef _SQUARE_H
#define _SQUARE_H

class Square
{
public:

Square ( );

Square ( double side);



double  Get_Length ( );

double Get_Area ( );
double Get_Perimeter ();
void Change_Side ( double s );

private:

   double side;
  
};

#endif//end of class definition


