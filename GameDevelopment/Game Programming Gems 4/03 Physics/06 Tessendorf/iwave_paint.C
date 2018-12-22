//------------------------------------------------
//
//  iwave_paint
//
//  demonstrates the generation and interaction of
//  waves around objects by allowing the user to 
//  paint obstructions and source, and watch iwave
//  propagation.
//
//  author: Jerry Tessendorf
//  August, 2003
//
//  This software is in the public domain.
//
//-------------------------------------------------

//-------------------------------------------------
//
//  usage:
//
//  iwave_paint is an interactive paint program
//  in which the user paints on a water surface and
//  the waves evolve and react with obstructions
//  in the water.
//
//  There are two paint modes.  Typing 'o' puts the
//  program in obstruction painting mode. When you
//  hold down the left mouse button and paint, you
//  will see a black obstruction painted.  This 
//  obstruction may be any shape.
//
//  Typing 's' puts the program in source painting 
//  mode.  Now painting with the left mouse button
//  down generates a source disturbance on the water
//  surface.  The waves it produces evolve if as you
//  continue to paint.  The waves bounce of off any
//  obstructions that have been painted or are
//  subsequently painted.
//
//  Typing 'b' clears all obstructions and waves.
//
//  Typing '=' and '-' brightens and darkens the display
//  of the waves.
//
//  Pressing the spacebar starts and stops the wave
//  evolution. While the evolution is stopped, you
//  can continue painting obstructions.
//
//
//  This code was written and runs under Linux. The
//  compile command is
//
//  g++ iwave_paint.C -O2 -o iwave_paint -lglut -lGL
//
//  The default size of the grid in the simulation
//  is 100x100.  This size runs at a reasonable speed
//  on a 500 MHz P3.  For larger grids, change the
//  line
//
//  iwidth = iheight = 100;
//
//  in main() to a larger size.  For example, a grid
//  size of 200x200 ( iwidth = iheight = 200; ) runs
//  well on a 2.8 GHz P4.  
//
//
//-------------------------------------------------




#include <cmath>
#include <GL/gl.h>   // OpenGL itself.
#include <GL/glu.h>  // GLU support library.
#include <GL/glut.h> // GLUT support library.

#include <iostream>

using namespace std;

int iwidth, iheight, size;
float *display_map;
float *obstruction;
float *source;

float *height;
float *previous_height;
float *vertical_derivative;

float scaling_factor;
float kernel[13][13];

int paint_mode;
enum{ PAINT_OBSTRUCTION, PAINT_SOURCE };

bool regenerate_data;
bool toggle_animation_on_off;

float dt, alpha, gravity;

float obstruction_brush[3][3];
float source_brush[3][3];

int xmouse_prev, ymouse_prev;


//--------------------------------------------------------
//
//  Initialization routines
//
//  
// Initialize all of the fields to zero
void Initialize( float *data, int size, float value )
{
   for(int i=0;i<size;i++ ) { data[i] = value; }
}

// Compute the elements of the convolution kernel
void InitializeKernel()
{
    double dk = 0.01;
    double sigma = 1.0;
    double norm = 0;

    for(double k=0;k<10;k+=dk)
    {
        norm += k*k*exp(-sigma*k*k);
    }

    for( int i=-6;i<=6;i++ )
    {
	for( int j=-6;j<=6;j++ )
	{
	   double r = sqrt( (float)(i*i + j*j) );
           double kern = 0;
	   for( double k=0;k<10;k+=dk)
	   {
	       kern += k*k*exp(-sigma*k*k)*j0(r*k);
	   }
           kernel[i+6][j+6] = kern / norm;
	}
    }
}


void InitializeBrushes()
{
   obstruction_brush[1][1] = 0.0;
   obstruction_brush[1][0] = 0.5;
   obstruction_brush[0][1] = 0.5;
   obstruction_brush[2][1] = 0.5;
   obstruction_brush[1][2] = 0.5;
   obstruction_brush[0][2] = 0.75;
   obstruction_brush[2][0] = 0.75;
   obstruction_brush[0][0] = 0.75;
   obstruction_brush[2][2] = 0.75;

   source_brush[1][1] = 1.0;
   source_brush[1][0] = 0.5;
   source_brush[0][1] = 0.5;
   source_brush[2][1] = 0.5;
   source_brush[1][2] = 0.5;
   source_brush[0][2] = 0.25;
   source_brush[2][0] = 0.25;
   source_brush[0][0] = 0.25;
   source_brush[2][2] = 0.25;
}

void ClearObstruction()
{
    for(int i=0;i<size;i++ ){ obstruction[i] = 1.0; }
}

void ClearWaves()
{
    for(int i=0;i<size;i++ )
    {
       height[i] = 0.0;
       previous_height[i] = 0.0;
       vertical_derivative[i] = 0.0;
    }
}

//----------------------------------------------------

void ConvertToDisplay()
{
    for(int i=0;i<size;i++ ){ display_map[i] = 0.5*( height[i]/scaling_factor + 1.0 )*obstruction[i]; }
}

//----------------------------------------------------
//
//  These two routines, 
//
//    ComputeVerticalDerivative()
//    Propagate()
//
//  are the heart of the iWave algorithm.
//
//  In Propagate(), we have not bothered to handle the
//  boundary conditions.  This makes the outermost
//  6 pixels all the way around act like a hard wall.
//

