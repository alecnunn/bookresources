//
// Land demo for OpenPTC 1.0 C++ API
//
// Based on Heightmap example from Hornet (RIP)
// PTC version Copyright (c) 1998 Marcus Fletcher (cus@commsat.demon.co.uk)
//
// Updated to OpenPTC 1.0 by Glenn Fiedler (ptc@gaffer.org)
//
// Cursor keys to move, <Pause> to brake and <Esc> to quit
//

#include "ptc.h"
#include <stdio.h>
#include <math.h>

#define PI 3.141592654

#define SCREENWIDTH  320
#define SCREENHEIGHT 200

char8 HMap[256*256];  // Height field
char8 CMap[256*256];  // Color map

int lasty[SCREENWIDTH],  // Last pixel drawn on a given column
    lastc[SCREENWIDTH];  // Color of last pixel on a column

int  CosT [2048], 
  SinT [2048];  // Cosine and Sine tables
int  FOV=256;   // half of the xy field of view (This is based on the 0-2048 convention)

//
// Function prototypes
//
void ComputeMap(void);
int Clamp(int x);
void Line(int x0,int y0,int x1,int y1,int hy,int s, int32 *surface_buffer, int fadeout);
void View(int x0,int y0,int angle, int height, int32 *surface_buffer);
void InitTables (void);

//
//     Main function
//
int APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nCmdShow)
{
    try
    {
        Format format(32, 0x00FF0000, 0x0000FF00, 0x000000FF);
        Console console;
        console.open("Land demo",SCREENWIDTH,SCREENHEIGHT,format);
        Surface surface(SCREENWIDTH,SCREENHEIGHT,format);
   
  //
  // Compute the height map
  //
  ComputeMap();
  InitTables();

  int k, x0, y0;
  k=x0=y0=0;

  int height=-200;
  double angle=0;
  double deltaAngle=0;
  double deltaSpeed=4096;
  double CurrentSpeed=deltaSpeed*10;

        // time scaling constant
        const double scale = 20;

        // create timer
        Timer timer;

        // start timer
        timer.start();

        // main loop
        while (true)
        {
            // get time delta between frames
            const double delta = timer.delta();
            
            // clear surface
            surface.clear();
            
            // lock surface pixels
            int32 *pixels = (int32*) surface.lock();

            // draw current landscape view
            View(x0,y0,(int)angle,height,pixels);

            // unlock surface
            surface.unlock();

            // copy surface to console
            surface.copy(console);
            
            // update console
            console.update();

            // check key press
            while (console.key())
            {
                // read key press
                Key key = console.read();

                // handle key press
                switch (key.code())
                {
                    case Key::UP:
                    {
                        // increase speed
                        CurrentSpeed += deltaSpeed * delta * scale;
                    }
                    break;

                    case Key::DOWN:
                    {
                        // decrease speed
                        CurrentSpeed -= deltaSpeed * delta * scale;
                    }
                    break;
           
                    case Key::LEFT: 
                    {
                        // turn to the left
                        deltaAngle -= 1;
                    }
                    break;
        
                    case Key::RIGHT:
                    {
                        // turn to the right
                        deltaAngle += 1;
                    }
                    break;

                    case Key::SPACE:
                    {
                        // stop moving
                        CurrentSpeed = 0;
                        deltaAngle = 0;
                    }
                    break;
                    
                    case Key::ESCAPE: 
                    {
                        // close console
                        console.close();
                        
                        // exit
                        return 0;
                    }
                    break;
                }
            }

            //
            // Update position/angle
            //
            angle += deltaAngle * delta * scale;

            int index = int(angle) & 0x7FF;
            x0+=(int)(CurrentSpeed * CosT[index])>>8;
            y0+=(int)(CurrentSpeed * SinT[index])>>8;
        }
    }
    catch (Error error)
    {
        //
        // report error
        //
        error.report();
        return 0;
    }
}

//
// Heightfield and colormap computation
//
void ComputeMap(void)
{
 int p,i,j,k,k2,p2;

 //
 // Start from a plasma clouds fractal
 //
 HMap[0]=128;
 for ( p=256; p>1; p=p2 )
 {
  p2=p>>1;
  k=p*8+20; 
  k2=k>>1;
  for ( i=0; i<256; i+=p )
  {
   for ( j=0; j<256; j+=p )
   {
    int a,b,c,d;

    a=HMap[(i<<8)+j];
    b=HMap[(((i+p)&255)<<8)+j];
    c=HMap[(i<<8)+((j+p)&255)];
    d=HMap[(((i+p)&255)<<8)+((j+p)&255)];

    HMap[(i<<8)+((j+p2)&255)]=
    Clamp(((a+c)>>1)+(rand()%k-k2));
    HMap[(((i+p2)&255)<<8)+((j+p2)&255)]=
    Clamp(((a+b+c+d)>>2)+(rand()%k-k2));
    HMap[(((i+p2)&255)<<8)+j]=
    Clamp(((a+b)>>1)+(rand()%k-k2));
   }
  }
 }

 //
 // Smoothing
 //
 for ( k=0; k<3; k++ )
 {
  for ( i=0; i<256*256; i+=256 )
  {
   for ( j=0; j<256; j++ )
   {
    HMap[i+j]=(HMap[((i+256)&0xFF00)+j]+HMap[i+((j+1)&0xFF)]+
    HMap[((i-256)&0xFF00)+j]+HMap[i+((j-1)&0xFF)])>>2;
   }
  }
 }

 //
 // Color computation (derivative of the height field)
 //
 for ( i=0; i<256*256; i+=256 )
 {
  for ( j=0; j<256; j++ )
  {
   k=128+(HMap[((i+256)&0xFF00)+((j+1)&255)]-HMap[i+j])*4;
   
   if ( k<0 ) 
    k=0; 
   if (k>255) 
    k=255;
   
   CMap[i+j]=k;
  }
 }
}

