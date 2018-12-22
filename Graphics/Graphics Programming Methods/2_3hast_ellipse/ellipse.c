// A Fast All Integer Ellipse Discretization Algorithm
// Tony Barrera, Barrera Kristiansen AB; Anders Hast, 
// Creative Media Lab, University of Gävle; 
// and Ewert Bengtsson, Centre For Image Analysis, Uppsala University
// tony.barrera@spray.se,  aht@hig.se, and ewert@cb.uu.se


// Click left mouse button in order to choose center and click again
// to choose height&width for ellipses to be drawn
#include <GL/glut.h>
#define X 400
#define Y 300

int x_center;
int y_center;


void Ellipse(int x0,int y0,int a, int b)
{
  int d,u,v,a2,b2,s,t,du,dv;
  int ax, by,cx, dy;
  int temp;

  a2=a*a;b2=b*b;
  t=a2*b;
  d=b2-t;
  du=b2<<1;
  dv=-a2<<1;
  u=du+b2;
  v=dv*(b-1);
  s=0;
  
  ax=x0;
  by=y0+b;
  cx=x0;
  dy=y0-b;
  
  
  while(s<t){
 
    glVertex2f(ax,by);
    glVertex2f(cx,by);
    glVertex2f(ax,dy);
    glVertex2f(cx,dy);

    if(d>=0){
	  d=d+v;
	  v=v-dv;
	  by--;
	  dy++;
	  t=t-a2;
	}
      
   
    d=d+u;
    u=u+du;
    ax++;
    cx--;
    s=s+b2;
  }
  t=b2*a;
  d=a2-t;
  temp=-du;
  du=-dv;
  dv=temp;
  u=du+a2;
  v=dv*(a-1);
  s=0;
  
  ax=y0;
  by=x0+a;
  cx=y0;
  dy=x0-a;
  
  while(s<t){
    glVertex2i(by,ax);
    glVertex2i(by,cx);
    glVertex2i(dy,ax);
    glVertex2i(dy,cx);

    if(d>=0){
	  d=d+v;
	  v=v-dv;
	  by--;
	  dy++;
	  t=t-b2;
	}
     
    d=d+u;
    u=u+du;
    ax++;
    cx--;
    s=s+a2;
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
      Ellipse(x_center, y_center, abs(x_center-x), abs(y_center-y));
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
  glutCreateWindow("Ellipse"); 
  glClearColor(1.0, 1.0, 1.0, 1.0); 
  gluOrtho2D(0.0, X, Y,  0.0);
  glColor3f(0.0,0.5,0.1);
  glutDisplayFunc(myDisplay); 
  glutMouseFunc(myMouseFunc);
  glutMainLoop();
}