void ComputeVerticalDerivative()
{
    // first step:  the interior
    for(int ix=6;ix<iwidth-6;ix++)
    {
        for(int iy=6;iy<iheight-6;iy++)
	{
	    int index = ix + iwidth*iy;
	    float vd = 0;
	    for(int iix=-6;iix<=6;iix++)
	    {
	       for(int iiy=-6;iiy<=6;iiy++)
	       {
		  int iindex = ix+iix + iwidth*(iy+iiy);
	          vd += kernel[iix+6][iiy+6] * height[iindex];
	       }
	    }
	    vertical_derivative[index] = vd;
	}
    }
}

void Propagate()
{
    // apply obstruction
    for( int i=0;i<size;i++ ) { height[i] *= obstruction[i]; }
    
    // compute vertical derivative
    ComputeVerticalDerivative();

    // advance surface
    float adt = alpha*dt;
    float adt2 = 1.0/(1.0+adt);
    for( int i=0;i<size;i++ )
    {
       float temp = height[i];
       height[i] = height[i]*(2.0-adt) - previous_height[i] - gravity*vertical_derivative[i];
       height[i] *= adt2;
       height[i] += source[i];
       height[i] *= obstruction[i];
       previous_height[i] = temp;
       // reset source each step
       source[i] = 0;
    }   
}


//------------------------------------------
//
//  Painting and display code
//

void resetScaleFactor( float amount )
{
   scaling_factor *= amount;
}

void DabSomePaint( int x, int y )
{
   int xstart = x - 1;
   int ystart = y - 1;
   if( xstart < 0 ){ xstart = 0; }
   if( ystart < 0 ){ ystart = 0; }

   int xend = x + 1;
   int yend = y + 1;
   if( xend >= iwidth ){ xend = iwidth-1; }
   if( yend >= iheight ){ yend = iheight-1; }

   
   if( paint_mode == PAINT_OBSTRUCTION )
   {
      for(int ix=xstart;ix <= xend; ix++)
      {
         for( int iy=ystart;iy<=yend; iy++)
	 {
            int index = ix + iwidth*(iheight-iy-1);
	    obstruction[index] *= obstruction_brush[ix-xstart][iy-ystart];
	 }
      }
   }
   else if( paint_mode == PAINT_SOURCE )
   {
      for(int ix=xstart;ix <= xend; ix++)
      {
         for( int iy=ystart;iy<=yend; iy++)
	 {
            int index = ix + iwidth*(iheight-iy-1);
	    source[index] += source_brush[ix-xstart][iy-ystart];
	 }
      }
   }
   return; 
}

//----------------------------------------------------
//
//  GL and GLUT callbacks
//
//----------------------------------------------------



void cbDisplay( void )
{
   glClear(GL_COLOR_BUFFER_BIT );
   glDrawPixels( iwidth, iheight, GL_LUMINANCE, GL_FLOAT, display_map );
   glutSwapBuffers();
}

// animate and display new result
void cbIdle()
{
   if( toggle_animation_on_off ) { Propagate(); }
   ConvertToDisplay();
   cbDisplay();	
}


void cbOnKeyboard( unsigned char key, int x, int y )
{
   switch (key) 
   {
      case '-': case '_':
      resetScaleFactor( 1.0/0.9 );
      regenerate_data = true;
      break;

      case '+': case '=':
      resetScaleFactor( 0.9 );
      regenerate_data = true;
      break;

      case ' ':
      toggle_animation_on_off = !toggle_animation_on_off;

      case 'o':
      paint_mode = PAINT_OBSTRUCTION;
      break;

      case 's':
      paint_mode = PAINT_SOURCE;
      break;

      case 'b':
      ClearWaves();
      ClearObstruction();
      Initialize( source, size, 0.0 );
      break;
      
      default:
      break;
   }
}

void cbMouseDown( int button, int state, int x, int y )
{
   if( button != GLUT_LEFT_BUTTON ) { return; }
   if( state != GLUT_DOWN ) { return; }
   xmouse_prev = x;
   ymouse_prev = y;
   DabSomePaint( x, y );
}



void cbMouseMove( int x, int y )
{
   xmouse_prev = x;
   ymouse_prev = y;
   DabSomePaint( x, y );
}




//---------------------------------------------------

int main(int argc, char** argv)
{
   // initialize a few variables
   iwidth = iheight = 100;
   size = iwidth*iheight;

   dt = 0.03;
   alpha = 0.3;
   gravity = 9.8 * dt * dt;

   scaling_factor = 1.0;
   toggle_animation_on_off = true;

   // allocate space for fields and initialize them
   height              = new float[size];
   previous_height     = new float[size];
   vertical_derivative = new float[size];
   obstruction         = new float[size];
   source              = new float[size];
   display_map         = new float[size];

   ClearWaves();
   ClearObstruction();
   ConvertToDisplay();
   Initialize( source, size, 0 );

   InitializeBrushes();

   // build the convolution kernel
   InitializeKernel();


   
   // GLUT routines
   glutInit(&argc, argv);

   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
   glutInitWindowSize( iwidth, iheight );

   // Open a window 
   char title[] = "iWave Demo";
   int Window_ID = glutCreateWindow( title );
   
   glClearColor( 1,1,1,1 );

   glutDisplayFunc(&cbDisplay);
   glutIdleFunc(&cbIdle);
   glutKeyboardFunc(&cbOnKeyboard);
   glutMouseFunc( &cbMouseDown );
   glutMotionFunc( &cbMouseMove );

   glutMainLoop();
   return 1;

};
