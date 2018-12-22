//Example using functions Chapter 8

#include <iostream.h>
int add(int x,int y);
int subtract (int x, int y );
int multiply (int x,int y );
double divide (double x,double y );

int add(int x,int y)
{
int m;
 m = x + y;
 return m;
}

int subtract (int x, int y )
{
int m;
 m = x - y;
 return m;
}


int multiply (int x,int y )
{
int m;
 m = x * y;
 return m;
}

double divide (double x,double y )
{
double m;
 m = x / y;
 return m;
}



int main ()
{

int a,b;
int ans_add, ans_sub, ans_mult; double result;


cout << "Please type two integers."<< endl;
cin>>a >> b;

ans_add = add ( a,b);
ans_sub = subtract (a,b);
ans_mult = multiply (a,b);
result = divide ( a, b ); // this works since the integers will be converted to doubles!

cout << "The answers to the four problems we executed are as follows: "<< endl;
cout << a << "+" << b << " =  " << ans_add << endl;
cout << a << "-" << b << " =  " << ans_sub << endl;
cout << a << "*" << b << " =  " << ans_mult << endl;
cout << a << "/" << b << " =  " << result << endl;

return 0;

}//end of program


