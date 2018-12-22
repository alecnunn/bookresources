// Chapter 20 : Factorial (uses recursion)

#include <iostream.h>

int Fact( int n); // this is the function heading

int Fact( int n) // see how little code this function has.
{
if (n==1)
return 1;
else return n * Fact(n-1);
}


int main ()
{

int x, ans;
 
cout <<"Please type a positive number (not too large !) "<<
"for which we will find the factorial."<< endl;
 cin >> x;
 while ( x <= 0)
 {
 cout << "Sorry. You need to type a positive number. => " << endl;
 cin >> x;
 }
  ans = Fact(x);// the function is being called.
  cout << "The factorial of the number " << x << " is " << ans << endl; 
 
 return 0;
}//end of program


