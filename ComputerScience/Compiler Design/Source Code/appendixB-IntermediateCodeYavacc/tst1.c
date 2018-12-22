#include<stdio.h>
int x,y,z;
int main()
{
int a,b,c,

	a=10;
	b=20;
	x=30;
	y=25;
	while(a < b)
	{
		c=a++;
		if(x > y)
			x=30;
		else
			x=y;
	}
	z = x + y;
}