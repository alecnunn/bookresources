/* Copyright (C) Jesse Laeuchli, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jesse Laeuchli, 2001"
 */
#include <iostream.h>
#include <fstream.h>
#include<math.h>
#define mapsize 64
float heightmap[mapsize][mapsize];
void savefractal(int x, int y)
{
	fstream file;
	file.open("test2.raw",ios::out,filebuf::sh_none);
	for(int a=0;a<y;a++)
	for(int b=0;b<x;b++)
	{
		file<<(heightmap[a][b])<<" ";
	}
	file<<endl;
}
double cosineinterpolation(double number1,double number2,double x)
{
	double ft;
	double f;
	double ret;
	ft = x * 3.1415927;
	f = (1 - cos(ft)) * .5;
	ret=number1*(1-f) + number2*f;
	return ret;
}
double randomnumber(int x,int y)
{
	int n=x+y*57;
	n=(n<<13)^n;
	double ret;
	ret= (1 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 2147483647) / 1073741824.0);
	return ret;    
}
float smoothrandom(int x,int y)
{
	float corners=(randomnumber(x-1,y-1)+randomnumber(x+1,y-1)+randomnumber(x-1,y+1)+randomnumber(x+1,y+1))/16;
	float sides   = (randomnumber(x-1, y)+randomnumber(x+1, y)+randomnumber(x, y-1)+randomnumber(x, y+1) ) /  8;
	float center  =  randomnumber(x, y) / 4;   
	float ret=corners+sides+center;
	return ret;
}
float noise(float x, float y)
{
	int xinterger=x;
	float fractionx=x- xinterger;
	int yinteger=y;
	float fractiony=y- yinteger;
	float v1,v2,v3,v4,i1,i2;
	float ret;
	v1=smoothrandom(xinterger, yinteger);
	v2=smoothrandom(xinterger + 1, yinteger);
	v3=smoothrandom(xinterger, yinteger + 1);
	v4=smoothrandom(xinterger + 1, yinteger + 1);
	i1= cosineinterpolation (v1,v2,fractionx);
	i2= cosineinterpolation (v3,v4,fractionx);
	ret = cosineinterpolation (i1,i2,fractiony);
	return ret;
}

float MultiFractal(float x, float y, float octaves, float amplitude, float frequency, float h,float offset)
{
	float ret=1;
	for(int i=0;i<(octaves-1);i++)
	{
		ret *=(offset)*( noise (x* frequency, y* frequency)* amplitude);
		amplitude*=h;
	}
	return ret;	
}



void main()
{
	cout<<"Octaves"<<endl;
	float oct=0;
	float am=0;
	float freq=0;
	float h;
	float startx=0;
	float starty=0;
	float offset
	cin>>oct;
	cout<<"Amplitude"<<endl;
	cin>>am;
	cout<<"Frequency"<<endl;
	cin>>freq;
	cout<<"H"<<endl;
	cin>>h;
	cout<<"Start X"<<endl;
	cin>>startx;
	cout<<"Start Y"<<endl;
	cin>>starty;
	cout<<"Offset";
	cin>>offset;
	float temp=0;
	for(int y=0;y<mapsize;y++)
	for(int x=0;x<mapsize;x++)
	{
		temp=MultiFractal(x+startx,y+starty,oct,am,freq,h,offset);
		heightmap[y][x]=temp;
	}
	savefractal(mapsize,mapsize);
}


