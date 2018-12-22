import java.io.*;
import java.util.*;

// Chapter 23 ( online)
public class Matrixtest
{ 

public static void main ( String[] Args )
{
int group[][]= new int [6][4] ;

for (int n = 0; n<4; n++)
{

group[0][n]=5;  // assigns the first row which is row 0.
}

for (int n = 0; n<4; n++)
{

System.out.println(group[0][n]);
}


for (int n = 0; n<4; n++)
{

group[1][n]=10;  // assigns the second row which is row 1.
}

for (int n = 0; n<4; n++)
{

System.out.println(group[1][n]);
}

// now nested for loops to assign the matrix

for (int m = 0; m<6; m++)
{
   for ( int n = 0; n<4; n++) 
     {
       group[m][n]= m+n;  // assigns the sum (m +n) to each member.
     }
     
}     

//now nested for loops to see the contents of the matrix

for (int m = 0; m<6; m++)
{
   for ( int n = 0; n<4; n++) 
     {
      System.out.print (group[m][n]);  
     }
System.out.println(); // causes a line feed in between lines     
}     



}  // ends main


}// ends program




