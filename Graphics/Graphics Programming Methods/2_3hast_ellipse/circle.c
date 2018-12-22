// A Fast All Integer Ellipse Discretization Algorithm
// Tony Barrera, Barrera Kristiansen AB; Anders Hast, 
// Creative Media Lab, University of Gävle; 
// and Ewert Bengtsson, Centre For Image Analysis, Uppsala University
// tony.barrera@spray.se,  aht@hig.se, and ewert@cb.uu.se

// Click left mouse button in order to choose center and click again
// to choose radius for circles to be drawn
#include <math.h>
#include <GL/glut.h>
#define X 400
#define Y 300

int x_center;
int y_center;



void Circle(int x0, int y0,int R)
{
  int x,y,d,s,x1,y1,x2,y2;
  int ax,by,cy,dy,ex,fx,gx,hy;
  y1=y0+1;
  y2=y0-1;
  x1=x0+1;
  x2=x0-1;
  y=R-1;
  x=0;
  d=-R;
  s=3;
  
  ax=x0;
  by=y0;
  cy=y1+y;
  dy=y2-y;
  ex=x1+y;
  fx=x2-y;
  gx=x0;
  hy=y0;
  y++;
  
  while(x<=y)
    {
 
      glVertex2i(ax,cy);
      glVertex2i(ex,by);
      glVertex2i(gx,cy);
      glVertex2i(fx,by);
      glVertex2i(ax,dy);
      glVertex2i(ex,hy);
      glVertex2i(gx,dy);
      glVertex2i(fx,hy);
      
      if(d>=0)
	{
	  d=d-2*y;
	  cy--;
	  dy++;
	  ex--;
	  fx++;
	  y--;
	}
      d=d+s;
      s=s+2;
      
      ax--;
      by--;
      gx++;
      hy++;
      x++;
    }
  
}





void myDisplay(){
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
}


void myMouseFunc(int button, int state, int x, int y) {
	if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
    if (x_center==0) {
      x_center=x;
      y_center=y;
    }
    else {
      glBegin(GL_POINTS);
      Circle(x_center, y_center, sqrt((x_center-x)*(x_center-x)+(y_center-y)*(y_center-y)));
      x_center=0;
      glEnd();
      glFlush();
    }
  }
}


void main(int argc,char ** argv){
  glutInit(&argc, argv);
  glutInitWindowSize(X,Y);
  glutInitWindowPosition(0,0);
  glutCreateWindow("Circle"); 
  glClearColor(1.0, 1.0, 1.0, 1.0); 
  gluOrtho2D(0.0, X, Y,  0.0);
  glColor3f(0.0,0.5,0.1);
  glutDisplayFunc(myDisplay); 
  glutMouseFunc(myMouseFunc);
  glutMainLoop();
}