//
// Reduces a value to 0..255 (used in height field computation)
//
int Clamp(int x)
{
 return (x<0 ? 0 : (x>255 ? 255 : x));
}

//
// Draw the view from the point x0,y0 (16.16) looking at angle a
//
void View(int x0,int y0,int angle, int height, int32 *surface_buffer)
{
 //
 // Initialize last-y and last-color arrays
 //
 for ( int d=0; d<SCREENWIDTH; d++ )
 {
  lasty[d]=SCREENHEIGHT;
  lastc[d]=-1;
 }

 //
 // Compute the xy coordinates; a and b will be the position inside the
 // single map cell (0..255).
 //
 int u0=(x0>>16)&0xFF;    
 int a=(x0>>8)&0xFF;
 int v0=((y0>>8)&0xFF00); 
 int b=(y0>>8)&0xFF;
 int u1=(u0+1)&0xFF;
 int v1=(v0+256)&0xFF00;

 //
 // Fetch the height at the four corners of the square the point is in
 //
 int h0=HMap[u0+v0]; 
 int h1=HMap[u1+v0]; 
 int h2=HMap[u0+v1];
 int h3=HMap[u1+v1];

 //
 // Compute the height using bilinear interpolation
 //
 h0=(h0<<8)+a*(h1-h0);
 h2=(h2<<8)+a*(h3-h2);
 int h=((h0<<8)+b*(h2-h0))>>16;

 //
 // Draw the landscape from near to far without overdraw
 //
 for ( d=0; d<150; d+=1+(d>>6) )
  Line(x0+(d<<8)*CosT[(angle-FOV)&0x7FF],
   y0+(d<<8)*SinT[(angle-FOV)&0x7FF], 
   x0+(d<<8)*CosT[(angle+FOV)&0x7FF],
   y0+(d<<8)*SinT[(angle+FOV)&0x7FF],
   height,(100<<8)/(d+1), 
   surface_buffer, 
   d);
}

//
// Draw a "section" of the landscape; x0,y0 and x1,y1 and the xy coordinates
// on the height field, hy is the viewpoint height, s is the scaling factor
// for the distance. x0,y0,x1,y1 are 16.16 fixed point numbers and the
// scaling factor is a 16.8 fixed point value.
//
void Line(int x0,int y0,int x1,int y1,int hy,int s, int32 *surface_buffer, int fadeout)
{
 // Compute xy speed
 int sx=(x1-x0)/SCREENWIDTH;
 int sy=(y1-y0)/SCREENWIDTH;

 for ( int i=0; i<SCREENWIDTH; i++ )
 {
  //
  // Compute the xy coordinates; a and b will be the position inside the
  // single map cell (0..255).
  //
  int a=(x0>>8)&0xFF;
  int b=(y0>>8)&0xFF;
  
  int u0=(x0>>16)&0xFF;
  int u1=(u0+1)&0xFF;
  int v0=((y0>>8)&0xFF00); 
  int v1=(v0+256)&0xFF00;

  //
  // Fetch the height at the four corners of the square the point is in
  //
  int h0=HMap[u0+v0]; 
  int h1=HMap[u1+v0]; 
  int h2=HMap[u0+v1];
  int h3=HMap[u1+v1];

  //
  // Compute the height using bilinear interpolation
  //
  h0=(h0<<8)+a*(h1-h0);
  h2=(h2<<8)+a*(h3-h2);
  int h=((h0<<8)+b*(h2-h0))>>16;

  //
  // Fetch the color at the centre of the square the point is in
  //
  h0=CMap[u0+v0];
  h1=CMap[u1+v0];
  h2=CMap[u0+v1];
  h3=CMap[u1+v1];

  //
  // Compute the color using bilinear interpolation (in 16.16)
  //
  h0=(h0<<8)+a*(h1-h0);
  h2=(h2<<8)+a*(h3-h2);
  int c=((h0<<8)+b*(h2-h0));

  //
  // Compute screen height using the scaling factor
  //
  int y=(((h-hy)*s)>>11)+(SCREENHEIGHT>>1);

  //
  // Draw the column
  //
  if ( y<(a=lasty[i]) )
  {
   int coord_x = i;
   int coord_y = a;
   int sc,cc;

   if ( lastc[i]==-1 )
    lastc[i]=c;

   sc=(c-lastc[i])/(a-y);
   cc=lastc[i];

   if ( a>(SCREENHEIGHT-1))
   {
    coord_y-=(a-(SCREENHEIGHT-1));
    a=(SCREENHEIGHT-1); 
   }

   if ( y<0 ) 
    y=0;
   
   while ( y<a )
   {
    int currentColor = cc>>18;
    int32 *pixel = surface_buffer+(coord_y*SCREENWIDTH)+coord_x;

    *pixel = ((currentColor<<2) * (150-fadeout)/150) << 8;

    cc+=sc;
    coord_y-=1;
    a--;
   }

   lasty[i]=y;
  }
  lastc[i]=c;

  //
  // Advance to next xy position
  //
  x0+=sx; y0+=sy;
 }
}

//
// Calculate the lookup tables
//
void InitTables (void)
{
 for (int a = 0; a < 2048; a++)
 {
  //
  // Precalculate cosine
  //
  float result = (float)(cos ((float)a * PI / 1024.0) * 256.0);

  CosT [a] = (int)result;

  //
  // and sine
  //
  result = (float)(sin ((float)a * PI / 1024.0) * 256.0);
  SinT [a] = (int)result;
 }
}
